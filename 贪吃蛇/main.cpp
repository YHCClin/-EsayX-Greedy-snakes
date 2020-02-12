/*****************************************************
	@Copyright: Linyuhang
	FileName：main.cpp
	ProjectName: 贪吃蛇
	compilerEnv: VS2017 + EasyX Lib
	Author: Linyuhang
	FinalEdit: 2020.2.11
******************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <Windows.h>
#include <time.h>
#include <conio.h> // 键盘
#include <stdlib.h>
#include <cmath>
#include <graphics.h>

// 引入多媒体设备接口
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

constexpr auto SNAKE_MAX_LEN = 100;
constexpr auto SNAKE_SIZE = 10;
constexpr auto FOOD_SIZE = 10;

constexpr auto SLOW = 150;
constexpr auto NORMAL = 100;
constexpr auto FAST = 50;
int SPEED = NORMAL;

int CUR_MAX_SCORE = 0;
int SCORE = 0;
char score[10] = "";

// 当前关卡
int LEVEL = 1;

char *itoChar(int sc)
{
	sprintf(score, "%d", sc);
	return score;
}

// 方向
typedef enum ch {
	up = 72,
	down = 80,
	left = 75,
	right = 77,
	pause = 112,
	speed = 115,
	choose = 99,
}CH;// 绑定键盘映射

// 坐标结构体
class Coor {
	friend class Food;
	friend class Snake;
public:
	int x, y;
	Coor(int x_, int y_) { x = x_; y = y_; }
};

// 蛇的结构体
class Snake {
public:
	// 蛇身数组
	std::vector<Coor> sbody = { Coor(0,0), Coor(10,0), Coor(20,0),Coor(30,0),Coor(40,0),Coor(50,0),Coor(60,0),Coor(70,0) };
	CH dir = down;
	int length;
	Snake() { length = sbody.size(); }
};
Snake SNAKE;

class Food {
public:
	Coor coor = Coor(0,0);
	Food() 
	{
		srand((unsigned int)time(NULL));
		coor.x = (rand() % (600 / SNAKE_SIZE)) * 10;
		coor.y = (rand() % (600 / SNAKE_SIZE)) * 10;
	}
};
Food food;

// 障碍物类
class Obstacal {
public:
	// 用矩形来表示障碍物{x1,y1,x2,y2}
	std::vector<std::vector<int>> Obs = { {100, 150, 500, 170},{290, 170, 310, 430},{100, 430, 500, 450} };
	Obstacal(std::vector<std::vector<int>>& obs){
		Obs = obs;
	}
};
// 关卡
std::vector<std::vector<int>> l1 = { {100, 150, 500, 170},{290, 170, 310, 430},{100, 430, 500, 450} };
Obstacal lev1 = Obstacal(l1);
std::vector<std::vector<int>> l2 = { {100, 150, 500, 170},{100,150,120,450}};
Obstacal lev2 = Obstacal(l2);
std::vector<Obstacal> Level = { lev1,lev2 };




// 初始化
void InitSnake();

// 绘图函数
void DrawSnake();
void DrawFood(Food& food);
void DrawScore();
void DrawGameStatus(char *status);
void DrawGameData();
void DrawObs();
void Draw(); // 集成显示函数

// 用于数据保存
void SavePlayerData();

// 用于过程处理
void MoveSnake(Food& food);
void ChangeDir();
void IsCollision();
void UpdateFood(Food &food);

// 背景图片
IMAGE bk_img;

int main()
{
	InitSnake();
	while (1)
	{
		while (!_kbhit())
		{
			BeginBatchDraw();
			//setbkcolor(WHITE);
			Draw();
			FlushBatchDraw();
			//saveimage("F:\Python\bk_img.png");
			cleardevice();
			Sleep(SPEED);
		}
		ChangeDir();
	}

	mciSendString("close all", 0, 0, 0); // 关闭所有多媒体文件

	getchar();
	EndBatchDraw();
	closegraph();
}

void InitSnake()
{
	loadimage(&bk_img,"bk_img.png");
	int width = bk_img.getwidth();
	int height = bk_img.getheight();
	initgraph(width, height);

	// 载入背景音乐
	mciSendString("open bkmusic.mp3 alias bk",0,0,0);
	mciSendString("play bk repeat",0,0,0);
	mciSendString("open collision.mp3 alias dead", 0, 0, 0);
	mciSendString("open eat.mp3 alias eat", 0, 0, 0);
	SNAKE.sbody[0].x = 0;
	SNAKE.sbody[0].y = 0;
	SNAKE.dir = down;
	setbkcolor(WHITE);
}

void MoveSnake(Food &food)
{
	if (SNAKE.length > 1)
	{
		for (int i = SNAKE.length-1; i > 0; i--)
		{
			SNAKE.sbody[i].x = SNAKE.sbody[i - 1].x;
			SNAKE.sbody[i].y = SNAKE.sbody[i - 1].y;
		}
	}
	switch (SNAKE.dir)
	{
	case up:
		SNAKE.sbody[0].y -= SNAKE_SIZE;
		break;
	case down:
		SNAKE.sbody[0].y += SNAKE_SIZE;
		break;
	case left:
		SNAKE.sbody[0].x -= SNAKE_SIZE;
		break;
	case right:
		SNAKE.sbody[0].x += SNAKE_SIZE;
		break;
	default:
		break;
	}
	IsCollision();
	// 吃食物
	if (SNAKE.sbody[0].x == food.coor.x && SNAKE.sbody[0].y == food.coor.y)
	{
		mciSendString("play eat from 0", 0, 0, 0);
		UpdateFood(food);
		int X = SNAKE.sbody[SNAKE.length - 1].x;
		int Y = SNAKE.sbody[SNAKE.length - 1].y;
		SNAKE.sbody.push_back(Coor(X, Y));
		SNAKE.length++;
		SCORE += SNAKE.length * 2;
		if (SCORE > CUR_MAX_SCORE) CUR_MAX_SCORE = SCORE;
		if (SCORE > 500 && SCORE < 2000) SPEED = 90;
		else if (SCORE >= 2000 && SCORE < 3000) SPEED = 70;
		else if (SCORE >= 3000) SPEED = FAST;
	}
	DrawSnake();
}

void DrawSnake()
{
	setlinecolor(BLACK);
	setfillcolor(YELLOW);
	setlinestyle(PS_SOLID, 1);
	srand((unsigned int)time(NULL));
	for (int i = 0; i < SNAKE.length; i++)
	{
		if (i == 0) 
		{
			fillrectangle(SNAKE.sbody[i].x, SNAKE.sbody[i].y, SNAKE.sbody[i].x + SNAKE_SIZE, SNAKE.sbody[i].y + SNAKE_SIZE);
			setfillcolor(BLACK);
			fillcircle(SNAKE.sbody[i].x + (SNAKE_SIZE / 2), SNAKE.sbody[i].y + (SNAKE_SIZE / 2), SNAKE_SIZE / 3);
		}
		else
		{
			setfillcolor(RGB(rand() % 256, rand() % 256, rand() % 256));
			fillcircle(SNAKE.sbody[i].x + (SNAKE_SIZE / 2), SNAKE.sbody[i].y + (SNAKE_SIZE / 2), SNAKE_SIZE / 2);// 蛇身为圆形
		}
	}
}

void ChangeDir()
{
	int move;
	move = _getch(); //  接收键盘响应
	switch (move)
	{
	case up:
		if (SNAKE.dir != down)
		{
			SNAKE.dir = up;
		}
		break;
	case down:
		if (SNAKE.dir != up)
		{
			SNAKE.dir = down;
		}
		break;
	case left:
		if (SNAKE.dir != right)
		{
			SNAKE.dir = left;
		}
		break;
	case right:
		if (SNAKE.dir != left)
		{
			SNAKE.dir = right;
		}
		break;
	case pause:
	{
		Draw();
		settextcolor(BLUE);
		char status[10] = "PAUSE";
		DrawGameStatus(status);
		FlushBatchDraw();
		while (!_kbhit());
		break;
	}
	case speed:
		switch (_getch())
		{
		case 49:
			SPEED = SLOW;
			break;
		case 50:
			SPEED = NORMAL;
			break;
		case 51:
			SPEED = FAST;
			break;
		default:
			break;
		}
		break;
	case choose:
		switch (_getch())
		{
		case 49:
			LEVEL = 0;
			SNAKE = Snake();
			SNAKE.dir = down;
			break;
		case 50:
			LEVEL = 1;
			SNAKE = Snake();
			SNAKE.dir = down;
			break;
		default:
			break;
		}
	default:
		break;
	}
}


void DrawFood(Food &food)
{
	setfillcolor(RGB(rand() % 256, rand() % 256, rand() % 256));
	fillrectangle(food.coor.x, food.coor.y, food.coor.x + FOOD_SIZE, food.coor.y + FOOD_SIZE);
}


void DrawScore()
{
	settextcolor(RGB(rand() % 256, rand() % 256, rand() % 256));
	settextstyle(60, 20, "黑体");
	outtextxy(620, 35, itoChar(SCORE));
}


void DrawGameStatus(char *status)
{
	settextcolor(RED);
	settextstyle(60, 20, "黑体");
	outtextxy(210, 270, status);
	settextstyle(30, 15, "黑体");
	outtextxy(135, 335, "PRESS ANYKEY TO RESUME");
	DrawScore();
	DrawGameData();
}

void Draw()
{
	putimage(0, 0, &bk_img);
	MoveSnake(food);
	DrawFood(food);
	//DrawBoundary();
	DrawScore();
	DrawGameData();
	DrawObs();
}

void DrawGameData()
{
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	settextstyle(10, 10, "黑体");
	outtextxy(690, 140, itoChar(SNAKE.length));
	outtextxy(720, 170, itoChar(SPEED));
}

void DrawHelp()
{

}

// 保存本轮游戏最终数据
void SavePlayerData()
{
	//std::ofstream GameData;
	//GameData.open("GameData.dat", std::ios::app);
}

// 碰撞检测函数
void IsCollision()
{
	bool isCollision = false;
	int x = SNAKE.sbody[0].x, y = SNAKE.sbody[0].y;
	if (x == 600 || y == 600 || x < 0 || y < 0) // 边界碰撞检测
	{
		isCollision = true;
	} 
	else if (SNAKE.length > 2) // 自身碰撞检测
	{
		for (int i = 3; i < SNAKE.length; i++)
		{
			if (x == SNAKE.sbody[i].x && y == SNAKE.sbody[i].y)
			{
				isCollision = true;
				break;
			}
		}
	}
	 // 障碍物碰撞检测
	{	
		int x_ = x + (SNAKE_SIZE / 2), y_ = y + (SNAKE_SIZE / 2);
		for (size_t i = 0; i < Level[LEVEL].Obs.size(); i++)
		{
			if (x_ > Level[LEVEL].Obs[i][0] && y_ > Level[LEVEL].Obs[i][1] && x_ < Level[LEVEL].Obs[i][2] && y_ < Level[LEVEL].Obs[i][3])
			{
				isCollision = true;
				break;
			}
		}
	}
	// 碰撞后处理
	if (isCollision)
	{
		mciSendString("play dead from 0", 0, 0, 0);
		SNAKE = Snake();
		SNAKE.dir = down;
		char status[10] = "GAME OVER";
		cleardevice();
		putimage(0, 0, &bk_img);
		DrawGameStatus(status);
		FlushBatchDraw();
		while (!_kbhit()); // 按任意键继续
		SCORE = 0;
	}
}

// 更新食物位置
void UpdateFood(Food &food)
{
	while (true)
	{
		srand((unsigned int)time(NULL));
		food.coor.x = (rand() % (600 / SNAKE_SIZE)) * 10;
		food.coor.y = (rand() % (600 / SNAKE_SIZE)) * 10;
		// 食物不能在障碍物上
		bool isOk = true;
		for (size_t i = 0; i < Level[LEVEL].Obs.size(); i++)
		{
			if (food.coor.x+5 >= Level[LEVEL].Obs[i][0] && food.coor.y+5 >= Level[LEVEL].Obs[i][1] && food.coor.x+5 <= Level[LEVEL].Obs[i][2] && food.coor.y+5 <= Level[LEVEL].Obs[i][3])
			{
				isOk = false;
				break;
			}
		}
		if (isOk) break;
	}
}


void DrawObs()
{
	setfillcolor(BLACK);
	for (size_t i = 0; i < Level[LEVEL].Obs.size(); i++)
	{
		fillrectangle(Level[LEVEL].Obs[i][0], Level[LEVEL].Obs[i][1], Level[LEVEL].Obs[i][2], Level[LEVEL].Obs[i][3]);
	}
}
