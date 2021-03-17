  
CPPFLAGS += -std=c++11 -W -Wall -g -Wno-unused-parameter
CPPFLAGS += -I include

all : bin/print_canonical

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y
	bison -v -d src/parser.y -o src/parser.tab.cpp

src/lexer.yy.cpp : src/lexer.flex src/parser.tab.hpp
	flex -o src/lexer.yy.cpp  src/lexer.flex


bin/c_compiler : bin/compiler src/wrapper.sh
	cp src/wrapper.sh bin/c_compiler
	chmod u+x bin/c_compiler

bin/compiler : src/compiler.cpp
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/compiler $^

bin/print_canonical : src/print_canonical.o src/parser.tab.o src/lexer.yy.o src/parser.tab.o
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/print_canonical $^
	
clean :
	rm src/*.o
	rm bin/*
	rm src/*.tab.cpp
	rm src/*.yy.cpp
	rm src/*.tab.hpp
