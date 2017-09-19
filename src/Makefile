#
# 'make depend' uses makedepend to automatically generate dependencies
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'FeedforwardNT'
# 'make clean'  removes all .o and executable files
#

CC = clang

# Compile flags
CFLAGS = -Wall -O3

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

