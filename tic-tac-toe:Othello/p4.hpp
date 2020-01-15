//Name: Jie Li
//#52536988
//get some idea from
//https://www.geeksforgeeks.org/implementation-of-tic-tac-toe-game/

#ifndef P4_HPP_
#define P4_HPP_
#include <iostream>
#include <vector>
#include <typeinfo>
using namespace std;

struct piece
{
    unsigned int color;   //0 is empty, 1 is black, 2 is white, 3 is unknown
    unsigned int column;
    unsigned int row;
};

unsigned int get_index(unsigned int c,unsigned int r,unsigned int row)
{
    return row * r + c;  //the index of board equal to row * row_number + column_number
}

class Chess
{
public:
    unsigned int turn;   // for turn 1 means it is black's turn, 2 means it is white's turn, black starts first
    unsigned int column;
    unsigned int row;
    unsigned int board_length;
    vector<unsigned int> board;   //build by 0,1,2  0 is empty which is . and 1 is black which is X, 2 is white which is O
    vector<piece> used_space;
    vector<piece> unused_space;
public:

    Chess& operator=(const Chess& c);
    virtual void set_board() = 0;
    void print_board();
    piece*  index_to_rc(unsigned int index);
    virtual bool legal_move(unsigned int c, unsigned int r, unsigned int color) = 0;
    virtual bool Move(unsigned int c, unsigned int r, unsigned int color);
    virtual unsigned int winner_check() = 0;
    virtual void start() = 0;
    virtual void print_rule() = 0;
    virtual string get_input() = 0;
    virtual void used_set();
    virtual void unused_set();
};

Chess& Chess::operator=(const Chess& c)
{
    if(this != &c)
    {
        column = c.column;
        row = c.row;
        board = c.board;
        turn = c.turn;
        used_space = c.used_space;
        unused_space = c.unused_space;
        board_length = c.board_length;
    }
    return *this;
}


void Chess::print_board()
{
    cout << "  ";
    for (int aa = 0; aa < column; aa++)
    {
        cout << aa << " ";
    }
    cout << endl;
    for (int bb = 0; bb < row; bb ++)
    {
        cout << bb;
        for (int cc = 0; cc < column; cc++)
        {
            if (board[cc+row*bb] == 0)
            {
                cout << " .";
            }
            if (board[cc+row*bb] == 1)
            {
                cout << " X";
            }
            if (board[cc+row*bb] == 2)
            {
                cout << " O";
            }
        }
        cout << endl;
    }
}
piece* Chess:: index_to_rc(unsigned int index)
{
    piece* a = new piece;
    unsigned int c = index % column;
    unsigned int r = index / column;
//    cout << column;
//    cout << r << " " << c << endl;
    a-> column = c;
    a-> row = r;
    a-> color = 0;
    return a;
}
bool Chess:: Move(unsigned int c, unsigned int r, unsigned int color)
{
    if( not legal_move(c,r,color) )
    {
        return false;
    }
    else
    {
        unsigned int index = c+r*row;
        board[index] = color;
        if(turn == 1)
        {
            turn = 2;
        }
        else
        {
            turn = 1;
        }
        return true;
    }
}
void Chess:: used_set()
{
    for (int aa = 0; aa < board_length; aa++)
    {
        if (board[aa] != 0)
        {
            piece* check = new piece;
            check = index_to_rc(aa);
            check-> color = board[aa];
            used_space.push_back(*check);
        }
    }
}
void Chess:: unused_set()
{
    for (int aa = 0; aa < board_length; aa++)
    {
        if (board[aa] == 0)   //那个点为空格（0）
        {
            unused_space.push_back(*index_to_rc(aa));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//part for TTT
class TTT:public Chess
{
public:
    TTT(unsigned int c, unsigned int r);
    void set_board() override;
    TTT& operator=(const TTT& t);
    //    ~TTT();
    virtual bool legal_move(unsigned int c, unsigned int r, unsigned int color) override;
    unsigned int winner_check() override;
    virtual void start() override;
    virtual void print_rule() override;
    virtual string get_input() override;
    bool check_full();
private:
    unsigned int rowcheck()
    {
        for (int aa = 0; aa < row; aa++)
        {
            if(board[get_index(0,aa,row)] == board[get_index(1,aa,row)] and board[get_index(1,aa,row)] == board[get_index(2,aa,row)] and
               board[get_index(0,aa,row)] != 0)
            {
                return board[get_index(0,aa,row)];
            }
        }
        return 0;
    }
    unsigned int columncheck()
    {
        for (int aa = 0; aa < column; aa++)
        {
            if(board[get_index(aa,0,row)] == board[get_index(aa,1,row)] and board[get_index(aa,1,row)] == board[get_index(aa,2,row)] and
               board[get_index(aa,0,row)] != 0)
            {
                return board[get_index(aa,0,row)];
            }
        }
        return 0;
    }
    unsigned int diagonalcheck()
    {
        if (board[get_index(0,0,row)] == board[get_index(1,1,row)] and
            board[get_index(1,1,row)] == board[get_index(2,2,row)] and
            board[get_index(0,0,row)] != ' ')
        {
            return board[get_index(0,0,row)];
        }
        if (board[get_index(0,2,row)] == board[get_index(1,1,row)] and
            board[get_index(1,1,row)] == board[get_index(2,0,row)] and
            board[get_index(0,2,row)] != ' ')
        {
            return board[get_index(0,2,row)];
        }
        return 0;
    }

};
void TTT::set_board()
{
    for (int aa = 0; aa < board_length; aa++)
    {
        board.push_back(0);
    }
}

TTT& TTT::operator=(const TTT& t)
{
    if(this != &t)
    {
        column = t.column;
        row = t.row;
        board = t.board;
        turn = t.turn;
        used_space = t.used_space;
        unused_space = t.unused_space;
        board_length = t.board_length;
    }
    return *this;
}
TTT::TTT(unsigned int c, unsigned int r)
{
    column = c;
    row = r;
    board_length = r*c;
    turn = 1;
};

unsigned int TTT::winner_check()
{
    if (rowcheck() != 0)
    {
        return rowcheck();
    }
    if (columncheck() != 0)
    {
        return columncheck();
    }
    if (diagonalcheck() != 0)
    {
        return diagonalcheck();
    }
    return 0;
}

//TTT::~TTT()
//{
//    for(piece aa : used_space)
//    {
//        delete &aa;
//    }
//}

void TTT::print_rule()
{
    cout << "Welcome to Tic-Tac-Toe game" << endl;
    cout << "           Rule:             " << endl;
    cout << "This is a two player game, there are two types of chess pieces" << endl;
    cout << "X(black) and O(white), . represent empty space" << endl;
    cout << "enter column space row color to move" << endl;
    cout << "EX: '2 1 X' means put black in column 2 and row 1" << endl;
    cout << "enter Q to quit" << endl;
    cout << "Have fun!" << endl;
}
string TTT::get_input()
{
    string cc;
    cin >> cc;
    if (cc == "Q")
    {
        return "QUIT";
    }
    string rr;
    string col;
    cin >> rr >> col;
    if (cc != "0" and cc != "1" and cc != "2")
    {
        return "invalid";
    }
    if (rr != "0" and rr != "1" and rr != "2")
    {
        return "invalid";
    }
    if (col != "X" and col != "O")
    {
        return "invalid";
    }
    return cc + rr + col;
}

bool TTT::legal_move(unsigned int c, unsigned int r, unsigned int color)
{
    if (color != turn)
    {
        return false;
    }
    for(piece aa: used_space)
    {
        if (aa.column == c and aa.row == r)
        {
            return false;
        }
    }
    return true;
}
//check if the board is full, if it is full, return true
bool TTT::check_full()
{
    for (int aa:board)
    {
        if (aa == 0)
        {
            return false;
        }
    }
    return true;
}

void TTT::start()
{
    print_rule();
    set_board();
    print_board();
    used_set();
    unused_set();
    while (true)
    {
        string get_index;
        get_index = get_input();
        if(get_index == "QUIT")
        {
            cout << "Bye!" << endl;
            break;
        }
        if(get_index == "invalid")
        {
            cout <<"invalid" << endl;
        }
        else
        {
            unsigned int cc = stoi(get_index.substr(0,1));
            unsigned int rr = stoi(get_index.substr(1,1));
            unsigned int col;
            if(get_index.substr(2,1) == "O")
            {
                col = 2;
            }
            else
            {
                col = 1;
            }
            bool check = legal_move(cc, rr, col);
            if ( not check)
            {
                cout << "invalid" << endl;
            }
            else
            {
                Move(cc, rr, col);
                piece* newone = new piece;
                newone-> color = col;
                newone -> row = rr;
                newone-> column = cc;
                used_space.push_back(*newone);
                int ind = 0;
                for (piece aa : unused_space)
                {
                    if (aa.color == col and aa.column == cc and aa.row == rr)
                    {
                        unused_space.erase(unused_space.begin() + ind);
                    }
                    ind ++;
                }
                unsigned int winner = winner_check();
                if (winner == 1)
                {
                    print_board();
                    cout << "X wins" << endl;
                    break;
                }
                if (winner == 2)
                {
                    print_board();
                    cout << "O wins" << endl;
                    break;
                }
                if (check_full())
                {
                    cout << "it is full, game over"  << endl;
                    break;
                }
                cout << endl;
                print_board();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////

class Othello:public Chess
{
public:
    vector<piece> xspace;
    vector<piece> ospace;
public:
    Othello(unsigned int c, unsigned int r); //done
    Othello& operator=(const Othello& o); //done
    virtual void set_board() override;  //done
    virtual bool legal_move(unsigned int c, unsigned int r, unsigned int color) override; //done
    virtual unsigned int winner_check() override; //done
    virtual void start() override;
    virtual void print_rule() override; //done
    virtual string get_input() override; //done
    void flip(unsigned int c, unsigned int r, unsigned int color); //done
    void do_flip(unsigned int c1, unsigned int r1, unsigned int c2, unsigned int r2, unsigned int col); //done
    void set_xspace();   //done
    void set_ospace();  //done
    void move(unsigned int c, unsigned int r, unsigned int color);
    int* same_c_index(unsigned int c1, unsigned int r1,unsigned int c2, unsigned int r2, unsigned int color);
    int* same_r_index(unsigned int c1, unsigned int r1,unsigned int c2, unsigned int r2, unsigned int color);
    int* all_diff_index(unsigned int c1, unsigned int r1,unsigned int c2, unsigned int r2, unsigned int color);
public:
    void array_init(int* a)
    {
        for (int aa = 0; aa < 6; aa++)
        {
            a[aa] = 0;
        }
    }
    //these checker is going to check different parts to see if we can put pieces in one position
    //it is going to return one array, the first will be 0 or 1, 0 means this position is avaliable, 0 means illegal
    //the second unsigned int can be 2,4. it means the number of coordinate we have (since it contain row and column, it *2)
    //the coordinates are coordinates that the initial coordinate can  coorporate to flip
    //ex: XOOOX, one coordinate is for the second X (the form of coordinate is column,row).
    int* rowcheck(unsigned int c, unsigned int r, unsigned int col)  //it is ok
    {
        int* coordinate = new int[6];
        array_init(coordinate);
        unsigned int control = 0;
        for (int aa = c - 1; aa > -1; aa--)
        {
            if(board[get_index(aa, r, row)] == 0)
            {
                break;
            }
            if(board[get_index(aa, r, row)] == col and control == 0)
            {
                break;
            }
            if(board[get_index(aa, r, row)] == col and control != 0)
            {
                *(coordinate + 0) = 1;
                *(coordinate + 1 ) = 2;
                *(coordinate + 2) = aa;
                *(coordinate + 3) = r;
                break;
            }
            control ++;
        }
        control = 0;
        for (unsigned int aa = c + 1; aa < 8; aa++)
        {
            if(board[get_index(aa, r, row)] == 0)
            {
                break;
            }
            if(board[get_index(aa, r, row)] == col and control == 0)
            {
                break;
            }
            if(board[get_index(aa, r, row)] == col and control != 0)
            {
                if(*(coordinate + 1) == 0)
                {
                    *(coordinate) = 1;
                    *(coordinate + 1 ) = 2;
                    *(coordinate + 2) = aa;
                    *(coordinate + 3) = r;
                    break;
                }
                else
                {
                    *(coordinate + 1 ) = 4;
                    *(coordinate + 4) = aa;
                    *(coordinate + 5) = r;
                    break;
                }
            }
            control ++;
        }
        return coordinate;
    }
    int* columncheck(unsigned int c, unsigned int r, unsigned int col)  //it is ok
    {
        int* coordinate = new int[6];
        array_init(coordinate);
        unsigned int control = 0;
        for (int aa = r - 1; aa > -1; aa--)
        {
            if(board[get_index(c, aa, row)] == 0)
            {
                break;
            }
            if(board[get_index(c, aa, row)] == col and control == 0)
            {
                break;
            }
            if(board[get_index(c, aa, row)] == col and control != 0)
            {
                *(coordinate + 0) = 1;
                *(coordinate + 1 ) = 2;
                *(coordinate + 2) = c;
                *(coordinate + 3) = aa;
                break;
            }
            control ++;
        }
        control = 0;
        for (unsigned int aa = r + 1; aa < 8; aa++)
        {
            if(board[get_index(c, aa, row)] == 0)
            {
                break;
            }
            if(board[get_index(c, aa, row)] == col and control == 0)
            {
                break;
            }
            if(board[get_index(c, aa, row)] == col and control != 0)
            {
                if(*(coordinate + 1) == 0)
                {
                    *(coordinate + 0) = 1;
                    *(coordinate + 1 ) = 2;
                    *(coordinate + 2) = c;
                    *(coordinate + 3) = aa;
                    break;
                }
                else
                {
                    *(coordinate + 1 ) = 4;
                    *(coordinate + 4) = c;
                    *(coordinate + 5) = aa;
                    break;
                }
            }
            control ++;
        }
        return coordinate;
    }
    int* diagonalcheck(unsigned int c, unsigned int r, unsigned int col)
    {
        int* coordinate = new int[6];
        array_init(coordinate);
        unsigned int control = 0;
        int position_column = c;
        int position_row = r;
        while (position_column < 8 and position_row > -1)  //point move up, right
        {
            position_row --;
            position_column ++;
            int index = get_index(position_column, position_row, row);
            if(board[index] == col and control == 0)
            {
                break;
            }
            if(board[index] == 0)
            {
                break;
            }
            if (board[index] == col and control != 0)
            {
                coordinate[0] = 1;
                coordinate[1] = 2;
                coordinate[2] = position_column;
                coordinate[3] = position_row;
                break;
            }
            control ++;
        }
        control = 0;
        position_row = r;
        position_column = c;
        while (position_column >-1 and position_row < 8)  //point move down, left
        {
            position_row ++;
            position_column --;
            if(board[get_index(position_column, position_row, row)] == col and control == 0)
            {
                break;
            }
            if(board[get_index(position_column, position_row, row)] == 0)
            {
                break;
            }
            if (board[get_index(position_column, position_row, row)] == col and control != 0)
            {
                if(*(coordinate + 1) == 0)
                {
                    *(coordinate + 0) = 1;
                    *(coordinate + 1 ) = 2;
                    *(coordinate + 2) = position_column;
                    *(coordinate + 3) = position_row;
                    break;
                }
                else
                {
                    *(coordinate + 1 ) = 4;
                    *(coordinate + 4) = position_column;
                    *(coordinate + 5) = position_row;
                    break;
                }
            }
            control ++;
        }
        control = 0;
        position_row = r;
        position_column = c;
        while (position_column > -1 and position_row > -1)  //upper and left
        {
            position_column --;
            position_row --;
            int index = get_index(position_column, position_row, row);
            if(board[index] == col and control == 0)
            {
                break;
            }
            if(board[index] == 0)
            {
                break;
            }
            if (board[index] == col and control != 0)
            {
                coordinate[0] = 1;
                coordinate[1] = 2;
                coordinate[2] = position_column;
                coordinate[3] = position_row;
                break;
            }
            control ++;
        }
        control = 0;
        position_row = r;
        position_column = c;
        while(position_column < 8 and position_row < 8)
        {
            position_column ++;
            position_row ++;
            int index = get_index(position_column, position_row, row);
            if(board[index] == col and control == 0)
            {
                break;
            }
            if(board[index] == 0)
            {
                break;
            }
            if (board[index] == col and control != 0)
            {
                coordinate[0] = 1;
                coordinate[1] = 2;
                coordinate[2] = position_column;
                coordinate[3] = position_row;
                break;
            }
            control ++;
        }
        return coordinate;
    }
};
//return one array which contains the coordinate for both point, these two points should have same same column number
//the row number is different, and it will have an order, the bigger is in the front
//ex:
//....OO.
//.......
//the right one should be in the first position
int* Othello::same_c_index(unsigned int c1, unsigned int r1,unsigned int c2, unsigned int r2, unsigned int color)
{
    int* a = new int[5];
    int bigger;
    int smaller;
    if (r1 < r2)
    {
        bigger = r1;
        smaller = r2;
    }
    else
    {
        bigger = r2;
        smaller = r1;
    }
    a[0] = c1;
    a[1] = bigger;
    a[2] = c2;
    a[3] = smaller;
    a[4] = color;
    return a;
}
//the smaller one should be in the first position
//ex:
//.....O..
//.....O..
//the upper one should be in the first position
int* Othello::same_r_index(unsigned int c1, unsigned int r1,unsigned int c2, unsigned int r2, unsigned int color)
{
    int* a = new int[5];
    int bigger;
    int smaller;
    if (c1 > c2)
    {
        bigger = c2;
        smaller = c1;
    }
    else
    {
        bigger = c1;
        smaller = c2;
    }
    a[0] = bigger;
    a[1] = r1;
    a[2] = smaller;
    a[3] = r2;
    a[4] = color;
    return a;
}
//this means that both column number and row number are different, so it will return array that column is the biggest;
//the bigger one should be right and upper position of the lower one, ex:
//......OX
//.....O..
// the O with X should be the upper one, the lower O should be the lower one

int* Othello::all_diff_index(unsigned int c1, unsigned int r1,unsigned int c2, unsigned int r2, unsigned int color)
{
    int* a = new int[5];
    int bigger;
    int row_b;
    int smaller;
    int row_s;
    if(c1 > c2)
    {
        bigger = r1;
        row_b = c1;
        smaller = r2;
        row_s = c2;
    }
    else
    {
        bigger = r2;
        row_b = c2;
        smaller = r1;
        row_s = c1;
    }
    a[0] = row_b;
    a[1] = bigger;
    a[2] = row_s;
    a[3] = smaller;
    a[4] = color;
    return a;
}

Othello::Othello(unsigned int c, unsigned int r)
{
    turn = 1;
    column = c;
    row = r;
    board_length = c * r;
}
Othello& Othello:: operator=(const Othello& o)
{
    if(this != &o)
    {
        column = o.column;
        row = o.row;
        board = o.board;
        turn = o.turn;
        used_space = o.used_space;
        unused_space = o.unused_space;
        board_length = o.board_length;
    }
    return *this;
}

void Othello::set_board()
{
    for (int aa = 0; aa < board_length; aa ++)
    {
        if(aa == 27 or aa == 36)
        {
            board.push_back(2);
        }
        if(aa == 35 or aa == 28)
        {
            board.push_back(1);
        }
        if (aa != 27 and aa != 36 and aa != 35 and aa != 28)
        {
            board.push_back(0);
        }
    }
}
bool Othello::legal_move(unsigned int c, unsigned int r, unsigned int color)
{
    int* cc = columncheck( c,  r,  color);
    int* rr = rowcheck(c,r,color);
    int* dd = diagonalcheck(c,r,color);
    if (cc[0] == 0 and rr[0] == 0 and dd[0] == 0)
    {
        delete cc;
        delete rr;
        delete dd;
        return false;
    }
    else
    {
        delete cc;
        delete rr;
        delete dd;
        return true;
    }
}

void Othello::do_flip(unsigned int c1, unsigned int r1, unsigned int c2, unsigned int r2, unsigned int col)
{
    if(c1 == c2)
    {
        int* a = same_c_index(c1,r1,c2,r2,col);
        for (unsigned int aa = a[1]; aa < a[3];aa++)
        {
            int index = get_index(c1,aa,row);
            board[index] = col;
        }
        delete a;
    }
    if(r1 == r2)
    {
        int* a = same_r_index(c1,r1,c2,r2,col);
//        cout << a[0] << " " << a[1]<< " " << a[2]<< " " << a[3]<< " " << a[4] << endl;
//        unsigned int aa = a[0];
//        while (aa < a[2])
//        {
//            int index = get_index(aa, r1, row);
//            board[index] = col;
//            aa--;
//        }
        for (int aa = a[0]; aa < a[2]; aa++)
        {
//            cout << aa <<" ";
            int index = get_index(aa, r1, row);
            board[index] = col;
        }
        delete a;
    }
    if (r1 != r2 and c1 != c2)
    {
        int*a = all_diff_index(c1,r1,c2,r2,col);
        if(a[1] < a[3])
        {
            unsigned int rr = a[1];
            for (int aa = a[0]; aa > a[2]; aa--)
            {
                cout << aa << " " << a[2] << endl;
                int index = get_index(aa, rr, row);
                board[index] = col;
                rr++;
            }
        }
        else
        {

            unsigned int rr = a[1];
            unsigned int cc = a[0];
            unsigned int x = a[2];
            while (cc > x)
            {
//                cout << cc << " " << a[2] << endl;
                unsigned int index = get_index(cc, rr, row);
                board[index] = col;
                cc = cc - 1;
                rr = rr - 1;
            }
//            for (unsigned int aa = a[0]; aa > t; aa--)
//            {
//                cout << aa << " " << t << endl;
//                unsigned int index = get_index(aa, rr, row);
//                board[index] = col;
//                rr--;
//            }
        }
        delete a;
    }
}

void Othello::flip(unsigned int c, unsigned int r, unsigned int color)
{
    int* cc = columncheck(c,r,color);
    int* rr = rowcheck(c,r,color);
    int* dd = diagonalcheck(c,r,color);
    int c1 = -1;
    int r1 = -1;
    int c2 = -1;
    int r2 = -1;
    if(cc[0] != 0)
    {
        if(cc[1] == 2)
        {
            c1 = cc[2];
            r1 = cc[3];
            do_flip(c, r, c1, r1, color);
        }
        else
        {
            c1 = cc[2];
            r1 = cc[3];
            c2 = cc[4];
            r2 = cc[5];
            do_flip(c, r, c1, r1, color);
            do_flip(c, r, c2, r2, color);
        }
    }
    delete cc;
    if(dd[0] != 0)
    {
        if(cc[1] == 2)
        {
            c1 = dd[2];
            r1 = dd[3];
            do_flip(c, r, c1, r1, color);
        }
        else
        {
            c1 = dd[2];
            r1 = dd[3];
            c2 = dd[4];
            r2 = dd[5];
            do_flip(c, r, c1, r1, color);
            do_flip(c, r, c2, r2, color);
        }
    }
    delete dd;
    if(rr[0] != 0)
    {
        if(cc[1] == 2)
        {
            c1 = rr[2];
            r1 = rr[3];
//            cout << r1 << " " << c1 << endl << c << " " << r << endl;
            do_flip(c, r, c1, r1, color);
        }
        else
        {
            c1 = rr[2];
            r1 = rr[3];
            c2 = rr[4];
            r2 = rr[5];
            do_flip(c, r, c1, r1, color);
            do_flip(c, r, c2, r2, color);
        }
    }
    delete rr;
}

void Othello::set_xspace()
{
    int b = 0;
    for (int aa : board)
    {
        piece* a = index_to_rc(b);
//        cout  << a-> column << " " << a-> row << endl;
        if (legal_move(a-> column, a-> row,1))
        {
            xspace.push_back(*a);
        }
        b++;
    }
}
void Othello:: set_ospace()
{
    int b = 0;
    for (int aa:board)
    {
        piece* a = index_to_rc(b);
        if(legal_move(a-> column, a-> row, 2))
        {
            ospace.push_back(*a);
        }
        b++;
    }
}

void Othello::move(unsigned int c, unsigned int r, unsigned int color)
{
    unsigned int index = get_index(c, r, row);
    board[index] = color;
    unsigned int t;
    if(turn == 1)
    {
        t = 2;
    }
    else
    {
        t = 1;
    }
    for (piece a: xspace)
    {
//        cout << a.color <<" " << a.column << " " << a.row << " " << endl << color << "," << c << "," << r << endl;
        if(a.column == c and a.row == r and color == 1)
        {
            turn = t;
            flip(c,r,color);
        }
    }
    for (piece a: ospace)
    {
        if(color == 2 and a.column == c and a.row == r)
        {
            turn = t;
            flip(c,r,color);
        }
    }
}
unsigned int Othello:: winner_check()
{
    unsigned int xnumber = 0;
    unsigned int onumber = 0;
    for (int aa : board)
    {
        if (aa == 0)
        {
            return 0;
        }
        if (aa == 1)
        {
            xnumber++;
        }
        if(aa == 2)
        {
            onumber ++;
        }
    }
    unsigned int determine = 0;
    for (piece aa:xspace)
    {
        determine ++;
    }
    for (piece aa:ospace)
    {
        determine ++;
    }
    if (determine != 0)
    {
        return 0;
    }
    if(xnumber > onumber)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

string Othello::get_input()
{
    string a;
    if (turn == 1)
    {
        a = "X's turn: ";
    }
    else
    {
        a = "O's turn: ";
    }
    cout << a;
    string cc;
    cin >> cc;
    if (cc == "Q")
    {
        return "QUIT";
    }
    string rr;
    cin >> rr;
    if (cc != "0" and cc != "1" and cc != "2" and cc != "3" and cc != "4" and cc != "5" and cc != "6" and cc != "7")
    {
//        cout << "get input" << endl; //hahaha
        return "invalid";
    }
    if (rr != "0" and rr != "1" and rr != "2" and rr != "3" and rr != "4" and rr != "5" and rr != "6" and rr != "7")
    {
//        cout << "get input" << endl; //hahaha
        return "invalid";
    }
    return cc + rr;
}

void Othello::print_rule()
{
    cout << "Welcome to Othello!" << endl;
    cout << "Rules:      " << endl;
    cout << "       please enter column_number space row_number to " <<endl;
    cout << "       determine the position you want, enter Q to quit" << endl;
    cout << "       ex: 2 3  means you want to put piece in column 2 and row 3"<< endl;
    cout << "       X means black, O means white, black always start first" << endl;
    cout << "       turn will reverse automatically unless there is no space for" << endl;
    cout << "       one part to put" << endl;
    cout << " Have fun !" << endl;
}

void Othello::start()
{
    print_rule();
    set_board();
    print_board();
    while(true)
    {
        set_ospace();
        set_xspace();
        string get_index;
        get_index = get_input();
        if(get_index =="QUIT")
        {
            cout << "Bye!" << endl;
            break;
        }
        if (get_index == "invalid")
        {
            cout << "invalid" << endl;
        }
        else
        {
            unsigned int cc = stoi(get_index.substr(0,1));
            unsigned int rr = stoi(get_index.substr(1,1));
            bool check = legal_move(cc, rr, turn);
            if (not check)
            {
                cout << "invalid" << endl;
            }
            else
            {
                move(cc,rr,turn);
                flip(cc, rr, turn);
            }
            unsigned int winner = winner_check();
            if(winner == 1)
            {
                print_board();
                cout <<"X wins!"<< endl;
                break;
            }
            if(winner == 2)
            {
                print_board();
                cout << "O wins!" << endl;
                break;
            }
        }
        print_board();
    }
}


#endif /* P4_HPP_ */
