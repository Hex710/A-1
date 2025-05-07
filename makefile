parametrosCompilacao=-Wall #-Wshadow
nomePrograma=vina

all: $(nomePrograma)

$(nomePrograma): vina.o archive.o lz.o
	gcc -o $(nomePrograma) vina.o archive.o lz.o -lm $(parametrosCompilacao)

vina.o: vina.c
	gcc -c vina.c $(parametrosCompilacao)

archive.o: archive.h archive.c
	gcc -c archive.c $(parametrosCompilacao)

lz.o: lz.h lz.c
	gcc -c lz.c $(parametrosCompilacao)

clean:
	rm -f *.o *.gch $(nomePrograma)