CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -g -std=c99
LDLIBS  = -lm
MAIN    = vina
ENTREGA = $(MAIN)

# lista de arquivos de cabeçalho (a completar)
HDR = lz.h vina.h conjunto.h eventos.h

# lista de arquivos-objeto (a completar)
# não inclua conjunto.o, senão ele será removido com "make clean"
OBJ = lz.o vina.o eventos.o

# construir o executável
$(MAIN): $(MAIN).o $(OBJ)

# construir os arquivos-objeto (a completar)
$(MAIN).o: $(MAIN).c $(HDR)

# construir os TADs
lz.o: lz.c lz.h
eventos.o: eventos.c eventos.h

# executar
run: $(MAIN)
	./$(MAIN)

# testar no Valgrind
valgrind: $(MAIN)
	valgrind --leak-check=full --track-origins=yes ./$(MAIN)

# gerar arquivo TGZ para entregar
tgz: clean
	-mkdir -p /tmp/$(USER)/$(ENTREGA)
	chmod 0700 /tmp/$(USER)/$(ENTREGA)
	cp *.c *.h makefile /tmp/$(USER)/$(ENTREGA)
	tar czvf $(ENTREGA).tgz -C /tmp/$(USER) $(ENTREGA)
	rm -rf /tmp/$(USER)
	@echo "Arquivo $(ENTREGA).tgz criado para entrega"

# limpar arquivos temporários
clean:
	rm -f *~ $(OBJ) $(MAIN) /tmp/$(USER)/$(ENTREGA) $(ENTREGA).tgz