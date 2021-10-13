CFLAGS := -Wall -Wextra -Wpedantic
EXE := server client

all: $(EXE)

client: utils.o grille.o joueur.o tlv.o
server: utils.o grille.o joueur.o tlv.o
tlv: grille.o joueur.o utils.o

utils.o: utils.h
grille.o: grille.h
tlv.o: grille.h tlv.h utils.h joueur.h
joueur.o: joueur.h

clean:
	$(RM) $(EXE) *~ *.o

.PHONY: all clean