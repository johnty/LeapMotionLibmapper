OS := $(shell uname)
ARCH := $(shell uname -m)

ifeq ($(OS), Linux)
  ifeq ($(ARCH), x86_64)
    LEAP_LIBRARY := ../lib/x64/libLeap.so -Wl,-rpath,../lib/x64
  else
    LEAP_LIBRARY := ../lib/x86/libLeap.so -Wl,-rpath,../lib/x86
  endif
else
  # OS X
  LEAP_LIBRARY := ../lib/libLeap.dylib
endif

LIBMAPPER_INC := /usr/local/include/mapper-0/
MAPPER_LIBRARY := /usr/local/lib/libmapper-0.dylib
LIBMAPPER_SRC := /Users/johnty/Documents/libmapper-related/libmapper/include

all:	Sample LeapLibmapper testsend

Sample: Sample.cpp
	$(CXX) -Wall -g -I../include Sample.cpp -o Sample $(LEAP_LIBRARY)

LeapLibmapper: LeapLibmapper.cpp
	$(CXX) -Wall -g -O -std=c++11 -I../include -I$(LIBMAPPER_INC) LeapLibmapper.cpp -o LeapLibmapper $(LEAP_LIBRARY) $(MAPPER_LIBRARY)

testsend: testsend.c
	$(CC) -Wall -g -O -I$(LIBMAPPER_INC) testsend.c -o testsend $(MAPPER_LIBRARY)

testsend: testrecv.c
	$(CC) -Wall -g -O -I$(LIBMAPPER_INC) testrecv.c -o testrecv $(MAPPER_LIBRARY)

ifeq ($(OS), Darwin)
	install_name_tool -change @loader_path/libLeap.dylib ../lib/libLeap.dylib Sample
	install_name_tool -change @loader_path/libLeap.dylib ../lib/libLeap.dylib LeapLibmapper
endif

clean:
	rm -rf Sample Sample.dSYM
	rm -rf LeapLibmapper LeapLibmapper.dSYM
	rm -rf testsend testsend.dSYM
	mr -rf testrecv testrecv.dSYM

