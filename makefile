UNAME_S:=$(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CXX:=clang++
else
	CXX:=g++
endif


all:
	$(CXX) -std=c++11 writeStream.cpp -o writeStream.exe -ladios2_cxx11 -lpthread
	$(CXX) -std=c++11 readStream.cpp -o readStream.exe -ladios2_cxx11 -lpthread


clean:
	rm -f *.exe
