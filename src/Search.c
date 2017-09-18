//
//  Search.c
//  WeatherSearch
//
//  Created by Seddik hakime on 18/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#include "Search.h"

bool all(bool array[], size_t n) {
    
    bool all = true;
    for (int i=0; i<n; i++) {
        if (!array[i]) {
            all = false;
            break;
        }
    }
    return all;
}

void search(void) {
    
    extern directory_node *store;
    extern record *queries;
    extern size_t number_queries;
    
    record **queries_results;
    queries_results = (record **)malloc(number_queries*sizeof(record *));
    for (int i=0; i<number_queries; i++) {
        queries_results[i] = NULL;
    }
    
    record *q_pt = queries;
    directory_node *d_pt = NULL;
    dictionary *query_dic_pt = NULL, *record_dic_pt = NULL;
    record *match_pt, *match_head;
    int k = 0;
    while (q_pt != NULL) { // Loop through queries
        bool first_match = true;
        bool found[q_pt->number_key_values];
        d_pt = store;
        
        while (d_pt != NULL) {
            match_pt = NULL;
            match_head = NULL;
            for (int i=0; i<d_pt->number_files; i++) { // Loop through files in directory
                record *r = d_pt->db[i];
                record *r_pt = r;
                while (r_pt != NULL) {  // Loop through records in file
                    memset(found,0,sizeof(found));
                    query_dic_pt = q_pt->key_value;
                    for (int j=0; j<q_pt->number_key_values; j++) {
                        record_dic_pt = r_pt->key_value;
                        while (record_dic_pt != NULL) {
                            if (strcmp(record_dic_pt->key, query_dic_pt->key) == 0) {
                                if (strcmp(record_dic_pt->value, query_dic_pt->value) == 0) {
                                    found[j] = true;
                                    break;
                                }
                            }
                            record_dic_pt = record_dic_pt->next;
                        }
                        query_dic_pt = query_dic_pt->next;
                    }
                    if (all(found, q_pt->number_key_values)) { // Add the record to the list of found records matching the query
                        if (first_match) {
                            record *match = allocateRecord();
                            stpcpy(match->file, r_pt->file);
                            stpcpy(match->directory, r_pt->directory);
                            queries_results[k] = match;
                            match_head = match;
                            k++;
                            first_match = false;
                        } else {
                            match_pt = allocateRecord();
                            stpcpy(match_pt->file, r_pt->file);
                            stpcpy(match_pt->directory, r_pt->directory);
                            match_head->next = match_pt;
                            match_pt->previous = match_head;
                            match_head = match_pt;
                        }
                    }
                    r_pt = r_pt->next;
                }
                
            }
            d_pt = d_pt->next;
        }
        q_pt = q_pt->next;
    }
    
    for (int i=0; i<number_queries; i++) {
        record *r = queries_results[i];
        record *r_pt = r;
        while (r_pt != NULL) {
            printf("File: %s.\n", r_pt->file);
            r_pt = r_pt->next;
        }
    }
    printf("here....\n");
}