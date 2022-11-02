FLAGS = g++ -g -std=c++17 -Wall -Wextra -Werror
DATA = "In this application, you can test a neural network for guessing letters."

all: install test

install:
	cd View && qmake && make && make clean && rm Makefile && mv View.app ..
	mv View.app/Contents/MacOS/View ./MLP
	./MLP

uninstall:
	rm -rf View.app MLP

reinstall: uninstall
	make install

dist:
	tar cfz MLP.tgz MLP

dvi:
	touch info.dvi
	@(echo $(DATA) >> info.dvi)

compile:
	$(FLAGS) tests.cpp Controller/*.h Controller/*.cpp \
	Model/GraphModel/*.h Model/GraphModel/*.cpp Model/MatrixModel/*.h Model/MatrixModel/*.cpp -lgtest -g

test: compile
	./a.out

leaks_check: compile
	CK_FORKS=no leaks -atExit -- ./a.out

style_check:
	cp ../materials/linters/.clang-format .
	clang-format -n View/*.h View/*.cpp Controller/*.h Controller/*.cpp \
	Model/GraphModel/*.h Model/GraphModel/*.cpp Model/MatrixModel/*.h Model/MatrixModel/*.cpp

clean:
	rm -rf MLP.tgz a.out* View.app info.dvi MLP build-View-Desktop_x86_darwin_generic_mach_o_64bit-Debug
