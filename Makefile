ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
endif

ifeq ($(detected_OS),Windows)
	obj_type = i386pe
	mkdir = mkdir $(subst /,\,$(1)) > nop 2>&1 || (exit 0)
	rm = $(wordlist 2,65535,$(foreach FILE,$(subst /,\,$(1)),& del $(FILE) > nop 2>&1)) || (exit 0)
	rmdir = rmdir $(subst /,\,$(1)) > nop 2>&1 || (exit 0)
	echo = echo $(1)
else
	obj_type = elf_i386
	mkdir = mkdir -p $(1)
	rm = rm $(1) > /dev/null 2>&1 || true
	rmdir = rmdir $(1) > /dev/null 2>&1 || true
	echo = echo "$(1)"
endif

ifeq ($(detected_OS),Windows)
WINAPI = -lmingw32 -lkernel32 -lm -ldxguid -ldxerr8 -luser32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lgdi32 -lcomdlg32 -lwinspool
WINAPI+= -lcomctl32 -luuid -lrpcrt4 -ladvapi32 -lwsock32 -lshlwapi -lversion -lwinpthread -ldbghelp -lpthread
endif

OBJS = ./*cpp
HEADS = ./patcher/binPatcher.hpp
PREP = -include $(HEADS)
CC = g++

ASTYLE = aStyle

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L ./lib

INCLUDE_PATHS = -I ./boost_lib -I .

align_size = 0x100
align_data = 0x1000
align_rdata = 0x1000
align_bss = 0x1000
align_idata = 0x1000

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = FaPatcher.exe

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -static -w -DOBJ_NAME='"$(OBJ_NAME)"' -O3 -s

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -static-libgcc -static-libstdc++

LOCAL_LIBS = -lpatcher -lfilesystem -lsystem -lpebliss -lasmjit

#-oformat -Ttext=0x006B8FB9
#echo align_size = '$(align_size);' > Env.ld

align:
	echo align_size = '$(align_size);' > Env.ld

peLib:
	$(MAKE) all -C ./pe_lib

boostLib:
	$(MAKE) all -C ./boost_lib/filesystem
	$(MAKE) all -C ./boost_lib/system

patcherLib:
	$(MAKE) all OBJ_NAME=$(OBJ_NAME) -C ./patcher

asmjitLib:
	$(MAKE) all -C ./asmjit_lib

cleanall:
	$(MAKE) clean -C ./boost_lib/filesystem
	$(MAKE) clean -C ./boost_lib/system
	$(MAKE) clean -C ./pe_lib
	$(MAKE) clean -C ./asmjit_lib
	$(MAKE) clean -C ./patcher
	rm -Rf ./build

cleanbuild:
	rm -Rf ./build

directories:
	$(call mkdir, /build)
	$(call mkdir, /preprocessor)

ext_sector:
	$(MAKE) all_individual -C ./sections
	$(MAKE) link -C ./sections

_hooks:
	$(MAKE) all OBJ_NAME=$(OBJ_NAME_) OBJS=$(OBJS) -C ./hooks

_fast_hooks:
	$(MAKE) fast_compile -C ./hooks

ext_gpp_link:
	$(call echo,align_data = '$(align_data);' > Env.ld)
	$(call echo,align_rdata = '$(align_rdata);' >> Env.ld)
	$(call echo,align_bss = '$(align_bss);' >> Env.ld)
	$(call echo,align_idata = '$(align_idata);' >> Env.ld)
	ld -T ./linker/sectionLinker.ld -static -m  $(obj_type) $(PRIME_NAME) -o $(TMP_NAME)

hook_gpp_link:
	$(call echo,align_size = '$(align_size);' > Env.ld)
	ld -T ./linker/hookLinker.ld -static -m  $(obj_type) $(PRIME_NAME) -o $(TMP_NAME)

rip_out_binary:
	objcopy --strip-all -O binary -R .eh_fram $(TMP_NAME) $(PRIME_NAME)

pre_comp_h:
	$(CC) -w $(INCLUDE_PATHS) $(HEADS)

#This is the target that compiles our executable
all : peLib boostLib asmjitLib patcherLib
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(LOCAL_LIBS) -o $(OBJ_NAME)
	@echo ./FaPatcher built successfully.

allwin : peLib boostLib asmjitLib patcherLib
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(LOCAL_LIBS) $(WINAPI) -o $(OBJ_NAME)
	@echo ./FaPatcher built successfully.

format:
	$(ASTYLE) --style=allman --indent=tab --recursive ./*.cpp, *.h, *.hpp

clean_f:
	find . -type f -name '*.orig' -delete

clean_g:
	find . -type f -name '*.gch' -delete

