# ------------------------------------------------------------------------
#  This is the Make file for the blackjack program
#
#  Created: 2016-05-03
#

OBJECTS=main.o table.o card.o

FLAGS=-Wall -c

all: blackjack


blackjack: $(OBJECTS)
	gcc $(OBJECTS) -o blackjack

main.o: main.c table.h common.h card.h
	gcc $(FLAGS) main.c

table.o: table.c table.h common.h card.h
	gcc $(FLAGS) table.c

card.o: card.c card.h
	gcc $(FLAGS) card.c

clean:
	rm -f $(OBJECTS) blackjack

dist:
	tar -cvzf dist4.tgz Makefile main.c card.c card.h table.c table.h common.h

