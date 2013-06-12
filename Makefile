judger:judger.cpp common.hxx common.cpp
	g++ -Wall -D_debug judger.cpp common.cpp -o judger -std=c++11
test:judger
	./judger
	
