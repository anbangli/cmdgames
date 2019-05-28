/***********************************************************************
《俄罗斯方块》（Tetris， 俄文：Тетрис）是一款由俄罗斯人阿列克谢·
帕基特诺夫于1984年6月发明的休闲游戏。基本规则是移动、旋转和摆放游戏自动
输出的各种方块，使之排列成完整的一行或多行并且消除得分。

本程序特色：（1）使用C语言编写，程序中使用了数组，但未使用指针和结构体；
（2）终端窗口中打印汉字特殊字符，简洁美观。
原作者：LiuTianyong   https://github.com/LiuTianyong/didactic-octo-waffle

原程序是把俄罗斯方块与猜数游戏合并在一起。现删减为俄罗斯方块。
把程序中各种小函数合并到主函数中，一些全局变量改为局部变量，简化整个程序。
修改者：李安邦 （anbangli@qq.com）  2019年5月 
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <conio.h>

//定义全局变量
const int HGT = 19;	//高度行数
const int WID = 20;	//宽度列数
int Map[HGT][WID];	//地图的大小

const int VARIANTS = 28;	//变体种数：7种方块，每种有4种变形，共28种
//每种方块有 4 个小方块，隐含以其中某个小方块为基点，坐标为(0, 0)， 然后
//只需依次列出其它3个小方块相对于该块的 x 和 y 值（共 6 个值）即可。
int tetris[VARIANTS][6] = {	//所有种类方块的信息。第一维度表示种类，第二维度表示坐标
	{ 0,-1,0,-2,1,0 },{ 0,1,1,0,2,0 },{ -1,0,0,1,0,2 },{ 0,-1,-1,0,-2,0 },
//  ■                ■□
//  ■      □■■      ■        ■
//  □■    ■          ■    ■■□
	{ 0,-1,0,1,-1,0 },{ 0,1,1,0,-1,0 },{ 1,0,0,-1,0,1 },{ 1,0,-1,0,0,-1 },
//    ■              ■        ■
//  ■□    ■□■ 	  □■    ■□■
//    ■      ■      ■
	{ -1,1,0,1,1,0 },{ 0,-1,1,0,1,1 },{ -1,0,0,-1,1,-1 },{ -1,-1,-1,0,0,1 },
//	          ■        ■■    ■
//	  □■    □■ 	  ■□      ■□
//	■■        ■                ■
	{ -1,0,0,1,1,1 },{ 0,1,1,-1,1,0 },{ -1,0,0,1,1,1 },{ 0,1,1,-1,1,0 },
//	            ■                ■
//	■□      □■ 	  ■□      □■
//	  ■■    ■        ■■    ■
	{ -1,0,0,-1,0,-2 },{ -1,0,-2,0,0,1 },{ 0,1,0,2,1,0 },{ 0,-1,1,0,2,0 },
//	  ■                □■    ■
//	  ■    ■■□ 	    ■      □■■
//	■□        ■      ■
	{ 0,1,1,0,1,1 },{ 0,-1,1,0,1,-1 },{ -1,0,0,-1,-1,-1 },{ -1,0,-1,1,0,1 },
//	□■      ■■      ■■    ■□
//	■■      □■ 	    ■□    ■■
	{ 0,-1,0,1,0,2 },{ -1,0,1,0,2,0 },{ 0,1,0,-1,0,-2 },{ 1,0,-1,0,-2,0 }
//	  ■                ■
//	  □    ■□■■ 	■    ■■□■
//	  ■                □
//    ■                ■
};

//函数原型声明
void gotoxy(int x, int y);	//定位光标
void HideCursor();	// 隐藏光标
void ShowCursor();	//显示光标

void DrawMap(int next, int score);	//绘制地图
void DrawTetris(int cur, int tx, int ty, int show);	//绘制方块(show为0时擦除）

int Moveable(int x, int y, int cur);		//判断方块是否可以移动
void CheckScore(int next, int &score, int &Gamespeed);	//检查得分

//在制作终端窗口的游戏时，需要在屏幕上精确定位并输出数据，
//所以需要自定义相应的函数 gotoxy、HideCursor和 ShowCursor
//这三个函数要用到结构体和Windows编程中的知识，初学者不必弄懂
void gotoxy(int x, int y) { //定位光标位置
	COORD coord;	//定义结构体coord (坐标系coord)
	coord.X = x;
	coord.Y = y;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	//定义句柄变量handle并获取控制台输出句柄(值为-11)
	SetConsoleCursorPosition(handle, coord); //移动光标
}

void HideCursor() { //隐藏光标
	CONSOLE_CURSOR_INFO cursor_info = {1, 0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}

void ShowCursor() { //显示光标
	CONSOLE_CURSOR_INFO cursor_info = {20, 1};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}


void DrawMap(int next, int score) {	//绘制地图及附属参数
	int x, y;
	//静态绘制地图，每个元素占2列
	gotoxy(WID-5, 0);	 //首行(第0行)中央
	printf("俄罗斯方块");//显示游戏名称
	gotoxy(0, 1);
	for (y = 0; y<HGT; y++) {	//行
		for (x = 0; x<WID; x++) {	//列
			if (y == 0 || y == HGT - 1 	//顶行或底行
			        || x == 0 || x == WID - 1) //左墙或右墙
				printf("##");
			else if (Map[y][x] == 0) 	//空白
				printf("  ");
			else if (Map[y][x] == 2) 	//小方块占据
				printf("■");
			else if (Map[y][x] == 3)	//本行已填满
				printf("☆");

		}
		puts("");
	}

	//绘制右边区域的信息：下一方块，得分，级别
	gotoxy(WID*2+2, 2);
	printf("方块:");
	//清除下一方块
	for (y = 2; y <= 10; y++) {
		for (x = WID*2; x <= WID*2+10; x++) {
			gotoxy(x + 1, y + 1);
			printf("  ");
		}
	}
	//重绘下一方块
	gotoxy(WID*2+5, 6);
	printf("■");
	for (y = 0; y<6; y = y + 2) {
		gotoxy(WID*2+5 + 2 * tetris[next][y], 6 + tetris[next][y+1]);
		printf("■");
	}
	gotoxy(WID*2+4, 12);
	printf("得分 :   %d", score);
	gotoxy(WID*2+4, 14);
	printf("级别 :   %d", score / 100 + 1);
}

void DrawTetris(int cur, int tx, int ty, int show) {//绘制方块 (show 为 0 时为擦除）
	int x = tx * 2;	//每个方块横向占2格，所以x=tx*2
	int y = ty + 1;	//根据地图绘制函数，屏幕首行用于书写游戏标题，所以要 +1
	if (y >= 2) {
		gotoxy(x, y);
		printf(show ==0? "  " : "■");	//"□");
	}
	for (int i = 0; i < 6; i = i + 2) {
		x = (tx + tetris[cur][i])*2;
		y = ty +1 + tetris[cur][i+1];
		if ( y >= 2 ) {
			gotoxy(x, y);
			printf(show == 0? "  " : "■");
		}
	}
}

int Moveable(int y, int x, int cur) {	//判断方块是否可以移动
	if (Map[y][x] != 0)return 0;
	int x2, y2;
	for (int i = 0; i<6; i = i + 2) {
		x2 = x + tetris[cur][i];
		y2 = y + tetris[cur][i+1];
		//如果 x 超出地图横向坐标，或 y 超出纵向下方坐标，则不可移动
		if ( x2 < 0 || x2 > WID-2 || y2 >= HGT-1)
			return 0;
		//如果(x2,y2)坐标在地图坐标范围内，且地图上已有小块，则不可移动
		if ( x2 >= 0 && x2 < WID-2 && y2 >=0 && y2 < HGT-1 && Map[y2][x2] != 0)
			return 0;
	}
	return 1;
}


int main() {	//俄罗斯方块
	srand(time(0));
	int cur = rand() % VARIANTS;	//当前方块种类
	int next = rand() % VARIANTS;	//下一个方块种类
	int tmp;
	//tx，ty: 移动中的方块(tetris)的x，y坐标
	int tx = WID/2;		//方块从顶部中央开始落下
	int ty = 0;

	int key;	//用户输入的键值记录
	int score = 0;	//游戏得分
	int Gamespeed = 200;	//游戏速度（接受用户输入的间隔，单位为ms）

	//初始化地图
	int y, x;
	for (y = 0; y < HGT; y++) {
		for (x = 0; x<WID; x++)
			if (y == HGT - 1 || x == 0 || x == WID - 1)
				Map[y][x] = -1;	//左墙、右墙和底部，三面为墙
		//（顶部并不设为墙，以便程序中处理方块落下）
			else
				Map[y][x] = 0;	//中间为空
	}
	HideCursor();	//隐藏光标
	DrawMap(next, score);	//绘出地图和右边参数

	while(1) {
		DrawTetris(cur, tx, ty, 1);	//显示方块(第二参数为1）
		if (kbhit()) {	//如果用户按了键，则获取键并作判断
			key = getch();	//获取键值方块移动
			while (kbhit()) 	//用户连续按键时，读取所有字符
				key = getch();
			DrawTetris(cur, tx, ty, 0);	//擦除方块(第二参数为0）
			if (key == 'H') {	//up, 变形
				tmp = cur;
				cur++;
				if (cur % 4 == 0 && cur != 0)
					cur = cur - 4;
				if (Moveable(ty, tx, cur) != 1)
					cur = tmp;
			} else if (key == 'P' && Moveable(ty + 1, tx, cur) == 1) //down
				ty++;
			else if (key == 'K' && Moveable(ty, tx - 1, cur) == 1)	//left
				tx--;
			else if (key == 'M' && Moveable(ty, tx + 1, cur) == 1)	//right
				tx++;
			else if (key == 27) {	//ESC
				gotoxy(0, HGT+1);
				ShowCursor();//显示光标
				printf("游戏暂停，是否继续(Y/N)?");
				if ((key = toupper(getch())) == 'Y') {
					HideCursor();
					printf("\r                         ");
				} else
					return 0;
			}
			DrawTetris(cur, tx, ty, 1);	//绘制方块（第二参数为1）
		} else {	//无按键
			if (Moveable(ty+1, tx, cur) == 1) {	//方块位置合法，可以下落
				DrawTetris(cur, tx, ty, 0);	//擦除方块
				ty++;	//下落
				DrawTetris(cur, tx, ty, 1);	//绘制方块
			} else { //方块碰撞，方块所在位置都填实（地图位置值取为2）
				Map[ty][tx] = 2;
				for (int i = 0; i < 6; i = i + 2) {
					Map[ty + tetris[cur][i+1]][tx + tetris[cur][i]] = 2;
				}
				CheckScore(next, score, Gamespeed);	//检查如果行满则消隐方块

				if (ty <= 2) {	//方块停止在地图首行，则判断为游戏结束
					gotoxy(0, HGT+1);
					printf("Game Over！");
					return 0;
				} else {	//否则还要继续落下新的方块
					cur = next;
					next = abs(rand() % VARIANTS);
					tx = WID/2;
					ty = 0;
				}
				DrawMap(next, score);	//绘制地图（此处刷新下一个方块）
			}
		}
		Sleep(Gamespeed);
	}
	ShowCursor();
	return 0;
}


/*************************************************
Function: CheckScore()
Description: 检查是否有某行被小方块填满并消隐该行
*************************************************/
void CheckScore(int next, int &score, int &Gamespeed) {
	int y=HGT-1, x, full;
	while(y>=1) {	//行
		full = 1;	//检查本行是否已填满
		for (x = 1; x< WID - 1; x++) {//列
			if (Map[y][x] != 2) { 	//有空块
				full = 0;
				break;
			}
		}
		if (full == 0) {	//本行未填满
			y--;
		} else {	//本行已填满
			for (x = 1; x < WID -1; x++)
				Map[y][x]=3;	//标志本行已填满
			DrawMap(next, score); //重绘地图（填满的位置额外显示）
			Sleep(200);
			//逐行下落
			for (int y2 = y; y2>1; y2--) 
				for (x = 1; x < WID - 1; x++)
					Map[y2][x] = Map[y2 - 1][x];
			DrawMap(next, score);//重绘地图
			score += 10;
			if (score % 100 == 0 && score != 0)  
				Gamespeed -= 50;	//游戏速度加快 
		}
	}
}

