CXX=g++
CXXFLAGS=`pkg-config --cflags glfw3` -g
LDLIBS=`pkg-config --static --libs glfw3` -lxcb -lstdc++

all: final_version

final_version: final_version.o

clean:
	rm -rf final_version 
