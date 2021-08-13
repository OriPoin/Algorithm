CC=g++
CFLAGS=-Wall
AR=ar
SOURCEDIR		:= src
UTILDIR			:= util
BUILDDIR		:= build
SOURCES			:= $(wildcard $(UTILDIR)/*.cpp)
CLI_SRC			:= $(UTILDIR)/cli.cpp
OBJECTS			:= $(patsubst $(UTILDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
TARGET			:= bpt
LIBS			:= -lreadline

Opt_FLAGS=-march=native -O3 -pipe
FLTO=-flto=auto -fwhole-program
SSP_FLAGS=-fno-stack-protector

COMMON_FLAGS=${Opt_FLAGS} ${SSP_FLAGS} ${FLTO}
# COMMON_FLAGS=-g3
# LDFLAGS=-Wl,-O3 -Wl,--as-needed -Wl,--hash-style=gnu -Wl,--sort-common -Wl,--strip-all

all: dir  ${BUILDDIR}/bpt.o ${BUILDDIR}/bpt.a $(BUILDDIR)/$(TARGET) ${BUILDDIR}/gendb

$(BUILDDIR)/$(TARGET): ${CLI_SRC}
	$(CC) ${COMMON_FLAGS} ${LDFLAGS} $^ ${BUILDDIR}/bpt.a -o $@ $(LIBS)

${BUILDDIR}/bpt.o:
	${CC} ${COMMON_FLAGS} -c ${SOURCEDIR}/bpt.cpp -o $(BUILDDIR)/bpt.o

${BUILDDIR}/bpt.a:
	${AR} -r ${BUILDDIR}/bpt.a $(BUILDDIR)/bpt.o

${BUILDDIR}/gendb:
	${CC} $(UTILDIR)/gendb.cpp -o $@ ${COMMON_FLAGS}

dir:
	mkdir -p ${BUILDDIR}

clean:
	rm -f $(BUILDDIR)/* *~ 

run :
	${BUILDDIR}/${TARGET}