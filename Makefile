build_run_main:
	g++ -std=c++14 -Werror -Wuninitialized -o bin/main src/main.cpp && ./bin/main

test:
	g++ -std=c++14 -Werror -Wuninitialized -o bin/test test-unit/test.cpp && ./bin/test

test-RangeTree:
	g++ -std=c++14 -Werror -Wuninitialized -o bin/testRange src/testRange.cpp && ./bin/testRange

