
//
//  main.c
//  WeatherSearch
//
//  Created by Seddik hakime on 07/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#include "CreateRecordDB.h"
#include "Search.h"

bool UPDATE_RECORD_DB = false;
char **entry_keys = NULL;
size_t number_keys;
size_t number_queries = 0;

directory_node *store = NULL;
record *queries = NULL;

void init(void) {
    
    entry_keys = getSearchableKeys(&number_keys);
    if (entry_keys == NULL) {
        fatal(PROGRAM_NAME, "problem loading the keys definition file.");
    }
    
#ifdef VERBOSE
    fprintf(stdout, "%s: number of keys used in the record DB: %d.\n", PROGRAM_NAME, (int)number_keys);
    for (int i=0; i<number_keys; i++) {
        char *str = &entry_keys[i][0];
        fprintf(stdout, "key: %s\n", str);
    }
#endif
    
    DIR *dir = NULL;
    struct dirent *file = NULL;
    bool is_record_db = false;
    char *record_db_file = "records.db";
    
    dir = opendir("./params");
    if (!dir) {
        dir = opendir("../params");
        if (!dir) {
            fatal(PROGRAM_NAME, "directory params not found.");
        }
    }
    while ((file = readdir(dir)) != NULL) {
        if (strcmp(file->d_name, record_db_file) == 0) {
            is_record_db = true;
            break;
        }
    }
    
    if (!is_record_db || UPDATE_RECORD_DB) {
        fprintf(stdout, "Building the record DB....\n");
        CreateRecordDB();
        fprintf(stdout, "\n");
    } else {
        fprintf(stdout, "Load existing record DB....\n");
        ReadRecordDB();
        fprintf(stdout, "\n");
    }
}

void deallocate(void) {
    
    free(entry_keys);
    
    directory_node *d_pt = store;
    while (d_pt != NULL) {
        directory_node *pt = d_pt->next;
        for (int i=0; i<d_pt->number_files; i++) {
            record *r = d_pt->db[i];
            record *r_pt = r;
            while (r_pt != NULL) {
                record *pt = r_pt->next;
                dictionary *dic_pt = r_pt->key_value;
                while (dic_pt != NULL) {
                    dictionary *pt = dic_pt->next;
                    if (dic_pt->next != NULL) dic_pt->next = NULL;
                    if (dic_pt->previous != NULL) dic_pt->previous = NULL;
                    free(dic_pt);
                    dic_pt = pt;
                }
                if (r_pt->next != NULL) r_pt->next = NULL;
                if (r_pt->previous != NULL) r_pt->previous = NULL;
                free(r_pt);
                r_pt = pt;
            }
        }
        free(d_pt->db);
        
        if (d_pt->next != NULL) d_pt->next = NULL;
        if (d_pt->previous != NULL) d_pt->previous = NULL;
        free(d_pt);
        d_pt = pt;
    }
    
    record *r_pt = queries;
    while (r_pt != NULL) {
        record *pt = r_pt->next;
        dictionary *dic_pt = r_pt->key_value;
        while (dic_pt != NULL) {
            dictionary *pt = dic_pt->next;
            if (dic_pt->next != NULL) dic_pt->next = NULL;
            if (dic_pt->previous != NULL) dic_pt->previous = NULL;
            free(dic_pt);
            dic_pt = pt;
        }
        if (r_pt->next != NULL) r_pt->next = NULL;
        if (r_pt->previous != NULL) r_pt->previous = NULL;
        free(r_pt);
        r_pt = pt;
    }
}

int main(int argc, const char * argv[]) {
    
    if (argc > 1) {
        char *valid_argument ="-update_record_db";
        if (strcmp(argv[1],valid_argument) == 0) {
            UPDATE_RECORD_DB = true;
        } else {
            fatal(PROGRAM_NAME, "invalid argument. Give -update_record_db to rebuild the record DB.");
        }
    }
    
    init();
    
    // Get the queries
    const char *keyword = "query";
    queries = getQueries(keyword);
    
    // Do the search
    search();
    
    deallocate();
    return 0;
}
