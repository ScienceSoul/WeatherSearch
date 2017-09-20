//
//  Utils.h
//  WeatherSearch
//
//  Created by Seddik hakime on 12/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#ifndef Utils_h
#define Utils_h

#include <dirent.h>
#include "Defs.h"

#define PROGRAM_NAME "WeatherSearch"

void __attribute__((overloadable)) fatal(char head[_Nonnull]);
void __attribute__((overloadable)) fatal(char head[_Nonnull], char message[_Nonnull]);
void __attribute__((overloadable)) fatal(char head[_Nonnull], char message[_Nonnull], int n);
void __attribute__((overloadable)) fatal(char head[_Nonnull], char message[_Nonnull], double n);
void __attribute__((overloadable)) warning(char head[_Nonnull], char message[_Nonnull]);
void __attribute__((overloadable)) warning(char head[_Nonnull], char message[_Nonnull], int n);
void __attribute__((overloadable)) warning(char head[_Nonnull], char message[_Nonnull], double n);

void __attribute__((overloadable)) parseArgument(const char * _Nonnull argument, char * _Nonnull value, size_t * _Nonnull len);

char * _Nullable * _Nullable getSearchableKeys(size_t * _Nonnull number_keys);
record * _Nullable getQueries(const char * _Nonnull keyword);
void output_db(bool to_file, FILE * _Nullable file);

#endif /* Utils_h */
