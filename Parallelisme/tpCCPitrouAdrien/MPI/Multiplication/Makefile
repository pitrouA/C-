MPICXX = mpic++
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)


all: maitre esclave

%.o: %.cpp
	$(MPICXX) -o $@ -c $< 

maitre: maitre.o fonctions.o
	$(MPICXX) -o maitre maitre.o fonctions.o

esclave: esclave.o fonctions.o
	$(MPICXX) -o esclave esclave.o fonctions.o

run: all
	mpirun -np 1 ./maitre 20 40 4

clean:
	@rm -rf *.o

cleanall: clean
	@rm -rf maitre esclave

