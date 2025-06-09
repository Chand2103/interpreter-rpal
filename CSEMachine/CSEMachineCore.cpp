// CSEMachineCore.cpp

#include "CSEMachine.h"
#include "LexicalAnalyzer.h"
#include <iostream>
#include <math.h>
using namespace std;

CSEMachine::CSEMachine() {}

CSEMachine::~CSEMachine() {}

CSEMachine::CSEMachine(TreeNode* input){
    this->inputTree = input;
    this->deltaCounter = 0;
    this->currDeltaNum = 0;
    this->envCounter = 0;
    this->envStack.push(0);
    this->currEnv = 0;
    this->envMap = map<int,int>();
    this->printCalled = false;
}

void CSEMachine::evaluateTree(){
    createControlStructures(this->inputTree);
    Token envToken("env",envCounter);
    stack<Token> controlStack;
    stack<Token> executionStack;
    controlStack.push(envToken);
    envMap[0] = -1;
    for(auto& token : deltaMap[0]) controlStack.push(token);
    executionStack.push(envToken);
    int whileCount = 0;
    while(controlStack.size() != 1){
        Token currToken = controlStack.top(); controlStack.pop();
        processCurrentToken(currToken, controlStack, executionStack);
        if(whileCount++ > 5000) break;
    }
    if(!printCalled) cout << endl;
}

Token CSEMachine::applyOperator(Token firstToken, Token secondToken, Token currToken){
    string op = currToken.value;
    if (firstToken.type == LexicalAnalyzer::INT) {
        int a = stoi(firstToken.value), b = stoi(secondToken.value), res;
        if (op == "*") res = a * b;
        else if (op == "+") res = a + b;
        else if (op == "-") res = a - b;
        else if (op == "/") res = a / b;
        else if (op == "**") res = pow(a, b);
        else if (op == "gr") return a > b ? Token("true","true") : Token("false","false");
        else if (op == "ls") return a < b ? Token("true","true") : Token("false","false");
        else if (op == "ge") return a >= b ? Token("true","true") : Token("false","false");
        else if (op == "le") return a <= b ? Token("true","true") : Token("false","false");
        else if (op == "eq") return a == b ? Token("true","true") : Token("false","false");
        else if (op == "ne") return a != b ? Token("true","true") : Token("false","false");
        return Token(to_string(res), LexicalAnalyzer::INT);
    } else if (firstToken.type == LexicalAnalyzer::STR) {
        if (op == "eq") return firstToken.value == secondToken.value ? Token("true","true") : Token("false","false");
        if (op == "ne") return firstToken.value != secondToken.value ? Token("true","true") : Token("false","false");
    } else if (firstToken.type == "true" || firstToken.type == "false") {
        if (op == "or") return (firstToken.type == "true" || secondToken.type == "true") ? Token("true","true") : Token("false","false");
        if (op == "&") return (firstToken.type == "true" && secondToken.type == "true") ? Token("true","true") : Token("false","false");
        if (op == "eq") return (firstToken.type == secondToken.type) ? Token("true","true") : Token("false","false");
        if (op == "ne") return (firstToken.type != secondToken.type) ? Token("true","true") : Token("false","false");
    }
    return Token("", "");
}

void CSEMachine::processCurrentToken(Token &currToken,stack<Token> &controlStack, stack<Token> &executionStack){
	//cout<<"Control stack top: "<<currToken.type <<" Exe top: "<<executionStack.top().type<< endl;
	//cout<<"Control stack top: "<<currToken.value <<" Exe top: "<<executionStack.top().value<< endl;
	if(currToken.type == LexicalAnalyzer::OPT){
		Token firstToken = executionStack.top();
		executionStack.pop();
		Token secondToken = executionStack.top();
		executionStack.pop();
		Token resultToken = applyOperator(firstToken, secondToken, currToken);
		executionStack.push(resultToken);
	}else if(currToken.type == "neg"){
		Token firstToken = executionStack.top();
		executionStack.pop();
		int paramVal = atoi(firstToken.value.c_str());
		paramVal = -paramVal;
		Token newToken(intToString(paramVal), LexicalAnalyzer::INT);
		executionStack.push(newToken);
	}else if(currToken.type =="not"){
		Token firstToken = executionStack.top();
		executionStack.pop();
		if(firstToken.value == "true"){
			executionStack.push(Token("false","false"));
		}else{
			executionStack.push(Token("true","true"));
		}
	//}else if(currToken.type == LexicalAnalyzer::ID && isParamter(currToken)){
	}else if(currToken.type == LexicalAnalyzer::ID && isParamter(currToken)){
		string varName = currToken.value;
		int temp = currEnv;
		//cout <<  "Current env "<<temp<< endl;
		pair<int,string> keyPair(temp,varName);
		map<key_pair,Token>::iterator it = paramMap.find(keyPair);
		while(paramMap.end() == it && temp>=0){
			temp = envMap[temp];
			keyPair.first = temp;
			//cout << "Temp: "<<temp;
			it = paramMap.find(keyPair);
		}
		if(paramMap.end() != it){
			Token paramValToken = it->second;
			//paramMap.erase(it);
			executionStack.push(paramValToken);
		}
	}else if(currToken.type == "gamma"){
		Token topExeToken = executionStack.top();
		executionStack.pop();
		if(topExeToken.type == "lambdaClosure"){
			Token env("env",++envCounter);
			//cout<< "Parent: "<< topExeToken.lambdaEnv << " Env: "<< envCounter;
			envMap[envCounter] = topExeToken.lambdaEnv;
			envStack.push(envCounter);
			currEnv = envCounter;
			if(topExeToken.isTuple == false){
				string paramName = topExeToken.lambdaParam;
				Token paramToken = executionStack.top();
				executionStack.pop();
				//int paramValue = atoi(paramToken.value.c_str());
				pair<int,string> keyPair(envCounter,paramName);
				paramMap[keyPair] = paramToken;
				//cout<< "Inside if"<<paramToken.type<<" Param name "<<paramName<< " Environment "<<envCounter<<endl;
			}else{
				//cout << "Inside else"<<endl;
				string tuple = topExeToken.lambdaParam;
				vector<string> params = split(tuple,',');
				Token valueTuple = executionStack.top();
				//cout << "Value tuple: "<< valueTuple.type<< valueTuple.value <<endl;
				executionStack.pop();
				vector<Token> tupleVector = valueTuple.tuple;
				//cout<< "Num of parameters "<< params.size() << " Num of values "<<tupleVector.size();
				for(unsigned int i=0;i<params.size();i++){
					if(params[i] != ""){
						pair<int,string> keyPair(envCounter,params[i].c_str());
						paramMap[keyPair] = tupleVector[i];
					}
					//cout<< "Inside for loop "<<endl;
				}
				//cout<< "Outside for"<<endl;
				////cout << "Escaping else"<<paramMap["x"]<<paramMap["y"]<<endl;
			}
			controlStack.push(env);
			executionStack.push(env);
			int lambdaNum = topExeToken.lambdaNum;
			vector<Token> delta = deltaMap[lambdaNum];
			for(int i=0;i<delta.size();i++){
				controlStack.push(delta[i]);
			}
		}else if(topExeToken.type == "YSTAR"){
			Token nextToken = executionStack.top();
			//cout << "Inside Ystar "<< nextToken.type<<endl;
			executionStack.pop();
			nextToken.type ="eta";
			executionStack.push(nextToken);
		}else if(topExeToken.type == "eta"){
			Token lambdaToken = topExeToken;
			lambdaToken.type = "lambdaClosure";
			executionStack.push(topExeToken);
			executionStack.push(lambdaToken);
			Token gammaToken("gamma","gamma");
			controlStack.push(gammaToken);
			controlStack.push(gammaToken);
		}else if(topExeToken.value == "Stern" || topExeToken.value == "stern"){
			Token stringToken = executionStack.top();
			executionStack.pop();
			string tokenValue = stringToken.value;
			tokenValue = tokenValue.substr(2,tokenValue.size()-3);
			tokenValue = "'"+tokenValue+"'";
			stringToken.value = tokenValue;
			executionStack.push(stringToken);
		}else if(topExeToken.value == "Stem" || topExeToken.value == "stem"){
			Token stringToken = executionStack.top();
			executionStack.pop();
			string tokenValue = stringToken.value;
			tokenValue = tokenValue.substr(1,1);
			tokenValue = "'"+tokenValue+"'";
			stringToken.value = tokenValue;
			executionStack.push(stringToken);
		}else if(topExeToken.value == "Conc" || topExeToken.value == "conc"){
			Token firstToken = executionStack.top();
			executionStack.pop();
			Token secondToken = executionStack.top();
			executionStack.pop();
			//cout<< "Inside Concat 1 "<<firstToken.value << " 2 "<<secondToken.value<<endl;
			string concatValue = firstToken.value.substr(1,firstToken.value.size()-2)+secondToken.value.substr(1,secondToken.value.size()-2);
			concatValue = "'"+concatValue+"'";
			//cout <<"Concat value "<<concatValue<<endl;
			Token newToken(concatValue,LexicalAnalyzer::STR);
			executionStack.push(newToken);
			//Removing extra gamma
			controlStack.pop();
		}else if(topExeToken.value == "ItoS" || topExeToken.value == "itos"){
			Token firstToken = executionStack.top();
			executionStack.pop();
			firstToken.type = LexicalAnalyzer::STR;
			firstToken.value = "'"+firstToken.value+"'";
			executionStack.push(firstToken);
			//Removing extra gamma
			//scontrolStack.pop();
		}else if(topExeToken.value == "Print" || topExeToken.value == "print"){
			printCalled = true;
			//cout << "Inside print" << endl;
			Token t = executionStack.top();
			executionStack.pop();
			if(t.isTuple == false){
				if(t.type== LexicalAnalyzer::STR){
					string tempStr =unescape(t.value.substr(1,t.value.size()-2));
					cout << tempStr;
					if(tempStr[tempStr.size()-1] == '\n')
						cout<<endl;
					//cout << t.value.substr(1,t.value.size()-2);
				}else if(t.type == "lambdaClosure"){
					cout <<"[lambda closure: "<<t.lambdaParam<<": "<<t.lambdaNum<<"]";
				}else{
					//cout<<t.value<<endl;
					cout<<t.value;
				}
				Token dummyToken("dummy","dummy");
				executionStack.push(dummyToken);
			}else{
				vector<Token> tupleVector = t.tuple;
				for(int i=0;i<tupleVector.size();i++){
					if(i==0){
						cout<<"(";
					}else{
						cout<<", ";
					}
					if(tupleVector[i].type == LexicalAnalyzer::STR){
						cout<< unescape(tupleVector[i].value.substr(1,tupleVector[i].value.size()-2));
					}else if(tupleVector[i].isTuple == true ){
						cout<<"Inside else if"<<endl;
						vector<Token> innerTuple = tupleVector[i].tuple;
						cout << "Size" << innerTuple.size()<<endl;
						if(innerTuple.size() == 1){
							if(innerTuple[0].type == LexicalAnalyzer::STR)
								cout<< unescape(innerTuple[0].value.substr(1,innerTuple[0].value.size()-2));
						}
					}else{
						cout << tupleVector[i].value;
					}
					if(i==tupleVector.size() -1){
						cout<<")";
					}
				}
			}
			//cout<< endl;
		}else if(topExeToken.value == "Isinteger"){
			Token t = executionStack.top();
			executionStack.pop();
			executionStack.push(t.type==LexicalAnalyzer::INT ? Token("true","true"):Token("false","false"));
		}else if(topExeToken.value == "Istruthvalue"){
			Token t = executionStack.top();
			executionStack.pop();
			executionStack.push(t.value=="true" || t.value=="false" ? Token("true","true"):Token("false","false"));
		}else if(topExeToken.value == "Isstring"){
			Token t = executionStack.top();
			executionStack.pop();
			executionStack.push(t.type==LexicalAnalyzer::STR ? Token("true","true"):Token("false","false"));
		}else if(topExeToken.value == "Istuple"){
			//cout<<"Inside is tuple"<<endl;
			Token t = executionStack.top();
			executionStack.pop();
			executionStack.push(t.isTuple==true ? Token("true","true"):Token("false","false"));
		}else if(topExeToken.value == "Isdummy"){
			Token t = executionStack.top();
			executionStack.pop();
			executionStack.push(t.value=="dummy" ? Token("true","true"):Token("false","false"));
		}else if(topExeToken.value == "Isfunction"){
			Token t = executionStack.top();
			executionStack.pop();
			executionStack.push(t.type=="lambdaClosure" ? Token("true","true"):Token("false","false"));
		}else if(topExeToken.value == "Order"){
			//cout<<"Inside Order "<<endl;
			Token t = executionStack.top();
			executionStack.pop();
			executionStack.push(Token(intToString(t.tuple.size()),LexicalAnalyzer::INT));
		}else if(topExeToken.value == "Null"){
			//cout<<"Inside Null "<<endl;
			Token t = executionStack.top();
			executionStack.pop();
			executionStack.push((t.value == "nil") ? Token("true","true"):Token("false","false"));
		}else if(topExeToken.isTuple == true){
			Token t = executionStack.top();
			executionStack.pop();
			if(t.type == LexicalAnalyzer::INT){
				int indx = atoi(t.value.c_str());
				indx -=1;
				executionStack.push(topExeToken.tuple[indx]);
			}
		}
	}else if(currToken.type =="env"){
		Token topToken = executionStack.top();
		executionStack.pop();
		executionStack.pop();
		executionStack.push(topToken);
		envStack.pop();
		currEnv = envStack.top();
	}else if(currToken.type == "beta"){
		Token topToken = executionStack.top();
		executionStack.pop();
		vector<Token> delta;
		if(topToken.value == "true"){
			delta = deltaMap[currToken.betaIfDeltaNum];
		}else{
			delta = deltaMap[currToken.betaElseDeltaNum];
		}
		for(unsigned int i=0;i<delta.size();i++){
			controlStack.push(delta[i]);
		}
	}else if(currToken.value == "tau"){
		int tauCount = currToken.tauCount;
		//cout << "Tau count "<< tauCount<< endl;
		string tuple="(";
		vector<Token> tupleVector;
		for(int i=0;i<tauCount;i++){
			Token t = executionStack.top();
			tupleVector.push_back(t);
			executionStack.pop();
			if(i == tauCount -1)
				tuple += t.value;
			else
				tuple += t.value +", ";
		}
		tuple +=")";
		//cout<< "Tuple value: " <<tuple<<endl;
		Token newToken(tuple,"tuple");
		newToken.tuple = tupleVector;
		newToken.isTuple = true;
		executionStack.push(newToken);
	}else if(currToken.value == "nil"){
		currToken.isTuple = true;
		executionStack.push(currToken);
	}else if(currToken.value == "aug"){
		//cout <<"Inside aug "<<endl;
		Token tuple = executionStack.top();
		executionStack.pop();
		Token toAdd = executionStack.top();
		executionStack.pop();
		if(tuple.value == "nil"){
			//Token newToken("("+toAdd.value+")","tuple");
			Token newToken(toAdd.value,"tuple");
			newToken.isTuple = true;
			newToken.tuple = vector<Token>();
			newToken.tuple.push_back(toAdd);
			executionStack.push(newToken);
		}else{
			tuple.tuple.push_back(toAdd);
			executionStack.push(tuple);
		}
	}else if(currToken.type == "lambdaClosure"){
		//cout<< "Inside lambdaclosure env set"<<endl;
		currToken.lambdaEnv = currEnv;
		executionStack.push(currToken);
	}else{
		executionStack.push(currToken);
	}
}
