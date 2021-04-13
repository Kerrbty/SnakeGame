#include "Snake.h"
#include <memory.h>
#include <stdlib.h>
#include <time.h>

// ��Ϸ�õ���ȫ����Ϣ(�����ƶ��ٶȣ��ⲿ����) 
typedef struct _SnakeInfo{
    // ������С 
    int width;   
    int height; 
    int* SnakeCage;  // ��ά���� 

    // ����Ϣ  
    CPoint Header;  // ��ͷ 
    int len;        // �߳��� 
    MoveDirection direction;  // �ƶ����� 
    int GameState;
}SnakeInfo, *PSnakeInfo;


SnakeInfo gsnake = {0};

// �������ʳ��λ�� 
void RandomFood(SnakeElement Food)
{
    // ֻ���� SNAKE_FOOD ���� SNAKE_REWARD 
    if (Food == SNAKE_FOOD || Food == SNAKE_REWARD)
    {
        // �������Ƚ�С���߱Ƚϳ������ѭ��Ч�ʿ����е�� 
        // ֮���ٿ����Ż� 
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

// ���������߽� 
void SetOutskirtLimit(int width, int height)
{
    gsnake.width = width;
    gsnake.height = height;
}

// ��ʼ����Ϸ���� 
int InitGame()
{
    // ��������� 
    srand((unsigned)time(NULL));

    if (gsnake.width <= 2 || gsnake.height <= 2)
    {
        return GAME_ERROR;
    }

    // ���ǵ�һ�����У����ͷ�ԭ�ڴ� 
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

    // ��ʼ���������ƶ��� 
    gsnake.direction = SNAKE_RIGHT;
    // �ߵ�һ��λ��������м䣨��ż���⣩ 
    gsnake.Header.x = gsnake.width/2 - 1;
    gsnake.Header.y = gsnake.height/2 - 1;
    gsnake.SnakeCage[gsnake.Header.y*gsnake.width + gsnake.Header.x] = SNAKE_BODY_MASK;
    gsnake.len = 1;

    // ���õ�һ��ʳ��λ�� 
    RandomFood(SNAKE_FOOD);
    gsnake.GameState = GAME_NORMAL;
    return GAME_NORMAL;
}

// �����ƶ����� 
void SetMoveDirection(MoveDirection direction)
{
    gsnake.direction = direction;
}

MoveDirection GetMoveDirection()
{
    return gsnake.direction;
}

// ���ƶ�һ�����������Ƿ������Ϸ(foodid �Ƿ�Ե�ĳ��ʳ��) 
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

    // ����Ƿ��ڱ߽� 
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

    // ��ײ���� 
    if (gsnake.GameState != GAME_NORMAL)
    {
        return gsnake.GameState;
    }

    // �ж��Ƿ���ʳ�� foodid 
    if ( (gsnake.SnakeCage[y*gsnake.width + x]&SNAKE_FOOD_MASK) == SNAKE_FOOD_MASK)
    {
        // ��ʳ��,���ؼ����ֵ 
        if (foodid)
        {
            *foodid = (SnakeElement)gsnake.SnakeCage[y*gsnake.width + x];
        }
        // �����1 
        gsnake.len++;

        bool reward = false;
        if (gsnake.len%5 == 0)
        {
            if (rand()%3 == 0)
            {
                // ����֮һ���ʳ����� 
                reward = true;
            }
        }
        if (reward)
        {
            // ������Ҳ����ͬʱ��ʳ�� 
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
                // ɾ����β 
                gsnake.SnakeCage[i] = SNAKE_EMPTY;
            }
        }
    }

    // ����ͷ 
    gsnake.SnakeCage[y*gsnake.width + x] = SNAKE_BODY_MASK;
    gsnake.Header.x = x;
    gsnake.Header.y = y;
    return GAME_NORMAL;
}

// ���ص�ǰ����ͼ 
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

// ��ȡ��ͷλ�� 
CPoint GetSnakeHeader()
{
    return gsnake.Header;
}

// ��ȡ��ǰ������ 
int GetSnakeLength()
{
    return gsnake.len;
}

// �ͷ���Ϸ��Դ 
void ReleaseGame()
{
    if (gsnake.SnakeCage != NULL)
    {
        free(gsnake.SnakeCage);
        gsnake.SnakeCage = NULL;
    }
    memset(&gsnake, 0, sizeof(SnakeInfo));
}