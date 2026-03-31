#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace std;

const int SIZE = 75;

char board[8][8];

bool whiteTurn = true;
bool gameOver = false;
string winnerText = "";

bool isWhite(char p){ return p >= 'A' && p <= 'Z'; }
bool isBlack(char p){ return p >= 'a' && p <= 'z'; }

void resetBoard()
{
    char temp[8][8] = {
        {'r','n','b','q','k','b','n','r'},
        {'p','p','p','p','p','p','p','p'},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {' ',' ',' ',' ',' ',' ',' ',' '},
        {'P','P','P','P','P','P','P','P'},
        {'R','N','B','Q','K','B','N','R'}
    };

    for(int r=0;r<8;r++)
        for(int c=0;c<8;c++)
            board[r][c] = temp[r][c];

    whiteTurn = true;
    gameOver = false;
    winnerText = "";
}

bool kingExists(char k)
{
    for(int r=0;r<8;r++)
        for(int c=0;c<8;c++)
            if(board[r][c]==k)
                return true;
    return false;
}

bool pathClear(int sr,int sc,int dr,int dc)
{
    int rStep = (dr-sr)==0 ? 0 : (dr-sr)/abs(dr-sr);
    int cStep = (dc-sc)==0 ? 0 : (dc-sc)/abs(dc-sc);

    sr += rStep;
    sc += cStep;

    while(sr != dr || sc != dc)
    {
        if(board[sr][sc] != ' ')
            return false;
        sr += rStep;
        sc += cStep;
    }
    return true;
}

bool isValidMove(int sr,int sc,int dr,int dc)
{
    char p = board[sr][sc];
    char target = board[dr][dc];

    if(p == ' ') return false;
    if(isWhite(p) && isWhite(target)) return false;
    if(isBlack(p) && isBlack(target)) return false;

    int rDiff = dr - sr;
    int cDiff = dc - sc;

    char lower = tolower(p);

    if(lower=='p')
    {
        int dir = isWhite(p) ? -1 : 1;

        if(cDiff==0 && target==' ')
        {
            if(rDiff==dir) return true;

            if((sr==6 && dir==-1 || sr==1 && dir==1)
               && rDiff==2*dir
               && board[sr+dir][sc]==' ')
                return true;
        }

        if(abs(cDiff)==1 && rDiff==dir && target!=' ')
            return true;

        return false;
    }

    if(lower=='r')
    {
        if(sr==dr || sc==dc)
            return pathClear(sr,sc,dr,dc);
        return false;
    }

    if(lower=='b')
    {
        if(abs(rDiff)==abs(cDiff))
            return pathClear(sr,sc,dr,dc);
        return false;
    }

    if(lower=='q')
    {
        if(sr==dr || sc==dc || abs(rDiff)==abs(cDiff))
            return pathClear(sr,sc,dr,dc);
        return false;
    }

    if(lower=='n')
    {
        if((abs(rDiff)==2 && abs(cDiff)==1) ||
           (abs(rDiff)==1 && abs(cDiff)==2))
            return true;
        return false;
    }

    if(lower=='k')
    {
        if(abs(rDiff)<=1 && abs(cDiff)<=1)
            return true;
        return false;
    }

    return false;
}

bool findKing(bool white, int &kr, int &kc)
{
    char king = white ? 'K' : 'k';

    for(int r=0;r<8;r++)
        for(int c=0;c<8;c++)
            if(board[r][c]==king)
            {
                kr=r;
                kc=c;
                return true;
            }
    return false;
}

bool isKingInCheck(bool white)
{
    int kr,kc;
    if(!findKing(white,kr,kc)) return false;

    for(int r=0;r<8;r++)
        for(int c=0;c<8;c++)
        {
            char p=board[r][c];
            if(p==' ') continue;

            if(white && isBlack(p))
                if(isValidMove(r,c,kr,kc)) return true;

            if(!white && isWhite(p))
                if(isValidMove(r,c,kr,kc)) return true;
        }

    return false;
}

bool hasAnyLegalMove(bool white)
{
    for(int sr=0;sr<8;sr++)
        for(int sc=0;sc<8;sc++)
        {
            char p=board[sr][sc];
            if(p==' ') continue;

            if((white && isWhite(p)) || (!white && isBlack(p)))
            {
                for(int dr=0;dr<8;dr++)
                    for(int dc=0;dc<8;dc++)
                    {
                        if(isValidMove(sr,sc,dr,dc))
                        {
                            char temp=board[dr][dc];
                            board[dr][dc]=board[sr][sc];
                            board[sr][sc]=' ';

                            bool check=isKingInCheck(white);

                            board[sr][sc]=board[dr][dc];
                            board[dr][dc]=temp;

                            if(!check) return true;
                        }
                    }
            }
        }
    return false;
}

int main()
{
    resetBoard();
    sf::RenderWindow window(sf::VideoMode(600,600), "Chess Game");

    sf::Texture textures[128];

    textures['P'].loadFromFile("images/white_pawn.png");
    textures['R'].loadFromFile("images/white_rook.png");
    textures['N'].loadFromFile("images/white_knight.png");
    textures['B'].loadFromFile("images/white_bishop.png");
    textures['Q'].loadFromFile("images/white_queen.png");
    textures['K'].loadFromFile("images/white_king.png");

    textures['p'].loadFromFile("images/black_pawn.png");
    textures['r'].loadFromFile("images/black_rook.png");
    textures['n'].loadFromFile("images/black_knight.png");
    textures['b'].loadFromFile("images/black_bishop.png");
    textures['q'].loadFromFile("images/black_queen.png");
    textures['k'].loadFromFile("images/black_king.png");

    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/arial.ttf");

    int selectedRow=-1, selectedCol=-1;

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type==sf::Event::Closed)
                window.close();

            if(event.type==sf::Event::MouseButtonPressed)
            {
                if(gameOver)
                {
                    resetBoard();
                    selectedRow=-1;
                    continue;
                }

                int x = event.mouseButton.x/SIZE;
                int y = event.mouseButton.y/SIZE;

                if(x<0||x>=8||y<0||y>=8) continue;

                if(selectedRow==-1)
                {
                    char piece=board[y][x];
                    if(piece!=' ' &&
                      ((whiteTurn&&isWhite(piece)) ||
                       (!whiteTurn&&isBlack(piece))))
                    {
                        selectedRow=y;
                        selectedCol=x;
                    }
                }
                else
                {
                    if(isValidMove(selectedRow,selectedCol,y,x))
                    {
                        char temp=board[y][x];
                        board[y][x]=board[selectedRow][selectedCol];
                        board[selectedRow][selectedCol]=' ';

                        if(!isKingInCheck(whiteTurn))
                        {
                            whiteTurn=!whiteTurn;

                            bool opponentWhite=whiteTurn;

                            if(isKingInCheck(opponentWhite) &&
                               !hasAnyLegalMove(opponentWhite))
                            {
                                winnerText = opponentWhite ?
                                    "Black Wins" : "White Wins";

                                gameOver=true;
                            }
                        }
                        else
                        {
                            board[selectedRow][selectedCol]=board[y][x];
                            board[y][x]=temp;
                        }
                    }

                    selectedRow=-1;
                }
            }
        }

        window.clear();

        for(int r=0;r<8;r++)
        {
            for(int c=0;c<8;c++)
            {
                sf::RectangleShape square(sf::Vector2f(SIZE,SIZE));
                square.setPosition(c*SIZE,r*SIZE);
                square.setFillColor((r+c)%2==0 ?
                    sf::Color(240,217,181):
                    sf::Color(181,136,99));
                window.draw(square);

                char p=board[r][c];
                if(p!=' ')
                {
                    sf::Sprite sprite(textures[p]);
                    sprite.setScale(
                        SIZE/(float)textures[p].getSize().x,
                        SIZE/(float)textures[p].getSize().y);
                    sprite.setPosition(c*SIZE,r*SIZE);
                    window.draw(sprite);
                }
            }
        }

        if(gameOver)
        {
            sf::Text line1, line2;

            line1.setFont(font);
            line2.setFont(font);

            line1.setCharacterSize(36);
            line2.setCharacterSize(28);

            line1.setFillColor(sf::Color::Red);
            line2.setFillColor(sf::Color::Black);

            line1.setString(winnerText + " by Checkmate!");
            line2.setString("Click Anywhere to Restart");

            sf::FloatRect bounds1 = line1.getLocalBounds();
            sf::FloatRect bounds2 = line2.getLocalBounds();

            line1.setPosition(300 - bounds1.width/2, 250);
            line2.setPosition(300 - bounds2.width/2, 300);

            window.draw(line1);
            window.draw(line2);
        }

        window.display();
    }

    return 0;
}
