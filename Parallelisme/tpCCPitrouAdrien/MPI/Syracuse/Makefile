MPICXX = mpic++
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)


all: maitre esclave

%.o: %.cpp
	$(MPICXX) -o $@ -c $< 

maitre: maitre.o syracuse.o
	$(MPICXX) -o maitre maitre.o syracuse.o

esclave: esclave.o syracuse.o
	$(MPICXX) -o esclave esclave.o syracuse.o
run: all
	mpirun -np 1 ./maitre 16 4 0

clean:
	@rm -rf *.o

cleanall: clean
	@rm -rf maitre esclave

