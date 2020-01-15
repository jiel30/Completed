
#include <iostream>
#include "p4.hpp"
using namespace std;
#include <vector>

int main()
{
    cout << "start" << endl;
    Othello a{8,8};
    a.start();
    a.set_board();
    a.set_ospace();
    a.set_xspace();
    a.print_board();
    if(a.legal_move(2, 4, 2))
    {
        cout << "true" << endl;
    }
    for (piece aa:a.ospace)
    {
        cout << aa.color << " " << aa.column << " " << aa.row << endl;
    }
    string get_index = a.get_input();
    if(get_index =="Q")
    {
        cout << "Bye!" << endl;
    }
    if (get_index == "invalid")
    {
        cout << "invalid";
    }
    unsigned int cc = stoi(get_index.substr(0,1));
    unsigned int rr = stoi(get_index.substr(1,1));
    int* x = a.same_r_index(5, 4, 3, 4, 1);
    int* xx =a.same_r_index( 2, 3,4, 3, 1);
    int* y = a.same_c_index(4,3,4,5, 1);
    int* z = a.all_diff_index(4, 3, 2, 5, 1);
    for (int aa = 0; aa < 5; aa++)
    {
        cout <<x[aa] <<" ";
    }
    cout << endl;
    for (int aa = 0; aa < 5; aa++)
    {
        cout <<xx[aa] <<" ";
    }
    cout << endl;
    a.print_board();
//    int* rowc = a.rowcheck(cc, rr, a.turn);
//    int* columnc = a.columncheck(cc, rr, a.turn);
//    for (int aa = 0; aa < 6; aa++)
//    {
//        cout <<rowc[aa] << endl;
//    }
////    cout <<rowc[0] <<rowc[1]<<rowc[2]<<rowc[3]<<rowc[4]<<rowc[5] << endl;
//    cout <<columnc[0] <<columnc[1]<<columnc[2]<<columnc[3]<<columnc[4]<<columnc[5] << endl;
//    int* dc = a.diagonalcheck(cc, rr, a.turn);
//    cout <<dc[0] <<dc[1]<<dc[2]<<dc[3]<<dc[4]<<dc[5] << endl;
//    a.move(4, 5, a.turn);
//    a.flip(4, 5, a.turn);
//    a.move(3, 2, a.turn);
//    a.flip(3, 2, a.turn);
////    a.move(2, 3, a.turn);  //检查rowflip
////    a.flip(2, 3, a.turn);
//    a.print_board();
    return 0;
}


