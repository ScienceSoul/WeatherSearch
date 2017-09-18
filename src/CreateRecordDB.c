//
//  CreateRecordDB.c
//  WeatherSearch
//
//  Created by Seddik hakime on 14/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#include "CreateRecordDB.h"

bool isValidFile(char *name) {
    bool valid = false;
    for (int i=0; i<strlen(name); i++) {
        if (name[i] == '.') {
            if((name[i+1] == 't' && name[i+2] == 'x' && name[i+3] == 't') ||
               (name[i+1] == 'g' && name[i+2] == 'r' && name[i+3] == 'd')) {
                valid = true;
            }
        }
    }
    return valid;
}

void CreateRecordDB(void) {
    
    extern directory_node *store;
    extern char **entry_keys;
    extern size_t number_keys;
    
    int err = 0;
    
    DIR *dir = NULL;
    FILE *in = NULL;
    char dirname[MAX_DIR_FILE_STRING];
    char filename[MAX_DIR_FILE_STRING];
    char key_value[MAX_KEY_VALUE_STRING];
    
    grib_handle *h = NULL;
    struct dirent *file = NULL;
    
    size_t number_namespaces = 5;
    
    // To skip read only and not coded keys
    // unsigned long key_iterator_filter_flags=GRIB_KEYS_ITERATOR_SKIP_READ_ONLY |
    // GRIB_KEYS_ITERATOR_SKIP_COMPUTED
    unsigned long key_iterator_filter_flags=GRIB_KEYS_ITERATOR_ALL_KEYS;
    
    // Get the information on where the data are located
    FILE *data_directories = fopen("data_directories.dat","r");
    if(!data_directories) {
        data_directories = fopen("./params/data_directories.dat","r");
        if(!data_directories) {
            data_directories = fopen("../params/data_directories.dat","r");
            if(!data_directories) {
                fprintf(stdout,"%s: can't find the file %s which defines the data directories.\n", PROGRAM_NAME, "data_directories.dat");
                fatal(PROGRAM_NAME);
            }
        }
    }
    
    char data_directory_name[MAX_DIR_FILE_STRING];
    size_t number_files = 0;
    bool first_dir_node = true;
    directory_node *d_head = NULL, *d_pos = NULL, *d_pt = NULL;
    do {
        fscanf(data_directories,"%s\n", data_directory_name);
        
        char path_to_directory[MAX_DIR_FILE_STRING];
        memset(path_to_directory, 0, sizeof(path_to_directory));
        stpcpy(path_to_directory, "./data/");
        if (strlen(path_to_directory)+strlen(data_directory_name) > MAX_DIR_FILE_STRING) {
            fatal(PROGRAM_NAME, "string larger than buffer in CreateRecordDB().");
        }
        strcat(path_to_directory, data_directory_name);
        dir = opendir(path_to_directory);
        if (!dir) {
            stpcpy(path_to_directory, "../data/");
            if (strlen(path_to_directory)+strlen(data_directory_name) > MAX_DIR_FILE_STRING) {
                fatal(PROGRAM_NAME, "string larger than buffer in CreateRecordDB().");
            }
            strcat(path_to_directory, data_directory_name);
            dir = opendir(path_to_directory);
            if (!dir) {
                fprintf(stderr, "%s: can't not find the data directory %s.\n", PROGRAM_NAME, path_to_directory);
                fatal(PROGRAM_NAME);
            } else {
                stpcpy(dirname, path_to_directory);
            }
        } else {
            stpcpy(dirname, path_to_directory);
        }
        
        fprintf(stdout, "%s: going to directory %s.\n", PROGRAM_NAME, dirname);
        
        // Allocate directory node
        if (first_dir_node) {
            d_head = allocateDirectoryNode();
            store = d_head;
            d_pos = d_head;
            d_pt = d_head;
            first_dir_node = false;
        } else {
            d_pt = allocateDirectoryNode();
        }
        
        number_files = 0;
        while ((file = readdir(dir)) != NULL) {
            if (file->d_name[0] == '.') {
                continue;
            }
            if (!isValidFile(file->d_name)) {
                continue;
            }
            number_files++;
        }
        fprintf(stdout, "%s: %d file(s) found in data directory.\n", PROGRAM_NAME, (int)number_files);
        
        // Allocate memory for the data base of records in current directory node
        d_pt->db = (record **)malloc(number_files*sizeof(record *));
        d_pt->number_files = number_files;
        
        rewinddir(dir);
        int idx = 0;
        while ((file = readdir(dir)) != NULL) {
            if (file->d_name[0] == '.') {
                continue;
            }
            if (!isValidFile(file->d_name)) {
                continue;
            }
            memset(filename, 0, sizeof(filename));
            stpcpy(filename, dirname);
            if (strlen(filename)+strlen("/") > MAX_DIR_FILE_STRING) {
                fatal(PROGRAM_NAME, "string larger than buffer in CreateRecordDB().");
            }
            strcat(filename, "/");
            if (strlen(filename)+strlen(file->d_name) > MAX_DIR_FILE_STRING) {
                fatal(PROGRAM_NAME, "string larger than buffer in CreateRecordDB().");
            }
            strcat(filename, file->d_name);
            in = fopen(filename, "r");
            if (!in) {
                fprintf(stderr, "%s: can't open the file %s.\n", PROGRAM_NAME, filename);
                fatal(PROGRAM_NAME);
            } else {
                fprintf(stdout, "%s: opening GRIB file: %s.\n", PROGRAM_NAME, filename);
            }
            
            // Loop through all handles of GRIB messages
            int grib_count = 0;
            record *r_head = NULL, *r_pos= NULL, *rec_pt = NULL;
            dictionary *dict_pt = NULL;
            bool first_record = true;
            while ((h = grib_handle_new_from_file(0, in, &err)) != NULL) {
                
                if(!h) {
                    fatal(PROGRAM_NAME, "unable to create grib handle.");
                }
                
                if (first_record) {
                    r_head = allocateRecord();
                    r_head->record_id = grib_count;
                    stpcpy(r_head->file, filename);
                    stpcpy(r_head->directory, data_directory_name);
                    
                    r_pos = r_head;
                    rec_pt = r_head;
                    first_record = false;
                } else {
                    rec_pt = allocateRecord();
                    rec_pt->record_id = grib_count;;
                    stpcpy(rec_pt->file, filename);
                    stpcpy(rec_pt->directory, data_directory_name);
                }
                
                for (int i=0; i<number_keys; i++) {
                    bool found = false;
                    char *key = &entry_keys[i][0];
                    // Loop over all namespaces
                    for (int j=0; j<number_namespaces; j++) {
                        char* name_space;
                        // Namespaces searched: "ls", "time", "parameter", "geography", "statistics"
                        if (j == 0) {
                            name_space = "ls";
                        } else if (j == 1) {
                            name_space = "time";
                        } else if (j == 2) {
                            name_space = "parameter";
                        } else if (j == 3) {
                            name_space = "geography";
                        } else {
                            name_space = "statistics";
                        }
                        
                        grib_keys_iterator *kiter = NULL;
                        kiter = grib_keys_iterator_new(h, key_iterator_filter_flags, name_space);
                        if (!kiter) {
                            fatal(PROGRAM_NAME, "unable to create keys iterator.");
                        }
                        
                        while (grib_keys_iterator_next(kiter)) {
                            const char *name = grib_keys_iterator_get_name(kiter);
                            if (strcmp(name, key) == 0) {
                                size_t vlen = MAX_KEY_VALUE_STRING;
                                bzero(key_value, vlen);
                                GRIB_CHECK(grib_get_string(h, name, key_value, &vlen), name);
                                
                                if (rec_pt->number_key_values == 0) {
                                    rec_pt->key_value = allocateDictionary();
                                    stpcpy(rec_pt->key_value->key, key);
                                    if (strlen(key_value) > MAX_KEY_VALUE_STRING) {
                                        fatal(PROGRAM_NAME, "string larger than buffer in CreateRecordDB().");
                                    }
                                    stpcpy(rec_pt->key_value->value, key_value);
                                    dict_pt = rec_pt->key_value;
                                    rec_pt->number_key_values++;
                                } else {
                                    dictionary *new_key_value = allocateDictionary();
                                    stpcpy(new_key_value->key, key);
                                    if (strlen(key_value) > MAX_KEY_VALUE_STRING) {
                                        fatal(PROGRAM_NAME, "string larger than buffer in CreateRecordDB().");
                                    }
                                    stpcpy(new_key_value->value, key_value);
                                    new_key_value->previous = dict_pt;
                                    dict_pt->next = new_key_value;
                                    dict_pt = new_key_value;
                                    rec_pt->number_key_values++;
                                }
                                found = true;
                                break;
                            }
                        }
                        
                        if (found) {
                            grib_keys_iterator_delete(kiter);
                            break;
                        }
                        grib_keys_iterator_delete(kiter);
                    }
                }
                
                if (!first_record) {
                    r_pos->next = rec_pt;
                    rec_pt->previous = r_pos;
                    r_pos = rec_pt;
                }
                grib_handle_delete(h);
                grib_count++;
            }
            d_pt->db[idx] = r_head;
            idx++;
            fclose(in);
        }
        
        if (dir) closedir(dir);
        
        if (!first_dir_node) {
            d_pos->next = d_pt;
            d_pt->previous = d_pos;
            d_pos = d_pt;
        }
        
    } while (!feof(data_directories));
    
    
    d_pt = store;
    while (d_pt != NULL) {
        for (int i=0; i<d_pt->number_files; i++) { // Loop through files in directory
            record *r = d_pt->db[i];
            record *r_pt = r;
            while (r_pt != NULL) { // Loop through records in file
                printf("record number: %d from file: %s.\n", r_pt->record_id, r_pt->file);
                dictionary *dic_pt = r_pt->key_value;
                while (dic_pt != NULL) { // Loop through keys/values in record
                    printf("key: %s value:%s.\n", dic_pt->key, dic_pt->value);
                    dic_pt = dic_pt->next;
                }
                r_pt = r_pt->next;
            }
        }
        d_pt = d_pt->next;
    }
    
    // Loop through handles of messages in a file and
    // display keys for each
    //    int grib_count = 0;
    //    while ((h = grib_handle_new_from_file(0, in, &err)) != NULL) {
    //        grib_count++;
    //        printf("-- GRIB N. %d --\n",grib_count);
    //        if(!h) {
    //            printf("ERROR: Unable to create grib handle\n");
    //            exit(1);
    //        }
    //
    //        // Get variable name associated to message
    //        size_t vlen = MAX_VAL_LEN;
    //        GRIB_CHECK(grib_get_string(h, "name", message_name, &vlen), 0);
    //        printf("Variable name: %s\n", message_name);
    //
    //        grib_keys_iterator *kiter = NULL;
    //        kiter = grib_keys_iterator_new(h, key_iterator_filter_flags, name_space);
    //        if (!kiter) {
    //            printf("Error: unable to create keys iterator.\n");
    //        }
    //
    //        while (grib_keys_iterator_next(kiter)) {
    //            const char *name = grib_keys_iterator_get_name(kiter);
    //            vlen = MAX_VAL_LEN;
    //            bzero(key_value, vlen);
    //            GRIB_CHECK(grib_get_string(h, name, key_value, &vlen), name);
    //            printf("%s = %s\n", name, key_value);
    //        }
    //
    //        grib_keys_iterator_delete(kiter);
    //        grib_handle_delete(h);
    //    }
    //
    //    rewind(in);
    //    h = grib_handle_new_from_file(0, in, &err);
    //    if (h == NULL) {
    //        printf("Error: unable to create handle from file %s\n.", filename);
    //        exit(-1);
    //    }
    //
    //    /* get the size of the values array*/
    //    GRIB_CHECK(grib_get_size(h,"values",&values_len),0);
    //
    //    values = (double *)malloc(values_len * sizeof(double));
    //
    //    // Get variable name associated to message
    //    size_t vlen = MAX_VAL_LEN;
    //    char var_name[MAX_VAL_LEN];
    //    GRIB_CHECK(grib_get_string(h, "name", var_name, &vlen), 0);
    //    printf("Values for variable %s: \n", var_name);
    //
    //    // Get the size of the values array
    //    GRIB_CHECK(grib_get_double_array(h, "values", values, &values_len), 0);
    //
    //    for (int i=0; i<values_len; i++) {
    //        printf("%d %.10e\n", i+1, values[i]);
    //    }
    //
    //    free(values);
    //
    //    GRIB_CHECK(grib_get_double(h, "max", &max), 0);
    //    GRIB_CHECK(grib_get_double(h, "min", &min), 0);
    //    GRIB_CHECK(grib_get_double(h, "average", &average), 0);
    //    
    //    printf("%d values found in %s\n.", (int)values_len, filename);
    //    printf("max=%.10e min=%.10e average=%.10e\n.", max, min, average);
    //    
    //    grib_handle_delete(h);
    //    
    //    fclose(in);
    
}
