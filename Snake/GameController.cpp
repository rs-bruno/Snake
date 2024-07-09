#include "GameController.h"

Direction Opposite(Direction dir)
{
	switch (dir)
	{
	case Direction::UP: return Direction::DOWN;
	case Direction::DOWN: return Direction::UP;
	case Direction::LEFT: return Direction::RIGHT;
	case Direction::RIGHT: return Direction::LEFT;
	default: abort();
	}
}

GameController::GameController()
{
	InitializeSnake(7);
}
void GameController::ChangeState(CycleState newState)
{
	if ((_state == CycleState::STOPPED || _state == CycleState::PAUSED) && newState == CycleState::RUNNING)
	{
		if (_state == CycleState::STOPPED)
		{
			_baseSpeed = 1.0f;
			InitializeSnake(7);
		}
		_state = newState;
		return;
	}
	if (_state == CycleState::RUNNING)
		_state = newState;
}
void GameController::ChangeHeadingDirection(Direction newDirection)
{
	if (Opposite(newDirection) != _snakeDir)
		_snakeDir = newDirection;
}
void GameController::ChangeUserSpeed(float newUserSpeed)
{
	if (newUserSpeed > 0.5f)
		_userSpeed = newUserSpeed;
}
bool GameController::Update()
{
	if (_state == CycleState::RUNNING)
	{
		FILETIME now = {};
		GetSystemTimeAsFileTime(&now);
		if (now - _lastMove < MS_TO_100NS(1000 / (_baseSpeed * _userSpeed)))
			return false;
		_lastMove = now;

		// Setup next head position
		auto _snakeHead = _snakeBody.front();
		switch (_snakeDir)
		{
		case Direction::UP:
			_snakeHead.y = (_snakeHead.y - 1);
			break;
		case Direction::DOWN:
			_snakeHead.y = (_snakeHead.y + 1);
			break;
		case Direction::LEFT:
			_snakeHead.x = (_snakeHead.x - 1);
			break;
		default:
			_snakeHead.x = (_snakeHead.x + 1);
			break;
		}
		_snakeHead.x = _snakeHead.x < 0 ? (_worldSizeX - 1) : _snakeHead.x % _worldSizeX;
		_snakeHead.y = _snakeHead.y < 0 ? (_worldSizeY - 1) : _snakeHead.y % _worldSizeY;
		_snakeBody.push_front(_snakeHead);
		_snakeBody.pop_back();

		return true;
	}
	return false;
}
void GameController::ResizeWorld(int blocksX, int blocksY)
{
	_worldSizeX = blocksX;
	_worldSizeY = blocksY;
}
void GameController::InitializeSnake(int len)
{
	_snakeDir = Direction::RIGHT;
	_snakeBody.clear();
	Position _snakeStartingHead;
	_snakeStartingHead.x = _worldSizeX / 2;
	_snakeStartingHead.y = _worldSizeY / 2;
	_snakeBody.push_back(_snakeStartingHead);
	for (int i = 0; i < len - 1; ++i)
	{
		_snakeStartingHead.x -= 1;
		_snakeBody.push_back(_snakeStartingHead);
	}
}
int GameController::GetBlockSize() const
{
	return _blockSize;
}
int GameController::GetWorldSizeX() const
{
	return _worldSizeX;
}
int GameController::GetWorldSizeY() const
{
	return _worldSizeY;
}