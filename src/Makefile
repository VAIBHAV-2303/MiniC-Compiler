ANTLR4=java -Xmx500M -cp "/usr/local/lib/antlr-4.8-complete.jar:$CLASSPATH" org.antlr.v4.Tool

antlr:	MiniC.g4
		
		$(ANTLR4) -visitor -no-listener -Dlanguage=Cpp -o generated -package miniC MiniC.g4
		
		g++ -g -std=c++14 main.cpp -o main \
		-I/usr/local/include/antlr4-runtime -Igenerated -I/usr/include/llvm-3.8  -I/usr/include/llvm-c-3.8 \
		-lantlr4-runtime -lLLVM-3.8

		g++ -g -std=c++14 printEBBs.cpp -o bonus \
		-I/usr/include/llvm-3.8 -I/usr/include/llvm-c-3.8 \
		-lLLVM-3.8
clean:	
		rm -rf generated main bonus temp.*