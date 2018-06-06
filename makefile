# 27-10-2015
# billy

EXE = serp # Executable
OBJ = serp.o # Objectes
COM = g++ # Compilador
LIB = -lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf # Llibreries (-l*, -L*, -I*)
MAC = # Macros (-D*)
AVS = -W -Wall -Wextra -ansi -pedantic # Avisos
OPT = -march=native -O2 -pipe # Optimitzacio
DEP = -g -DDEBUG # Depuracio, no recomanable junt amb $(OPT)
OPC = $(DEP) $(AVS) $(MAC) -std=c++14 # Opcions
DIR = /usr/local/bin # Directori per a instalar

all: $(EXE)

$(EXE): $(OBJ)
	@echo Enllaçant $(OBJ) per a crear $(EXE)
	$(COM) $(LIB) $(OBJ) -o $@

serp.o: serp.c++
	@echo Compilant $<
	$(COM) $(OPC) -c $<

exe: all
	./$(EXE) -f

install: all
	mkdir -p $(DIR)
	cp $(EXE) $(DIR)

clean:
	@echo Netejant...
	rm -rf $(EXE) $(OBJ) *~
