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
    int x;
    int y;
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
    SLOW_FRUIT,
    LIFE_FRUIT,
};

// Game controller class
class GameController
{
private:
    std::random_device _rnd;
    int _blockSize = 25; // dipx
    int _worldSizeX = 25; // blocks
    int _worldSizeY = 25;
    CycleState _state = CycleState::STOPPED;
    FILETIME _lastUpdate = {};
    FILETIME _lastMove = {};
    REFERENCE_TIME _specialFruitTimer = {};
    REFERENCE_TIME _speedUpTimer = {};
    float _baseSpeed = 1.0f;
    float _userSpeed = 10.0f; // multiplies _baseSpeed
    Direction _snakeDir = Direction::RIGHT;
    bool _allowDirChange = true;
    int _lives = 1;
public:
    list<Position> _snakeBody;
    map<Position, Fruit> _fruits;

public:
    GameController();
    void ChangeState(CycleState newState);
    void ChangeHeadingDirection(Direction newDirection);
    void ChangeUserSpeed(float newUserSpeed); // speed = 1 ==> snakes moves at 1 block/sec
    void Update(); // returns true if a frame redraw is required
    void ResizeWorld(int blocksX, int blocksY);
    void InitializeSnake(int len);
    int GetBlockSize() const;
    int GetWorldSizeX() const;
    int GetWorldSizeY() const;
    bool GameOver() const;

private:
    bool CollisionTest(Position pos); // TODO: use where appropriate
    void SpawnFruit(Fruit type = Fruit::GROWTH_FRUIT);
};
