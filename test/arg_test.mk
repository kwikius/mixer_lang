

ifeq ($(USE_CONCEPTS),True)
CXX         = g++-6
CXXFLAGS    = -std=c++14 -fconcepts 
else
CXX         ?= /usr/local/bin/g++
CXXFLAGS    ?= -std=c++11 
endif

TARGET = arg_test.exe

mixer_lang_lib = ../build/mixer_lang.a

local_objects = arg_test.o
local_files =  $(patsubst %.o,%.cpp,$(local_objects))

OBJECTS = $(local_objects)

PHONY : all test clean

all : $(TARGET)

$(TARGET) : $(OBJECTS) $(mixer_lang_lib)
	$(CXX) -o $(TARGET) $(CXXFLAGS) $(OBJECTS) $(mixer_lang_lib)

$(local_objects): %.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(mixer_lang_lib):
	make -C ../src/
 
.PHONY : clean
.PHONY : test

test : $(TARGET)
	./$(TARGET)

clean :
	rm -f *.o
	rm -f $(TARGET)
