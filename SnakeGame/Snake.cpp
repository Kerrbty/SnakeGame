#include "Snake.h"
#include <memory.h>
#include <stdlib.h>
#include <time.h>

// 游戏用到的全部信息(除了移动速度，外部控制) 
typedef struct _SnakeInfo{
    // 蛇笼大小 
    int width;   
    int height; 
    int* SnakeCage;  // 二维矩阵 

    // 蛇信息  
    CPoint Header;  // 蛇头 
    int len;        // 蛇长度 
    MoveDirection direction;  // 移动方向 
    int GameState;
}SnakeInfo, *PSnakeInfo;


SnakeInfo gsnake = {0};

// 随机生成食物位置 
void RandomFood(SnakeElement Food)
{
    // 只允许 SNAKE_FOOD 或者 SNAKE_REWARD 
    if (Food == SNAKE_FOOD || Food == SNAKE_REWARD)
    {
        // 如果矩阵比较小，蛇比较长，这个循环效率可能有点差 
        // 之后再考虑优化 
        while(true)
        {
            int x = rand()%gsnake.width;
            int y = rand()%gsnake.height;

            int p = y*gsnake.width + x;
            if ( SNAKE_EMPTY == gsnake.SnakeCage[p] )
            {
                gsnake.SnakeCage[p] = Food;
                break;
            }
        }
    }
}

// 设置蛇笼边界 
void SetOutskirtLimit(int width, int height)
{
    gsnake.width = width;
    gsnake.height = height;
}

// 初始化游戏数据 
int InitGame()
{
    // 设置随机数 
    srand((unsigned)time(NULL));

    if (gsnake.width <= 2 || gsnake.height <= 2)
    {
        return GAME_ERROR;
    }

    // 不是第一次运行，先释放原内存 
    if (gsnake.SnakeCage != NULL)
    {
        free(gsnake.SnakeCage);
    }

    gsnake.SnakeCage = (int*)malloc(gsnake.width*gsnake.height*sizeof(SnakeElement));
    if (gsnake.SnakeCage == NULL)
    {
        return GAME_MEMORY_ERROR;
    }
    memset(gsnake.SnakeCage, SNAKE_EMPTY, gsnake.width*gsnake.height*sizeof(SnakeElement));

    // 初始蛇身（向右移动） 
    gsnake.direction = SNAKE_RIGHT;
    // 蛇第一个位置在相对中间（奇偶问题） 
    gsnake.Header.x = gsnake.width/2 - 1;
    gsnake.Header.y = gsnake.height/2 - 1;
    gsnake.SnakeCage[gsnake.Header.y*gsnake.width + gsnake.Header.x] = SNAKE_BODY_MASK;
    gsnake.len = 1;

    // 设置第一个食物位置 
    RandomFood(SNAKE_FOOD);
    gsnake.GameState = GAME_NORMAL;
    return GAME_NORMAL;
}

// 设置移动方向 
void SetMoveDirection(MoveDirection direction)
{
    gsnake.direction = direction;
}

MoveDirection GetMoveDirection()
{
    return gsnake.direction;
}

// 蛇移动一步，并返回是否结束游戏(foodid 是否吃到某种食物) 
int MoveSnake(SnakeElement* foodid)
{
    if (foodid)
    {
        *foodid = SNAKE_EMPTY;
    }
    if (gsnake.GameState != GAME_NORMAL)
    {
        return gsnake.GameState;
    }

    // 检测是否在边界 
    int x = gsnake.Header.x;
    int y = gsnake.Header.y;
    do 
    {
        if (gsnake.direction == SNAKE_UP)
        {
            if (gsnake.Header.y == 0)
            {
                gsnake.GameState = GAME_STOP;
                break;
            }
            y--;
        }
        else if (gsnake.direction == SNAKE_DOWN)
        {
            if (gsnake.Header.y == gsnake.height-1)
            {
                gsnake.GameState = GAME_STOP;
                break;
            }
            y++;
        }
        else if (gsnake.direction == SNAKE_LEFT)
        {
            if (gsnake.Header.x == 0)
            {
                gsnake.GameState = GAME_STOP;
                break;
            }
            x--;
        }
        else if (gsnake.direction == SNAKE_RIGHT)
        {
            if (gsnake.Header.x == gsnake.width-1)
            {
                gsnake.GameState = GAME_STOP;
                break;
            }
            x++;
        }

        if ( (gsnake.SnakeCage[y*gsnake.width + x]&SNAKE_BODY_MASK) == SNAKE_BODY_MASK)
        {
            gsnake.GameState = GAME_STOP;
            break;
        }
    } while (false);

    // 蛇撞死了 
    if (gsnake.GameState != GAME_NORMAL)
    {
        return gsnake.GameState;
    }

    // 判断是否有食物 foodid 
    if ( (gsnake.SnakeCage[y*gsnake.width + x]&SNAKE_FOOD_MASK) == SNAKE_FOOD_MASK)
    {
        // 有食物,返回计算分值 
        if (foodid)
        {
            *foodid = (SnakeElement)gsnake.SnakeCage[y*gsnake.width + x];
        }
        // 蛇身加1 
        gsnake.len++;

        bool reward = false;
        if (gsnake.len%5 == 0)
        {
            if (rand()%3 == 0)
            {
                // 三分之一概率出奖励 
                reward = true;
            }
        }
        if (reward)
        {
            // 出奖励也可以同时出食物 
            RandomFood(SNAKE_REWARD);
        }
        else
        {
            RandomFood(SNAKE_FOOD);
        }
    }
    for (int i=0; i<gsnake.width*gsnake.height; i++)
    {
        if ( (gsnake.SnakeCage[i]&SNAKE_BODY_MASK) == SNAKE_BODY_MASK)
        {
            gsnake.SnakeCage[i]++;
            if ((gsnake.SnakeCage[i]^SNAKE_BODY_MASK) == gsnake.len)
            {
                // 删除蛇尾 
                gsnake.SnakeCage[i] = SNAKE_EMPTY;
            }
        }
    }

    // 新蛇头 
    gsnake.SnakeCage[y*gsnake.width + x] = SNAKE_BODY_MASK;
    gsnake.Header.x = x;
    gsnake.Header.y = y;
    return GAME_NORMAL;
}

// 返回当前的视图 
SnakeElement* GetSnakeState()
{
    return (SnakeElement*)gsnake.SnakeCage;
}

int GetOutskirtWidth()
{
    return gsnake.width;
}

int GetOutskirtHeight()
{
    return gsnake.height;
}

// 获取蛇头位置 
CPoint GetSnakeHeader()
{
    return gsnake.Header;
}

// 获取当前蛇身长度 
int GetSnakeLength()
{
    return gsnake.len;
}

// 释放游戏资源 
void ReleaseGame()
{
    if (gsnake.SnakeCage != NULL)
    {
        free(gsnake.SnakeCage);
        gsnake.SnakeCage = NULL;
    }
    memset(&gsnake, 0, sizeof(SnakeInfo));
}