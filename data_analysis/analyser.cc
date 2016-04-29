#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <locale.h>

#include "libstemmer.h"
#include "stemfile.h"
#include "tokenize.h"

#define THREAD_NUM 4

void *thread_analyser(struct ThreadData *thread_data);
int readData(std::istream &source);
void merge (struct ThreadData *thread_data, std::map<std::string, int> &result);


struct ThreadData {
	pthread_t thread;
	FILE *raw_file;
	std::map<std::string, int> roots;
};


int main(int argc, char *argv[])  {
	setlocale(LC_ALL, "");
	printf("Entered main. argc = %d, argv = %s\n", argc, argv);
	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "r");
	ThreadData threads[THREAD_NUM];

	for (int i = 0; i < THREAD_NUM; ++i) {
		std::string namefile = "in_";
		namefile.push_back('0' + i);
		threads[i].raw_file = fopen(namefile.c_str(), "w+");
		perror("open");
		printf("namefile = '%s'\n", namefile.c_str());
	}
	printf("Ended for.\n");

	char *buf;
	for (int i = 0; ; ++i) {
		printf("Entered for.\n");
		size_t size = 0;
		if (getline(&buf, &size, input) == -1) {
			printf("EOF.\n");
			break;
		} else {
			fprintf(threads[i % 4].raw_file, "%s\n", buf);
			std::cout << i << std::endl;
		}
	}
	
	for (int i = 0; i < THREAD_NUM; ++i) {
		printf("Before pthread_create.\n");
		fseek(threads[i].raw_file, 0, SEEK_SET);
		int check = pthread_create(&threads[i].thread, NULL, (void * (*)(void *)) thread_analyser, &threads[i]);
		if (check) {
			perror("pthread_create");
			exit(1);
		}
		std::cout << threads[i].thread << std::endl;
	}
	for (int i = 0; i < THREAD_NUM; ++i) {
		int check = pthread_join(threads[i].thread, NULL);
			if (check) {
			perror("pthread_join");
			exit(1);
		}
	}

	std::cout << "Ended working with threads." << std::endl;

	/*for (auto x : threads[0].roots) {
		std::cout << x.first << " : " << x.second << std:: endl;
	}

	printf("After dump\n");*/

	std::map<std::string, int> result;
	merge(threads, result);	

	FILE *output = fopen("output.txt", "w");
	//FILE *output1 = fopen("output1.txt", "w");

	/*for (auto x : threads[0].roots) {
		fprintf(output1, "%s : %d\n", x.first.c_str(), x.second);
	}*/

	for (auto x : result) {
		std::cout << x.first << " : " << x.second << std:: endl;
		fprintf(output, "%s : %d\n", x.first.c_str(), x.second);
	}

	printf("After dump\n");

	for (int i = 0; i < THREAD_NUM; ++i) {
		fclose(threads[i].raw_file);
	}
	fclose(output);

	return 0;
}

/* requires opened raw_file */
void *thread_analyser(struct ThreadData *thread_data) {
	//printf("Entered thread_analyser.\n");
	//char ch;

	//FILE *clean_file = fopen("tmp", "w+");
	//printf("Opened clean file tmp.\n");	
	const char *language = "english";
	const char *charenc = NULL;

	char *buf;
	size_t size = 0;
	getdelim(&buf, &size, -1, thread_data->raw_file);

	std::vector<std::string> words = tokenize(buf);

	sb_stemmer *stemmer;
	stemmer = sb_stemmer_new(language, charenc);
	printf("Created new stemmer.\n");
    printf("Got %d words\n", words.size());

	for (auto &word : words) {
		word = (const char *)sb_stemmer_stem(stemmer, (const sb_symbol *)word.c_str(), sizeof(word));
	}

	//stem_file(stemmer, thread_data->raw_file, clean_file); 
	printf("Executed sb_stemmer_stem.\n");
	sb_stemmer_delete(stemmer);
	printf("Deleted stemmer.\n");

	//fseek(clean_file, 0, SEEK_SET);

	/*std::string file;
	while ((ch = fgetc(clean_file)) != EOF) {
		file.push_back(ch);
	}*/

	for (auto root : words) {
		thread_data->roots[root] += 1;
	}

	//fclose(clean_file);
//	printf("End of thread_analyser\n");
}

void merge (struct ThreadData *thread_data, std::map<std::string, int> &result) {
	for (int i = 0; i < THREAD_NUM; ++i) {
		for (auto  x : thread_data[i].roots) {
			result[x.first] += x.second;
		}
	}
}