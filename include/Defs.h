//
//  Defs.h
//  WeatherSearch
//
//  Created by Seddik hakime on 12/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#ifndef Defs_h
#define Defs_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_KEY_VALUE_STRING 128
#define MAX_DIR_FILE_STRING 128

typedef struct dictionary {
    
    bool has_tag;
    char key[MAX_KEY_VALUE_STRING];
    char value[MAX_KEY_VALUE_STRING];
    char tag[1];
    struct dictionary * _Nullable next;
    struct dictionary * _Nullable previous;
    
} dictionary;

typedef struct record {
    
    int record_id;
    int number_key_values;
    char file[MAX_DIR_FILE_STRING];
    char directory[MAX_DIR_FILE_STRING];
    dictionary * _Nullable key_value;
    struct record * _Nullable next;
    struct record *_Nullable previous;
    
} record;

typedef struct directory_node {
    
    size_t number_files;
    record * _Nullable * _Nullable db;
    struct directory_node * _Nullable next;
    struct directory_node * _Nullable previous;
} directory_node;

record * _Nonnull allocateRecordNode(void);
dictionary * _Nonnull allocateDictionaryNode(void);
directory_node * _Nonnull allocateDirectoryNode(void);

#endif /* Defs_h */
