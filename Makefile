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

# QUAN_ROOT is path to my quan library https://github.com/kwikius/quan-trunk.git
ifneq ($(MAKECMDGOALS),clean)
ifndef QUAN_ROOT
$(error "QUAN_ROOT must be defined to path to my quan library --> https://github.com/kwikius/quan-trunk.git")
endif
endif

INCLUDES = $(QUAN_ROOT)

CC = g++
LD = g++

INCLUDE_ARGS = $(patsubst %,-I%,$(INCLUDES))

CFLAGS = -std=gnu++11 -Wall -Os 

local_objects = lexer.o main.o mixer.o symtab.o function.o arg_list.o \
fn_max.o fn_if.o

objects = $(local_objects) key_was_pressed.o

.PHONY: clean all

all: mixer_lang.exe

mixer_lang.exe : $(objects) 
	$(LD) $(objects) $(CFLAGS) -s -o mixer_lang.exe -lpthread

$(local_objects) : %.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDE_ARGS) -c $< -o $@

key_was_pressed.o : $(QUAN_ROOT)/quan_matters/src/linux/key_was_pressed.cpp
	$(CC) $(CFLAGS) $(INCLUDE_ARGS) -c $< -o $@

clean:
	-rm -rf *.o *.exe

