#pragma once

#include "libstemmer.h"

void stem_file(struct sb_stemmer * stemmer, FILE * f_in, FILE * f_out);
