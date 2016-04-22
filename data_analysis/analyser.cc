#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>

#include "libstemmer.h"
#include "stemfile.h"
#include "tokenize.h"

#define THREAD_NUM 4

std::vector<std::string> data;

void *thread_analyser(struct ThreadData *thread_data);
int readData(std::istream &source);


struct ThreadData {
	int start_line;
	int end_line;
	pthread_t thread;
	FILE *raw_file;
	std::map<std::string, int> roots;
};

int main(int argc, char *argv[])  {
	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	std::ifstream input(argv[1]);
	int lines_count = readData(input);
	if (lines_count == 0) {
		perror("readData");
	}
	int block_size = lines_count / THREAD_NUM;
	//int extra_size = lines_count % 4;
	ThreadData threads[THREAD_NUM];

	for (int i = 0; i < THREAD_NUM; ++i) {
		if (i != THREAD_NUM - 1) {
			threads[i].start_line = i * block_size;
			threads[i].end_line = (i + 1) * block_size - 1; 
		} else {
			threads[i].start_line = i * block_size;
			threads[i].end_line = lines_count - 1;
		}
		std::string namefile = "in";
		namefile.push_back('0' + i);
		threads[i].raw_file = fopen(namefile.c_str(), "rw");
	}
	for (int i = 0; i < THREAD_NUM; ++i) {
		for (int j = threads[i].start_line; j <= threads[i].end_line; ++j) {
			fprintf(threads[i].raw_file, "%s\n", data[i]);
		}
	}

	for (int i = 0; i < THREAD_NUM; ++i) {
		pthread_create(&threads[i].thread, NULL, (void * (*)(void *)) thread_analyser, &threads[i]);
	}
	for (int i = 0; i < THREAD_NUM; ++i) {
		pthread_join(threads[i].thread, NULL);
	}


	std::map<std::string, int> &result = threads[0].roots;
	for (int i = 1; i < THREAD_NUM; ++i) {
		for (auto count : threads[i].roots) {
			result[count.first] += count.second;
		}
	}

	for (auto count : result)
		std::cout << count.first << ": " << count.second << std::endl;

	return 0;
}

/* requires opened raw_file */
void *thread_analyser(struct ThreadData *thread_data) {
	char ch;

	FILE *clean_file = fopen("tmp", "r");
	char *language = "english";
	char *charenc = "UTF-8";

	sb_stemmer *stemmer;
	stemmer = sb_stemmer_new(language, charenc);
	stem_file(stemmer, thread_data->raw_file, clean_file);

	sb_stemmer_delete(stemmer);

	std::string file;
	while ((ch = fgetc(clean_file)) != EOF) {
		file.push_back(ch);
	}

	std::vector<std::string> words = tokenize(file.c_str());
	for (auto root : words) {
		thread_data->roots[root] += 1;
	}
}

int readData(std::istream &source) {
	std::string s;
	int count = 0;
	while (std::getline(source, s)) {
		++count;
		data.push_back(s);
	}
	return count;
}
