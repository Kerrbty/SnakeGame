#ifndef _SNAKE_CORE_HEADER_20210409_H_
#define _SNAKE_CORE_HEADER_20210409_H_

// ��Ϸ״̬ 
#define GAME_NORMAL         0
#define GAME_STOP           1
#define GAME_MEMORY_ERROR  -2
#define GAME_ERROR         -1

// ̰�����ƶ������� 
enum MoveDirection{
    SNAKE_UP = 1,
    SNAKE_DOWN = 2,
    SNAKE_LEFT = 3,
    SNAKE_RIGHT = 4,
};

// ������Ԫ�ض��� 
enum SnakeElement{
    SNAKE_EMPTY =  0x00000000, // �յ� 

    SNAKE_BODY_MASK = 0x10000000,  // ��������(�����0xFFFFFFF,268435455) 
    SNAKE_FOOD_MASK = 0x20000000,  // ʳ������ 
    SNAKE_FOOD   = SNAKE_FOOD_MASK|0x00000001, // ʳ�� 
    SNAKE_REWARD = SNAKE_FOOD_MASK|0x00000002, // ���ؽ��� 
};

// ���ھ�����λ�ã���Cǰ׺��Ϊ�˲���VS�����POINT������ 
typedef struct _CPoint 
{
    int x;
    int y;
}CPoint, *PCPoint;



//////////////////////////////////////////////////////////////////////////
// ����������С(�̶������̶���) 
void SetOutskirtLimit(int width, int height);

// ��ʼ����Ϸ 
int InitGame();

// �����ƶ����� 
void SetMoveDirection(MoveDirection direction);
MoveDirection GetMoveDirection();

// ���ƶ�һ�����������Ƿ������Ϸ(foodid �Ƿ�Ե�ĳ��ʳ��) 
int MoveSnake(SnakeElement* foodid);

// ���ص�ǰ����ͼ 
SnakeElement* GetSnakeState();

// ��ȡ���󳤿�
int GetOutskirtWidth();
int GetOutskirtHeight();

// ��ȡ��ͷλ�� 
CPoint GetSnakeHeader();

// ��ȡ��ǰ������ 
int GetSnakeLength();

// �ͷ���Ϸ��Դ 
void ReleaseGame();


#endif // _SNAKE_CORE_HEADER_20210409_H_ 
