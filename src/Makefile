VERSION=0.2.1
CC=		gcc
INCLUDES=	-I/usr/local/include 
LIBDIR=		-L/usr/local/lib
LIBS=		-lmrss
LDFLAGS+=	${LIBDIR}

OPSYS!=         uname
OS_VER!=        uname -r
OS_ARCH!=       uname -p

PODCASTDL_VERSION=  "${VERSION} for ${OPSYS}-${OS_VER} ${OS_ARCH}"
CFLAGS+=	-g3 -Wall -Werror -pipe -O2 -DPODCASTDL_VERSION_STRING=\"${PODCASTDL_VERSION}\"

PROG=		podcastdl
SRCS=		podcastdl.c podcast.c util.c
OBJS=		${SRCS:.c=.o}

PREFIX?=	/usr/local
BINDIR?=	${PREFIX}/bin
MANDIR?=	${PREFIX}/man/man1

all: ${PROG}

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} ${CFLAGS} ${INCLUDES} ${OBJS} -o $@ ${LIBS}

.c.o:
	${CC} -o $@ -c $< ${CFLAGS} ${INCLUDES}

install:
	install -o root -g wheel -m 755 ${PROG} ${BINDIR}

deinstall:
	rm -f ${BINDIR}/${PROG}

clean:
	rm -f ${PROG} *.o *.core
