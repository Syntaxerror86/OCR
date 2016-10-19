## Simple SDL mini code
 
CC=clang
 
CPPFLAGS= `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -Werror -std=c99 -O3
LDFLAGS=
LDLIBS=
 
SRC= network.c
OBJ= ${SRC:.c=.o}
 
all: network
 
network: ${OBJ}
 
clean:
	rm -f *~ *.o
	rm -f network
 
# END
