CC = gcc -Wall

ifdef DAL_DEBUG
DALDEBUG=-DDALDEBUG
endif

ifdef DAL_LOCAL
DALLOCAL=-DDALLOCAL
endif

#check target system, supported targets: macos, linux, and ixp4 (Intel XScale 4) using cross compiler
ifndef TARGET #if no target supplied
	#find out host OS 
	HOST_OS := $(shell uname)
	ifeq ($(HOST_OS), Darwin)
	TARGET_OS = macos
	endif
	ifeq ($(HOST_OS), Linux)
	TARGET_OS = linux
	endif 
else #if target was supplied
	ifeq ($(TARGET), linux)
	TARGET_OS = linux
	endif
	ifeq ($(TARGET), macos)
	TARGET_OS = macos
	endif

# Heaps
ifeq ($(HEAPS), 1)
HEAPS = 1
endif
ifeq ($(HEAPS), 0)
HEAPS = 0
endif
# Affinity algorithm
ifeq ($(AFFINITY), rr)
AFFINITY_STRING = RR
endif
ifeq ($(AFFINITY), dyn)
AFFINITY_STRING = DYN
endif
# Debug
ifeq ($(DEBUG), 1)
DEBUG_STRING = 1
endif
ifeq ($(DEBUG), 0)
DEBUG = 0
endif
ifeq ($(DEBUG), 2)
DEBUG = 2
endif
# Multi-core Debug
ifeq ($(MCDEBUG), true)
MCDEBUG = TRUE
endif
ifeq ($(MCDEBUG), false)
MCDEBUG = FALSE
endif
# CFLAGS = -g -c -DDALSMALL -DDALTEST $(DALDEBUG) $(DALLOCAL) -D$(TARGET_OS) $(HEAPS) -D$(AFFINITY) -D$(DEBUG) -D$(MCDEBUG)
CFLAGS = -g -c -DDALSMALL -DDALTEST $(DALDEBUG) $(DALLOCAL)
RMCMD = rm -f

TestFindMinimumHopPath: TestFindMinimumHopPath.o Makefile
	$(CC) -o TestFindMinimumHopPath TestFindMinimumHopPath.o


TestBitMatrix: TestBitMatrix.o BitMatrix.o BitMatrix.h \
	Makefile BitMatrix.h
	$(CC) -o TestBitMatrix TestBitMatrix.o BitMatrix.o 

Test_DAL_mem: Person.o DAL_mem.o Test_DAL_mem.o DAL_error.o
	$(CC) -o Test_DAL_mem Person.o DAL_mem.o Test_DAL_mem.o DAL_error.o

TestList: DAL_mem.o DAL_error.o DAL_random.o Person.o \
	IteratedList.o TestList.o 
	$(CC) -o TestList DAL_mem.o DAL_error.o DAL_random.o Person.o \
	IteratedList.o TestList.o

TestMap: BSTMap.o DAL_mem.o FunctionPair.o TestMap.o
	$(CC) -o TestMap BSTMap.o DAL_mem.o FunctionPair.o TestMap.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RMCMD) *.o

vclean: clean
	$(RMCMD) *~ *.exe*

