  
CPPFLAGS += -std=c++17 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I inc

all : bin/print_canonical

src/maths_parser.tab.cpp src/maths_parser.tab.hpp : src/parser.y
	bison -v -d src/parser.y -o src/maths_parser.tab.cpp

src/maths_lexer.yy.cpp : src/lexer.flex src/maths_parser.tab.hpp
	flex -o src/maths_lexer.yy.cpp  src/lexer.flex

bin/c_compiler : bin/compiler src/wrapper.sh
	cp src/wrapper.sh bin/c_compiler
	chmod u+x bin/c_compiler

bin/compiler : src/compiler.cpp
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/compiler $^

bin/print_canonical : src/print_canonical -o src/maths_parser.tab.o src/lexer.yy.o src/maths_parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/print_canonical $^
	
clean :
	rm -f src/*.o
	rm -f bin/*
