// Author: Mario Talevski
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <vector>

using namespace std;

void printGrid(vector<vector<bool>> &grid);
void determineState(vector<vector<bool>> &grid);
void clearScreen(void);

/* Flag set by ‘--verbose’. */
static int verbose_flag;
int main(int argc, char *argv[])
{
    clearScreen();
    vector<vector<bool>> grid;

    string filename;
    cout << "                         THE GAME OF life - Implementation in C++" << endl;
    cout << endl;
    cout << endl;
    cout << endl;
    cout << "Also known simply as life, " << endl;
    cout << "is a cellular automaton devised by the British mathematician John Horton Conway in 1970." << endl;
    cout << endl;
    cout << "Rules" << endl;
    cout << "The universe of the Game of life is an infinite two-dimensional orthogonal grid of square cells," << endl;
    cout << "each of which is in one of two possible states, life or dead. Every" << endl;
    cout << "cell interacts with its eight neighbours, which are the cells that are horizontally, vertically, or diagonally adjacent." << endl;
    cout << "At each step in time, the following transitions occur:" << endl;
    cout << "1. Any live cell with fewer than two live neighbours dies, as if caused by under-population." << endl;
    cout << "2. Any live cell with two or three live neighbours lives on to the next generation." << endl;
    cout << "3. Any live cell with more than three live neighbours dies, as if by over-population." << endl;
    cout << "4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction." << endl;
    cout << endl;
    cout << "x - living cell" << endl;
    cout << ". - dead cell" << endl;
    cout << endl;

    // parse cmd arguments
    int arguments;

    while (1)
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
                        {0, 0, 0, 0}};
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
                puts("option m\n");
                break;
            }

            case 'l':
            {
                printf("option -l with value `%s'\n", optarg);
                // read game board from file
                ifstream readfile(optarg);

                if (readfile.is_open())
                {
                    string fileline;
                    bool isFirstLine = true;
                    int i = 0;
                    while (getline(readfile, fileline))
                    {
                        if (isFirstLine)
                        {
                            isFirstLine = false;
                            stringstream ss(fileline);
                            vector <string> numbers;
                            string temp;
                            while (getline(ss, temp, ',')) {
                                numbers.push_back(temp);
                            }
                            const unsigned int rows = stoi(numbers[0]);
                            const unsigned int cols = stoi(numbers[1]);
                            grid.resize(rows, vector<bool>(cols));
                        } else {
                            for (size_t j = 0; j < grid[i].size(); j++)
                            {
                                grid[i][j] = fileline[j] == 'x';
                            }
                            i++;
                        }
                    }
                }
                else
                {
                    cout << "No such file, try again." << endl;
                }

                break;
            }

            case 's':
            {
                printf("option -s with value `%s'\n", optarg);
                break;
            }

            case 'g':
            {
                printf("option -g with value `%s'\n", optarg);
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

    while (true)
    {
        printGrid(grid);
        determineState(grid);
        usleep(200000);
        clearScreen();
    }
}

void clearScreen(void)
{
    cout << "\033[2J\033[1;1H";
}

void printGrid(vector<vector<bool>> &grid)
{
    for (size_t i = 0; i < grid.size(); i++)
    {
        for (size_t j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] == true)
            {
                cout << " x ";
            }
            else
            {
                cout << " . ";
            }
            if (j == grid[i].size() - 1)
            {
                cout << endl;
            }
        }
    }
}

void copyGrid(vector<vector<bool>> &grid, vector<vector<bool>> &gridCp)
{
    for (size_t i = 0; i < grid.size(); i++)
    {
        for (size_t j = 0; j < grid[i].size(); j++)
        {
            gridCp[i][j] = grid[i][j];
        }
    }
}

void determineState(vector<vector<bool>> &grid)
{
    vector<vector<bool>> gridCp(grid.size(), vector<bool>(grid[0].size()));
    copyGrid(grid, gridCp);
    int rows = grid.size();
    int cols = grid[0].size();
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