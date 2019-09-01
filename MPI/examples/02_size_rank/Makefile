TARGET = prog.out

all: $(TARGET)

$(TARGET): prog.c
	mpicc -o $@ $<

run: $(TARGET)
	mpirun -n 4 $<

clean: $(TARGET)
	rm $<
