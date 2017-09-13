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

char * _Nullable * _Nullable loadSearchableKeys(size_t * _Nonnull number_keys) {
    
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
        keys[i] = (char *)malloc(256*sizeof(char ));
    }
    memset(*keys, 0, inputs*sizeof(char));
    
    char string[64];
    char value[64];
    size_t value_len;
    int lineCount = 1;
    int empty = 0;
    *number_keys = 0;
    while(1) {
        memset(string, 0, sizeof(string));
        memset(value, 0, sizeof(value));
        fscanf(f1,"%s\n", string);
        
        if (lineCount == 1 && string[0] != '{') {
            fatal(PROGRAM_NAME, "syntax error in the file for the input keys. File should start with <{>");
        } else if (lineCount == 1) {
            lineCount++;
            continue;
        } else if(string[0] == '\0') {
            empty++;
            if (empty > 1000) {
                fatal(PROGRAM_NAME, "syntax error in the file for the input keys. File should end with <}>");
            }
            continue;
        }
        
        if (string[0] == '!') continue; // Comment line
        if (string[0] == '}') break;    // End of file
        
        parseArgument(string, value, &value_len);
        stpncpy(keys[*number_keys], value, value_len);
        (*number_keys)++;
        lineCount++;
    }
    
    return keys;
}
