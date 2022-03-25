CC = g++
CFLAGS = -std=c++11 -lglpk

EXP = jmy
OBJ = util.o main.cpp

main: $(OBJ)
	$(CC) $(CFLAGS) -o $(EXP) $(OBJ)

util.o: util.hpp util.cpp
	$(CC) $(CFLAGS) -c util.cpp

clean:
	rm *.o *.db $(EXP)
