SOURCE = prog.c
TARGET = prog.out
BOARD = 5000x5000.board

all: $(TARGET)

$(TARGET): $(SOURCE)
	mpicc -o $@ $<

run: $(TARGET)
	mpirun -n 4 $< $(BOARD)

clean:
	find . -type f -not -name 'Makefile' -not -name '*.c' -not -name '*.h' -delete
