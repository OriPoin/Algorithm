CC=g++
CFLAGS=-Wall
AR=ar
SOURCEDIR		:= src
UTILDIR			:= util
BUILDDIR		:= build
# LIBRARY_PATH	:= /usr/lib64/
SOURCES			:= $(wildcard $(UTILDIR)/*.cpp)
CLI_SRC			:= $(UTILDIR)/cli.cpp
OBJECTS			:= $(patsubst $(UTILDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
TARGET			:= bpt
LIBS			:= -lreadline

all: dir  ${BUILDDIR}/bpt.o ${BUILDDIR}/bpt.a $(BUILDDIR)/$(TARGET) ${BUILDDIR}/gendb

# $(BUILDDIR)/$(TARGET): ${CLI_SRC}
# $(CC) $^ -o $@ $(LIBS)
$(BUILDDIR)/$(TARGET): ${CLI_SRC}
	$(CC) -g3 $^ ${BUILDDIR}/bpt.a -o $@ $(LIBS)

${BUILDDIR}/bpt.o:
	${CC} -g3 -c ${SOURCEDIR}/bpt.cpp -o $(BUILDDIR)/bpt.o

${BUILDDIR}/bpt.a:
	${AR} -r ${BUILDDIR}/bpt.a $(BUILDDIR)/bpt.o

${BUILDDIR}/gendb:
	${CC} $(UTILDIR)/gendb.cpp -o $@ -g3


# #link parameter
# LIB := libbpt.so

# #link
# ${build}/$(LIB):${BUILDDIR}/bpt.o
#     $(CC) -shared -o -fPIC -o $@ $^
# #compile
# ${build}/bpt.o:${SOURCEDIR}/bpt.cpp
#     $(CC) -c -fPIC $^ -o $@

dir:
	mkdir -p ${BUILDDIR}

clean:
	rm -f $(BUILDDIR)/* *~ 

run :
	${BUILDDIR}/${TARGET}