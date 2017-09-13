
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

record **db = NULL;

void init(void) {
    
    entry_keys = loadSearchableKeys(&number_keys);
    if (entry_keys == NULL) {
        fatal(PROGRAM_NAME, "problem loadind the keys definition file.");
    }
    fprintf(stdout, "%s: number of keys used to build the data records: %d.\n", PROGRAM_NAME, (int)number_keys);
    for (int i=0; i<number_keys; i++) {
        char *str = &entry_keys[i][0];
        printf("key: %s\n", str);
    }
    
    CreateRecordDB();
}

int main(int argc, const char * argv[]) {
    
    
    init();

    free(entry_keys);
    free(db);
    return 0;
}
