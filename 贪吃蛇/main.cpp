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
void InitSnake();
void MoveSnake(Food& food);
void DrawSnake();
void ChangeDir();
void DrawFood(Food& food);
void DrawScore();
void DrawBoundary();
void DrawGameStatus(char *status);
void DrawGameData();

void Draw();

int main()
{
	InitSnake();
	while (1)
	{
		while (!_kbhit())
		{
			BeginBatchDraw();
			setbkcolor(WHITE);
			Draw();
			FlushBatchDraw();
			//saveimage("F:\Python\bk_img.png");
			cleardevice();
			Sleep(SPEED);
		}
		ChangeDir();
	}
	mciSendString("close all", 0, 0, 0);
	getchar();
	EndBatchDraw();
	closegraph();
}

void InitSnake()
{
	initgraph(800, 600);
	// 载入背景音乐
	mciSendString("open bkmusic.mp3 alias bk",0,0,0);
	mciSendString("play bk repeat",0,0,0);
	mciSendString("open collision.mp3 alias dead", 0, 0, 0);
	mciSendString("open eat.mp3 alias eat", 0, 0, 0);
	SNAKE.sbody[0].x = 0;
	SNAKE.sbody[0].y = 0;
	SNAKE.dir = down;
	//SNAKE.length = 3;
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
	bool isCollision = false;
	if (SNAKE.sbody[0].x == 600 || SNAKE.sbody[0].y == 600 || SNAKE.sbody[0].x < 0 || SNAKE.sbody[0].y < 0) // 边界碰撞检测
	{
		isCollision = true;
	}
	else if(SNAKE.length > 2)
	{
		for (int i = 3; i < SNAKE.length; i++)
		{
			if (SNAKE.sbody[0].x == SNAKE.sbody[i].x && SNAKE.sbody[0].y == SNAKE.sbody[i].y)
				isCollision = true;
		}
	}
	// 碰撞后复原
	if (isCollision)
	{
		mciSendString("play dead from 0", 0, 0, 0);
		SNAKE = Snake();
		SNAKE.dir = down;
		char status[10] = "GAME OVER";
		DrawGameStatus(status);
		FlushBatchDraw();
		while (!_kbhit());
		SCORE = 0;
	}
		// 吃食物
	if (SNAKE.sbody[0].x == food.coor.x && SNAKE.sbody[0].y == food.coor.y)
	{
		mciSendString("play eat from 0", 0, 0, 0);
		srand((unsigned int)time(NULL));
		food.coor.x = (rand() % (600 / SNAKE_SIZE)) * 10;
		food.coor.y = (rand() % (600 / SNAKE_SIZE)) * 10;
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
	settextcolor(BLACK);
	settextstyle(15, 15, "黑体");
	outtextxy(610, 10, "SCORE: ");
	settextcolor(RGB(rand() % 256, rand() % 256, rand() % 256));
	settextstyle(60, 20, "黑体");
	outtextxy(620, 35, itoChar(SCORE));
}

void DrawBoundary()
{
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 2);
	line(600, 0, 600, 600);
	line(600, 100, 800, 100);
	line(0, 0, 800, 0);
	line(0, 0, 0, 600);
	line(0, 600, 600, 600);
	line(600, 400, 800, 400);
}

void DrawGameStatus(char *status)
{
	settextcolor(RED);
	settextstyle(60, 20, "黑体");
	outtextxy(210, 270, status);
	settextstyle(30, 15, "黑体");
	outtextxy(135, 335, "PRESS ANYKEY TO RESUME");
	Draw();
}

void Draw()
{
	MoveSnake(food);
	DrawFood(food);
	DrawBoundary();
	DrawScore();
	DrawGameData();
}

void DrawGameData()
{
	settextcolor(BLACK);
	settextstyle(15, 15, "黑体");
	outtextxy(610, 110, "REAL TIME: ");
	settextstyle(10, 10, "黑体");
	outtextxy(610, 140, "Lenght: ");
	outtextxy(690, 140, itoChar(SNAKE.length));
	outtextxy(610, 170, "MAX_SCORE: ");
	outtextxy(720, 170, itoChar(CUR_MAX_SCORE));
	outtextxy(610, 200, "CUR_SPEED(ms): ");
	outtextxy(750, 200, itoChar(SPEED));
}