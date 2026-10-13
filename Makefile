USE_DEBUG = NO
# USE_BMP and USE_PNG are superceded by USE_UNICODE, which selects GDI+
USE_BMP = NO
USE_PNG = NO
USE_UNICODE = YES
USE_64BIT = NO
USE_CLANG = NO
USE_CYGWIN = NO

include der_libs\tool_select.mak

#*****************************************************************************
ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
LFLAGS= -mwindows 
else
CFLAGS=-Wall -O2 -c
LFLAGS=-s -mwindows 
endif
CFLAGS += -Wno-write-strings

ifeq ($(USE_UNICODE),YES)
CFLAGS += -DUNICODE -D_UNICODE
LFLAGS += -dUNICODE -d_UNICODE
endif

# link library files
CFLAGS += -Ider_libs
CSRC=images.cpp
CSRC+=der_libs/common_funcs.cpp \
der_libs/gdi_plus.cpp \
der_libs/gdiplus_setup.cpp \
der_libs/common_win.cpp \
der_libs/winmsgs.cpp 

OBJS = $(CSRC:.cpp=.o)

BASE=images
BIN=$(BASE).exe

LIBS= -lgdi32 -lgdiplus

#************************************************************
%.o: %.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) $< -o $@

#************************************************************
all: $(BIN)

clean:
	rm -vf $(BIN) $(OBJS) *.zip 

wc:
	wc -l $(CSRC)
	
clint:
	cmd /C "python ..\ClaudeLint.py --exclude der_libs"
	
cppc:
	cmd /C "cppcheck --project=compile_commands.json --std=c++14 --suppressions-list=./.suppress.cppcheck"

check:
	cmd /C "d:\llvm\bin\clang-tidy.exe $(CSRC)"

depend:
	makedepend $(CFLAGS) $(CSRC)

$(BIN): $(OBJS)
	$(TOOLS)/$(GNAME) $(OBJS) $(LFLAGS) -o $(BIN) $(LIBS) 

# DO NOT DELETE

der_libs/common_funcs.o: der_libs/common.h
der_libs/gdi_plus.o: der_libs/common.h der_libs/gdi_plus.h
der_libs/gdiplus_setup.o: der_libs/gdi_plus.h
der_libs/common_win.o: der_libs/common.h der_libs/commonw.h
images.o: der_libs/common.h der_libs/gdi_plus.h der_libs/gdiplus_setup.h
