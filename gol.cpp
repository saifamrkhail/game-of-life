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
#include "Timing.h"

using namespace std;

void determineState(vector<vector<bool>> &grid, int rows, int cols);

int main(int argc, char *argv[]) {
    //clear scren
    cout << "\033[2J\033[1;1H";
    string inputFileName, outputFileName = "";
    int generations = 250;
    int measure = 0;

    //parse command line arguments

    int arguments;
    while (true) {
        static struct option long_options[] =
                {
                        {"load",        required_argument, 0, 'l'},
                        {"save",        required_argument, 0, 's'},
                        {"generations", required_argument, 0, 'g'},
                        {"measure",     no_argument,       0, 'm'},
                        {"help",        no_argument,       0, 'h'}
                };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        arguments = getopt_long(argc, argv, "mhl:s:g:", long_options, &option_index);

        /* Detect the end of the options. */
        if (arguments == -1) {
            break;
        }

        switch (arguments) {
            case 0: {
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0) {
                    break;
                }
                printf("option %s", long_options[option_index].name);
                if (optarg) {
                    printf(" with arg %s", optarg);
                }
                printf("\n");
                break;
            }

            case 'h': {
                cout << "usage: "
                     << "gol [--load | -l <filename>] [--save | -s <filename>] [--generations | -g <n>] [--measure | -m]"
                     << endl;
                cout << "[--load | -l] <filename> (filename to read input board)" << endl;
                cout << "[--save | -s] <filename> (filename to save output board)" << endl;
                cout << "[--generations | -g] <n> (run n generations)" << endl;
                cout << "[--measure | -m] (print time measurements)" << endl;
                exit(1);
            }

            case 'm': {
                measure = 1;
                break;
            }

            case 'l': {
                if (strcmp("-g", optarg) == 0
                    || strcmp("--generations", optarg) == 0
                    || strcmp("-s", optarg) == 0
                    || strcmp("--save", optarg) == 0) {
                    std::cerr << "Invalid argument for --load: " << optarg << std::endl;
                    std::cerr << optarg << " is an argument of gol" << std::endl;
                    exit(0);
                }
                inputFileName = optarg;
                break;
            }

            case 's': {
                if (strcmp("-l", optarg) == 0
                    || strcmp("--load", optarg) == 0
                    || strcmp("-g", optarg) == 0
                    || strcmp("--generations", optarg) == 0) {
                    std::cerr << "Invalid argument for --save: " << optarg << std::endl;
                    std::cerr << optarg << " is an argument of gol" << std::endl;
                    exit(0);
                }
                outputFileName = optarg;
                break;
            }

            case 'g': {
                if (strcmp("-l", optarg) == 0
                    || strcmp("--load", optarg) == 0
                    || strcmp("-s", optarg) == 0
                    || strcmp("--save", optarg) == 0) {
                    std::cerr << "Invalid argument for --generations: " << optarg << std::endl;
                    std::cerr << optarg << " is an argument of gol" << std::endl;
                    exit(0);
                }
                try {
                    generations = stoi(optarg);
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Invalid argument for generations: " << optarg << std::endl;
                    exit(0);
                }
                break;
            }

            case '?': {
                for (size_t i = optind; i < argc + 1; i++) {
                    if (strcmp("-l", argv[i - 1]) == 0 || strcmp("--load", argv[i - 1]) == 0) {
                        cerr << "option --load reuires an argument" << endl;
                        exit(0);
                    } else if (strcmp("-s", argv[i - 1]) == 0 || strcmp("--save", argv[i - 1]) == 0) {
                        cerr << "option --save requires an argument" << endl;
                        exit(0);
                    } else if (strcmp("-g", argv[i - 1]) == 0 || strcmp("--generations", argv[i - 1]) == 0) {
                        cerr << "option --generations requires an argument" << endl;
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

    Timing *timing = Timing::getInstance();

    timing->startSetup();
    std::ifstream readfile(inputFileName);
    if (!readfile.is_open()) {
        printf("The given file '%s' is not found.\n", inputFileName.c_str());
        return 1;
    }
    int rows, cols = 0;
    string line;
    getline(readfile, line);
    stringstream ss(line);
    string token;
    getline(ss, token, ',');
    cols = std::stoi(token);
    getline(ss, token, ',');
    rows = stoi(token);

    std::vector<std::vector<bool>> grid(rows, std::vector<bool>(cols, false));
    char c;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            readfile >> c;
            if (c == 'x') {
                grid[i][j] = true;
            }
        }
    }
    timing->stopSetup();

    timing->startComputation();
    for (int i = 0; i < generations; i++) {
        determineState(grid, rows, cols);
    }
    timing->stopComputation();

    timing->startFinalization();
    //write grid to file
    ofstream writeFile(outputFileName);
    writeFile << cols << "," << rows << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j]) {
                writeFile << 'x';
            } else {
                writeFile << '.';
            }
        }
        writeFile << endl;
    }
    timing->stopFinalization();
    
    cout << "executed successfully" << endl;
    if (measure) {
        string timerResult = timing->getResults();
        cout << timerResult << endl;
    }
    return 0;
}

void determineState(vector<vector<bool>> &grid, int rows, int cols) {
    vector<vector<bool>> gridCp(rows, vector<bool>(cols));
    //copy grid
    for (size_t i = 0; i < grid.size(); i++) {
        for (size_t j = 0; j < grid[i].size(); j++) {
            gridCp[i][j] = grid[i][j];
        }
    }

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            int alive = 0;
            for (int m = -1; m < 2; m++) {
                for (int n = -1; n < 2; n++) {
                    if (!(m == 0 && n == 0)) {
                        int r = ((i + m) + rows) % rows;
                        int c = ((j + n) + cols) % cols;
                        if (gridCp[r][c]) {
                            ++alive;
                        }
                    }
                }
            }
            if (alive < 2) {
                grid[i][j] = false;
            } else if (alive == 3) {
                grid[i][j] = true;
            } else if (alive > 3) {
                grid[i][j] = false;
            }
        }
    }
}