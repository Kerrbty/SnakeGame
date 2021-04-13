#ifndef _SNAKE_CORE_HEADER_20210409_H_
#define _SNAKE_CORE_HEADER_20210409_H_

// 游戏状态 
#define GAME_NORMAL         0
#define GAME_STOP           1
#define GAME_MEMORY_ERROR  -2
#define GAME_ERROR         -1

// 贪吃蛇移动方向定义 
enum MoveDirection{
    SNAKE_UP = 1,
    SNAKE_DOWN = 2,
    SNAKE_LEFT = 3,
    SNAKE_RIGHT = 4,
};

// 矩阵中元素定义 
enum SnakeElement{
    SNAKE_EMPTY =  0x00000000, // 空挡 

    SNAKE_BODY_MASK = 0x10000000,  // 蛇身掩码(最长蛇身0xFFFFFFF,268435455) 
    SNAKE_FOOD_MASK = 0x20000000,  // 食物掩码 
    SNAKE_FOOD   = SNAKE_FOOD_MASK|0x00000001, // 食物 
    SNAKE_REWARD = SNAKE_FOOD_MASK|0x00000002, // 过关奖励 
};

// 点在矩阵中位置（加C前缀是为了不与VS定义的POINT混淆） 
typedef struct _CPoint 
{
    int x;
    int y;
}CPoint, *PCPoint;



//////////////////////////////////////////////////////////////////////////
// 设置蛇笼大小(固定？不固定？) 
void SetOutskirtLimit(int width, int height);

// 初始化游戏 
int InitGame();

// 设置移动方向 
void SetMoveDirection(MoveDirection direction);
MoveDirection GetMoveDirection();

// 蛇移动一步，并返回是否结束游戏(foodid 是否吃到某种食物) 
int MoveSnake(SnakeElement* foodid);

// 返回当前的视图 
SnakeElement* GetSnakeState();

// 获取矩阵长宽
int GetOutskirtWidth();
int GetOutskirtHeight();

// 获取蛇头位置 
CPoint GetSnakeHeader();

// 获取当前蛇身长度 
int GetSnakeLength();

// 释放游戏资源 
void ReleaseGame();


#endif // _SNAKE_CORE_HEADER_20210409_H_ 
