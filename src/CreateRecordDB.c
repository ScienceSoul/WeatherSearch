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

//
// Create a record DB
//
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
#ifdef VERBOSE
        fprintf(stdout, "%s: going to directory %s.\n", PROGRAM_NAME, dirname);
#endif
        
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
#ifdef VERBOSE
        fprintf(stdout, "%s: %d file(s) found in data directory.\n", PROGRAM_NAME, (int)number_files);
#endif
        
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
            }
#ifdef VERBOSE
            fprintf(stdout, "%s: opening GRIB file: %s.\n", PROGRAM_NAME, filename);
#endif
            
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
                    r_head = allocateRecordNode();
                    r_head->record_id = grib_count;
                    stpcpy(r_head->file, filename);
                    stpcpy(r_head->directory, data_directory_name);
                    
                    r_pos = r_head;
                    rec_pt = r_head;
                    first_record = false;
                } else {
                    rec_pt = allocateRecordNode();
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
                                    rec_pt->key_value = allocateDictionaryNode();
                                    stpcpy(rec_pt->key_value->key, key);
                                    if (strlen(key_value) > MAX_KEY_VALUE_STRING) {
                                        fatal(PROGRAM_NAME, "string larger than buffer in CreateRecordDB().");
                                    }
                                    stpcpy(rec_pt->key_value->value, key_value);
                                    dict_pt = rec_pt->key_value;
                                    rec_pt->number_key_values++;
                                } else {
                                    dictionary *new_key_value = allocateDictionaryNode();
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
    
    FILE *out = fopen("./params/records.db","w");
    if (!out) {
        out = fopen("../params/records.db", "w");
        if (!out) {
            fatal(PROGRAM_NAME, "error when creating the records DB file.");
        }
    }
    output_db(true, out);
#ifdef VERBOSE
    output_db(false, NULL);
#endif
}

//
// Read an existing record DB
//
void ReadRecordDB(void) {
    
    extern directory_node *store;
    
    char *record_db_file = "records.db";
    char *keyword = "record";
    
    FILE *f1 = fopen("./params/records.db","r");
    if(!f1) {
        f1 = fopen("../params/records.db","r");
    }
    
    int lineCount = 1;
    int idx = 0;
    int indx_file = 0;
    char ch = 0;
    char str[MAX_KEY_VALUE_STRING];
    char buff[MAX_KEY_VALUE_STRING];
    char meta[MAX_KEY_VALUE_STRING];
    char id[MAX_KEY_VALUE_STRING];
    char directory[MAX_KEY_VALUE_STRING];
    char file[MAX_KEY_VALUE_STRING];
    char nb_file[MAX_KEY_VALUE_STRING];
    char prev_directory[MAX_KEY_VALUE_STRING];
    char prev_file[MAX_KEY_VALUE_STRING];
    bool first_character = true;
    bool new_query;
    bool first_r_node = true;
    bool first_d_node = true;
    directory_node *d_head = NULL, *d_pos = NULL, *d_pt = NULL;
    record *r_head = NULL, *r_pos = NULL, *r_pt = NULL;
    dictionary *kv_head = NULL, *kv_pos = NULL, *kv_pt = NULL;
    
    d_head = allocateDirectoryNode();
    store = d_head;
    d_pos = d_head;
    while(1) {
        ch = fgetc(f1);
        if (ch == -1) {
            fprintf(stderr, "%s: syntax error in the file %s. File should end with <}>.\n", PROGRAM_NAME, record_db_file);
            fatal(PROGRAM_NAME);
        }
        if (ch == ' ') continue;
        if (first_character && ch != '{') { // First character in file should be {
            fprintf(stderr, "%s: syntax error in the file %s. File should start with <{>.\n", PROGRAM_NAME, record_db_file);
            fatal(PROGRAM_NAME);
        } else if (first_character && ch == '{'){
            first_character = false;
            continue;
        }
        if (ch == '}') { // End of file,
            // but don't forget to add the last records for the very last file
            d_pt->db[indx_file] = r_head;
            break;
        }
        
        if(ch == '\n'){
            lineCount++;
        } else {
            if (idx > MAX_KEY_VALUE_STRING) {
                fatal(PROGRAM_NAME, "string larger than buffer in ReadRecordDB().");
            }
            buff[idx] = ch;
            idx++;
            if (ch == '(') { // The meta data section
                int k = 0;
                int l = 0;
                memset(meta, 0, sizeof(meta));
                memset(id, 0, sizeof(id));
                memset(directory, 0, sizeof(directory));
                memset(file, 0, sizeof(file));
                memset(nb_file, 0, sizeof(nb_file));
                while (1) {
                    ch = fgetc(f1);
                    if (ch == ')') {
                        memcpy(nb_file, meta, k);
                        break;
                    };
                    if (ch == ',') {
                        if (l == 0) memcpy(id, meta, k);
                        if (l == 1) memcpy(directory, meta, k);
                        if (l == 2) memcpy(file, meta, k);
                        memset(meta, 0, sizeof(meta));
                        k = 0;
                        if (l > 2) {
                            fprintf(stderr, "%s: syntax error in the file %s. Too much commas in record metadata definition.\n", PROGRAM_NAME, record_db_file);
                            fatal(PROGRAM_NAME);
                        }
                        l++;
                    } else {
                        meta[k] = ch;
                        k++;
                    }
                }
            }
            if (ch == '{' && !first_character) {
                memset(str, 0, sizeof(str));
                memcpy(str, buff, 6);
                if (strcmp(str, keyword) != 0) {
                    fatal(PROGRAM_NAME, "incorrect keyword for record definition. Should be <record>.");
                }
                // New record definition starts here
                memset(buff, 0, sizeof(buff));
                idx = 0;
                if (first_r_node) {
                    r_head = allocateRecordNode();
                    r_head->record_id = atoi(id);
                    stpcpy(r_head->directory, directory);
                    stpcpy(r_head->file, file);
                    r_pos = r_head;
                    r_pt = r_head;
                    
                    stpcpy(prev_directory, directory);
                    stpcpy(prev_file, file);
                } else {
                    r_pt = allocateRecordNode();
                    r_pt->record_id = atoi(id);
                    stpcpy(r_pt->directory, directory);
                    stpcpy(r_pt->file, file);
                }
                new_query = true;
                bool field_line = false;
                bool first_kv_node = true;
                int found_key = 0;
                while(1) {
                    ch = fgetc(f1);
                    if (ch == ' ') continue;
                    if (ch == '}' && new_query) { // End of record
                        if (!first_r_node) {
                            if (strcmp(prev_file, file) != 0 && strcmp(prev_directory, directory) == 0) { // End of records for a given file, go to next file
                                if (first_d_node) {
                                    d_pos->db = (record **)malloc(atoi(nb_file)*sizeof(record *));
                                    d_pos->number_files = atoi(nb_file);
                                    d_pt = d_pos;
                                    first_d_node = false;
                                }
                                d_pt->db[indx_file] = r_head;
                                r_head = r_pt;
                                r_pos = r_pt;
                                stpcpy(prev_file, file);
                                indx_file++;
                            } else if (strcmp(prev_directory, directory) != 0) { // Go to next directory
                                d_pt->db[indx_file] = r_head;
                                r_head = r_pt;
                                r_pos = r_pt;
                                
                                d_pt = allocateDirectoryNode();
                                d_pt->number_files = atoi(nb_file);
                                d_pos->next = d_pt;
                                d_pt->previous = d_pos;
                                d_pt->db = (record **)malloc(atoi(nb_file)*sizeof(record *));
                                d_pos = d_pt;
                                stpcpy(prev_file, file);
                                stpcpy(prev_directory, directory);
                                indx_file = 0;
                            } else {
                                r_pos->next = r_pt;
                                r_pt->previous = r_pos;
                                r_pos = r_pt;
                            }
                        }
                        new_query = false;
                        first_r_node = false;
                        break;
                    }
                    if(ch == '\n'){
                        if (!field_line) {
                            lineCount++;
                            field_line = true;
                            continue;
                        }
                        memcpy(kv_pt->value, buff, idx);
                        if (!first_kv_node) {
                            kv_pos->next = kv_pt;
                            kv_pt->previous = kv_pos;
                            kv_pos = kv_pt;
                        }
                        r_pt->number_key_values++;
                        first_kv_node = false;
                        memset(buff, 0, sizeof(buff));
                        idx = 0;
                        lineCount++;
                        found_key = 0;
                    } else {
                        if (ch == ':') {
                            found_key++;
                            if (found_key > 1) {
                                fprintf(stderr, "%s: syntax error in the file %s. Maybe duplicate character <:>.\n", PROGRAM_NAME, record_db_file);
                                fatal(PROGRAM_NAME);
                            }
                            if (first_kv_node) {
                                kv_head = allocateDictionaryNode();
                                r_pt->key_value = kv_head;
                                kv_pos = kv_head;
                                kv_pt = kv_head;
                            } else {
                                kv_pt = allocateDictionaryNode();
                            }
                            memcpy(kv_pt->key, buff, idx);
                            memset(buff, 0, sizeof(buff));
                            idx = 0;
                        } else {
                            buff[idx] = ch;
                            idx++;
                            if (idx >= MAX_KEY_VALUE_STRING) {
                                fatal(PROGRAM_NAME, "string larger than buffer in ReadRecordDB().");
                            }
                        }
                    }
                }
            }
        }
    }
#ifdef VERBOSE
    output_db(false, NULL);
#endif
}











































