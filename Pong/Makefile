#Compiladores
CC= gcc
FLAGS= -g #-Wall -pedantic

vpath %.c src/
vpath %.o obj/
vpath %.h include/
# Macro Implicita
.c.o:
	$(CC) $(FLAGS) -c -I include/ -o obj/$@ $<
#programa : objectos
pong: pong.o window.o items.o
	$(CC) $(FLAGS) -o bin/$@ obj/pong.o obj/window.o obj/items.o -lpthread -lncurses  

pong.o: pong.c window.h

window.o: window.c window.h

items.o: items.c items.h

clean:
	rm bin/pong obj/*.o