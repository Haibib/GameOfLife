/*
Abudukelimu Aibibula
SSEA 106B
Extra Features: Random and GUI
Description: This file implements the logistics behind Conway's Game of Life and uses the lifegui.cpp file to give a gui option for displaying
animations of the game.
*/

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include "console.h"
#include "filelib.h"
#include "grid.h"
#include "gwindow.h"
#include "simpio.h"
#include "lifegui.h"
#include "testing/SimpleTest.h"
#include "vector.h"
#include "random.h"

using namespace std;

/**
 * The loadBoard function takes an already-open
 * ifstream and reads in the width and height of
 * the gameboard. Then it creates and populates
 * the gameboard and closes the ifsteram.
 *
 * Return value: A Grid<char> representing the
 * Game of Life starting game board
 */
//Creates a board based on the dictated file read from an ifstream.
Grid<char> loadBoard(ifstream &ifs) {
    string line="";
    getline(ifs, line);
    int length=stringToInteger(line);
    getline(ifs, line);
    int width=stringToInteger(line);
    Grid<char> board(length, width);
    int r=0;
    while(ifs.eof()==0&&r<length){
        getline(ifs, line);
        char* arr = new char[line.length() + 1];
        strcpy(arr, line.c_str());
        for(int c=0; c<width; c++){
            board[r][c]=arr[c];
        }
        r++;
        delete[] arr;
    }

    return board;
}
//Creates a board with a random width and length populated with random cells.
Grid<char> randomBoard(){
    int length=randomInteger(1, 50);
    int width=randomInteger(1, 50);
    Grid<char> board(length, width);
    for(int r=0; r<board.numRows(); r++){
        for(int c=0; c<board.numCols(); c++){
            if(randomBool()){
                board[r][c]='X';
            }
            else{
                board[r][c]='-';
            }
        }
    }
    return board;
}

/**
 * The printBoard function prints the board to the screen
 */
void printBoard(Grid<char> &gameboard) {
    for(int r=0; r<gameboard.numRows(); r++){
        for(int c=0; c<gameboard.numCols(); c++){
            cout<<gameboard[r][c];
        }
        cout<<endl;
    }
}
//Prints a gui version of the given board.
void printGuiBoard(Grid<char> &gameboard, Grid<char>&previousboard, LifeGUI &gui){
    gui.resize(gameboard.numRows(), gameboard.numCols());
    for(int r=0; r<gameboard.numRows(); r++){
        for(int c=0; c<gameboard.numCols(); c++){
            if(previousboard[r][c]==gameboard[r][c]&&previousboard[r][c]=='-'){
                continue;
            }
            else{
                if(gameboard[r][c]=='X'){
                    gui.drawCell(r, c, true);
                }
                else{
                    gui.drawCell(r, c, false);
                }
            }
        }
    }
}

/**
 * countNeighbors counts the neighbors of a grid squre.
 * '-' is an unpopulated square, and 'X' is a populated square.
 * If wrap is true, then squares that are on edges potentially
 * contribute to the overall count.
 *
 * Returns the neighbor count
 */
int countNeighbors(Grid<char> &gameboard, int row, int col, bool wrap) {
    int count=0;
    if(wrap){
        int y=(row-1+gameboard.numRows())%gameboard.numRows();
        int y2=(row+1+gameboard.numRows())%gameboard.numRows();
        int x=(col-1+gameboard.numCols())%gameboard.numCols();
        int x2=(col+1+gameboard.numCols())%gameboard.numCols();
        if(gameboard[y][x]=='X'){
            count++;
        }
        if(gameboard[row][x]=='X'){
            count++;
        }
        if(gameboard[y2][x]=='X'){
            count++;
        }
        if(gameboard[y][col]=='X'){
            count++;
        }
        if(gameboard[y2][col]=='X'){
            count++;
        }
        if(gameboard[y][x2]=='X'){
            count++;
        }
        if(gameboard[row][x2]=='X'){
            count++;
        }
        if(gameboard[y2][x2]=='X'){
            count++;
        }

    }
    else{
        int y=0;
        int x=0;
        y=row-1;
        if(y<0){
            y++;
        }
        x=col-1;
        if(x<0){
            x++;
        }
        while(y<=row+1&&y<gameboard.numRows()){
            x=col-1;
            if(x<0){
                x++;
            }
            while(x<=col+1&&x<gameboard.numCols()){
                if(!(y==row&&x==col)&&gameboard[y][x]=='X'){
                    count++;
                }
                x++;
            }
            y++;
        }
    }
    return count;
}

/**
 * The tick function runs one iteration of the game of life
 * on gameboard, using wrap to determine if the board should be
 * wrapped or not
 *
 */
void tick(Grid<char> &gameboard, bool wrap) {
    Grid<char> tempboard(gameboard.numRows(), gameboard.numCols());
    int count=0;
    for(int r=0; r<gameboard.numRows(); r++){
        for(int c=0; c<gameboard.numCols(); c++){
            count=countNeighbors(gameboard, r, c, wrap);
            if (count==0||count==1||count>=4||(count==2&&gameboard[r][c]=='-')){
                tempboard[r][c]='-';
            }else {
                tempboard[r][c]='X';
            }
        }
    }
    gameboard=tempboard;
}
//Helper function to set eiher a random board or one from a file.
void setBoard(Grid<char> &gameboard, string &line, ifstream &ifs){
    if(line=="random"||line=="Random"){
        gameboard = randomBoard();
    }
    else{
        gameboard = loadBoard(ifs);
    }
}
//Decides whether to print the board in text-based form or graphical form.
void getGORT(Grid<char> &gameboard, string &line, ifstream &ifs, string decision, bool wrap){
    Grid<char> previousboard(gameboard.numRows(), gameboard.numCols());
    line = getLine("Do you want g)raphical or t)ext-based interface?\n");
    while(!(line=="t"||line=="g")){
        line = getLine("Invalid input. Please input either t for text-based or g for graphical interface.\n");
    }
    if(decision=="t"){
        tick(gameboard, wrap);
        if(line=="t"){
            printBoard(gameboard);
        }
        else{
            LifeGUI gui;
            printGuiBoard(gameboard, previousboard, gui);
        }
    }else{
        int frame=getReal("How many frames?\n", "Illegal integer format. Try again.\n");
        int speed=getReal("How many milliseconds per frame?\n", "Illegal integer format. Try again.\n");
        cout<<"("<<frame<<" new generations are shown, with screen clear and "<<speed<<"ms pause before each)\n"<<endl;
        LifeGUI gui;
        //printGuiBoard(gameboard, previousboard, gui);
        for(int i =0; i<frame; i++){
            previousboard=gameboard;
            tick(gameboard, wrap);
            pause(speed);
            printGuiBoard(gameboard, previousboard, gui);
        }
    }
}
/**
 * The populateGrid function reads lines from the
 * already-open ifstream and populates the already-created
 * gameboard.
 */
void populateGrid(Grid<char> &gameboard, ifstream &ifs) {
    string line;
    line = getLine("Input a valid file name.\n");
    while(!(openFile(ifs, "boards/"+line)||line=="random"||line=="Random")){
        line = getLine("Invalid file name. Please input a valid file name\n");
    }
    setBoard(gameboard, line, ifs);
    string temp;
    do{
        while(!(line=="a"||line=="t"||line=="q")){
            line = getLine("a)nimate, t)ick, q)uit?\n");
        }
        temp=line;
        if(line!="q"){
            getGORT(gameboard, line, ifs, line, getYesOrNo("Do you want to wrap (y/n)?\n"));
        }
    }while(temp!="q");
}

// You should have more functions here
//Starts the function chain and runs the tests
int main() {
    ifstream ifs;
    Grid<char> gameboard;
    populateGrid(gameboard, ifs);
    if (runSimpleTests(SELECTED_TESTS)) {
        return 0;
    }
    ifs.close();

    cout << "Have a nice Life!" << endl;
    return 0;
}

// used for creating tests
void saveGame(Grid<char> gameboard, string filename) {
    ofstream ofs;
    ofs.open(filename, std::fstream::out);
    ofs << gameboard.numRows() << endl;
    ofs << gameboard.numCols() << endl;
    for (int row = 0; row < gameboard.numRows(); row++) {
        for (int col = 0; col < gameboard.numCols(); col++) {
            ofs << gameboard[row][col];
        }
        ofs << endl;
    }
    ofs.close();
}

PROVIDED_TEST("Test loadBoard function on simple.txt") {
    Grid<char> gameboardSoln = {
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', 'X', 'X', 'X', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'}
    };
    ifstream ifs;
    openFile(ifs, "boards/simple.txt");
    Grid<char> gameboard = loadBoard(ifs);
    EXPECT_EQUAL(gameboard, gameboardSoln);
}

PROVIDED_TEST("Test loadBoard function on format.txt") {
    Grid<char> gameboardSoln = {
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', 'X', 'X', 'X', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'}
    };
    ifstream ifs;
    openFile(ifs, "boards/format.txt");
    Grid<char> gameboard = loadBoard(ifs);
    EXPECT_EQUAL(gameboard, gameboardSoln);
}

PROVIDED_TEST("Test loadBoard function on stableplateau.txt") {
    Grid<char> gameboardSoln = {
        {'-', '-', '-', '-', '-', '-', '-', '-', 'X', 'X'},
        {'-', '-', '-', '-', '-', '-', '-', '-', 'X', 'X'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', 'X', '-', '-', '-', '-'},
        {'-', '-', '-', '-', 'X', '-', 'X', '-', '-', '-'},
        {'-', '-', '-', '-', 'X', '-', 'X', '-', '-', '-'},
        {'-', '-', '-', '-', '-', 'X', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'X', 'X', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'X', 'X', '-', '-', '-', '-', '-', '-', '-', '-'}
    };
    ifstream ifs;
    openFile(ifs, "boards/stableplateau.txt");
    Grid<char> gameboard = loadBoard(ifs);
    EXPECT_EQUAL(gameboard, gameboardSoln);
}

PROVIDED_TEST("Test simple.txt for one iteration") {
    ifstream ifs;
    openFile(ifs, "boards/simple.txt");
    Grid<char> gameboard = loadBoard(ifs);

    ifstream ifs2;
    openFile(ifs2, "boards-after-one-iteration/simple-after1.txt");
    Grid<char> gameboardAfterOne = loadBoard(ifs2);

    tick(gameboard, false);
    EXPECT_EQUAL(gameboard, gameboardAfterOne);
}

PROVIDED_TEST("Test all boards for one iteration, no wrapping") {
    Vector<string> files = {
        "diehard", "dinner-table", "fish", "flower",
        "format", "glider-explosion", "glider-gun",
        "glider", "quilt-square", "rpent", "seeds",
        "simple", "simple-edge", "snowflake", "spiral",
        "stableplateau", "tictactoe"
    };

    for (string &filename : files) {
        string boardFilename = "boards/" + filename + ".txt";
        string boardAfter1Filename =
                "boards-after-one-iteration/" +  filename + "-after1.txt";
        cout << boardFilename << endl;
        cout << boardAfter1Filename << endl;
        ifstream ifs;
        openFile(ifs, boardFilename);
        Grid<char> gameboard = loadBoard(ifs);
        tick(gameboard, false);

        ifstream ifs2;
        openFile(ifs2, boardAfter1Filename);
        Grid<char> gameboardAfter1 = loadBoard(ifs2);

        EXPECT_EQUAL(gameboard, gameboardAfter1);
//        saveGame(gameboard, boardAfter1Filename);
    }
}

PROVIDED_TEST("Test simple-edge for one iteration, with wrapping") {
    ifstream ifs;
    openFile(ifs, "boards/simple-edge.txt");
    Grid<char> gameboard = loadBoard(ifs);

    ifstream ifs2;
    openFile(ifs2, "boards-after-one-iteration/simple-edge-after1-wrap.txt");
    Grid<char> gameboardAfterOne = loadBoard(ifs2);

    tick(gameboard, true);
    EXPECT_EQUAL(gameboard, gameboardAfterOne);
}

STUDENT_TEST("Empty board"){
    Grid<char> gameboardSoln = {
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'}
    };
    Grid<char> gameboard = {
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-'}
    };
    tick(gameboard, true);
    EXPECT_EQUAL(gameboard, gameboardSoln);
}
STUDENT_TEST("Empty loadBoard"){
    Grid<char> gameboardSoln(0,0);
    ifstream ifs;
    openFile(ifs, "boards/empty.txt");
    Grid<char> gameboard = loadBoard(ifs);
    EXPECT_EQUAL(gameboard, gameboardSoln);
}


