// CSEMachineBuilder.cpp
#include <algorithm>
#include <iostream>
#include "CSEMachine.h"
#include "LexicalAnalyzer.h"
using namespace std;

void CSEMachine::createControlStructures(TreeNode* root) {
    pendingDeltaQueue.push(root);
    while (!pendingDeltaQueue.empty()) {
        vector<Token> currentDelta;
        TreeNode* node = pendingDeltaQueue.front(); pendingDeltaQueue.pop();
        preOrderTraversal(node, currentDelta);
        deltaMap[currDeltaNum++] = currentDelta;
    }
}

void CSEMachine::preOrderTraversal(TreeNode* root, vector<Token>& currentDelta) {
    if (root->value.type == "lambda") {
        if (root->left->value.value != ",") {
            currentDelta.emplace_back("lambdaClosure", root->left->value.value, ++deltaCounter);
        } else {
            TreeNode* c = root->left->left;
            string tuple;
            while (c) { tuple += c->value.value + ","; c = c->right; }
            Token lambdaClosure("lambdaClosure", tuple, ++deltaCounter);
            lambdaClosure.isTuple = true;
            currentDelta.push_back(lambdaClosure);
        }
        pendingDeltaQueue.push(root->left->right);
        if (root->right) preOrderTraversal(root->right, currentDelta);
    } else if (root->value.value == "->") {
        currentDelta.emplace_back("beta", deltaCounter + 1, deltaCounter + 2);
        pendingDeltaQueue.push(root->left->right);
        pendingDeltaQueue.push(root->left->right->right);
        root->left->right->right = nullptr;
        root->left->right = nullptr;
        deltaCounter += 2;
        if (root->left) preOrderTraversal(root->left, currentDelta);
        if (root->right) preOrderTraversal(root->right, currentDelta);
    } else {
        currentDelta.push_back(root->value);
        if (root->left) preOrderTraversal(root->left, currentDelta);
        if (root->right) preOrderTraversal(root->right, currentDelta);
    }
}

string CSEMachine::intToString(int val) {
    return to_string(val);
}

vector<string> CSEMachine::split(string input, char delim) {
    vector<string> result;
    string token;
    for (char ch : input) {
        if (ch == delim) {
            result.push_back(token);
            token.clear();
        } else token += ch;
    }
    result.push_back(token);
    return result;
}

bool CSEMachine::notFunction(string id) {
    static const vector<string> builtins = {"Stem","stem","Stern","stern","Print","print","Conc","conc","Istuple","Isinteger","Isfunction","Istruthvalue", "Isdummy","Order","Null"};
    return find(builtins.begin(), builtins.end(), id) == builtins.end();
}

bool CSEMachine::isParamter(Token currToken) {
    string var = currToken.value;
    int env = currEnv;
    key_pair key(env, var);
    auto it = paramMap.find(key);
    while (it == paramMap.end() && env >= 0) {
        env = envMap[env];
        key.first = env;
        it = paramMap.find(key);
    }
    return it != paramMap.end();
}

string CSEMachine::unescape(const string& s) {
    string res;
    auto it = s.begin();
    while (it != s.end()) {
        char c = *it++;
        if (c == '\\' && it != s.end()) {
            switch (*it++) {
                case '\\': c = '\\'; break;
                case 'n': c = '\n'; break;
                case 't': c = '\t'; break;
                default: continue;
            }
        }
        res += c;
    }
    return res;
}

void CSEMachine::printTuple(Token t) {
    vector<Token>& v = t.tuple;
    cout << "(";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) cout << ", ";
        if (v[i].type == LexicalAnalyzer::STR)
            cout << unescape(v[i].value.substr(1, v[i].value.size() - 2));
        else if (v[i].type == "tuple")
            printTuple(v[i]);
        else
            cout << v[i].value;
    }
    cout << ")";
}
