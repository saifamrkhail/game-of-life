all: clean gol run

gol:
	g++ -o2 -Wall -fopenmp gol.cpp Timing.cpp -o gol

clean:
	rm gol

run:
	./gol --load step1000_in_250generations/random4000_in.gol --save ai22m055_4000_cpu_out.gol --mode seq --measure