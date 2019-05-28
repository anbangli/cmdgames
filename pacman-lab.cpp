/*********************************************************************
  吃豆人(Pac-Man) 是电子游戏历史上的经典街机游戏，由日本南梦宫株式会社
（NAMCO）的岩谷彻设计，最早是1980年在街机平台发布。
  Pac-Man 被认为是80年代最经典的街机游戏之一。游戏者控制吃豆人吃掉迷宫
里面的所有豆子，同时尽可能躲避小鬼怪。一旦吃豆人吃掉能量药丸，它就可以
在一定时间内反过来欺负小鬼怪了。
  
此版本原始代码：https://github.com/azbardini/pac_dos_man
作者是巴西人 Augusto Zanella Bardini，原始代码中以葡萄牙语注释。 
利用机器翻译为英文注释。并对程序进行了部分修改。--anbangli@qq.com 
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <locale.h>

#define LINE            22
#define COLUMN          38

#define PILL            111
#define SUPER_PILL      42
#define WALL            35
#define GHOST           87
#define PACMAN          67
#define MAX_GHOSTS      5

//Struct of PacMan
typedef struct str_pacman {
	int x,y;
	int start_x,start_y;
	int power;
} Pacman;

//Struct of ghost
typedef struct str_ghost {
	int x,y;
	int start_x,start_y;
	int alive;
	char last_move;
} Ghost;

//functions

void gotoxy(int x, int y) {
	// Move the cursor to a place on the screen
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD) {
		x-1,y-1
	});
}

int Find_Ghost_Qtd(char map[][COLUMN]) {
	int i,j;
	int qnt = 0;
	for(i=0; i<LINE; i++)
		for(j=0; j<COLUMN; j++)
			if(map[i][j] == GHOST)
				qnt++;
	return qnt;
}

void SetConsoleSize(unsigned width, unsigned height) {
	SMALL_RECT r;
	COORD c;
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	c.X = width;
	c.Y = height;
	SetConsoleScreenBufferSize(stdoutHandle, c); // Set screen storage buffer size

	r.Left = 0;
	r.Top = 0;
	r.Right = width - 1;
	r.Bottom = height - 1;
	SetConsoleWindowInfo(stdoutHandle, 1, &r); // Adjust screen size
}

void Get_Map(char map[][COLUMN]) {	// Get the map data from file
	int i,j;
	FILE *fp;
	fp = fopen("pacman.txt","r");
	for(i=0; i<LINE; i++) {
		for(j=0; j<COLUMN; j++)
			fscanf(fp, "%c", &map[i][j]);
	}
	fclose(fp);
}

void Find_Pacman(char map[][COLUMN], int *posX, int *posY) {
	//Find pacman's position
	int i,j;
	for(i=0; i<LINE; i++)
		for(j=0; j<COLUMN; j++)
			if(map[i][j] == PACMAN) {
				*posX = j;
				*posY = i;
			}

}

void Find_Ghost(char map[][COLUMN],int *posX, int *posY,int current) {
	//Find the position of the ghost
	int i,j;
	int qnt = 0;
	for(i=0; i<LINE; i++)
		for(j=0; j<COLUMN; j++)
			if(map[i][j] == GHOST && current == qnt++) {
				*posX = j;
				*posY = i;
			}
}

void Print_Map(char map[][COLUMN]) {
	// Printed labyrinth on screen

	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//3-cyan  4-red 5-mega  6-orange  7-white  8-white2
	int i,j;
	//use Full-width characters for drawing
	for(i=0; i<LINE; i++) {
		for(j=0; j<COLUMN; j++) {
			if(map[i][j] == WALL) {
				SetConsoleTextAttribute(hConsole,  8);	//CYAN	//9);
				printf("█");
			} else if(map[i][j] == PILL) {
				SetConsoleTextAttribute(hConsole, 15);
				printf("Ｏ");
			} else if(map[i][j] == SUPER_PILL) {
				SetConsoleTextAttribute(hConsole, 14);
				printf("＊");
			} else if(map[i][j] == PACMAN) {
				SetConsoleTextAttribute(hConsole, 14);
				printf("Ｃ");
			} else if(map[i][j] == GHOST) {
				SetConsoleTextAttribute(hConsole, 12);
				printf("Ｗ");
			} else
				printf("  ");	//,map[i][j]);
		}
		printf("\n");
	}

}

void Update_Score(long *player_score,int score) {
	//update score

	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	*player_score = *player_score + score;
	gotoxy(1,LINE+1);
	SetConsoleTextAttribute(hConsole, 2);
	printf("Score: %d", *player_score);
}

void Update_Time(int *gameplay_time) {
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	gotoxy(1,LINE+2);
	SetConsoleTextAttribute(hConsole, 2);
	printf("Time:  %d", *gameplay_time);
}

void Update_Lives(int *lives) {
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	gotoxy(1,LINE+3);
	SetConsoleTextAttribute(hConsole, 2);
	printf("Lives: %d", *lives);
}

int Verify_Win(char map[][COLUMN]) {
	int i, j;
	for(i=0; i<LINE; i++)
		for(j=0; j<COLUMN; j++)
			if(map[i][j] == PILL)
				return 0;
	return 1;
}

void Restart_Game(int *start_x, int *start_y, int *posX, int *posY,int *mov,int *lives) {
	//Restart game

	int i;
	gotoxy(*posX+1,*posY+1);
	printf(" ");
	*posX = *start_x;
	*posY = *start_y;
	*mov = ' ';
	gotoxy((*start_x+1)*2-1,*start_y+1);	//gotoxy(*start_x+1,*start_y+1);
	printf("Ｃ");

	if(*lives != 0) {
		for(i=5; i>=0; i--) {
			Sleep(300);
			gotoxy(COLUMN-6,LINE+2);
			printf("ARE YOU READY?");
			Sleep(300);
			gotoxy(COLUMN-6,LINE+2);
			printf("      %d       ",i);
		}
		gotoxy(COLUMN-6,LINE+2);
		printf("    GO!      ");
		Sleep(250);
		gotoxy(COLUMN-6,LINE+2);
		printf("             ");
	}
}

void Print_Ghost(int *super) {
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (*super)
		SetConsoleTextAttribute(hConsole, 11);
	else
		SetConsoleTextAttribute(hConsole, 12);
	printf("Ｗ");
}

void Move_Pacman(int *posX, int *posY, char mov) {
	//Move Pacman based on where the player wants to go

	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 14);

	gotoxy((*posX+1)*2-1, *posY+1);	//gotoxy(*posX+1,*posY+1);
	printf("  ");	//clear
	switch(mov) {
		case 'd':	*posX = *posX + 1;	break;	//right
		case 'w':	*posY = *posY - 1;	break;	//up
		case 'a':	*posX = *posX - 1;	break;	//left
		case 's':	*posY = *posY + 1;	break;	//down	
	}
	gotoxy((*posX+1)*2-1, *posY+1);	//gotoxy(*posX+1,*posY+1);
	printf("Ｃ");	//draw Pacman

}

int module(int x) {
	if (x < 0)
		return -x;
	return x;
};

//Change!
void Move_Ghost(char map[][COLUMN], int *ghostX, int *ghostY, int *pacX, int *pacY, int *super, char* last_move) {
	int flag_same_direction = 1, flag_stopped = 1;      //flags not to change direction or stand still
	int difX = *ghostX - *pacX;                         //distance from the X axis from a ghost to the pacman
	int difY = *ghostY - *pacY;                         //distance from the axis Y from a ghost to the pacman
	if(*super == 1) {
		*last_move = 'p';
		difX *= -1;                         //distance from the X axis from a ghost to the pacman
		difY *= -1;                         //distance from the Y axis from a ghost to the pacman
	}
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//srand(time(NULL));
	if (rand()%10 < 7) {                                //70% Ghost's chance to follow pacman
		if(module(difX) > module(difY)) {
		//If the module of the distance of the X axis greater than that of Y, it tries to follow in X
			if(difX > 0) {	//if the distance of the ghost in X is greater than 0, try to go left
				if(map[*ghostY][*ghostX - 1] != WALL && *last_move != 'd') {
					gotoxy((*ghostX + 1)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 1,*ghostY + 1);
					if(map[*ghostY][*ghostX] == GHOST)
						printf(" ");
					else {
						if(map[*ghostY][*ghostX] == SUPER_PILL)
							SetConsoleTextAttribute(hConsole, 14);
						else
							SetConsoleTextAttribute(hConsole, 15);
						if(map[*ghostY][*ghostX] != 'C')
							printf("%c",map[*ghostY][*ghostX]);
						else
							printf("%c", ' ');
					}
					gotoxy(*ghostX *2-1,*ghostY + 1);	//gotoxy(*ghostX,*ghostY + 1);
					SetConsoleTextAttribute(hConsole, 12);
					Print_Ghost(super);
					*ghostX = *ghostX - 1;
					*last_move = 'a';
					flag_stopped = 0;                   //flag verifies that the ghost moves
				}

			} else {	//if the distance of the ghost in X is less than 0, try to go right
				if(map[*ghostY][*ghostX + 1] != WALL && *last_move != 'a') {
					gotoxy((*ghostX + 1)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 1,*ghostY + 1);
					if(map[*ghostY][*ghostX] == GHOST)
						printf(" ");
					else {
						if(map[*ghostY][*ghostX] == SUPER_PILL)
							SetConsoleTextAttribute(hConsole, 14);
						else
							SetConsoleTextAttribute(hConsole, 15);
						if(map[*ghostY][*ghostX] != 'C')
							printf("%c",map[*ghostY][*ghostX]);
						else
							printf("%c", ' ');
					}
					gotoxy((*ghostX + 2)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 2,*ghostY + 1);
					SetConsoleTextAttribute(hConsole, 12);
					Print_Ghost(super);
					*ghostX = *ghostX + 1;
					*last_move = 'd';
					flag_stopped = 0;                   //flag verifies that the ghost moves

				}

			}
		} else { // if the modulo of the distance of the axis Y is greater than that of the X, it tries to follow in Y
			if(difY > 0)   //if the distance of the ghost in Y is greater than 0, try to go up
			{
				if(map[*ghostY - 1][*ghostX] != WALL && *last_move != 'x') {
					gotoxy((*ghostX + 1)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 1,*ghostY + 1);
					if(map[*ghostY][*ghostX] == GHOST)
						printf(" ");
					else {
						if(map[*ghostY][*ghostX] == SUPER_PILL)
							SetConsoleTextAttribute(hConsole, 14);
						else
							SetConsoleTextAttribute(hConsole, 15);
						if(map[*ghostY][*ghostX] != 'C')
							printf("%c",map[*ghostY][*ghostX]);
						else
							printf("%c", ' ');
					}
					gotoxy((*ghostX + 1)*2-1,*ghostY);	//gotoxy(*ghostX + 1,*ghostY);
					Print_Ghost(super);
					*ghostY = *ghostY - 1;
					*last_move = 'w';
					flag_stopped = 0;                   //flag verifies that the ghost moves

				}

			} else	// if the ghost distance at Y is less than 0, try to go down
			{
				if(map[*ghostY + 1][*ghostX] != WALL && *last_move != 'w') {
					gotoxy((*ghostX + 1)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 1,*ghostY + 1);
					if(map[*ghostY][*ghostX] == GHOST)
						printf(" ");
					else {
						if(map[*ghostY][*ghostX] == SUPER_PILL)
							SetConsoleTextAttribute(hConsole, 14);
						else
							SetConsoleTextAttribute(hConsole, 15);
						if(map[*ghostY][*ghostX] != 'C')
							printf("%c",map[*ghostY][*ghostX]);
						else
							printf("%c", ' ');
					}
					gotoxy((*ghostX + 1)*2-1,*ghostY + 2);	//gotoxy(*ghostX + 1,*ghostY + 2);
					SetConsoleTextAttribute(hConsole, 12);
					Print_Ghost(super);
					*ghostY = *ghostY + 1;
					*last_move = 'x';
					flag_stopped = 0;                   //flag verifies that the ghost moves
				}

			}
		}
	}
	//if the ghost has not moved, the flag_stopped will be zeroed, this means that 
	//the ghost can not follow the pacman because there is a wall preventing.
	//In this case, the ghost acquires a random character, and goes anywhere as possible
	//until he can move.
	//RANDOMIC PART:
	if (flag_stopped == 1) {
		do {
			switch(rand()%4) {
				case 0: //Attempt to go to UP
					//if the wall is not up and the last movement is not down
					//If the ghost is out of existence, as tested here the program ignores that 
					//it can not "walk backwards" so the ghost escapes from the path 
					//without leaving the same thing made for the other 3 sides
					if(map[*ghostY - 1][*ghostX] != WALL && ((*last_move != 'x')||
					    ((map[*ghostY + 1][*ghostX] == WALL) && 
						 (map[*ghostY][*ghostX + 1] == WALL) && 
					     (map[*ghostY][*ghostX - 1] == WALL)    
					    )    )) {
						gotoxy((*ghostX + 1)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 1,*ghostY + 1);
						
						if(map[*ghostY][*ghostX] == GHOST)
							printf(" ");
						else {
							if(map[*ghostY][*ghostX] == SUPER_PILL)
								SetConsoleTextAttribute(hConsole, 14);
							else
								SetConsoleTextAttribute(hConsole, 15);
							if(map[*ghostY][*ghostX] != 'C')
								printf("%c",map[*ghostY][*ghostX]);
							else
								printf("%c", ' ');
						}
						gotoxy((*ghostX + 1)*2-1, *ghostY);	//gotoxy(*ghostX + 1,*ghostY);
						Print_Ghost(super);
						*ghostY = *ghostY - 1;
						*last_move = 'w';
						flag_same_direction = 0; //verifies that the ghost has moved randomly
						break;
					} else

					case 1: //Tentativa de ir para DIREITA
					if(map[*ghostY][*ghostX + 1] != WALL && ((*last_move != 'a')||
					        (
					            (map[*ghostY][*ghostX - 1] == WALL)&&
					            (map[*ghostY - 1][*ghostX] == WALL)&&
					            (map[*ghostY + 1][*ghostX] == WALL)
					        ))) {
						gotoxy((*ghostX + 1)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 1,*ghostY + 1);
						if(map[*ghostY][*ghostX] == GHOST)
							printf(" ");
						else {
							if(map[*ghostY][*ghostX] == SUPER_PILL)
								SetConsoleTextAttribute(hConsole, 14);
							else
								SetConsoleTextAttribute(hConsole, 15);
							if(map[*ghostY][*ghostX] != 'C')
								printf("%c",map[*ghostY][*ghostX]);
							else
								printf("%c", ' ');
						}
						gotoxy((*ghostX + 2)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 2,*ghostY + 1);
						SetConsoleTextAttribute(hConsole, 12);
						Print_Ghost(super);
						*ghostX = *ghostX + 1;
						*last_move = 'd';
						flag_same_direction = 0;  //verifies that the ghost has moved randomly
						break;
					} else

					case 2: //Tentativa de ir para BAIXO
					if(map[*ghostY + 1][*ghostX] != WALL && ((*last_move != 'w')||(
					            (map[*ghostY - 1][*ghostX] == WALL) &&
					            (map[*ghostY][*ghostX + 1] == WALL) &&
					            (map[*ghostY][*ghostX - 1] == WALL)
					        )
					                                        )) {
						gotoxy((*ghostX + 1)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 1,*ghostY + 1);
						if(map[*ghostY][*ghostX] == GHOST)
							printf(" ");
						else {
							if(map[*ghostY][*ghostX] == SUPER_PILL)
								SetConsoleTextAttribute(hConsole, 14);
							else
								SetConsoleTextAttribute(hConsole, 15);
							if(map[*ghostY][*ghostX] != 'C')
								printf("%c",map[*ghostY][*ghostX]);
							else
								printf("%c", ' ');
						}
						gotoxy((*ghostX + 1)*2-1,*ghostY + 2);	//gotoxy(*ghostX + 1,*ghostY + 2);
						SetConsoleTextAttribute(hConsole, 12);
						Print_Ghost(super);
						*ghostY = *ghostY + 1;
						*last_move = 'x';
						flag_same_direction = 0;  //verifies that the ghost has moved randomly
						break;
					} else

					case 3: //Tentativa de ir para ESQUERDA
					if(map[*ghostY][*ghostX - 1] != WALL && ((*last_move != 'd')||(
					            (map[*ghostY][*ghostX + 1] == WALL)&&
					            (map[*ghostY - 1][*ghostX] == WALL)&&
					            (map[*ghostY + 1][*ghostX] == WALL)
					        ))) {
						gotoxy((*ghostX + 1)*2-1,*ghostY + 1);	//gotoxy(*ghostX + 1,*ghostY + 1);
						if(map[*ghostY][*ghostX] == GHOST)
							printf(" ");
						else {
							if(map[*ghostY][*ghostX] == SUPER_PILL)
								SetConsoleTextAttribute(hConsole, 14);
							else
								SetConsoleTextAttribute(hConsole, 15);
							if(map[*ghostY][*ghostX] != 'C')
								printf("%c",map[*ghostY][*ghostX]);
							else
								printf("%c", ' ');
						}
						gotoxy(*ghostX*2-1,*ghostY + 1);	//gotoxy(*ghostX,*ghostY + 1);
						SetConsoleTextAttribute(hConsole, 12);
						Print_Ghost(super);
						*ghostX = *ghostX - 1;
						*last_move = 'a';
						flag_same_direction = 0;    //verifies that the ghost has moved randomly
						break;
					}
			}
		} while (flag_same_direction == 1);
	}
}

void Pause_Game(int *is_paused) {
	if(*is_paused) {
		gotoxy(COLUMN/2-3,LINE+3);
		printf("      ");
		*is_paused = *is_paused - 1;
	} else {
		gotoxy(COLUMN/2-3,LINE+3);
		printf("PAUSED");
		*is_paused = *is_paused + 1;
	}
}

void End_Game(long int *player_score, int *lives) {
	char end;
	system("cls");
	gotoxy(COLUMN-5,LINE/2);
	if(*lives == 0) {
		printf("GAME OVER");
		gotoxy(COLUMN-5,LINE/2+1);
		printf("SCORE: %d", *player_score);
	} else {
		printf("VICTORY!!!");
		gotoxy(COLUMN-5,LINE/2);
		printf("SCORE: %d", *player_score);
	}
	gotoxy(COLUMN-5,LINE/2+3);
	//printf("Press 'E' to exit");
	printf("Press ENTER to exit");
	//gotoxy(COLUMN-5,LINE/2+1);
	//while (tolower(end) != 'e')
	end = getchar();
}

//CODE:

/*"changelog":
 collision bug with ghosts fixed
 I put a timer for ghosts and pacman instead of sleep
 I made a flag pro pause on the letter p, locking all the moves
 I modified the function that governs the playing time because of the change in sleep to timer
 I created the is_powerful flag that will activate when the pacman eats a super-chip,
 she controls the speeds of ghosts
 gave superpower to pacman, automatically reduces ghosts, eats ghosts
 I added the game end showing score
 IN TRUTH THAT HAS NEVER BEEN DONE, BUT IT WORKS WITHOUT: (making the ghost only change direction when finding fork)
 COMPLETE ::: 70% chance of pacman being followed, and ghosts restart the game in the initial position

 Of all the items in the definition of the work, there are still:

 (minor importance) make the pacman receive command change of direction before he finds a fork.
 (minor importance) add directional arrows in the game
*/

int main() {
	//Variables:
	char map[LINE][COLUMN] = {	//define map and set example data
		"#####################################",
		"#   OO OO OOO OOO * OOOO OO OOO O   #",
		"# # ##### ######## ###### ####### # #",
		"# # ###      #####O####     ##### # #",
		"# #O########      C     #########O# #",
		"# #O   ##### #####O#### ####     O# #",
		"# #O   W     ##### ####          O# #",
		"#  O# ## ###      O     ## ######O  #",
		"# # # ##    O#####O####O   ###### # #",
		"# # #    ### ##### #### ##      # # #",
		"# ##########   O  W O   ########### #",
		"# ##########O##########o########### #",
		"#  O O*OO    ##########    O *  OO  #",
		"# #### # ###O##########O## ## ##### #",
		"#  ### # ### W OOOOO  W ## ## ####  #",
		"# ####   ######### # #####    ##### #",
		"# # ## # ######  #O# ##### ## ### # #",
		"# O    # ####### #O####### ## OO  O #",
		"# #### #         #O#       #  ##### #",
		"# #### ## ######## ###### ##  ##### #",
		"#    O OOOOOOOO   * OOOOO OO  OOO   #",
		"#####################################"
	};

	int i, j;
	int win =      0;
	int lives =    3;
	int qtGhosts = Find_Ghost_Qtd(map);

	//variables of movement
	int mov, last_mov;
	int pacman_speed =      800;
	int ghosts_speed;
	int is_paused =        0;
	int is_powerful =       0;

	//variables that show data to the user
	int gameplay_time =     0;
	long player_score =     0;

	//variable of the drive timer
	int msec, timer_pacman, last_timer_pacman, can_move_pacman;
	int timer_ghost, last_timer_ghost, can_move_ghost;
	float last_game_timer, game_timer;

	int flag_start_power =0, max_time_power=0;

	srand(time(NULL));
	Pacman player;
	Ghost ghosts[MAX_GHOSTS];
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	clock_t start = clock(), time_dif;
	clock_t start_power = clock();

	//Screen size:
	SetConsoleSize(COLUMN*2+2,LINE+4);

	//Obtaining map data from file 
	//Get_Map(map);

	//Finding pac-man:
	Find_Pacman(map,&player.start_x, &player.start_y);
	player.x = player.start_x;
	player.y = player.start_y;

	// Looking for ghosts:
	qtGhosts = Find_Ghost_Qtd(map);
	for(i=0; i<qtGhosts; i++) {
		Find_Ghost(map,&ghosts[i].start_x,&ghosts[i].start_y,i);
		ghosts[i].x = ghosts[i].start_x;
		ghosts[i].y = ghosts[i].start_y;
	}

	// Showing the labyrinth:
	Print_Map(map);

	// Showing the score:
	Update_Score(&player_score, 0);

	// Showing the time:
	Update_Time(&gameplay_time);

	// Showing lives:
	Update_Lives(&lives);

	//playing the game:
	while(!win && lives > 0) {

		// define ghosts speed when the pacman is with power or not
		if(is_powerful)
			ghosts_speed=pacman_speed*0.7;
		else 
			ghosts_speed = pacman_speed;

		Update_Time(&gameplay_time);
		// Waiting for player input:
		if(kbhit()) {
			while(kbhit()) {
				last_mov = mov;
				mov = getch();
			}
			mov = tolower(mov);

			// "Validating" player entry to stop pacman
			// if the player selects the direction parallel to the movement
			// in which there is a wall
			switch(mov) {
				case 'd':
					if(map[player.y][player.x + 1] == WALL)
						mov = last_mov;
					break;
				case 'w':
					if(map[player.y - 1][player.x ] == WALL)
						mov = last_mov;
					break;
				case 'a':
					if(map[player.y][player.x - 1] == WALL)
						mov = last_mov;
					break;
				case 's':
					if(map[player.y + 1][player.x ] == WALL)
						mov = last_mov;
					break;
				case 'x':
					break;
				case 'p':
					Pause_Game(&is_paused);
					mov = last_mov;
					break;
				default:
					mov = last_mov;
					break;
			}
		}

		//Moving the pacman:
		if(can_move_pacman && !is_paused) {         //if the timer allows movement
			can_move_pacman = 0;                    //withdraws permission
			switch(mov) {
				case 'd':
					if(map[player.y][player.x + 1] != WALL)
						Move_Pacman(&player.x,&player.y,mov);
					break;
				case 'w':
					if(map[player.y - 1][player.x] != WALL)
						Move_Pacman(&player.x,&player.y,mov);
					break;
				case 'a':
					if(map[player.y][player.x - 1] != WALL)
						Move_Pacman(&player.x,&player.y,mov);
					break;
				case 's':
					if(map[player.y + 1][player.x] != WALL)
						Move_Pacman(&player.x,&player.y,mov);
					break;
				case 'x':
					mov = ' ';
					last_mov = ' ';
			}
		}


		for(i=0; i<qtGhosts; i++) {                 //Testing if Pacman touched some ghost
			if(player.x == ghosts[i].x && player.y == ghosts[i].y) {
				if(!is_powerful) {
					for(j=0; j<qtGhosts; j++) {     //Testing if Pacman touched some ghost
						gotoxy((ghosts[j].x+1),(ghosts[j].y+1));
						printf("%c", ' ');
						ghosts[j].x = ghosts[j].start_x;
						ghosts[j].y = ghosts[j].start_y;
					}
					lives--;
					Update_Lives(&lives);

					if(lives > 0)
						Restart_Game(&player.start_x,&player.start_y,&player.x,&player.y,&mov,&lives);
				} else {
					ghosts[i].x = ghosts[i].start_x;
					ghosts[i].y = ghosts[i].start_y;
					map[player.y][player.x] = 32;
					Update_Score(&player_score, 200);
				}
			}
		}

		// Moving ghosts:
		if(can_move_ghost && !is_paused) {          //if the timer allows movement
			can_move_ghost = 0;                     //withdraws permission
			for(i=0; i<qtGhosts; i++) {
				Move_Ghost(map,&ghosts[i].x,&ghosts[i].y, &player.x, &player.y, &is_powerful, &ghosts[i].last_move);
			}
		}


		for(i=0; i<qtGhosts; i++) {                 //Testing if Pacman touched some ghost
			if(player.x == ghosts[i].x && player.y == ghosts[i].y) {
				if(!is_powerful) {

					for(j=0; j<qtGhosts; j++) { //Testing if Pacman touched some ghost
						gotoxy((ghosts[j].x+1),(ghosts[j].y+1));
						printf("%c", ' ');
						ghosts[j].x = ghosts[j].start_x;
						ghosts[j].y = ghosts[j].start_y;
					}
					lives--;
					Update_Lives(&lives);
					if(lives > 0)
						Restart_Game(&player.start_x,&player.start_y,&player.x,&player.y,&mov,&lives);
				} else {
					ghosts[i].x = ghosts[i].start_x;
					ghosts[i].y = ghosts[i].start_y;
					map[player.y][player.x] = 32;
					Update_Score(&player_score, 200);
				}
			}
		}

		if(map[player.y][player.x] == PILL) {       //Testing if Pacman picked up some tablet
			map[player.y][player.x] = 32;
			Update_Score(&player_score, 10);
		}
		if(map[player.y][player.x] == SUPER_PILL) { //Testing if Pacman picked up some super-chip
			map[player.y][player.x] = 32;
			Update_Score(&player_score, 50);
			flag_start_power = 1;                   //if you got caught gaining power
		}

		win = Verify_Win(map);                      //Checking if there is still any insert on the map:


		time_dif = clock() - start;                 //time difference from the beginning of the program to the current state
		msec = time_dif * 1000 / CLOCKS_PER_SEC;    //count of thousands

		can_move_pacman = 0;                        //flag de permissao do pacman
		timer_pacman = msec%(1000-pacman_speed);    //pacman timer zeroing every x msec
		if(last_timer_pacman > timer_pacman)        //if the timer is zero
			can_move_pacman = 1;                    //of the movement permit
		last_timer_pacman = timer_pacman;           //save last timer

		can_move_ghost = 0;                         //ghost permission flag, 
		timer_ghost = msec%(1000-ghosts_speed);     //ghost time zeroing every x msec
		if(last_timer_ghost> timer_ghost)           //if the timer is zero
			can_move_ghost = 1;                     //of the movement permit
		last_timer_ghost = timer_ghost;             //save last timer

		game_timer = msec%1000;                     //seconds of game time
		if(last_game_timer > game_timer)            //is zeroed,
			gameplay_time++;                        //increases one
		last_game_timer = msec%1000;                //save last timer

		//defines the maximum time of power
		if (flag_start_power==1) {
			max_time_power = msec + 5000;           //current time plus 5 seconds
			is_powerful = 1;                        //of power
			flag_start_power = 0;                   //zero a flag
		}

		if (msec > max_time_power)	//if the game time passes the maximum power,loses power
			is_powerful = 0;     


	}

	//End of the game
	End_Game(&player_score, &lives);
}
