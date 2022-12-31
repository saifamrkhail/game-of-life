// Author: Saifur Rahman Rahmani ai22m055
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <getopt.h>
#include <vector>
#include "Timing.h"

using namespace std;
void printGrid(vector<vector<bool>> &grid);
void determineState(vector<vector<bool>> &grid, int rows, int cols);

/* Flag set by ‘--verbose’. */
static int verbose_flag;
int main(int argc, char *argv[])
{
    //clear scren
    cout << "\033[2J\033[1;1H";
    string inputFileName, outputFileName = "";
    int generations = 250;
    int measure = 0;
    // parse cmd arguments
    int arguments;
    while (true)
    {
        static struct option long_options[] =
                {
                    /* These options set a flag. */
                    {"verbose", no_argument, &verbose_flag, 1},
                    {"brief", no_argument, &verbose_flag, 0},
                    /* These options don’t set a flag. We distinguish them by their indices. */
                    {"load", required_argument, 0, 'l'},
                    {"save", required_argument, 0, 's'},
                    {"generations", required_argument, 0, 'g'},
                    {"measure", no_argument, 0, 'm'},
                    {0, 0, 0, 0}
                };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        arguments = getopt_long(argc, argv, "ml:s:g:", long_options, &option_index);

        /* Detect the end of the options. */
        if (arguments == -1)
        {
            break;
        }

        switch (arguments)
        {
            case 0:
            {
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                {
                    break;
                }
                printf("option %s", long_options[option_index].name);
                if (optarg)
                {
                    printf(" with arg %s", optarg);
                }
                printf("\n");
                break;
            }

            case 'm':
            {
                measure = 1;
                break;
            }

            case 'l':
            {
                if (optarg == NULL) {
                    cerr << "No input file specified." << endl;
                    return 1;
                } else {
                    inputFileName = optarg;
                }

                break;
            }

            case 's':
            {
                if (optarg == NULL) {
                    cerr << "No writeFile file specified" << endl;
                    return 1;
                } else {
                    outputFileName = optarg;
                }

                break;
            }

            case 'g':
            {
                if (optarg == NULL) {
                    cerr << "The number of generation is not specified." << endl;
                    return 1;
                } else {
                    generations = stoi(optarg) ;
                }
                break;
            }

            case '?':
            {
                /* getopt_long already printed an error message. */
                break;
            }
            default:
            {
                abort();
            }
        }
    }
    /* Instead of reporting ‘--verbose’
       and ‘--brief’ as they are encountered,
       we report the final status resulting from them. */
    if (verbose_flag)
    {
        puts("verbose flag is set");
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf("%s ", argv[optind++]);
        }
        putchar('\n');
        exit(0);
    }

    Timing* timing = Timing::getInstance();
    timing->startSetup();

    std::ifstream readfile(inputFileName);
    if (!readfile.is_open())
    {
        printf("The given file '%s' is not found.\n", optarg);
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

    //printGrid(grid);

    timing->stopComputation();
    timing->startFinalization();

    //write the state of the grid
    ofstream writeFile(outputFileName);
    writeFile << cols << "," << rows << endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (grid[i][j]) {
                writeFile << 'x';
            }
            else {
                writeFile << '.';
            }
        }

        writeFile << endl;
    }

    timing->stopFinalization();

    if (measure) {
        string timerResult = timing->getResults();
        cout << timerResult << endl;
    } else {
        cout << "executed successfully" << endl;
    }

    return 0;
}

void printGrid(vector<vector<bool>> &grid)
{
    for (size_t i = 0; i < grid.size(); i++)
    {
        for (size_t j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] == true)
            {
                cout << "x";
            }
            else
            {
                cout << ".";
            }
            if (j == grid[i].size() - 1)
            {
                cout << endl;
            }
        }
    }
}

void determineState(vector<vector<bool>> &grid, int rows, int cols)
{
    vector<vector<bool>> gridCp(rows, vector<bool>(cols));
    //copy grid
    for (size_t i = 0; i < grid.size(); i++)
    {
        for (size_t j = 0; j < grid[i].size(); j++)
        {
            gridCp[i][j] = grid[i][j];
        }
    }

    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < cols; j++)
        {
            int alive = 0;
            for (int m = -1; m < 2; m++)
            {
                for (int n = -1; n < 2; n++)
                {
                    if (!(m == 0 && n == 0))
                    {
                        int r = ((i + m) + rows) % rows;
                        int c = ((j + n) + cols) % cols;
                        if (gridCp[r][c])
                        {
                            ++alive;
                        }
                    }
                }
            }
            if (alive < 2)
            {
                grid[i][j] = false;
            }
            else if (alive == 3)
            {
                grid[i][j] = true;
            }
            else if (alive > 3)
            {
                grid[i][j] = false;
            }
        }
    }
}