#
# Copyright (c) 2013 -2015 Andy Little 

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>

QUANTRACKER_ROOT_DIR := /home/andy/cpp/projects/quantracker/
OPTIMISATION_LEVEL  := Os

include $(QUANTRACKER_ROOT_DIR)include/quantracker/build/osd.mk

TARGET = ../build/stm32f405/mixer_lang.a

.PHONY: clean all sources

#CXX ?= g++

#CFLAGS = -std=gnu++11 -Wall -Os 

local_objects = arg_list.o lexer.o mixer.o symtab.o function.o 

sources  = $(patsubst %.o,%.cpp,$(local_objects))

headers = abc_expr.hpp arg_list.hpp basic_char_ptr_converter.hpp exprtree.hpp \
function.hpp lexer.hpp lookup.hpp mixer.hpp parse_number.hpp predicates.hpp

objects = $(patsubst %.o,obj/stm32f405/%.o,$(local_objects))
 
all: $(TARGET)

$(TARGET) : $(objects)
	$(AR) rcs $@ $(objects)

$(objects) : obj/stm32f405/%.o : %.cpp $(headers)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -rf obj/stm32f405/*.o $(TARGET)
