/*********************************************************************
程序功能：打砖块
作者：龙叔
链接：https://zhuanlan.zhihu.com/p/55963487
来源：知乎
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。

*********************************************************************/

#include <stdio.h>
#include <Windows.h>
#include <conio.h>

#define WIDTH 20//游戏地图宽度
#define HEIGHT 20//游戏地图高度

int map[HEIGHT][WIDTH];//地图二维数组
//0-空白，1-围墙，2-砖块，3-反弹板，4-小球

int y=HEIGHT-3, x=WIDTH/2;//小球的初始位置
int vy=-1, vx=1;	//小球的初始方向
int ban=WIDTH/2-3;	//反弹板的初始位置
DWORD old_time,new_time;//获取系统运行时间

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

void HideCursor() { //隐藏光标
	CONSOLE_CURSOR_INFO cursor_info = {1, 0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}

void ShowCursor() { //显示光标
	CONSOLE_CURSOR_INFO cursor_info = {20, 1};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info);
}

void init() { //初始化
	for(int i=0; i<HEIGHT; i++) {	//地图
		for(int j=0; j<WIDTH; j++) {
			if(j==0 || j==WIDTH-1 || i==HEIGHT-1 || i==0)
				map[i][j]=1;//1围墙
			else
				map[i][j]=0;//0空格
		}
	}

	for(int j=7; j<14; j++)
		map[HEIGHT-2][j]=3;//3反弹板

	for(int i=2; i<7; i++)
		for(int j=1; j<WIDTH-1; j++)
			if(j%2 == 0)
				map[i][j]=2;//2砖块

	map[y][x]=4;//4小球
}

void showall() { //显示全部
	for(int i=0; i<HEIGHT; i++) {	//行
		for(int j=0; j<WIDTH; j++) {	//列
			switch(map[i][j]) {
				case 0:	//0-空白
					printf("  ");
					break;
				case 1: {	//1-围墙
					if(j==0)
						printf("||");
					else if(j==WIDTH-1)
						printf("||\n");
					else if(i==0||i==HEIGHT-1)
						printf("－");
					break;
				}
				break;
				case 2:	//2-砖块
				case 3:	//3-反弹板
					printf("■");
					break;
				case 4:	//4-小球
					printf("●");
					break;
			}
		}
	}
}

void showxy(int x, int y, int v) { 	//在(x,y) 处按 v 值显示
	gotoxy(x*2, y);
	if (v == 0)	printf("  ");	//0-空白
	if (v == 2 || v == 3)	printf("■");	//2-砖块, 3-反弹板
	if (v == 4) printf("●");	//4-小球
}

void game() { //游戏主要逻辑部分
	int flag=0;//判断标志位
	int i, j;
	gotoxy(0,0);//将坐标移动到0，0点

	for(j=0; j<7; j++) {
		map[HEIGHT-2][j+ban]=0;//3横板
		//	showxy(j+ban, HEIGHT-2, 0);	//横板原位置清空
	}

	char ch;
	if(kbhit()) { //判断是否有键盘输入，有则用ch 接收
		ch = getch();
		while (kbhit()) 	//用户连续按键时，读取所有字符
			ch = getch();
		switch(ch) {
			case 75:	//left
			case 'a':
			case 'A':
				if(ban>1)
					ban--;
				break;
			case 77:	//right
			case 'd':
			case 'D':
				if(ban+7 < WIDTH-1)
					ban++;
				break;
		}
	}

	//Sleep(100);
	new_time=GetTickCount();
	if(new_time - old_time >= 150) {
		old_time = new_time;
		new_time = GetTickCount();

		for(j=1; j<ban; j++) 	//横板左边为空白
			showxy(j, HEIGHT-2, 0);
		for(j=ban; j<ban+7; j++) {	//横板
			map[HEIGHT-2][j] = 3;//3-横板
			showxy(j, HEIGHT-2, 3);
		}
		for(j=ban+7; j<WIDTH-1; j++) 	//横板右边为空白
			showxy(j, HEIGHT-2, 0);

//		for(j=0; j<7; j++){	//更新横板
//			map[HEIGHT-2][j+ban]=3;//3-横板
//			showxy(j+ban, HEIGHT-2, 3);	//重绘横板
//		}

		//flag=0, 先判断向上或左右碰撞，如果发生判断则不再判断斜碰
		if(vy==-1) { //小球向上方运动
			if(map[y-1][x]==2) { //如果上方是砖块
				map[y-1][x]=0;//则将砖块位置置为0
				showxy(x, y-1, 0);	//清除原有砖块
				vy=-vy;//改变运动方向
				flag=1;//将标志位置为1
			}
			if(map[y-1][x]==1) { //如果上方是围墙
				vy=-vy;//改变运动方向
			}
		}
		if(vx==-1) { //小球向左侧运动
			if(map[y][x-1]==2) {
				map[y][x-1]=0;
				showxy(x-1, y, 0);	//清除原有砖块
				vx=-vx;
				flag=1;
			}
			if(map[y][x-1]==1) {
				vx=-vx;
			}

		}
		if(vx==1) { //小球向右侧运动
			if(map[y][x+1]==2) {
				map[y][x+1]=0;
				showxy(x+1, y, 0);	//清除原有砖块
				vx=-vx;
				flag=1;
			}
			if(map[y][x+1]==1) {
				vx=-vx;
			}

		}
		if(vy==1) { //小球向下方运动
			if(map[y+1][x]==2) {
				map[y+1][x]=0;
				showxy(x, y+1, 0);	//清除原有砖块
				vx=-vx;
				flag=1;
			}
			if(map[y+1][x]==3) { //如果下方是反弹板
				vy=-vy;
			}
			if(y+1==HEIGHT-1) { //如果下方是底下边界，则游戏结束，退出
				gotoxy (WIDTH/2, HEIGHT+1);
				printf("Game Over!");
				getchar();
				exit(0);
			}
		}

		//未发生向上或左右碰撞，则判断是否发生斜碰
		if(flag==0 && map[y+vy][x+vx]==2) {
			map[y+vy][x+vx]=0;
			showxy(x+vx, y+vy, 0);	//清除原有砖块
			vy=-vy;
			vx=-vx;
		}

		//ball moves
		map[y][x]=0;//将小球原位置置为0
		showxy(x, y, 0);	//清除原位置的小球
		y = y + vy;
		x = x + vx;
		map[y][x]=4;
		showxy(x, y, 4);	//在新位置绘制小球

		//showall();
	}
}

int main() {
	HideCursor();
	init();
	showall();
	old_time=GetTickCount();
	while(1) {
		game();
	}
	ShowCursor();
	return 0;
}
