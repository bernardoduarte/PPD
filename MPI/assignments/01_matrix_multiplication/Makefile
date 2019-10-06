SOURCE = prog.c
TARGET = prog.out
A_FILE = A.matrix
B_FILE = B.matrix
C_FILE = C.matrix

all: $(TARGET)

$(TARGET): $(SOURCE)
	mpicc -o $@ $<

run: $(TARGET)
	mpirun -n 4 $< $(A_FILE) $(B_FILE) $(C_FILE)

clean:
	find . -type f -not -name 'Makefile' -not -name '*.c' -not -name '*.h' -delete
