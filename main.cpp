// To compile run :-
// g++ 211126.cpp -o 211126.out -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# include <iostream>
# include <cstring>
# include <time.h>
# include "raylib.h"
# include "raymath.h"

using namespace std;

# define ROWS 8
# define COLS 8

int which_player=1;
int game_mode=-1;
int cells_filled=4;
int white_count=2;
int black_count=2;

void delay(int number_of_seconds)
{
    int milli_seconds = 1000 * number_of_seconds;
    clock_t start_time = clock();
    while (clock() < start_time + milli_seconds)
        ;
}

class Cell{
    public:
        static const int cell_height=600/ROWS;
        static const int cell_width=600/COLS;
        bool is_empty=true;
        string colour_placed;
        int idx_i;
        int idx_j;

        Cell()
        {

        }

        void setIdx(int i, int j)
        {
            idx_i=i;
            idx_j=j;
        }

        void drawCell(Texture2D white_piece, Texture2D black_piece)
        {
            if(!is_empty)
            {
                if(colour_placed=="White")
                {
                    Rectangle src1 = {0, 0, white_piece.width, white_piece.height};
                    Rectangle dest1 = {(idx_i*cell_width), (idx_j*cell_height), cell_width, cell_height};
                    Vector2 origin1 = {0,0};       
                    DrawTexturePro(white_piece, src1, dest1, origin1, 0.0f, WHITE);
                }
                else if(colour_placed=="Black")
                {
                    Rectangle src2 = {0, 0, black_piece.width, black_piece.height};
                    Rectangle dest2 = {(idx_i*cell_width), (idx_j*cell_height), cell_width, cell_height};
                    Vector2 origin2 = {0,0};       
                    DrawTexturePro(black_piece, src2, dest2, origin2, 0.0f, WHITE);
                }
            }
            DrawRectangleLines(idx_i*cell_width,idx_j*cell_height,cell_width,cell_height,BLACK);

        }

        void placePiece(int player)
        {
           is_empty=false;        
           if(player==1)
           {
                colour_placed="Black";
           }
           else if(player==2)
           {
                colour_placed="White";
           }
        }

        void flip_colour()
        {
            if(colour_placed=="White")
            {
                colour_placed="Black";
            }
            else if(colour_placed=="Black")
            {
                colour_placed="White";
            }
        }
};

class Othello
{
    private:
        int i_offset;
        int j_offset;

        bool flipPossibility(Cell grid[ROWS+2][COLS+2], int x, int y, string colour)
        {
            if(grid[x][y].is_empty)
            {
                return false;
            }
            else if(grid[x][y].colour_placed==colour)
            {
                return false;
            }
            else if(grid[x][y].colour_placed!=colour){
                return true;
            }
            return false;
        }

    public:

        bool shouldFlip(Cell grid[ROWS+2][COLS+2], int x, int y, int x_offset, int y_offset,string colour_given)
        {
            string colour=colour_given;
            if(!(flipPossibility(grid, (x+x_offset), (y+y_offset), colour)))
            {
                return false;
            }
            else{
                while(flipPossibility(grid, (x+x_offset), (y+y_offset), colour))
                {
                    x+=x_offset;
                    y+=y_offset;
                    if(!(grid[x][y].is_empty) && grid[x][y].colour_placed!=colour && grid[x+x_offset][y+y_offset].colour_placed==colour)
                    {
                        return true;
                    }
                }
                return false;
            }
        }

        Othello() {}

        Othello(Cell grid[ROWS+2][COLS+2],int i_idx, int j_idx)
        {
            for(int k=0; k<8; k++)
            {
                int tmp_i_idx=i_idx;
                int tmp_j_idx=j_idx;
                switch (k)
                {
                    case 0:
                        i_offset=-1;
                        j_offset=-1;
                        break;

                    case 1:
                        i_offset=-1;
                        j_offset=0;
                        break;

                    case 2:
                        i_offset=-1;
                        j_offset=1;
                        break;

                    case 3:
                        i_offset=0;
                        j_offset=1;
                        break;

                    case 4:
                        i_offset=1;
                        j_offset=1;
                        break;

                    case 5:
                        i_offset=1;
                        j_offset=0;
                        break;

                    case 6:
                        i_offset=1;
                        j_offset=-1;
                        break;

                    case 7:
                        i_offset=0;
                        j_offset=-1;
                        break;
                                    
                }
                string colour=grid[tmp_i_idx][tmp_j_idx].colour_placed;
                if(!shouldFlip(grid, tmp_i_idx, tmp_j_idx, i_offset, j_offset, grid[tmp_i_idx][tmp_j_idx].colour_placed))
                {
                    continue;
                }
                while(flipPossibility(grid, (tmp_i_idx+i_offset), (tmp_j_idx+j_offset), colour))
                {
                    grid[tmp_i_idx+i_offset][tmp_j_idx+j_offset].flip_colour();
                    tmp_i_idx+=i_offset;
                    tmp_j_idx+=j_offset;
                }
            }
        }
};

class ImpFuncs{
    public:
        static const int window_height=600;
        static const int window_width=600;
        static const int footer_height=100;
        int num_valid_move;

    protected:
        bool move_valid[ROWS+2][COLS+2];

        void calcPieceCount(Cell grid[ROWS+2][COLS+2])
        {
            black_count=0;
            white_count=0;
            for(int i=1; i<=ROWS; i++)
            {
                for(int j=1; j<=COLS; j++)
                {
                    if(grid[i][j].colour_placed=="Black")
                    {
                        black_count+=1;
                    }
                    else if(grid[i][j].colour_placed=="White")
                    {
                        white_count+=1;
                    }
                }
            }
        }

        bool idxValid(int i, int j,Cell grid[ROWS+2][COLS+2])
        {
            if(!(grid[i+1][j+1].is_empty))
            {
                return false;
            }
            if(!(i>=0 && i<COLS && j>=0 && j<ROWS))
            {
                return false;
            }
            //return true;

            for(int k=0; k<8; k++)
            {
                int i_offset;
                int j_offset;
                int tmp_i_idx=i+1;
                int tmp_j_idx=j+1;
                switch (k)
                {
                    case 0:
                        i_offset=-1;
                        j_offset=-1;
                        break;

                    case 1:
                        i_offset=-1;
                        j_offset=0;
                        break;

                    case 2:
                        i_offset=-1;
                        j_offset=1;
                        break;

                    case 3:
                        i_offset=0;
                        j_offset=1;
                        break;

                    case 4:
                        i_offset=1;
                        j_offset=1;
                        break;

                    case 5:
                        i_offset=1;
                        j_offset=0;
                        break;

                    case 6:
                        i_offset=1;
                        j_offset=-1;
                        break;

                    case 7:
                        i_offset=0;
                        j_offset=-1;
                        break;
                                    
                }
                string colour;
                Othello tmp;
                if(which_player==2)
                {
                    colour="White";
                }
                else if(which_player==1)
                {
                    colour="Black";
                }
                if((tmp.shouldFlip(grid, tmp_i_idx, tmp_j_idx, i_offset, j_offset, colour)))
                {
                    return true;
                }
            }
            return false;
        }
        
        void initializeMoveValidArr()
        {
            for(int i=0; i<ROWS+2; i++)
            {
                for(int j=0; j<COLS+2; j++)
                {
                    move_valid[i][j]=false;
                }
            }
        }

        void validMoveCheck(Cell grid[ROWS+2][COLS+2])
        {
            num_valid_move=0;
            initializeMoveValidArr();
            for(int i=1; i<ROWS+1; i++)
            {
                for(int j=1; j<COLS+1; j++)
                {
                    if(!(grid[i][j].is_empty))
                    {
                        move_valid[i][j]=false;
                    }
                    else
                    {
                        if(!(idxValid(i-1, j-1, grid)))
                        {
                            move_valid[i][j]=false;
                        }
                        else
                        {
                            move_valid[i][j]=true;
                            num_valid_move+=1;
                        }
                    }

                }
            }
        }

        bool noValidMove()
        {
            for(int i=1; i<=ROWS; i++)
            {
                for(int j=1; j<=COLS; j++)
                {
                    if(move_valid[i][j]==true)
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        bool noMove(Cell grid[ROWS+2][COLS+2])
        {
            string colour1;
            string colour2="none";
            for(int i=1; i<=ROWS; i++)
            {
                for(int j=1; j<=COLS; j++)
                {
                    if(!grid[i][j].is_empty)
                    {
                        colour1=grid[i][j].colour_placed;
                        if(colour2!="none")
                        {
                            if(colour1!=colour2)
                            {
                                return false;
                            }
                        }
                        else
                        {
                            colour2=colour1;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            return true;
        }

    public:
        ImpFuncs() {}


};

class OthelloAI : public ImpFuncs
{
    public:
        int which_move_i;
        int which_move_j;

    private:
        int which_move_tmp_val;
        int randomness_generator;

        void copyBoard(Cell src[ROWS+2][COLS+2], Cell dest[ROWS+2][COLS+2])
        {
            for(int i=0; i<ROWS+2; i++)
            {
                for(int j=0; j<COLS+2; j++)
                {
                    dest[i][j].is_empty=src[i][j].is_empty;
                    dest[i][j].colour_placed=src[i][j].colour_placed;
                    dest[i][j].idx_i=src[i][j].idx_i;
                    dest[i][j].idx_j=src[i][j].idx_j;
                }
            }
        }

        void whichMoveToPlay(Cell grid[ROWS+2][COLS+2])
        {
            int tmp_which_player=which_player;
            Cell tmp_grid[ROWS+2][COLS+2];
            copyBoard(grid, tmp_grid);
            validMoveCheck(tmp_grid);
            int num_moves=num_valid_move;
            int k=0;
            for(int i=1; i<=ROWS; i++)
            {
                for(int j=1; j<=COLS; j++)
                {
                    randomness_generator=rand()%4;
                    if(move_valid[i][j]==true)
                    {
                        copyBoard(grid, tmp_grid);
                        tmp_grid[i][j].placePiece(tmp_which_player);
                        Othello othello_logic(tmp_grid, i, j);
                        k+=1;
                        calcPieceCount(tmp_grid);
                        if(tmp_which_player==2)
                        {
                            if(which_move_tmp_val<white_count)
                            {
                                which_move_tmp_val=white_count;
                                which_move_i=i;
                                which_move_j=j;
                            }
                            else if(randomness_generator==1 || randomness_generator==3)
                            {
                                if(which_move_tmp_val==white_count)
                                {
                                    which_move_tmp_val=white_count;
                                    which_move_i=i;
                                    which_move_j=j;
                                }
                            }
                        }
                        else if(tmp_which_player==1)
                        {
                            if(which_move_tmp_val<black_count)
                            {
                                which_move_tmp_val=black_count;
                                which_move_i=i;
                                which_move_j=j;
                            }
                            else if(randomness_generator==1 || randomness_generator==0)
                            {
                                if(which_move_tmp_val==white_count)
                                {
                                    which_move_tmp_val=white_count;
                                    which_move_i=i;
                                    which_move_j=j;
                                }
                            }
                        }
                    }
                }
            }
        }

    public:
        OthelloAI(Cell grid[ROWS+2][COLS+2])
        {
            which_move_tmp_val=0;
            whichMoveToPlay(grid);
            grid[which_move_i][which_move_j].placePiece(which_player);
        }

        
};

class MenuScreen{
    private:
        Cell tmp_cell;

        void setGridBegin(Cell grid[ROWS+2][COLS+2])
        {   
            for(int i=0; i<ROWS+2; i++)
            {
                for(int j=0; j<COLS+2; j++)
                {
                    grid[i][j].is_empty=true;
                }
            }
                grid[4][4].is_empty=false;
                grid[4][4].colour_placed="Black";

                grid[4][5].is_empty=false;
                grid[4][5].colour_placed="White";

                grid[5][4].is_empty=false;
                grid[5][4].colour_placed="White";

                grid[5][5].is_empty=false;
                grid[5][5].colour_placed="Black";
        }

    public:
        MenuScreen(Cell grid[ROWS+2][COLS+2])
        {
            cells_filled=4;
            which_player=1;
            white_count=2;
            black_count=2;
            setGridBegin(grid);
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 m_pos=GetMousePosition();
                int index_i=m_pos.x/tmp_cell.cell_width;
                int index_j=m_pos.y/tmp_cell.cell_height;
                
                if(index_i>=2 && index_i<5 && index_j>=3 && index_j<5)
                {
                    //DrawText("One1!", 600/2 -MeasureText("Draw!",64)/2,600/2-64/2,64,DARKGRAY);
                    game_mode=1;
                }   
                else if(index_i>=2 && index_i<5 && index_j>=5 && index_j<7)
                {
                    //DrawText("Two2!", 600/2 -MeasureText("Draw!",64)/2,600/2-64/2,64,DARKGRAY);
                    game_mode=2;
                }   
                else if(index_i>=2 && index_i<5 && index_j>=7 && index_j<9)
                {
                    //DrawText("Three!", 600/2 -MeasureText("Draw!",64)/2,600/2-64/2,64,DARKGRAY);
                    game_mode=3;
                }      
            }

            BeginDrawing();

                ClearBackground(RAYWHITE);
                DrawText("OTHELLO", ((600/2)-(MeasureText("OTHELLO", 75)/2)),50, 75, DARKGREEN);
                DrawText("GAME", ((600/2)-(MeasureText("GAME", 50)/2)),135, 50, DARKGREEN);
                DrawRectangleLines(2*tmp_cell.cell_width, 3*tmp_cell.cell_height, 4*tmp_cell.cell_width, 4*tmp_cell.cell_height, BLACK);
                DrawRectangleLines(2*tmp_cell.cell_width, 5*tmp_cell.cell_height, 4*tmp_cell.cell_width, 4*tmp_cell.cell_height, BLACK);
                DrawText("0 Player", 600/2 -MeasureText("0 Player",38)/2,600/2-38/2,38,DARKGRAY);
                DrawText("1 Player", 600/2 -MeasureText("1 Player",38)/2,600/2-38/2+150,38,DARKGRAY);
                DrawText("2 Player", 600/2 -MeasureText("2 Player",38)/2,600/2-38/2+300,38,DARKGRAY);

            EndDrawing();
        }
};

class EndScreen{
    private:
        Cell tmp_cell;
        
        void calcPieceCount(Cell grid[ROWS+2][COLS+2])
        {
            white_count=0;
            black_count=0;
            for(int i=1; i<=ROWS; i++)
            {
                for(int j=1; j<=COLS; j++)
                {
                    if(grid[i][j].colour_placed=="Black")
                    {
                        black_count+=1;
                    }
                    else if(grid[i][j].colour_placed=="White")
                    {
                        white_count+=1;
                    }
                }
            }
        }

        void showResult(Cell grid[ROWS+2][COLS+2])
        {
            calcPieceCount(grid);
            
            //char *s=strcat(strcat(strcat(strcat("Black(",(const char*)(black_count)),") : White("), (const char*)(white_count)),")");

            if(black_count>white_count)
            {
                DrawText("Black Wins!", 600/2 -MeasureText("Black Wins!",64)/2,600/2-64/2,64,DARKGRAY);
                //DrawText(s, 600/2 -MeasureText("Blace(  ) : White(  )",32)/2,600/2+64/2,32,DARKGRAY);
            }
            else if(black_count<white_count)
            {
                DrawText("White Wins!", 600/2 -MeasureText("White Wins!",64)/2,600/2-64/2,64,DARKGRAY);
            }
            else{
                DrawText("Draw!", 600/2 -MeasureText("Draw!",64)/2,600/2-64/2,64,DARKGRAY);
            }
            
        }

    public:
        EndScreen(Cell grid[ROWS+2][COLS+2])
        {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 m_pos=GetMousePosition();
                int index_i=m_pos.x/tmp_cell.cell_width;
                int index_j=m_pos.y/tmp_cell.cell_height;

                if(index_i>=2 && index_i<5 && index_j>=7 && index_j<9)
                {
                    //game_mode=-1;
                    CloseWindow();
                }
            }

            BeginDrawing();

                ClearBackground(RAYWHITE);
                showResult(grid);
                DrawRectangleLines(2*tmp_cell.cell_width, 7*tmp_cell.cell_height, 4*tmp_cell.cell_width, 2*tmp_cell.cell_height, BLACK);
                DrawText("Exit", 600/2 -MeasureText("Exit",38)/2,600/2-38/2+300,38,DARKGRAY);

            EndDrawing();
        }
};

class TwoPlayerWindow : public ImpFuncs{       
    public:
        TwoPlayerWindow(Cell grid[ROWS+2][COLS+2], Sound piece_move, Sound invalid_move, Texture2D white_piece, Texture2D black_piece, Texture2D cursor_sprite, int window_height, int window_width, int footer_height)
        {
            if(cells_filled<64)
                    {
                        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                        {
                            Vector2 m_pos=GetMousePosition();
                            int index_i = m_pos.x/Cell::cell_width;
                            int index_j = m_pos.y/Cell::cell_height;

                            if(idxValid(index_i, index_j, grid))
                            {
                                PlaySound(piece_move);
                                grid[index_i+1][index_j+1].placePiece(which_player);
                                Othello othello_logic(grid, index_i+1, index_j+1);
                                cells_filled+=1;
                                if(which_player==1)
                                {
                                    which_player=2;
                                }
                                else if(which_player==2)
                                {
                                    which_player=1;
                                }

                                validMoveCheck(grid);
                                if(noValidMove())
                                {
                                    if(which_player==1)
                                    {
                                        which_player=2;
                                    }
                                    else if(which_player==2)
                                    {
                                        which_player=1;
                                    }
                                }
                            }
                            else{
                                PlaySound(invalid_move);
                            }
                            
                        }

                        BeginDrawing();

                            //ClearBackground(Color{193, 132, 81});       - Wodden Brown
                            //ClearBackground(Color{28, 84, 35});         - Dark Green
                            //ClearBackground(GREEN);
                            ClearBackground(Color{0, 144, 103});
                            for(int i=0; i<ROWS; i++)
                            {
                                for(int j=0; j<COLS; j++)
                                {
                                    grid[i+1][j+1].setIdx(i,j);
                                    grid[i+1][j+1].drawCell(white_piece, black_piece);


                                    if(which_player==1)
                                    {
                                        Rectangle src2 = {0, 0, cursor_sprite.width, cursor_sprite.height};
                                        Rectangle dest2 = {(7*(Cell::cell_width)), (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                                        Vector2 origin2 = {0,0};
                                        DrawTexturePro(cursor_sprite, src2, dest2, origin2, 90.0f, WHITE);
                                    }   
                                    else if(which_player==2)
                                    {
                                        Rectangle src2 = {0, 0, cursor_sprite.width, cursor_sprite.height};
                                        Rectangle dest2 = {(1*(Cell::cell_width)), (window_height+(footer_height/6)+Cell::cell_height), Cell::cell_width, Cell::cell_height};
                                        Vector2 origin2 = {0,0};
                                        DrawTexturePro(cursor_sprite, src2, dest2, origin2, -90.0f, WHITE);
                                    }    
                                }
                            }
                            DrawText("OTHELLO", ((window_width/2)-(MeasureText("OTHELLO", 50)/2)), (window_height+(footer_height/2)-(50/2)), 50, RAYWHITE);

                            calcPieceCount(grid);
                            string tmp1 = to_string(white_count);
                            char* white_count_string=(char*)(tmp1.c_str());
                            string tmp2 = to_string(black_count);
                            char* black_count_string=(char*)(tmp2.c_str());

                            Rectangle src1 = {0, 0, white_piece.width, white_piece.height};
                            Rectangle dest1 = {0, (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                            Vector2 origin1 = {0,0};       
                            DrawTexturePro(white_piece, src1, dest1, origin1, 0.0f, WHITE);
                            DrawText((white_count_string), 25, (window_height+(footer_height/6))+25, 24, BLACK);
                            Rectangle src2 = {0, 0, black_piece.width, black_piece.height};
                            Rectangle dest2 = {(7*(Cell::cell_width)), (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                            Vector2 origin2 = {0,0};       
                            DrawTexturePro(black_piece, src2, dest2, origin2, 0.0f, WHITE);
                            DrawText((black_count_string), (7*(Cell::cell_width))+25, (window_height+(footer_height/6))+25, 24, RAYWHITE);

                        EndDrawing();
                    }
                    else if(cells_filled==64)
                    {
                        EndScreen end_screen(grid);
                    }
                    if(noMove(grid) && cells_filled<64)
                    {
                        EndScreen end_screen1(grid);
                        cells_filled=64;
                    }
        }
};

class OnePlayerWindow : public ImpFuncs
{
    public:
        OnePlayerWindow(Cell grid[ROWS+2][COLS+2], Sound piece_move, Sound invalid_move, Texture2D white_piece, Texture2D black_piece, Texture2D cursor_sprite, int window_height, int window_width, int footer_height)
        {
            if(cells_filled<64)
                {
                    int index_i;
                    int index_j;
                    if(which_player==1)
                    {
                        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                        {
                            Vector2 m_pos=GetMousePosition();
                            index_i = m_pos.x/Cell::cell_width;
                            index_j = m_pos.y/Cell::cell_height;
                            if(idxValid(index_i, index_j, grid))
                            {
                                PlaySound(piece_move);
                                grid[index_i+1][index_j+1].placePiece(which_player);
                                Othello othello_logic(grid, index_i+1, index_j+1);
                                cells_filled+=1;
                                if(which_player==1)
                                {
                                    which_player=2;
                                }
                                else if(which_player==2)
                                {
                                    which_player=1;
                                }
                                validMoveCheck(grid);
                                if(noValidMove())
                                {
                                    if(which_player==1)
                                    {
                                        which_player=2;
                                    }
                                    else if(which_player==2)
                                    {
                                        which_player=1;
                                    }
                                }
                            }
                            else{
                                PlaySound(invalid_move);
                            }

                        }
                    }
                    else if(which_player==2)
                    {
                        delay(1000);
                        OthelloAI tmp_AI(grid);
                        index_i=tmp_AI.which_move_i;
                        index_j=tmp_AI.which_move_j;
                        PlaySound(piece_move);
                        Othello othello_logic(grid, index_i, index_j);
                        cells_filled+=1;
                        if(which_player==1)
                        {
                            which_player=2;
                        }
                        else if(which_player==2)
                        {
                            which_player=1;
                        }
                        validMoveCheck(grid);
                        if(noValidMove())
                        {
                            if(which_player==1)
                            {
                                which_player=2;
                            }
                            else if(which_player==2)
                            {
                                which_player=1;
                            }
                        }
                    }
                    BeginDrawing();
                        //ClearBackground(Color{193, 132, 81});       - Wodden Brown
                        //ClearBackground(Color{28, 84, 35});         - Dark Green
                        //ClearBackground(GREEN);
                        ClearBackground(Color{0, 144, 103});
                        for(int i=0; i<ROWS; i++)
                        {
                            for(int j=0; j<COLS; j++)
                            {
                                grid[i+1][j+1].setIdx(i,j);
                                grid[i+1][j+1].drawCell(white_piece, black_piece);
                                if(which_player==1)
                                {
                                    Rectangle src2 = {0, 0, cursor_sprite.width, cursor_sprite.height};
                                    Rectangle dest2 = {(7*(Cell::cell_width)), (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                                    Vector2 origin2 = {0,0};
                                    DrawTexturePro(cursor_sprite, src2, dest2, origin2, 90.0f, WHITE);
                                }   
                                else if(which_player==2)
                                {
                                    Rectangle src2 = {0, 0, cursor_sprite.width, cursor_sprite.height};
                                    Rectangle dest2 = {(1*(Cell::cell_width)), (window_height+(footer_height/6)+Cell::cell_height), Cell::cell_width, Cell::cell_height};
                                    Vector2 origin2 = {0,0};
                                    DrawTexturePro(cursor_sprite, src2, dest2, origin2, -90.0f, WHITE);
                                }    
                            }
                        }
                        DrawText("OTHELLO", ((window_width/2)-(MeasureText("OTHELLO", 50)/2)), (window_height+(footer_height/2)-(50/2)), 50, RAYWHITE);
                        calcPieceCount(grid);
                        string tmp1 = to_string(white_count);
                        char* white_count_string=(char*)(tmp1.c_str());
                        string tmp2 = to_string(black_count);
                        char* black_count_string=(char*)(tmp2.c_str());
                        Rectangle src1 = {0, 0, white_piece.width, white_piece.height};
                        Rectangle dest1 = {0, (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                        Vector2 origin1 = {0,0};       
                        DrawTexturePro(white_piece, src1, dest1, origin1, 0.0f, WHITE);
                        DrawText((white_count_string), 25, (window_height+(footer_height/6))+25, 24, BLACK);
                        Rectangle src2 = {0, 0, black_piece.width, black_piece.height};
                        Rectangle dest2 = {(7*(Cell::cell_width)), (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                        Vector2 origin2 = {0,0};       
                        DrawTexturePro(black_piece, src2, dest2, origin2, 0.0f, WHITE);
                        DrawText((black_count_string), (7*(Cell::cell_width))+25, (window_height+(footer_height/6))+25, 24, RAYWHITE);
                    EndDrawing();
                }
                else if(cells_filled==64 || noMove(grid))
                {
                    EndScreen end_screen(grid);
                }
        }
};

class ZeroPlayerWindow : public ImpFuncs{
    public:
        ZeroPlayerWindow(Cell grid[ROWS+2][COLS+2], Sound piece_move, Sound invalid_move, Texture2D white_piece, Texture2D black_piece, Texture2D cursor_sprite, int window_height, int window_width, int footer_height)
        {
            if(cells_filled<64)
                {
                    delay(1000);
                    OthelloAI tmp_AI(grid);
                    int index_i=tmp_AI.which_move_i;
                    int index_j=tmp_AI.which_move_j;
                    PlaySound(piece_move);
                    Othello othello_logic(grid, index_i, index_j);
                    cells_filled+=1;
                    if(which_player==1)
                    {
                        which_player=2;
                    }
                    else if(which_player==2)
                    {
                        which_player=1;
                    }
                    validMoveCheck(grid);
                    if(noValidMove())
                    {
                        if(which_player==1)
                        {
                            which_player=2;
                        }
                        else if(which_player==2)
                        {
                            which_player=1;
                        }
                    }
                
                    BeginDrawing();
                        //ClearBackground(Color{193, 132, 81});       - Wodden Brown
                        //ClearBackground(Color{28, 84, 35});         - Dark Green
                        //ClearBackground(GREEN);
                        ClearBackground(Color{0, 144, 103});
                        for(int i=0; i<ROWS; i++)
                        {
                            for(int j=0; j<COLS; j++)
                            {
                                grid[i+1][j+1].setIdx(i,j);
                                grid[i+1][j+1].drawCell(white_piece, black_piece);
                                if(which_player==1)
                                {
                                    Rectangle src2 = {0, 0, cursor_sprite.width, cursor_sprite.height};
                                    Rectangle dest2 = {(7*(Cell::cell_width)), (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                                    Vector2 origin2 = {0,0};
                                    DrawTexturePro(cursor_sprite, src2, dest2, origin2, 90.0f, WHITE);
                                }   
                                else if(which_player==2)
                                {
                                    Rectangle src2 = {0, 0, cursor_sprite.width, cursor_sprite.height};
                                    Rectangle dest2 = {(1*(Cell::cell_width)), (window_height+(footer_height/6)+Cell::cell_height), Cell::cell_width, Cell::cell_height};
                                    Vector2 origin2 = {0,0};
                                    DrawTexturePro(cursor_sprite, src2, dest2, origin2, -90.0f, WHITE);
                                }    
                            }
                        }
                        DrawText("OTHELLO", ((window_width/2)-(MeasureText("OTHELLO", 50)/2)), (window_height+(footer_height/2)-(50/2)), 50, RAYWHITE);
                        calcPieceCount(grid);
                        string tmp1 = to_string(white_count);
                        char* white_count_string=(char*)(tmp1.c_str());
                        string tmp2 = to_string(black_count);
                        char* black_count_string=(char*)(tmp2.c_str());
                        Rectangle src1 = {0, 0, white_piece.width, white_piece.height};
                        Rectangle dest1 = {0, (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                        Vector2 origin1 = {0,0};       
                        DrawTexturePro(white_piece, src1, dest1, origin1, 0.0f, WHITE);
                        DrawText((white_count_string), 25, (window_height+(footer_height/6))+25, 24, BLACK);
                        Rectangle src2 = {0, 0, black_piece.width, black_piece.height};
                        Rectangle dest2 = {(7*(Cell::cell_width)), (window_height+(footer_height/6)), Cell::cell_width, Cell::cell_height};
                        Vector2 origin2 = {0,0};       
                        DrawTexturePro(black_piece, src2, dest2, origin2, 0.0f, WHITE);
                        DrawText((black_count_string), (7*(Cell::cell_width))+25, (window_height+(footer_height/6))+25, 24, RAYWHITE);
                    EndDrawing();
                }
                else if(cells_filled==64 || noMove(grid))
                {
                    EndScreen end_screen(grid);
                }
        }
};

class GameWindow : public ImpFuncs
{
    protected:
        Texture2D black_piece;
        Texture2D white_piece;
        Texture2D cursor_sprite;
        Texture2D wodden_board;
        Sound piece_move;
        Sound invalid_move;
        

    public:
    
        void initializeWindow(Cell grid[ROWS+2][COLS+2])
        {   
                InitWindow(window_width,window_height+footer_height,"OTHELLO");
                InitAudioDevice();
                black_piece=LoadTexture("resources/black.png");
                white_piece=LoadTexture("resources/white.png");
                cursor_sprite=LoadTexture("resources/cursor.png");
                wodden_board=LoadTexture("resources/wodden.png");
                piece_move=LoadSound("resources/move.mp3");
                invalid_move=LoadSound("resources/notify.mp3");

                //Adding initial 4 pieces
                grid[4][4].placePiece(which_player);
                which_player=2;
                grid[4][5].placePiece(which_player);
                which_player=1;
                grid[5][5].placePiece(which_player);
                which_player=2;
                grid[5][4].placePiece(which_player);
                which_player=1;

                SetTargetFPS(90);

                while(!WindowShouldClose())
                {
                    if(game_mode==-1)
                    {
                        MenuScreen menu(grid);
                    }
                    else if(game_mode==1)
                    {
                        ZeroPlayerWindow zero_game_obj(grid, piece_move, invalid_move, white_piece, black_piece, cursor_sprite, window_height, window_width, footer_height);
                    }
                    else if(game_mode==2)
                    {
                        OnePlayerWindow one_game_obj(grid, piece_move, invalid_move, white_piece, black_piece, cursor_sprite, window_height, window_width, footer_height);
                    }
                    else if(game_mode==3)
                    {
                        TwoPlayerWindow two_game_obj(grid, piece_move, invalid_move, white_piece, black_piece, cursor_sprite, window_height, window_width, footer_height);
                    }
                    //TwoPlayerWindow game_obj(grid, piece_move, invalid_move, white_piece, black_piece, cursor_sprite, window_height, window_width, footer_height);
                    //MenuScreen a;
                }
            
            }
    public:
        GameWindow() {};

        GameWindow(Cell grid[ROWS+2][COLS+2])
        {
            //which_player=1;
            cells_filled=4;
            initializeWindow(grid);
        }
};


const int ImpFuncs::window_height;
const int ImpFuncs::window_width;
const int ImpFuncs::footer_height;
//int ImpFuncs::which_player=1;
//bool ImpFuncs::move_valid[ROWS+2][COLS+2];


int main(void)
{
    Cell grid[ROWS+2][COLS+2];
    GameWindow tmp(grid);

    return 0;
}
