
################################################################################
# Make Test Library
################################################################################
LIBNAME=libplugintest.so
BINDIR=./bin
OBJDIR=./obj
SRCDIR=./src
LIBDIR=../bin

ZEROVM_ROOT=../../..

CC=gcc -Wall
LD=ld


OBJS= lib_test.o

NEXE_CC=/usr/bin/x86_64-nacl-gcc -Wall
NEXE= hello_world

INCLUDES=-I${ZEROVM_ROOT}/api -I${ZEROVM_ROOT}/src/zplugin_manager

NEXE_INCLUDES=${INCLUDES} -I${ZEROVM_ROOT}/tests/functional/include
NEXE_CFLAGS=-msse4.1 -O2 -nostartfiles -nostdlib -fno-builtin


CFLAGS=-fpic ${INCLUDES}
LFLAGS=${GLIB_LFLAGS} 

VPATH=${SRCDIR}:${INCDIR}:${OBJDIR}
.PHONY: all clean ${NEXE}

all: ${LIBNAME} ${NEXE:%=%.nexe}

clean:
	rm -rf ${BINDIR} ${OBJDIR} ${NEXE:%=%.manifest}

${BINDIR}/${LIBNAME}: ${OBJS}
	@mkdir -p ${BINDIR} ${OBJDIR}
	${LD} -o ${BINDIR}/${LIBNAME} -shared ${^:%=${OBJDIR}/%}

${LIBNAME}: ${LIBNAME:%=${BINDIR}/%}

.c.o: 
	@mkdir -p ${OBJDIR}
	${CC} ${CFLAGS} -c -o ${OBJDIR}/${@F:${OBJDIR}/%=%} ${LFLAGS} ${<}

${NEXE:%=${BINDIR}/%.nexe}: 
	@mkdir -p ${BINDIR}
	${NEXE_CC} ${NEXE_CFLAGS} -o ${@} -I${ZEROVM_ROOT} ${NEXE_INCLUDES} \
		${SRCDIR}/${NEXE:%=%.c} ../../functional/include/libzvmlib.a

${NEXE:%=%.nexe}: ${NEXE:%=${BINDIR}/%.nexe} ${NEXE:%=%.manifest}

${NEXE}: ${NEXE:%=${BINDIR}/%.nexe}

${NEXE:%=%.manifest}: ${SRCDIR}/${NEXE:%=%.template}
	sed 's#PWD#${PWD}#g' ${<} > ${@}

