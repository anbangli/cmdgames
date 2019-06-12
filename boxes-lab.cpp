
/*********************************************************************
	程序名：推箱子游戏
	版权：
	作者：
	日期：2019-06-05 07:17
	说明：
https://cloud.tencent.com/developer/article/1193289

通过w,s,a,d实现上下左右的移动

声明画图函数
然后分别对移动位置进行情况判断：
1.如果人前面是空地，人走过去
2.如果人前面是目的地，人走过去
3.如果人前面是箱子。有以下两种情况：
  如果人前面是箱子，而箱子前面是空地。
  如果人的前面是箱子，而箱子前面是目的地。
4.如果人前面是已经进入某目的地的箱子。有以下两种情况：
  如果人前面是已经进入某目的地的箱子,而箱子前面是空地。
  如果人前面是已经进入某目的地的箱子，而箱子前面是另一目的地。
5.对箱子原先位置的判断。
6.对人原先位置进行判断。

原程序的冗余代码较多，进行了简单改写。--Anbangli, 2019-6-5
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <conio.h>

//地图关卡数据
//每一关的地图是一个二维数组，多个关卡的数据构成三维数组
const int CASMAX = 5;	//内置的关卡总数
const int ROW = 12;
const int COLUMN =12;
int  map[CASMAX][ROW][COLUMN] = {
	//0:空的 1:▆ :墙
	//3:☆   4:□  //目的地和箱子
	//5:人		  //人
	//7:★	//目的(3)和箱子(4)在一起
	//8:※	//人(5)和目的(3)在一起
	//（不用 2 和 6，以避免冲突）

//地图数据来源：https://blog.csdn.net/sexyback_/article/details/85633812
//修改数值以适合本程序。

	{	{ 0, 0, 1, 1, 1, 0, 0, 0 },  //地图一(8*8)
		{ 0, 0, 1, 3, 1, 0, 0, 0 },
		{ 0, 0, 1, 0, 1, 1, 1, 1 },
		{ 1, 1, 1, 4, 0, 4, 3, 1 },
		{ 1, 3, 0, 4, 5, 1, 1, 1 },
		{ 1, 1, 1, 1, 4, 1, 0, 0 },
		{ 0, 0, 0, 1, 3, 1, 0, 0 },
		{ 0, 0, 0, 1, 1, 1, 0, 0 }
	},
	{	{1,1,1,1,1,0,0,0,0},   //地图二(9*9)
		{1,5,0,0,1,0,0,0,0},
		{1,0,4,4,1,0,1,1,1},
		{1,0,4,0,1,0,1,3,1},
		{1,1,1,0,1,1,1,3,1},
		{0,1,1,0,0,0,0,3,1},
		{0,1,0,0,0,1,0,0,1},
		{0,1,0,0,0,1,1,1,1},
		{0,1,1,1,1,1,0,0,0}
	},
	{	{ 0, 1, 1, 1, 1, 1, 1, 1, 0, 0 },  //地图三(7*10)
		{ 0, 1, 0, 0, 0, 0, 0, 1, 1, 1 },
		{ 1, 1, 4, 1, 1, 1, 0, 0, 0, 1 },
		{ 1, 0, 5, 0, 4, 0, 0, 4, 0, 1 },
		{ 1, 0, 3, 3, 1, 0, 4, 0, 1, 1 },
		{ 1, 1, 3, 3, 1, 0, 0, 0, 1, 0 },
		{0,1,1,1,1,1,1,1,1,0}
	},
	{	{ 0, 1, 1, 1, 1, 0 },    //地图四(8*6)
		{ 1, 1, 0, 0, 1, 0 },
		{ 1, 5, 4, 0, 1, 0 },
		{ 1, 1, 4, 0, 1, 1 },
		{ 1, 1, 0, 4, 0, 1 },
		{ 1, 3, 4, 0, 0, 1 },
		{ 1, 3, 3, 5, 3, 1 },
		{ 1, 1, 1, 1, 1, 1 }
	},
	{	{ 0, 1, 1, 1, 1, 1, 0, 0 },  //地图五(8*8)
		{ 0, 1, 0, 5, 0, 1, 1, 1 },
		{ 1, 1, 0, 1, 4, 0, 0, 1 },
		{ 1, 0, 5, 3, 0, 3, 0, 1 },
		{ 1, 0, 0, 4, 4, 0, 1, 1 },
		{ 1, 1, 1, 0, 1, 3, 1, 0 },
		{ 0, 0, 1, 0, 0, 0, 1, 0 },
		{ 0, 0, 1, 1, 1, 1, 1, 0 }
	}
};

//	{
//		1, 1, 1, 1, 1, 1, 1, 1,
//		1, 0, 0, 0, 0, 0, 0, 1,
//		1, 3, 1, 0, 1, 1, 3, 1,
//		1, 4, 0, 0, 4, 0, 3, 1,
//		1, 0, 1, 0, 1, 1, 4, 1,
//		1, 0, 0, 5, 0, 0, 0, 1,
//		1, 1, 1, 1, 1, 1, 1, 1
//	},
//	{
//		1, 1, 1, 1, 1, 1, 1, 1,
//		1, 0, 0, 0, 0, 0, 0, 1,
//		1, 3, 1, 0, 1, 1, 3, 1,
//		1, 3, 4, 5, 4, 0, 3, 1,
//		1, 4, 1, 0, 1, 1, 4, 1,
//		1, 0, 0, 0, 0, 0, 0, 1,
//		1, 1, 1, 1, 1, 1, 1, 1
//	}

int mapbak[ROW][COLUMN];	//用于对地图进行一次备份以供撤消操作 

//在制作终端窗口的游戏时，需要在屏幕上精确定位并输出数据，
//所以需要自定义相应的函数 gotoxy、HideCursor和 ShowCursor
//这三个函数要用到结构体和Windows编程中的知识，初学者不必弄懂 
//屏幕左上角为(0,0)，横向向右为 X 轴，竖向向下为 Y 轴 
int gotoxy(int x, int y) { //定位光标位置到(x,y) 
	COORD coord;	//定义结构体coord (坐标系coord)
	coord.X = x;
	coord.Y = y;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); 
	//定义句柄变量handle并获取控制台输出句柄(值为-11)
	SetConsoleCursorPosition(handle, coord); //移动光标到(x,y) 
}

//地图绘图
void DrawMap(int cas) {	//cas 为当前关卡编号
	//system("cls");	//使用系统内部命令 cls 清除屏幕
	gotoxy(0,0);
	printf("\n\t\t推箱子   第【%d】关\n\n",cas+1);

	for (int i = 0; i < ROW; i++) {		//行
		printf("\t\t");
		for (int j = 0; j < COLUMN; j++) {	//列

			switch (map[cas][i][j]) {
				case 0:	//0:空的
					printf("  ");
					break;
				case 1:	//1:▆ :墙
					printf("▆");
					break;
					//3：☆ 4：□  //目的地和箱子
				case 3:
					printf("☆");
					break;
				case 4:
					printf("□");
					break;
				case 5:	//5：人		  //人
					printf("人");
					break;
				case 8:	  //人(5)和目的(3)在一起
					printf("个");
					break;
				case 7:		//目的(3)和箱子(4)在一起
					printf("★");
					break;


			}
		}
		printf("\n");
	}
}


int isWin(int cas) {	//判断是否胜利
	int count = 0;
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < 8; x++) {
			if (map[cas][y][x] == 3 || map[cas][y][x] == 8)
				//箱子目的地未放箱子，或人站在目的地
				count++;
		}
	}
	return (count==0? 1:0);	//箱子目的地的空闲数目为0，胜利
}

void backup(int cas) {	//在发生移动之前作一次备份，以供一次撤消
	int x, y;
	for (y = 0; y < ROW; y++) 	//保存地图（以供一次撤消）
		for (x = 0; x < COLUMN; x++)
			mapbak[y][x] = map[cas][y][x];
}

int main() {
	
	int y, x;	//人的位置
	int mx=0, my=0; //move X, move Y
	int moved = 0;	//在一次循环中是否发生移动 
	char ch;

	int cas = 0;	//当前关卡编号
	DrawMap(cas);	//绘出地图
	backup(cas);

	while (1) {
		moved = 0;
		//定位人的位置：人在空地上，或人站在目的地上
		for (y = 0; y < ROW; y++) {
			for (x = 0; x < COLUMN; x++)
				if (map[cas][y][x] == 5 || map[cas][y][x] == 8)
					break;	//跳出内层循环
			if (map[cas][y][x] == 5 || map[cas][y][x] == 8) {
				break;	//跳出外层循环
			}
		}

		ch = _getch();	//获得字符输入（不需按回车）
		mx =0;
		my =0;
		switch (ch) {	//根据按键来判断试图移动的方向（保存在 mx 和 my 中）
			case 72:
			case 'w':
			case 'W':
				mx = 0;
				my = -1;
				break;
			case 80:	//down
			case 's':
			case 'S':
				mx = 0;
				my = 1;
				break;
			case 75:	//left
			case 'a':
			case 'A':
				mx = -1;
				my = 0;
				break;
			case 77:	//right
			case 'd':
			case 'D':
				mx = 1;
				my = 0;
				break;
			case 'z':	//撤消(留待后面处理) 
			case 'Z':
				break;
			case 27:	//ESC
				//gotoxy(0, HGT+1);
				//ShowCursor();//显示光标
				printf("\n\t\t结束游戏(Y/N)?");
				if ((ch = getch()) == 'Y' || ch=='y') {
					//	HideCursor();
					exit(0);
				} else
					printf("\r                         ");
				break;
			default:
				break;
		}
		//判断是否可以行走
		//情况1：下一个地方等于空地或者是目的 能走
		if (map[cas][y+my][x + mx] == 0 || map[cas][y+my][x + mx] == 3) {
			//0+5: 人走到空白地；3+5=8：人走到目的地
			map[cas][y+my][x + mx] += 5;	//新地方(map[y+my][x+mx])人（5）来了
			map[cas][y][x] -= 5;	//老地方(map[y][x])人（5）走了
			moved = 1;	//发生了移动
		}

		//情况2：下一个地方是箱子，需要判断箱子的下一个地方是不是目的和空地
		else if (map[cas][y+my][x + mx] == 4 || map[cas][y+my][x + mx] == 7) {
			//做箱子的下一个地方判断能不能走
			if (map[cas][y+my*2][x + mx*2] == 0 || map[cas][y+my*2][x + mx*2] == 3) {
				backup(cas);	//移动之前作一次备份
				//新的地方箱子来了
				map[cas][y+my*2][x + mx*2] += 4;  //箱子的位置：箱子（-4）走了 人来（+5）
				map[cas][y+my][x + mx] += 1;
				map[cas][y][x] -= 5;//原来的地方人走了
			}
			moved = 1;	//发生了移动
		}

		if (ch == 'z' || ch =='Z') {	//撤消
			for (y = 0; y < ROW; y++) 	//恢复上一次移动前的地图
				for (x = 0; x < COLUMN; x++)
					map[cas][y][x] = mapbak[y][x];
			moved = 1;	//也视为发生了移动
		}

		if (moved)	{	//发生了移动
			DrawMap(cas);	//绘出地图
		}

		if (isWin(cas)) {
			printf("\n\t\tYou win !");
			getchar();
			cas++;
			if (cas == CASMAX)	//到达最大关数
				break;
			DrawMap(cas);	//绘出新关卡的地图
			backup(cas);
		}

	}
	printf("\n\t\tGame Over");
	getchar();
	return 0;
}
