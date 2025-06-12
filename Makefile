USE_DEBUG = NO
# USE_BMP and USE_PNG are superceded by USE_UNICODE, which selects GDI+
USE_BMP = NO
USE_PNG = NO
USE_UNICODE = YES
USE_64BIT = NO

ifeq ($(USE_64BIT),YES)
TOOLS=d:\tdm64\bin
else
#TOOLS=c:\mingw\bin
#TOOLS=c:\TDM-GCC-64\bin
TOOLS=d:\tdm32\bin
endif

#*****************************************************************************
# notes on compiler quirks, using MinGW/G++ V4.3.3
# if I build with -O3, I get following warnings from g++ :
#   wfuncs.cpp: In function 'int light_a_flare(HWND__*)':
#   wfuncs.cpp:338: warning: array subscript is above array bounds
# where light_a_flare() starts at line 779 !!
# If I build with -O2, I get no such warnings.
# In either case, PcLint V9 is giving no warnings on this code.
#*****************************************************************************

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g -mwindows 
LFLAGS=
else
CFLAGS=-Wall -O2 -mwindows -Weffc++ 
LFLAGS=-s
endif
CFLAGS += -Wno-write-strings

ifeq ($(USE_UNICODE),YES)
CFLAGS += -DUNICODE -D_UNICODE
LFLAGS += -dUNICODE -d_UNICODE
LiFLAGS += -DUNICODE -D_UNICODE
endif

# link library files
LiFLAGS += -Ider_libs
CFLAGS += -Ider_libs
CSRC=der_libs/common_funcs.cpp \
der_libs/gdi_plus.cpp \
der_libs/gdiplus_setup.cpp \
der_libs/common_win.cpp \
der_libs/winmsgs.cpp 

# separate local source files from library files,
# so that wc operation is more appropriate.
CBASE=images.cpp
CSRC += $(CBASE)

#  clang-tidy options
CHFLAGS = -header-filter=.*
CHTAIL = -- -Ider_libs
ifeq ($(USE_64BIT),YES)
CHTAIL += -DUSE_64BIT
endif
ifeq ($(USE_UNICODE),YES)
CHTAIL += -DUNICODE -D_UNICODE
endif

# iface_lib.cpp 

OBJS = $(CSRC:.cpp=.o)

BASE=images
BIN=$(BASE).exe

LIBS= -lgdi32 -lgdiplus

#************************************************************
%.o: %.cpp
	$(TOOLS)\g++ $(CFLAGS) -c $< -o $@

#************************************************************
all: $(BIN)

clean:
	rm -vf $(BIN) $(OBJS) *.zip *.bak *~

wc:
	wc -l $(CBASE) *.rc
	
check:
	cmd /C "d:\clang\bin\clang-tidy.exe $(CHFLAGS) $(CSRC) $(CHTAIL)"

lint:
	cmd /C "c:\lint9\lint-nt +v -width(160,4) $(LiFLAGS) -ic:\lint9 mingw.lnt -os(_lint.tmp) lintdefs.cpp $(CSRC)"

depend:
	makedepend $(CFLAGS) $(CSRC)
$(BIN): $(OBJS)
	$(TOOLS)\g++ $(CFLAGS) $(LFLAGS) $(OBJS) -o $@ $(LIBS)

# DO NOT DELETE

der_libs/common_funcs.o: der_libs/common.h
der_libs/gdi_plus.o: der_libs/common.h der_libs/gdi_plus.h
der_libs/gdiplus_setup.o: der_libs/gdi_plus.h
der_libs/common_win.o: der_libs/common.h der_libs/commonw.h
images.o: der_libs/gdi_plus.h der_libs/gdiplus_setup.h
