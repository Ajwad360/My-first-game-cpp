#include <iostream>
#include <conio.h>
#include <windows.h>
#include <string.h>



using namespace std;
static const int W = 50;
static const int H = 20;


//Various sequences for boss
bool sq1 = false; //First lvl
bool sq2 = false; //When boss is shooting from up
bool sq3 = false; //When boss comes down
bool movAllow = false; // A/D button activation during boss 
int health = 3; //Player health
int difficulty = 3; // 3 for Normal 5 for Hard
string healthbars[3] = {"###         ", "#######      ", "############"}; //Player health bar

void resetALL();
void gotHit();

class Console
{
    HANDLE h;

public:
    Console()
    {
        h = GetStdHandle(STD_OUTPUT_HANDLE);
        hideCursor();
        //lockSize();
    }

    void gotoxy(int x, int y)
    {
        COORD pos{(SHORT)x, (SHORT)y};
        SetConsoleCursorPosition(h, pos);
    }

    void hideCursor()
    {
        CONSOLE_CURSOR_INFO info{100, FALSE};
        SetConsoleCursorInfo(h, &info);
    }

    void lockSize()
    {
        SMALL_RECT rect{0, 0, W - 1, H + 1};
        COORD size{W, H + 2};
        SetConsoleWindowInfo(h, TRUE, &rect);
        SetConsoleScreenBufferSize(h, size);
    }
};

Console console;



class Level
{
public:

    string menuUI[14] = {
        "\033[30m",
        "##########################",
        "##########################",
        "##                      ##",
        "##                      ##",
        "##                      ##",
        "##                      ##",
        "##                      ##",
        "##                      ##",
        "##                      ##",
        "##########################",
        "##########################",
        "\033[0m",
    };

    string chatBoxUI[7] = {
        "#####################################",
        "#                                   #",
        "#                                   #",
        "#                                   #",
        "#                                   #",
        "#####################################",

    };


    string map[H] = {};

    //First Lvl
    string L1[H] = {
        "##################################################",
        "#                  #                              ",
        "#    ############  #                              ",
        "#    #             Q                              ",
        "#    # #############                              ",
        "#    #       <     #            ###$$#############",
        ">    #       <     #        ##   <$$$            #",
        ">    Q                  ##       <$$$   HELLO    #",
        "     #####$$##########           <$$$$           #",
        "####H#############################################",
        "#####         ####################################",
        "#                                                #",
        "#                                                #",
        "#                          <                     #",
        "#                          <                     #",
        "#                     ##   <                     #",
        "#               ###                        <     <",
        "#            ##                            <     <",
        "#         ##                       <    #         ",
        "############$$$$$$$$$$$$$#########################",
    };

    //Boss room
    string L2[H] = {
            "##################################################",
            "Q                                                Q",
            "Q                                                Q",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "#                                                #",
            "Q                                                Q",
            "Q                                                Q",
            "##################################################",
        };
    
    //Changing lvl
    void nextLevel(int number) {
        if (number == 1) {
            for (int i = 0; i < H; i++)
            {
                map[i] = L1[i];
            }
            
        } else if (number == 2) {
            for (int i = 0; i < H; i++)
            {
                map[i] = L2[i];
            }
            
        }
    }

    //Generating the map in terminal
    void drawMap()
    {
        console.gotoxy(0, 0);
        for (int y = 0; y < H; y++)
        {
            cout << map[y] << endl;
        }
    }
};

class Player {
    public:
    bool onGround = true;
    bool blockHead = false;
    bool xcollision = false;
    bool reverse = false;

    int x = 1, y = 18; // originals are x = 1 and y = 18 second level start x = 1, y = 6 test x = 37, y = 3
    int oldx = 1, oldy = 18;
};

Player player;
Level level;


class HoriBullet {
    public:

    int hbx, oldhbx, ypos;

    bool active = false;
    bool reverse = false;
    bool collision = false;

    void __init__(int x, int y, bool STATE, bool ACTIVE) {
        hbx = x;
        oldhbx= x;
        ypos = y;
        reverse = STATE;
        active = ACTIVE;
    }

    void xBlockCheck(int x, int y) {
        //Check for blocks
        if (reverse == false && level.map[y][x] == 'Q') {
            collision = true;
            active = false;
            return;
        }

        else if (reverse == true && level.map[y][x - 2] == 'Q') {
            collision = true;
            active = false;
            return;
        }
        collision = false;
    }

    void move()
    {   
        if (active == false) return;
        // delete old pos
        console.gotoxy(oldhbx, ypos);
        cout << " ";

        xBlockCheck(oldhbx + 1, ypos);
        if (collision == true) {
            active = false;
            return;
        }

        if (reverse == false) hbx++;
        
        else if (reverse == true) hbx--;

        

        // new pos
        console.gotoxy(hbx, ypos);
        cout << "\033[0;31m\033[1m-\033[0m";
        oldhbx = hbx;
    }
    
    void reset() {
        //delete pos
        console.gotoxy(hbx, ypos);
        cout << " ";
        __init__(0, 0, false, false);
    }
};

class VertBullet {
    public:
    int oldy, y, xpos;
    bool inGround = true;

    void __init__(int Y, int XPOS, bool STATE) {
        oldy = Y;
        y = Y;
        xpos = XPOS;
        inGround = STATE;
    }

    void move() {
        if (inGround != false) return;

        //delete pos
        console.gotoxy(xpos, oldy);
        cout << " ";

        //Checking ground
        if (y + 1 != 19) {
            y++;
        }
        else {
            if (player.y == y && xpos == player.x && inGround == false) {
            gotHit();
            reset();
            }

            inGround = true;
            return;
        }

        if (player.y == y && xpos == player.x && inGround == false) {
            gotHit();
            reset();
        }


        //new pos
        console.gotoxy(xpos, y);
        cout << "\033[0;31m\033[1m!\033[0m";

        oldy = y;
    }

    void reset() {
        //delete pos
        console.gotoxy(xpos, y);
        cout << " ";

        __init__(0, 0, true);
        
    }
};

class Menu {
    public:
    int cursorX = W/2;
    int cursorY = H/4;
    int select = 0;


    string options[3] = {"Start Game", "Exit Game", "Difficulty"};
    string difficulties[2] = {"Normal", "Hard"};
    string instructions[4] = {"W TO MOVE UP", "S TO MOVE DOWN", "SPACE TO SELECT"};
    string ingameINSTRUCTIONS[3] = {">>INGAME INSTRUCTIONS<<", "W TO JUMP", "A/D to move [DURING BOSS FIGHT ONLY]"};

    //Going up in the menu
    void up() {
  
                if (cursorY - 1 != 4) {

                    console.gotoxy(cursorX - 2, cursorY);
                    cout << "  ";

                    notSelected();

                    cursorY--;

                    selected();

                    console.gotoxy(cursorX - 2, cursorY);
                    cout << "\033[32m\033[1m>>\033[0m";
                }

                else {
                    console.gotoxy(cursorX - 2, cursorY);
                    cout << "  ";

                    notSelected();

                    cursorY = 7;

                    selected();
                    
                    console.gotoxy(cursorX - 2, cursorY);
                    cout << "\033[32m\033[1m>>\033[0m";
                }
    
    }

    //Going down in menu
    void down() {
        if (cursorY + 1 != 8) {

                    console.gotoxy(cursorX - 2, cursorY);
                    cout << "  ";

                    notSelected();

                    cursorY++;

                    selected();


                    console.gotoxy(cursorX - 2, cursorY);
                    cout << "\033[32m\033[1m>>\033[0m";
                }
        else {
                    console.gotoxy(cursorX - 2, cursorY);
                    cout << "  ";

                    console.gotoxy(cursorX, cursorY);
                    cout << options[cursorY - 5];

                    cursorY = 5;

                    console.gotoxy(cursorX, cursorY);
                    cout << "\033[0;31m\033[1m" << options[cursorY - 5] << "\033[0m";

                    console.gotoxy(cursorX - 2, cursorY);
                    cout << "\033[32m\033[1m>>\033[0m";
            }
    }


    //Difficulty selection
    void diffops() {
            for (int i = 0; i < 2; i++) {
                console.gotoxy(cursorX + 12, 7 + i);
                cout << difficulties[i];

                }

                cursorX = cursorX + 11;
                console.gotoxy(cursorX, cursorY);
                cout << "\033[0;33m>\033[0m";


                while (true) {
                    char k = _getch();

                    //Go down
                    if (k == 's') {
                        if (cursorY + 1 == 8) {
                            console.gotoxy(cursorX, cursorY);
                            cout << " ";

                            cursorY++;

                            console.gotoxy(cursorX, cursorY);
                            cout << "\033[0;33m>\033[0m";


                        }
                    }

                    //Go up
                    if (k == 'w') {
                        if (cursorY - 1 == 7) {
                            console.gotoxy(cursorX, cursorY);
                            cout << " ";

                            cursorY--;

                            console.gotoxy(cursorX, cursorY);
                            cout << "\033[0;33m>\033[0m";


                        }
                    }

                    //Press space to select START GAME OR EXIT
                    if (k == ' ') {
                        if (cursorY == 7) difficulty = 3;
                        else difficulty = 5;

                        console.gotoxy(cursorX, cursorY);
                        cout << " ";

                        cursorX = W/2;
                        cursorY = 5;

                    for (int i = 0; i < 2; i++) {
                        console.gotoxy(cursorX + 12, 7 + i);
                        cout << "      ";
                    }
                    
        
                    up();
                    break;
                }   
            } //while true

        }

    void notSelected() {
        console.gotoxy(cursorX, cursorY);
        cout << options[cursorY - 5];
    }

    void selected() {
        console.gotoxy(cursorX, cursorY);
        cout << "\033[0;31m\033[1m" << options[cursorY - 5] << "\033[0m";
    }

    //Opening the menu
    void menuOpen() { 

        system("CLS");

        for (int i = 0; i < 12; i++) {
            console.gotoxy(cursorX - 5, 1 + i);
            cout << level.menuUI[i] << endl;

        }

        console.gotoxy(cursorX - 2, cursorY);
        cout << "\033[32m\033[1m>>\033[0m";



        for (int i = 0; i < 3; i++) {
            console.gotoxy(50, 3 + i);
            cout << "\033[32m\033[4m" << instructions[i];

            console.gotoxy(50, 7 + i);
            cout << ingameINSTRUCTIONS[i] << "\033[0m";

            if (i == 0) {
                console.gotoxy(cursorX, cursorY + i);
                cout << "\033[0;31m\033[1m" << options[i] << "\033[0m";
                continue;
            }
                console.gotoxy(cursorX, cursorY + i);
                cout << options[i];
        } 



        

        while (true) {
    
            console.gotoxy(1, cursorY);
            cout << cursorY;

            char k = _getch();
            if (k == 'w') up();
            
            if (k == 's')  down();

            if (cursorY == 7) diffops();

            if (k == ' ' && cursorY == 5) break;
            if (k == ' ' && cursorY == 6) exit(0);

        }

    system("CLS");
    }


};

Menu menu;
HoriBullet onetime;
HoriBullet inshots[20];
VertBullet verBullets[20];

class Boss {

    public:
    int xback, xforw, ypos;
    bool bossVerShooting = false;
    bool active = false;
    bool bossreverse = false;
    int turns = 0; 
    int hits = 0;

    void __init__(int XBACK, int XFORW, int YPOS, bool STATE) {
        xback = XBACK;
        xforw = XFORW;
        ypos = YPOS;
        active = STATE;
    }

    void xBlockCheck(int x, int y) {
        //Check for reverse blocks
        if (bossreverse == false && level.map[y][x] == 'Q' && sq3 == false) {
            bossreverse = true;
            turns++;          
            return;
        }

        else if (bossreverse == true && level.map[y][x - 2] == 'Q' && sq3 == false) {
            bossreverse = false;
            turns++;

            return;


        }
    }

    //boss movement
    void move() {
        if (active == false) return;

        if (turns == difficulty) {
            sq3 = true;
            sq2 = false;
            active = false;
            stop = false;
            
            if (bossreverse == true) xforw--;
            else xforw++;

            turns = 0;
            for (int t = 0; t < 20; t++) {
                verBullets[t].reset();
                inshots[t].reset();
            }

            return;
            
        }


        // delete back old pos
        console.gotoxy(xback, ypos);
        cout << " ";
        console.gotoxy(xback, ypos - 1);
        cout << " ";
        
        xBlockCheck(xback + 1, ypos);

        if (bossreverse == false) {
            xforw++;
        }
        else if (bossreverse == true) {
            xforw--;
        }

        console.gotoxy(xforw, ypos);
        cout << "&";
        console.gotoxy(xforw, ypos - 1);
        cout << "&";

        
        xBlockCheck(xforw + 1, ypos);
    
        console.gotoxy(xback, ypos);
        cout << " ";
        console.gotoxy(xback, ypos - 1);
        cout << " ";

        if (bossreverse == false) {
        console.gotoxy(xforw + 1, ypos);
        cout << "&";
        console.gotoxy(xforw + 1, ypos - 1);
        cout << "&";
        }
        else {
        

        console.gotoxy(xforw - 1, ypos);
        cout << "&";
        console.gotoxy(xforw - 1, ypos - 1);
        cout << "&";
        }

        xback = xforw;

    }
    bool stop = true; //for downphase and upphase

    //Boss getting down
    void downphase() {
        if (stop == true) return;

        console.gotoxy(player.x, player.y);
        cout << " ";

        if (bossreverse == true) {
            console.gotoxy(4, 18);
            cout << "o";
        }
        else {
            console.gotoxy(44, 18);
            cout << "o";
        }

        while (stop == false) {
            //delete pos
            console.gotoxy(xback, ypos - 1);
            cout << " ";
            console.gotoxy(xforw, ypos - 1);
            cout << " ";

            if (ypos + 1 != 19) ypos++;
            else {

                console.gotoxy(xback, ypos - 1);
                cout << "X";
                console.gotoxy(xforw, ypos - 1);
                cout << "X";

                console.gotoxy(xback, ypos);
                cout << "X";
                console.gotoxy(xforw, ypos);
                cout << "X";

                bossVerShooting = true;
                stop = true;

                if (bossreverse == true) player.oldx = 4, player.oldy = 18, player.x = 4, player.y = 18;
                else {player.oldx = 44, player.oldy = 18, player.x = 44, player.y = 18;}

                Hcd1 = 4, Hcd2 = 7;

                return;
                }


            //new pos
            console.gotoxy(xback, ypos);
            cout << "&";
            console.gotoxy(xforw, ypos);
            cout << "&";
            Sleep(1000 / 6);
        }
    

    }

    //Boss going up again
    void upphase() {

        if (bossreverse == true) {
            console.gotoxy(3, 18);
            cout << "o";
            player.oldx = 3, player.x = 3, player.oldy = 18, player.y = 18;
        }
        else {
            console.gotoxy(44, 18);
            cout << "o";
            player.oldx = 44, player.x = 44, player.oldy = 18, player.y = 18;
        }

        if (hits == 1) hit1text();
        else if (hits == 2) hit2text();

        while (true) {

            //delete pos
            console.gotoxy(xback, ypos);
            cout << " ";
            console.gotoxy(xforw, ypos);
            cout << " ";

            if (ypos - 2 != 0) ypos--;
            else {
                console.gotoxy(xback, ypos);
                cout << "&";
                console.gotoxy(xforw, ypos);
                cout << "&";
                sq3 = false;
                active = true;
                
                if (bossreverse == true) xforw++;
                else xforw--;

                chatClear();
                return;
            }


            //new pos 
            console.gotoxy(xback, ypos - 1);
            cout << "&";
            console.gotoxy(xforw, ypos - 1);
            cout << "&";
            Sleep(1000 / 6);
        }

    }

    //Bullet cooldowns
    int Hcd1 = 4;
    int Hcd2 = 7;
    int bulnum = 0;

    //After downphase, boss shoots
    void verShooting() {    

        if (bossVerShooting == false) return;

        Hcd1--;
        Hcd2--;   

        if (bossreverse == true) {   
            if (Hcd1 == 0) {
                inshots[bulnum].__init__(44, 17, true, true);
                bulnum++;
                Hcd1 = 10;
                
            }

            if (bulnum == 20) bulnum = 0;

            if (Hcd2 == 0) {
                inshots[bulnum].__init__(44, 18, true, true);
                bulnum++;
                Hcd2 = 16;
            }
            if (bulnum == 20) bulnum = 0;
        }
        else {
            if (Hcd1 == 0) {
                inshots[bulnum].__init__(3, 17, false, true);
                bulnum++;
                Hcd1 = 10;
                
            }

            if (bulnum == 20) bulnum = 0;

            if (Hcd2 == 0) {
                inshots[bulnum].__init__(3, 18, false, true);
                bulnum++;
                Hcd2 = 16;
            }
            if (bulnum == 20) bulnum = 0;

        }
        
        for (int t = 0; t < 20; t++) {
            inshots[t].move();


            if (inshots[t].ypos == 18 && inshots[t].reverse == true) {
                if (inshots[t].hbx == player.x - 1 && inshots[t].ypos == player.y && inshots[t].active == true) {inshots[t].reset(); gotHit();}
            }
            else if (inshots[t].ypos == 17 && inshots[t].reverse == false) {
                if (inshots[t].hbx == player.x + 1 && inshots[t].ypos == player.y && inshots[t].active == true) {inshots[t].reset(); gotHit();}
            }

            else if (inshots[t].hbx == player.x && inshots[t].ypos == player.y && inshots[t].active == true) {inshots[t].reset(); gotHit();}
            
            }

        }  

//Boss chat when player enters room
    void text1() {

        cout << "\033[0;31m\033[1m";

        console.gotoxy(60, 5);
        cout << "You really think...";
        console.gotoxy(60, 6);
        cout << "you can beat ME?!";


        for (int w = 0; w < 10; w++) {
            Sleep(1000 / 3);
        }

        cout << "\033[0m";
        chatClear();
    }

    void chatClear() {
        for (int i = 0; i < 4; i++) {
            console.gotoxy(60, 4 + i);
            cout << "                                 ";
        }
    }

//Hitting boss first time text
    void hit1text() {
        cout << "\033[0;31m\033[1m";

        console.gotoxy(60, 5);
        cout << "One lucky hit doesn't faze me";

        cout << "\033[0m";
    }

//Hitting boss second time text
    void hit2text() {
        cout << "\033[0;31m\033[1m";

        console.gotoxy(60, 5);
        cout << "Enough playing...";
        console.gotoxy(60, 6);
        cout << "Time to get serious";

        cout << "\033[0m";
    }

    //Message after player gets damaged once
    void gotDamaged1() {
        cout << "\033[0;31m\033[1m";

        console.gotoxy(60, 5);
        cout << "I haven't even started trying and";
        console.gotoxy(60, 6);
        cout << "and you already got hit?";

        cout << "\033[0m";
    } 

    //Message after player gets damaged twice
    void gotDamaged2() {
        cout << "\033[0;31m\033[1m";

        console.gotoxy(60, 5);
        cout << "You're learning....";
        console.gotoxy(60, 6);
        cout << "too slowly";

        cout << "\033[0m";
    }

    //Message after player gets damaged thrice
    void gotDamaged3() {
        cout << "\033[0;31m\033[1m";

        console.gotoxy(60, 5);
        cout << "Try again in another";
        console.gotoxy(60, 6);
        cout << "100 years";

        cout << "\033[0m";
    }

    void endSequence() {
        console.gotoxy(xback, ypos);
        cout << " ";
        console.gotoxy(xforw, ypos);
        cout << " ";

        console.gotoxy(xback, ypos - 1);
        cout << " ";
        console.gotoxy(xforw, ypos - 1);
        cout << " ";

        console.gotoxy(60, 12);
        cout << "                   ";

        console.gotoxy(60, 10);
        cout << "Congratulations on beating the boss!";
        console.gotoxy(60, 11);
        cout << "Thanks for playing!";
        for (int t = 0; t < 15; t++) {
            Sleep(1000 / 2);

        }

        resetALL();
        
        level.nextLevel(1);

        menu.menuOpen();

        level.drawMap();
        console.gotoxy(player.oldx, player.oldy);
        cout << "o";
        
    }
};


Boss boss;

class Game
{
public:
    
    string p1;  

    Game()
    {
        level.nextLevel(1);
        system("CLS");

        menu.menuOpen();

        level.drawMap();
        console.gotoxy(player.oldx, player.oldy);
        cout << "o";
    }

    void yBlockCheck(int x, int y)
    {
        //Checking for jump collision
        if (level.map[y - 2][x] == '#')
        {
            player.blockHead = true;
        }
        else
            player.blockHead = false;

        //Checking for lava
        if (level.map[y][x] == '$')
        {
            player.onGround = true;
            levelFail();
        }

        //Checking if player on ground
        if (!(level.map[y][x] == '#'))
        {
            player.onGround = false;
            return;
        }

        // Hitting boss
        if ((x == boss.xback || x == boss.xforw) && ((y - 1 == boss.ypos - 1) || y - 1 == boss.ypos) && sq3 == true ) {

            for (int t = 0; t < 20; t++) {
                inshots[t].reset();
            }


            boss.bossVerShooting = false, sq3 = false, sq2 = true, boss.turns = 0;
            boss.hits++;

            if (boss.hits == 3) boss.endSequence();
            boss.upphase();
        }


        player.onGround = true;
    }

    void xBlockCheck(int x, int y)
    {

        //Checking for pins
        if (level.map[y][x] == '<')
            levelFail();

        //Checking for shot
        if (x == onetime.hbx && y == onetime.ypos) levelFail();

        //Checking for reverse
        if (level.map[y][x] == 'Q' && player.reverse == false)
        {
            player.reverse = true;
            return;
        }
        if (level.map[y][x - 2] == 'Q' && player.reverse == true)
        {
            player.reverse = false;
            return;
        }

        //Checking for block ahead
        if (level.map[y][x] == '#' && player.reverse == false)
        {
            player.xcollision = true;
            return;
        }
        //Reverse block ahead
        else if (level.map[y][x - 2] == '#' && player.reverse == true)
        {
            player.xcollision = true;
            return;
        }

        player.xcollision = false;
    }

    void teleport()
    {
        //Second floor teleport in first lvl
        if (player.x == 49 && player.y == 18)
        {
            // delete pos
            console.gotoxy(player.x, player.y);
            cout << " ";

            // new pos
            player.x = 1, player.y = 7, player.oldx = 1, player.oldy = 7;
            console.gotoxy(player.x, player.y);
        }

        //Jump pad in first lvl
        if (level.map[player.y + 1][player.x] == 'H')
        {
            while (true)
            {
                // delete old pos
                console.gotoxy(player.x, player.y);
                cout << " ";

                yBlockCheck(player.x, player.y + 1);

                if (player.blockHead == false)
                    player.y -= 1;
                else
                {
                    player.oldx = player.x;
                    player.x += 1;
                    console.gotoxy(player.x, player.y);
                    cout << "o";

                    break;
                }
                // new pos
                console.gotoxy(player.x, player.y);
                cout << "o";
                player.oldy = player.y;
                Sleep(1000 / 8);
            
            }
        }

        //Going to second lvl
        if (player.x == 49 && (player.y == 4 || player.y == 3 || player.y == 2))
        {
            system("CLS");
            level.nextLevel(2);
            level.drawMap();

            for (int i = 0; i < 6; i++) {
                console.gotoxy(58, 3 + i);
                cout << level.chatBoxUI[i];
            }

            sq1 = false;
            movAllow = true;
            sq2 = true;
            player.oldx = 3, player.x = 3, player.oldy = 17, player.y = 17;
            boss.__init__(17, 17, 2, true);
            boss.text1();
        }

        //Generating shot in first lvl
        if (player.x == 32 && player.y == 4) {
           onetime.__init__(49, 4, true, true);
            sq1 = true;
        }
    }

    //Level failed
    void levelFail()
    {
        console.gotoxy(10, 20);
        cout << "LEVEL FAILED";
        char k;
        while (true)
        {
            if (_kbhit())
            {
                k = _getch();
                if (k == 'o')
                {   
                    resetALL();
                    break;
                }
            }
        }
    }
    //move for player
    void move()
    {   
        //Showing health
        if (sq2 == true || sq3 == true) {


            console.gotoxy(60, 11);
            if (health == 3) cout << "\033[0;32m\033[1m";
            else if (health == 2) cout << "\033[0;33m\033[1m";
            else cout << "\033[0;31m\033[1m";

            cout << "HEALTH  -->  " << health;

            console.gotoxy(60, 12);

            if (health == 0) cout << "     ";
            else cout << healthbars[health - 1];

            cout << "\033[0m";

        }

        //If health becomes zero   
        if (health == 0) levelFail();

        // delete old pos
        console.gotoxy(player.oldx, player.oldy);
        cout << " ";

        xBlockCheck(player.x + 1, player.y);

        //Player movement (reverse or not reverse)
        if (player.xcollision == false && player.reverse == false)
        {
            player.x = player.oldx + 1;
        }
        else if (player.xcollision == false && player.reverse == true)
        {
            player.x = player.oldx - 1;
        }

        // new pos
        console.gotoxy(player.x, player.y);

        
        //Generating the player character
        if (sq3 == true) p1 = player.reverse ? "<" : ">";
        else {p1 = player.onGround ? "o" : "O";}
        cout << p1;


        player.oldx = player.x;

        //Bullet movement on lvl
        if (sq1 == true) {
            onetime.move();
        }

        //Going to different places in first lvl
        teleport();
        
        //boss movement and raining bullets from both sides sequence
        if (sq2 == true) {
            boss.move();
            if (boss.active == true) {
            rain();
            horirain();
            }

        }

        //Boss comes down and shoots
        if (sq3 == true) {
            boss.downphase();
            boss.verShooting();
        }

    }

    //Checking for key presses 
    void update()
    {
        if (_kbhit())
        {
            char k = _getch();

            //Jump
            if (k == 'w' && player.onGround == true) {
                jump();
                while (_kbhit()) {
                    if (_getch() != 'w') break;
                }
            }

            //Reset
            if (k == 'o')
            {
                resetALL();
                player.oldx = 38, player.x = 38, player.oldy = 3, player.y = 3;
            }

            // A/D movement in boss room
            if (movAllow) {
                if (k == 'a') {
                    player.reverse = true;
                    while (_kbhit()) if (_getch() != 'a') break;
                }
                else if (k == 'd') {
                    player.reverse = false;
                    while (_kbhit()) if (_getch() != 'd') break;
                }
            }
        }

        player.oldy = player.y;

        //Jumping logic
        yBlockCheck(player.x, player.y + 1);

        if (player.blockHead == true)
            times = 10;

        if (times < 2 && player.blockHead == false)
        {
            player.y -= 1;
            times += 1;
        }
        else if (player.onGround == false)
        {
            player.y += 1;
        }
        else
        {
            times = 10;
        }
    }
    int times = 10;

    void jump()
    {
        if (times != 10)
            return;

        times = 0;
    }

    //cooldown for vertical bullets
    int Vcooldown1 = 5;
    int Vcooldown2 = 7;
    int bulnum = 0;

    //Vertical bullets spawning from the boss
    void rain() {

        Vcooldown1--;
        Vcooldown2--;

        if (bulnum == 20) {
            bulnum = 0;
        }
        if (Vcooldown1 == 0) {
            verBullets[bulnum].__init__(boss.ypos + 1, boss.xforw, false);
            Vcooldown1 = 5;
            bulnum++;

        }

        if (bulnum == 20) {
            bulnum = 0;
        }

        if (Vcooldown2 == 0) {
            verBullets[bulnum].__init__(boss.ypos + 1, boss.xforw, false);
            Vcooldown2 = 7;
            bulnum++;
        }


        //for moving all the bullets that have been created
        for (int t = 0; t < 20; t++) {
            verBullets[t].move();
        }
    }

    //Cooldown for horizontal bullets
    int Hcooldown1 = 15;
    int Hcooldown2 = 20;
    int Hbulnum = 0;
    int upBUL = 0;
    int downBUL = 0;

    //Horizontal bullets coming from both sides of the room
    void horirain() {
        
        Hcooldown1--;
        Hcooldown2--;
        
        if (Hbulnum == 20) {
            Hbulnum = 0;
            upBUL = 0;
        }
        if (Hcooldown1 == 0) {
            downBUL++;
            int adjustment = downBUL % 2 == 0 ? 1 : 0;
            inshots[Hbulnum].__init__(48 - adjustment, 18, true, true);
            Hcooldown1 = 19;
            Hbulnum++;
        }

        if (Hbulnum == 20) {
            downBUL = 0;
            Hbulnum = 0;
        }

        if (Hcooldown2 == 0) {
            upBUL++;
            int adjustment = upBUL % 2 == 0 ? 1 : 0;
            inshots[Hbulnum].__init__(1 + adjustment, 17, false, true);
            Hcooldown2 = 25;
            Hbulnum++;

        }



        for (int t = 0; t < 20; t++) {
            inshots[t].move();
            
            if (inshots[t].hbx == player.x && inshots[t].ypos == player.y && inshots[t].active == true) {
                gotHit();
                inshots[t].reset();
            }
        }

    }

    //Keeps running everything
    void run()
    {
        while (true)
        {   
            move();
            update();
            Sleep(1000 / 6);
        }
    }
};

Game game;

//Resetting game
void resetALL() {
    for (int t = 0; t < 20; t++) {
        verBullets[t].reset();
        inshots[t].reset();
    }

    boss.__init__(17, 17, 2, false);
    boss.bossreverse = false;
    boss.turns = 0, boss.bossVerShooting = false;
    game.Vcooldown1 = 5, game.Vcooldown2 = 7, game.Hcooldown1 = 15, game.Hcooldown2 = 20; game.bulnum = 0, game.Hbulnum = 0;
    boss.Hcd1 = 4, boss.Hcd2 = 7, boss.hits = 0;
    player.oldx = 1, player.oldy = 17, player.x = 1, player.y = 17, player.reverse = false, sq1 = false, sq2 = false, sq3 = false, boss.active = false, movAllow = false, health = 3;
    
    

    system("CLS");
    level.nextLevel(1);
    level.drawMap();
    
}

//Player gets hit
void gotHit() {
    health--;
    if (health == 0) {boss.gotDamaged3(); return;}
    else {
        if (health == 2) {
            boss.gotDamaged1();
        }
        else if (health == 1) {
            boss.gotDamaged2();
        }

        for (int t = 0; t < 7; t++) {
            console.gotoxy(player.x, player.y);
            cout << " ";

            Sleep(1000 / 6);

            console.gotoxy(player.x, player.y);
            cout << game.p1;

            Sleep(1000 / 6);

        }

        boss.chatClear();
    }
}

int main()
{
    game.run();
    return 0;
}
