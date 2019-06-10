/*************************************************************************************
 ** Program Name: Project1 (Langton Ant)
 ** Author: Jeremy Einhorn
 ** Date: July 7, 2017
 ** Description: This is the main file.  Options are set and the Langton Ant begins.
 **************************************************************************************/

#include <iostream>
#include"antPrint.hpp"
#include"ant.hpp"
#include"board.hpp"
#include"input_validation.hpp"
#include"range_limit.hpp"
#include <cstdlib>
#include <getopt.h>
using std::cout;
using std::cin;
using std::endl;
#ifdef linux
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#ifdef WINDOWS
#include <windows.h>
#endif

void sleep(int sleepMs) {
#ifdef linux
    usleep(sleepMs * 1000);
#endif
#ifdef WINDOWS
    Sleep(sleepMs);
#endif
}

void printHelp() {
    std::cout <<
        "--rsize <n>: set row size (max=80,min=2,default=20)\n"
        "--csize <n>: set column size (max=80,min=2,default=50)\n"
        "--steps <n>: steps the ant will take (default=200)\n"
        "--delay <n>: delay between steps, in milliseconds (default=0)\n"
        "--antx  <n>: ant starting X coordinate\n"
        "--anty  <n>: ant starting Y coordinate\n"
        "--debug:     step iteratively\n"
        "--help:      show help\n";
    exit(1);
}


void clearScreen() {
#ifdef WINDOWS
    std::system("cls");
#else
    std::system ("clear");
#endif
}

void getWinSize(int *x, int *y){
#ifdef linux
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    *x = w.ws_row - 1;
    *y = w.ws_col - 1;
#else // assert this works on Windows
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns, rows;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    x = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#endif
}

int main(int argc, char** argv) {
    //int variables to hold the user entered row size
    //column size, the x coord of the ant and
    //the y coord of the ant.  The accum variable
    //keeps track of what step the ant is on

    int antx, anty, step, rsize, csize, debug, delay, accum = 0;
    const char* const short_opts = "r:c:s:d:Dh";
    const option long_opts[] = {
        {"rsize" , required_argument , nullptr , 'r'} ,
        {"csize" , required_argument , nullptr , 'c'} ,
        {"steps" , required_argument , nullptr , 's'} ,
        {"delay" , required_argument , nullptr , 'd'} ,
        {"debug" , no_argument       , nullptr , 'D'} ,
        {"help"  , no_argument       , nullptr , 'h'} ,
        {nullptr , no_argument       , nullptr ,  0 }
    };
    while (true) { // parse arguments
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
        if (opt == -1) break;
        switch (opt) {
            case 'r':
                rsize = std::stoi(optarg);
                break;
            case 'c':
                csize = std::stoi(optarg);
                break;
            case 's':
                step = std::stoi(optarg);
                break;
            case 'd':
                delay = std::stoi(optarg);
                break;
            case 'D':
                debug = 1;
                break;
            case 'h':
            case '?':
            default:
                printHelp();
                break;
        }
    }

    if(!rsize || !csize) getWinSize(&rsize, &csize);

    antx = rand() % rsize + 2;
    anty = rand() % csize + 2;

    if(!step) step = 1000000;

    //dynamic allocation of a 2D array using the user entered
    //row size and column size
    char **board_arr = new char*[rsize];
    for (int row = 0; row < rsize; row++)
        board_arr[row] = new char[csize];

    //new Board and Ant objects using the 2D array,
    //the row and column size and
    //the ants first coords
    Board *board = new Board(board_arr, rsize, csize);
    Ant ant(board, antx, anty);

    //keeps going until the last step is reached
    while (accum < step) {
        accum += 1;

        //int variables to hold the ants coords before it moves
        int arow = ant.getRow();
        int acol = ant.getCol();
        //uses the coords to move the ant
        ant.moveAnt(arow, acol);

        //prints a board with an ant on it
        clearScreen();
        printToScreen(board, ant);
        sleep(delay);

        //int variables hold the ants updates coords
        //these will be used to let the user know where
        //exactly the ant is
        int newRow = ant.getRow();
        int newCol = ant.getCol();

        if(debug){
            cout << "This is step " << accum << "." << endl;
            cout << "The ant is at position " << newRow << ", "
                << newCol << endl;
            cout << "Please hit enter for the next step." << endl;

            //user must hit enter to move onto the next step
            cin.get();
        }
    }

    //frees the space used for the dynamically allocated 2D array
    for (int row = 0; row < rsize; row++)
        delete[] board_arr[row];
    delete[] board_arr;
}
