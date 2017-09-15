
//
//  main.c
//  WeatherSearch
//
//  Created by Seddik hakime on 07/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#include "CreateRecordDB.h"

#define MAX_VAL_LEN  1024

bool UPDATE_RECORDS = false;
char **entry_keys = NULL;
size_t number_keys;

directory_node *store = NULL;
record *queries = NULL;

void init(void) {
    
    entry_keys = getSearchableKeys(&number_keys);
    if (entry_keys == NULL) {
        fatal(PROGRAM_NAME, "problem loading the keys definition file.");
    }
    fprintf(stdout, "%s: number of keys used in the record DB: %d.\n", PROGRAM_NAME, (int)number_keys);
    for (int i=0; i<number_keys; i++) {
        char *str = &entry_keys[i][0];
        fprintf(stdout, "key: %s\n", str);
    }
    
    CreateRecordDB();
}

int main(int argc, const char * argv[]) {
    
    
    init();
    const char *keyword = "query";
    queries = getRecordsOrQueries(keyword);
    record *q_pt = queries;
    while (q_pt != NULL) {
        dictionary *kv_pt = q_pt->key_value;
        while (kv_pt != NULL) {
            printf("%s:%s.\n", kv_pt->key, kv_pt->value);
            kv_pt = kv_pt->next;
        }
        q_pt = q_pt->next;
    }


    free(entry_keys);
    free(store);
    //free(queries);
    return 0;
}
