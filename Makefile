all: clean gol run

gol:
	g++ -o2 -Wall -fopenmp gol.cpp Timing.cpp -o gol

clean:
	rm gol random250_out.gol2

run:
	./gol --load step1000_in_250generations/random250_in.gol --save random250_out.gol2 --measure