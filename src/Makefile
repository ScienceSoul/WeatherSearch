#
# 'make depend' uses makedepend to automatically generate dependencies
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'FeedforwardNT'
# 'make clean'  removes all .o and executable files
#

CC = clang

# Compile flags
CFLAGS = -Wall -O3 #-DVERBOSE

INCLUDES = -I../include -I/Users/seddikhakime/Documents/WeatherSearch/gribapi/include

# Needs the GRIB API 
LFLAGS = -L/Users/seddikhakime/Documents/WeatherSearch/gribapi/lib -lgrib_api

LIBS = -lm

SRCS = main.c Defs.c CreateRecordDB.c Search.c Utils.c 

# define the C object files
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)

# define the executable file
MAIN = WeatherSearch

.PHONY: depend clean

all:    $(MAIN)
	@echo Program compiled

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

main.o: ../include/CreateRecordDB.h
main.o: /Users/seddikhakime/Documents/WeatherSearch/gribapi/include/grib_api.h
main.o: /Users/seddikhakime/Documents/WeatherSearch/gribapi/include/grib_api_windef.h
main.o: /Users/seddikhakime/Documents/WeatherSearch/gribapi/include/grib_api_version.h
main.o: ../include/Utils.h ../include/Defs.h ../include/Search.h
Defs.o: ../include/Defs.h
CreateRecordDB.o: ../include/CreateRecordDB.h
CreateRecordDB.o: /Users/seddikhakime/Documents/WeatherSearch/gribapi/include/grib_api.h
CreateRecordDB.o: /Users/seddikhakime/Documents/WeatherSearch/gribapi/include/grib_api_windef.h
CreateRecordDB.o: /Users/seddikhakime/Documents/WeatherSearch/gribapi/include/grib_api_version.h
CreateRecordDB.o: ../include/Utils.h ../include/Defs.h
Search.o: ../include/Search.h ../include/Defs.h
Utils.o: ../include/Utils.h ../include/Defs.h
