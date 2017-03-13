

CXX ?= g++


INCLUDES = -I../../include/

CFLAGS = -std=gnu++11 -Wall -Os 

TARGET = bin/taranis_cstr_mixer.exe

local_objects = main.o key_was_pressed.o joystick.o

objects = $(patsubst %.o,obj/%.o,$(local_objects))

mixer_lang_lib = ../../build/mixer_lang.a
 
.PHONY: clean all

all: $(TARGET)

$(TARGET) : $(objects) $(mixer_lang_lib)
	$(CXX) $(CFLAGS)  -s $(objects) -o $@ $(mixer_lang_lib) -lpthread

$(objects) : obj/%.o : %.cpp $(mixer_lang_lib)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(mixer_lang_lib):
	make -C ../../src/

clean:
	-rm -rf obj/*.o $(TARGET)