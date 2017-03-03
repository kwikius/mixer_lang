

ifeq ($(USE_CONCEPTS),True)
CXX         = g++-6
CXXFLAGS    = -std=c++14 -fconcepts 
else
CXX         ?= /usr/local/bin/g++
CXXFLAGS    ?= -std=c++11 
endif

TARGET = arg_test.exe

local_objects = arg_test.o
local_files =  $(patsubst %.o,%.cpp,$(local_objects))

parent_objects = function.o arg_list.o
parent_files =  $(patsubst %.o,../src/%.cpp,$(parent_objects))

OBJECTS = $(local_objects) $(parent_objects)

PHONY : all test clean

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CXX) -o $(TARGET) $(CXXFLAGS) $(OBJECTS)

$(local_objects): %.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(parent_objects) : %.o : ../src/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@
 
.PHONY : clean
.PHONY : test

test : $(TARGET)
	./$(TARGET)

clean :
	rm -f *.o
	rm -f $(TARGET)
