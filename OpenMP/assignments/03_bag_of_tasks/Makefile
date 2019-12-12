

prog.out: prog.c timer.h
	gcc -o prog.out prog.c -lm -fopenmp

tests: prog.out
	echo "small"
	for threads in 1 2 4 ; do \
		for execution in 1 2 3 ; do \
			./prog.out 100000 1 10 $$threads ; \
		done ; \
	done
	echo "medium"
	for threads in 1 2 4 ; do \
		for execution in 1 2 3 ; do \
			./prog.out 1000000 1 10 $$threads ; \
		done ; \
	done
	echo "large"
	for threads in 1 2 4 ; do \
		for execution in 1 2 3 ; do \
			./prog.out 10000000 1 10 $$threads ; \
		done ; \
	done

