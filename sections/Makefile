ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
endif

#OBJS specifies which files to compile as part of the project 
SRCS=$(wildcard *.cpp)

OBJS=$(SRCS:.cpp=.o)
LINK_OBJS:= $(addprefix ../build/,$(OBJS))

PROGS = $(patsubst %.cpp,../build/%.o,$(SRCS))

#HEADS = funcDef.h

#CC specifies which compiler we're using 
CC = g++
#INCLUDE_PATHS specifies the additional include paths we'll need 
#INCLUDE_PATHS = -I ./SDL2-2.0.8/i686-w64-mingw32/include/

#LIBRARY_PATHS specifies the additional library paths we'll need 
LIBRARY_PATHS = -L ../lib

#COMPILER_FLAGS specifies the additional compilation options we're using 
# -w suppresses all warnings 
# -Wl,-subsystem,windows gets rid of the console window 
ifeq ($(detected_OS),Windows)
COMPILER_FLAGS = -fleading-underscore
LD_FLAGS = --image-base 0x128A000 -m i386pe

else
LD_FLAGS = -Ttext-segment 0x128D000 -m elf_i386
endif

#LINKER_FLAGS specifies the libraries we're linking against 

#STD = -static-libgcc -static-libstdc++ 

LINKER_FLAGS = -I. -m32 -O3 -fno-exceptions -fno-asynchronous-unwind-tables -Wall -w -fpermissive -Wextra -masm=intel

#this flag ensures that gcc compiler uses 'push' instructions for functions parameters. 
FUNC_CALL_PARAMS_FLAGS = -mpush-args -mno-accumulate-outgoing-args -mno-stack-arg-probe

#BOOST = -lboost_thread-mgw63-mt-d-x32-1_67 -lboost_regex-mgw63-mt-d-x32-1_67 -lboost_system-mgw63-mt-d-x32-1_67
 
OUTPUT = ../build/ext_sector.o 
 
#This is the target that compiles our executable 
all_individual : $(PROGS)
../build/%.o: %.cpp
	$(CC) -c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(FUNC_CALL_PARAMS_FLAGS) $(STD) $(LINKER_FLAGS) $(BOOST) -o $@ $<

all :
	$(CC) $(HEADS) -c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(STD) $(LINKER_FLAGS) $(BOOST) -o $(OUTPUT) ext_sector.cpp
	
# --image-base. only windows ld supports this. It marks the starting virtual address of the file built. 	
link:
	ld $(LD_FLAGS) -Map ../build/mapfile.map -o ../build/exxt_sector.o $(LINK_OBJS)
