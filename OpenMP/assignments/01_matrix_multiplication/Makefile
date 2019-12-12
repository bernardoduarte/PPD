

prog.out: prog.c timer.h
	gcc -o prog.out -fopenmp prog.c

tests: prog.out
	echo "small"
	for threads in 1 2 4 ; do \
		for execution in 1 2 3 ; do \
			./prog.out 100 100 100 100 1 10 $$threads ; \
		done ; \
	done
	echo "medium"
	for threads in 1 2 4 ; do \
		for execution in 1 2 3 ; do \
			./prog.out 100 1000 1000 100 1 10 $$threads ; \
		done ; \
	done
	echo "large"
	for threads in 1 2 4 ; do \
		for execution in 1 2 3 ; do \
			./prog.out 1000 1000 1000 1000 1 10 $$threads ; \
		done ; \
	done

