CXXFLAGS=-std=c++14
LDLIBS=-lstdc++ -lpthread -lunistring

analyser: analyser.o stemfile.o tokenize.o snowball/libstemmer.o

analyser.o: stemfile.h tokenize.h

snowball/libstemmer.o:
	$(MAKE) -C libstemmer
