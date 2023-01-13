/**
 * Game of Life
 *
 * The "game" is a zero-player game, meaning that its evolution is determined by its initial state, requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves, or, for advanced "players", by creating patterns with particular properties.
 * 1.Any live cell with fewer than two live neighbours dies, as if caused by under-population.
 * 2.Any live cell with two or three live neighbours lives on to the next generation.
 * 3.Any live cell with more than three live neighbours dies, as if by over-population.
 * 4.Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
 * The universe of the Game of Life is an infinite two-dimensional orthogonal grid of square cells, each of which is in one of two possible states, alive or dead. Every cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent. At each step in time, the following transitions occur:
 * The initial pattern constitutes the seed of the system. The first generation is created by applying the above rules simultaneously to every cell in the seed—births and deaths occur simultaneously, and the discrete moment at which this happens is sometimes called a tick (in other words, each generation is a pure function of the preceding one). The rules continue to be applied repeatedly to create further generations.
 *
 * @author: Saifur Rahman Rahmani <ai22m055@technikum-wien.at>
 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <getopt.h>
#include <vector>
#include <cstring>
#include <omp.h>
#include "Timing.h"

using namespace std;
const unsigned int state[2][8] = {
        {0, 0, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0}
};
//void determineState(vector<vector<bool>> &grid, vector<vector<bool>> &copy, unsigned int rows, unsigned int cols, unsigned int (&state)[2][8], unsigned int threads);
//void determineState(vector<vector<bool>> &grid, vector<vector<bool>> &copy, unsigned int rows, unsigned int cols);

int main(int argc, char *argv[]) {
    //clear screen
    cout << "\033[2J\033[1;1H";
    string inputFileName = "";
    string outputFileName = "out.gol";
    unsigned int generations = 250;
    int threads = 8;
    bool measure = false;
    //bool omp = false;
    /*const unsigned int state[2][8] = {
            {0, 0, 1, 0, 0, 0, 0, 0},
            {0, 1, 1, 0, 0, 0, 0, 0}
    };*/
    char characters[2] = {'.', 'x'};
    unsigned int rows = 0, cols = 0;



/* parse command line arguments*/
    int arguments;
    while (true) {
        static struct option long_options[] =
                {
                        {"load",        required_argument, 0, 'l'},
                        {"save",        required_argument, 0, 's'},
                        {"generations", required_argument, 0, 'g'},
                        {"measure",     no_argument,       0, 'm'},
                        {"help",        no_argument,       0, 'h'},
                        {"mode",        required_argument, 0, 'd'},
                        {"threads",     required_argument, 0, 't'}};
        /* getopt_long stores the option index here. */
        int option_index = 0;

        arguments = getopt_long(argc, argv, "mhl:s:g:d:t:", long_options, &option_index);

        /* Detect the end of the options. */
        if (arguments == -1) {
            break;
        }

        switch (arguments) {

            case 'h': {
                cout << "usage: "
                     << "gol [--load | -l <filename>] [--save | -s <filename>] [--generations | -g <n>] [--measure | -m]"
                     << endl;
                cout << "[--load | -l] <filename> (filename to read input board)" << endl;
                cout << "[--save | -s] <filename> (filename to save output board | by defualt 'out.gol')" << endl;
                cout << "[--generations | -g] <n> (run n generations | by default '250')" << endl;
                cout << "[--measure | -m] (print time measurements)" << endl;
                cout << "[--mode | -d] <seq | omp> (select between seq and omp)" << endl;
                cout << "[--threads | -t] <num> (set number of threads | by default 1)" << endl;
                exit(1);
            }

            case 'm': {
                measure = true;
                break;
            }

            case 'l': {
                if (strcmp("-g", optarg) == 0 || strcmp("--generations", optarg) == 0 || strcmp("-s", optarg) == 0 ||
                    strcmp("--save", optarg) == 0 || strcmp("-m", optarg) == 0 || strcmp("--measure", optarg) == 0 ||
                    strcmp("-t", optarg) == 0 || strcmp("--threads", optarg) == 0 ||
                    strcmp("-d", optarg) == 0 || strcmp("--mode", optarg) == 0) {
                    std::cerr << "Invalid argument for --load: " << optarg << std::endl;
                    std::cerr << optarg << " is an argument of gol" << std::endl;
                    exit(0);
                }
                inputFileName = optarg;
                break;
            }

            case 's': {
                if (strcmp("-l", optarg) == 0 || strcmp("--load", optarg) == 0 || strcmp("-g", optarg) == 0 ||
                    strcmp("--generations", optarg) == 0 || strcmp("-m", optarg) == 0 ||
                    strcmp("--measure", optarg) == 0 || strcmp("-t", optarg) == 0 || strcmp("--threads", optarg) == 0 ||
                    strcmp("-d", optarg) == 0 || strcmp("--mode", optarg) == 0) {
                    std::cerr << "Invalid argument for --save: " << optarg << std::endl;
                    std::cerr << optarg << " is an argument of gol" << std::endl;
                    exit(0);
                }
                outputFileName = optarg;
                break;
            }

            case 'g': {
                if (strcmp("-l", optarg) == 0 || strcmp("--load", optarg) == 0 || strcmp("-s", optarg) == 0 ||
                    strcmp("--save", optarg) == 0 || strcmp("-m", optarg) == 0 || strcmp("--measure", optarg) == 0 ||
                    strcmp("-t", optarg) == 0 || strcmp("--threads", optarg) == 0 ||
                    strcmp("-d", optarg) == 0 || strcmp("--mode", optarg) == 0) {
                    std::cerr << "Invalid argument for --generations: " << optarg << std::endl;
                    std::cerr << optarg << " is an argument of gol" << std::endl;
                    exit(0);
                }
                try {
                    generations = stoi(optarg);
                }
                catch (const std::invalid_argument &e) {
                    std::cerr << "Invalid argument for generations: " << optarg << std::endl;
                    exit(0);
                }

                cout << generations << endl;
                exit(1);
                break;
            }

            case 'd' : {
                if (strcmp("-l", optarg) == 0 || strcmp("--load", optarg) == 0 || strcmp("-s", optarg) == 0 ||
                    strcmp("--save", optarg) == 0 || strcmp("-m", optarg) == 0 || strcmp("--measure", optarg) == 0
                    || strcmp("-t", optarg) == 0 || strcmp("--threads", optarg) == 0) {
                    std::cerr << "Invalid argument for --mode: " << optarg << std::endl;
                    std::cerr << optarg << " is an argument of gol" << std::endl;
                    exit(0);
                }

                if (strcmp("seq", optarg) != 0 && strcmp("omp", optarg) != 0) {
                    std::cerr << "Invalid argument for --mode: " << optarg << std::endl;
                    std::cerr << optarg << " --mode options are 'seq' or 'omp'" << std::endl;
                    exit(0);
                }

                if (strcmp("omp", optarg) == 0) {
                    //omp = true;
                }
                break;
            }

            case 't': {
                if (strcmp("-l", optarg) == 0 || strcmp("--load", optarg) == 0 || strcmp("-s", optarg) == 0 ||
                    strcmp("--save", optarg) == 0 || strcmp("-m", optarg) == 0 || strcmp("--measure", optarg) == 0 ||
                    strcmp("-d", optarg) == 0 || strcmp("--mode", optarg) == 0) {
                    std::cerr << "Invalid argument for --threads: " << optarg << std::endl;
                    std::cerr << optarg << " is an option of gol" << std::endl;
                    exit(0);
                }
                try {
                    threads = stoi(optarg);
                }
                catch (const std::invalid_argument &e) {
                    std::cerr << "Invalid argument for threads: " << optarg << std::endl;
                    exit(0);
                }
                break;
            }

            case '?': {
                for (int i = optind; i < argc + 1; i++) {
                    if (strcmp("-l", argv[i - 1]) == 0 || strcmp("--load", argv[i - 1]) == 0) {
                        cerr << "option --load reuires an argument" << endl;
                        exit(0);
                    } else if (strcmp("-s", argv[i - 1]) == 0 || strcmp("--save", argv[i - 1]) == 0) {
                        cerr << "option --save requires an argument" << endl;
                        exit(0);
                    } else if (strcmp("-g", argv[i - 1]) == 0 || strcmp("--generations", argv[i - 1]) == 0) {
                        cerr << "option --generations requires an argument" << endl;
                        exit(0);
                    } else if (strcmp("-d", argv[i - 1]) == 0 || strcmp("--mode", argv[i - 1]) == 0) {
                        cerr << "option --mode requires an argument" << endl;
                        exit(0);
                    } else if (strcmp("-t", argv[i - 1]) == 0 || strcmp("--threads", argv[i - 1]) == 0) {
                        cerr << "option --threads requires an argument" << endl;
                        exit(0);
                    }
                }
                break;
            }
            default: {
                abort();
            }
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf("non-option argument: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        putchar('\n');
        exit(0);
    }
    string empty = "";
    if (inputFileName.compare(empty) == 0) {
        cout << "Provide an input file to initialize the grid." << endl;
        cout << "$ gol -l <input-file>" << endl;
        exit(0);
    }

    /*read file*/

    Timing *timing = Timing::getInstance();

    timing->startSetup();
    ifstream readfile(inputFileName);

    if (!readfile.is_open()) {
        printf("The given file '%s' is not found.\n", inputFileName.c_str());
        return 1;
    }


    string line;

    if (!getline(readfile, line)) {
        cerr << "Error reading input" << endl;
        exit(0);
    }

    stringstream ss(line);
    string token;

    if (!getline(ss, token, ',')) {
        cerr << "Error reading input" << endl;
        exit(0);
    }

    try {
        cols = std::stoi(token);
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "Invalid value for columns: " << token << std::endl;
        exit(0);
    }

    if (!getline(ss, token, ',')) {
        cerr << "Error reading input" << endl;
        exit(0);
    }

    try {
        rows = stoi(token);
    }
    catch (const std::invalid_argument &e) {
        std::cerr << "Invalid value for rows: " << token << std::endl;
        exit(0);
    }

    vector<vector<bool>> grid(rows, vector<bool>(cols, false));
    char c;
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            readfile >> c;
            grid[i][j] = c == 'x';
        }
    }
    timing->stopSetup();

    /*compute generations*/
    timing->startComputation();
    vector<vector<bool>> copy(rows, vector<bool>(cols));
    for (unsigned int gen = 0; gen < generations; gen++) {
        copy = grid;
        #pragma omp parallel num_threads(threads)
        #pragma omp for
        for (unsigned int i = 0; i < rows; i++) {
            for (unsigned int j = 0; j < cols; j++) {
                int alive = copy[((i - 1) + rows) % rows][((j - 1) + cols) % cols] +
                            copy[((i - 1) + rows) % rows][(j + cols) % cols] +
                            copy[((i - 1) + rows) % rows][((j + 1) + cols) % cols] +
                            copy[(i + rows) % rows][((j - 1) + cols) % cols] +
                            copy[(i + rows) % rows][((j + 1) + cols) % cols] +
                            copy[((i + 1) + rows) % rows][((j - 1) + cols) % cols] +
                            copy[((i + 1) + rows) % rows][(j + cols) % cols] +
                            copy[((i + 1) + rows) % rows][((j + 1) + cols) % cols];
                grid[i][j] = state[grid[i][j]][alive-1];
            }
        }
        //determineState(grid, copy, rows, cols, state, threads);
        //determineState(grid, copy, rows, cols);
    }
    timing->stopComputation();

    /*write to file*/
    timing->startFinalization();
    ofstream writeFile(outputFileName);
    writeFile << cols << "," << rows << endl;
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            writeFile << characters[grid[i][j]];
        }
        writeFile << endl;
    }
    timing->stopFinalization();

    if (measure) {
        string timerResult = timing->getResults();
        cout << timerResult << endl;
    }
    return 0;
}
//void determineState(vector<vector<bool>> &grid, vector<vector<bool>> &copy, unsigned int rows, unsigned int cols, unsigned int (&state)[2][8], unsigned int threads) {
/*void determineState(vector<vector<bool>> &grid, vector<vector<bool>> &copy, unsigned int rows, unsigned int cols) {
#pragma omp parallel num_threads(1)
    #pragma omp for
    for (unsigned int i = 0; i < rows; i++) {
        for (unsigned int j = 0; j < cols; j++) {
            int alive = copy[((i - 1) + rows) % rows][((j - 1) + cols) % cols] +
                        copy[((i - 1) + rows) % rows][(j + cols) % cols] +
                        copy[((i - 1) + rows) % rows][((j + 1) + cols) % cols] +
                        copy[(i + rows) % rows][((j - 1) + cols) % cols] +
                        copy[(i + rows) % rows][((j + 1) + cols) % cols] +
                        copy[((i + 1) + rows) % rows][((j - 1) + cols) % cols] +
                        copy[((i + 1) + rows) % rows][(j + cols) % cols] +
                        copy[((i + 1) + rows) % rows][((j + 1) + cols) % cols];
            grid[i][j] = state[grid[i][j]][alive-1];
        }
    }
}*/