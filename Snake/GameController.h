#pragma once

#include "framework.h"
using namespace std;

enum CycleState
{
    STOPPED,
    RUNNING,
    PAUSED,
};
struct Position
{
    int coordX;
    int coordY;
}; 
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};
Direction Opposite(Direction dir);
enum Fruit
{
    GROWTH_FRUIT,
    SPEED_FRUIT,
};

// Game controller class
class GameController
{
private:
    int _blockSize = 20; // dipx
    int _worldSizeX = 45; // blocks
    int _worldSizeY = 45;
    CycleState _state = CycleState::STOPPED;
    FILETIME _lastMove = {};
    float _baseSpeed = 1.0f;
    float _userSpeed = 10.0f; // multiplies _baseSpeed
    Direction _snakeDir = Direction::RIGHT;
public:
    list<Position> _snakeBody;
    map<Position, Fruit> _fruits;

public:
    GameController();
    void ChangeState(CycleState newState);
    void ChangeHeadingDirection(Direction newDirection);
    void ChangeUserSpeed(float newUserSpeed); // speed = 1 ==> snakes moves at 1 block/sec
    bool Update(); // returns true if a frame redraw is required
    void ResizeWorld(int blocksX, int blocksY);
    int GetBlockSize() const;
    int GetWorldSizeX() const;
    int GetWorldSizeY() const;
private:
    void InitializeSnake(int len);
};
