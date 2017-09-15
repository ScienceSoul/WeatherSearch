//
//  Utils.c
//  WeatherSearch
//
//  Created by Seddik hakime on 12/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#include "Utils.h"

static int formatType;
void format(char * _Nullable head, char * _Nullable message, int *iValue, double *dValue);

void __attribute__((overloadable)) fatal(char head[]) {
    
    formatType = 1;
    format(head, NULL, NULL, NULL);
}

void __attribute__((overloadable)) fatal(char head[], char message[]) {
    
    formatType = 2;
    format(head, message, NULL, NULL);
}

void __attribute__((overloadable)) fatal(char head[], char message[], int n) {
    
    formatType = 3;
    format(head, message, &n, NULL);
}

void __attribute__((overloadable)) fatal(char head[], char message[], double n) {
    
    formatType = 4;
    format(head, message, NULL, &n);
}

void __attribute__((overloadable)) warning(char head[], char message[])
{
    fprintf(stdout, "%s: %s\n", head, message);
}

void __attribute__((overloadable)) warning(char head[], char message[], int n)
{
    fprintf(stdout, "%s: %s %d\n", head, message, n);
}

void __attribute__((overloadable)) warning(char head[], char message[], double n)
{
    fprintf(stdout, "%s: %s %f\n", head, message, n);
}

void format(char * _Nullable head, char * _Nullable message, int *iValue, double *dValue) {
    
    fprintf(stderr, "##                    A FATAL ERROR occured                   ##\n");
    fprintf(stderr, "##        Please look at the error log for diagnostic         ##\n");
    fprintf(stderr, "\n");
    if (formatType == 1) {
        fprintf(stderr, "%s: Program will abort...\n", head);
    } else if (formatType == 2) {
        fprintf(stderr, "%s: %s\n", head, message);
    } else if (formatType == 3) {
        fprintf(stderr, "%s: %s %d\n", head, message, *iValue);
    } else if (formatType == 4) {
        fprintf(stderr, "%s: %s %f\n", head, message, *dValue);
    }
    if (formatType == 2 || formatType == 3 || formatType == 4)
        fprintf(stderr, "Program will abort...\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "################################################################\n");
    fprintf(stderr, "################################################################\n");
    exit(-1);
}

void __attribute__((overloadable)) parseArgument(const char * _Nonnull argument, char * _Nonnull value, size_t * _Nonnull len) {
    
    int idx_arg = 0;
    int idx_key = 0;
     *len = 0;
    
    size_t arg_len = strlen(argument);
    if (argument[0] != '<' || argument[arg_len-1] != '>') fatal(PROGRAM_NAME, "input argument for keys definition should start with < and end with >.");
    
    while (argument[idx_arg] != '>') {
        if (argument[idx_arg] == '<') {
            idx_arg++;
            continue;
        }
        value[idx_key] = argument[idx_arg];
        idx_arg++;
        idx_key++;
        (*len)++;
    }
}

char * _Nullable * _Nullable getSearchableKeys(size_t * _Nonnull number_keys) {
    
    // Very basic parsing of our input parameters file.
    // TODO: Needs to change that to something more flexible and with better input validation
    
    char **keys = NULL;
    
    FILE *f1 = fopen("keys.dat","r");
    if(!f1) {
        f1 = fopen("./params/keys.dat","r");
        if(!f1) {
            f1 = fopen("../params/keys.dat","r");
            if(!f1) {
                fprintf(stdout,"%s: can't find the file %s which defines the searchable keys.\n", PROGRAM_NAME, "keys.dat");
                return NULL;
            }
        }
    }
    
    // Figure out the number of lines in file
    int inputs = 0;
    char ch = 0;
    do {
        ch = fgetc(f1);
        if(ch == '\n'){
            inputs++;
        }
    }
    while (!feof(f1));
    rewind(f1);
    
    if (inputs <= 0) return NULL;
    keys = (char **)malloc(inputs*sizeof(char *));
    for (int i=0; i<inputs; i++) {
        keys[i] = (char *)malloc(MAX_KEY_VALUE_STRING*sizeof(char));
    }
    memset(*keys, 0, inputs*sizeof(char));
    
    char string[MAX_KEY_VALUE_STRING];
    char value[MAX_KEY_VALUE_STRING];
    size_t value_len;
    int lineCount = 1;
    int empty = 0;
    *number_keys = 0;
    while(1) {
        memset(string, 0, sizeof(string));
        memset(value, 0, sizeof(value));
        fscanf(f1,"%s\n", string);
        
        if (lineCount == 1 && string[0] != '{') {
            fatal(PROGRAM_NAME, "syntax error in the file for the input keys. File should start with <{>.");
        } else if (lineCount == 1) {
            lineCount++;
            continue;
        } else if(string[0] == '\0') {
            empty++;
            if (empty > 1000) {
                fatal(PROGRAM_NAME, "syntax error in the file for the input keys. File should end with <}>.");
            }
            continue;
        }
        
        if (string[0] == '!') continue; // Comment line
        if (string[0] == '}') break;    // End of file
        
        parseArgument(string, value, &value_len);
        if (value_len > MAX_KEY_VALUE_STRING) {
            fatal(PROGRAM_NAME, "string larger than buffer in getSearchableKeys().");
        }
        stpcpy(keys[*number_keys], value);
        (*number_keys)++;
        lineCount++;
    }
    
    return keys;
}

record * _Nullable getRecordsOrQueries(const char * _Nonnull keyword) {
    
    record *queries = NULL;
    
    char *search_input_file = "queries.txt";
    
    FILE *f1 = fopen(search_input_file,"r");
    if(!f1) {
        char str[MAX_DIR_FILE_STRING];
        stpcpy(str, "../");
        strcat(str, search_input_file);
        f1 = fopen(str,"r");
        if(!f1) {
            fprintf(stdout,"%s: can't find the queries file %s.\n", PROGRAM_NAME, search_input_file);
            return NULL;
        }
    }
    
    int lineCount = 1;
    bool first_character = true;
    char ch = 0;
    bool new_query;
    char buff[MAX_KEY_VALUE_STRING];
    int idx = 0;
    int total_key_values = 0;
    bool first_q_node = true;
    record *q_head = NULL, *q_pos = NULL, *q_pt = NULL;
    dictionary *kv_head = NULL, *kv_pos = NULL, *kv_pt = NULL;
    while(1) {
        ch = fgetc(f1);
        if (ch == -1) {
            fprintf(stderr, "%s: syntax error in the file %s. File should end with <}>.\n", PROGRAM_NAME, search_input_file);
            fatal(PROGRAM_NAME);
        }
        if (ch == ' ') continue;
        if (first_character && ch != '{') { // First character in file should be {
            fprintf(stderr, "%s: syntax error in the file %s. File should start with <{>.\n", PROGRAM_NAME, search_input_file);
            fatal(PROGRAM_NAME);
        } else if (first_character && ch == '{'){
            first_character = false;
            continue;
        }
        if (ch == '}') { // End of file
            break;
        }
        
        if(ch == '\n'){
            lineCount++;
        } else {
            if (idx > MAX_KEY_VALUE_STRING) {
                fatal(PROGRAM_NAME, "string larger than buffer in getQueries().");
            }
            buff[idx] = ch;
            idx++;
            if (ch == '{' && !first_character) {
                char str[MAX_KEY_VALUE_STRING];
                stpncpy(str, buff, 5);
                str[strlen(str)] = '\0';
                if (strcmp(str, keyword) != 0) {
                    fatal(PROGRAM_NAME, "incorrect keyword for query or record definition. Should be <query> or <record>.");
                }
                // New query definition starts here
                memset(buff, 0, sizeof(buff));
                idx = 0;
                if (first_q_node) {
                    q_head = allocateRecord();
                    queries = q_head;
                    q_pos = q_head;
                    q_pt = q_head;
                } else {
                    q_pt = allocateRecord();
                }
                new_query = true;
                bool field_line = false;
                bool first_kv_node = true;
                int found_key = 0;
                while(1) {
                    ch = fgetc(f1);
                    if (ch == ' ') continue;
                    if (ch == '}' && new_query) { // End of query
                        if (!first_q_node) {
                            q_pos->next = q_pt;
                            q_pt->previous = q_pos;
                            q_pos = q_pt;
                        }
                        total_key_values += q_pt->number_key_values;
                        new_query = false;
                        first_q_node = false;
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
                        q_pt->number_key_values++;
                        first_kv_node = false;
                        memset(buff, 0, sizeof(buff));
                        idx = 0;
                        lineCount++;
                        found_key = 0;
                    } else {
                        if (ch == ':') {
                            found_key++;
                            if (found_key > 1) {
                                fprintf(stderr, "%s: syntax error in the file %s. Maybe duplicate character <:>.\n", PROGRAM_NAME, search_input_file);
                                fatal(PROGRAM_NAME);
                            }
                            if (first_kv_node) {
                                kv_head = allocateDictionary();
                                q_pt->key_value = kv_head;
                                kv_pos = kv_head;
                                kv_pt = kv_head;
                            } else {
                                kv_pt = allocateDictionary();
                            }
                            memcpy(kv_pt->key, buff, idx);
                            memset(buff, 0, sizeof(buff));
                            idx = 0;
                        } else {
                            buff[idx] = ch;
                            idx++;
                            if (idx >= MAX_KEY_VALUE_STRING) {
                                fatal(PROGRAM_NAME, "string larger than buffer in getQueries().");
                            }
                        }
                    }
                }
            }
        }
    }
    if (total_key_values == 0) {
        fatal(PROGRAM_NAME, "nothing to search!");
    }
    
    return queries;
}
















