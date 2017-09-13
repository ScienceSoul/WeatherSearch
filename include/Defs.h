//
//  Defs.h
//  WeatherSearch
//
//  Created by Seddik hakime on 12/09/2017.
//  Copyright © 2017 ScienceSoul. All rights reserved.
//

#ifndef Defs_h
#define Defs_h


#endif /* Defs_h */

#define MAX_VAL_LEN  1024

typedef struct dictionary {
    
    char key[MAX_VAL_LEN];
    char value[MAX_VAL_LEN];
    struct dictionary * _Nullable next;
    struct dictionary * _Nullable previous;
    
} dictionary;

typedef struct record {
    
    int record_id;
    int number_key_values;
    char file[128];
    dictionary * _Nullable key_value;
    struct record * _Nullable next;
    struct record *_Nullable previous;
    
} record;
