all :
	g++ Evaluator.cpp LexicalAnalyzer.cpp Token.cpp TreeNode.cpp TreeTransformer.cpp CSEMachine.cpp Parser.cpp -std=c++11 -o myrpal

cl :
	rm -f *.o myrpal
