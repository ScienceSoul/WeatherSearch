//
//  Defs.c
//  WeatherSearch
//
//  Created by Seddik hakime on 14/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "Defs.h"

record * _Nonnull allocateRecordNode(void) {
    
    record *r = (record *)malloc(sizeof(record));
    *r = (record){.record_id=0, .number_key_values=0, .key_value=NULL, .next=NULL, .previous=NULL};
    bzero(r->file, MAX_DIR_FILE_STRING);
    bzero(r->directory, MAX_DIR_FILE_STRING);
    return r;
}

dictionary * _Nonnull allocateDictionaryNode(void) {
    
    dictionary *d = (dictionary *)malloc(sizeof(dictionary));
    *d = (dictionary){.has_tag=false, .next=NULL, .previous=NULL};
    bzero(d->key, MAX_KEY_VALUE_STRING);
    bzero(d->value, MAX_KEY_VALUE_STRING);
    bzero(d->tag, 1);
    return d;
}

directory_node * _Nonnull allocateDirectoryNode(void) {
    
    directory_node *n = (directory_node *)malloc(sizeof(directory_node));
    *n = (directory_node){.number_files=0, .db=NULL, .next=NULL, .previous=NULL};
    return n;
}
