# WeatherSearch

### Description:

Prototype program that returns a file or a set of files matching queries on weather information stored in GRIB files.

* Language: C
* Compiler: Clang/LLVM
* Platform: masOS/Linux 
* Required library(ies): GRIB-API

Compilation:

```
cd src
make depend
make
```

Run with:

```
./WeatherSearch
```

The program is written so that it can search files in a data set (eg. the NCEP products inventory) which match input queries. The program works as follows:
* It uses a set of keys to build a record data base from a data set. Those keys are GRIB file keys and are stored in params/keys.dat. Using those keys, the program will go through all the GRIB files in the data set in order to build the record data base. Each record contains the required keys and their values. This record data base is stored in params/records.db and need to be created only
once. However it should be recreated if the data set has changed.
* Queries are defined in a file (queries.txt) and are the inputs of the program. A query defines the search to be made in the data set by defining a key and its value. A tag (=, > or <) can also be added for certain keys to indicate a condition in the search (the tag = has the same effect as not giving any tag). For example, the following

    ```
    {
     query {
     name:Temperature
     typeOfLevel:isobaricInhPa
     level:1
     dataDate:20170910
     max:>275
     }
     query {
     name:Temperature
     typeOfLevel:heightAboveSea
     max:>300
     }
    }
    ```
    defines two queries. The first one will return all GRIB files in which the isobaric temperature at 100 Pa on 2017/09/10 has a maximum value higher than 275 K. The second query will return all GRIB files in which the height above sea temperature has a maximum value higher than 300 K irrespective of the data date and the height level.


Additional notes:

* The record data base can be rebuilt at any time by running the program as follows
    ```
    ./WeatherSearch -update_record_db
    ```
    A new record data base will be created automatically if it is not found.
* The program can be compiled to write more outputs to the console by uncommenting the -DVERBOSE preprocessor flag in src/Makefile.
