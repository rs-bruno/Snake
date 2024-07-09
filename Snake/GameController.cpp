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

bool operator==(const Position& l, const Position& r) {
	return (l.x == r.x && l.y == r.y);
}
bool operator<(const Position& l, const Position& r) {
	return (l.x < r.x || (l.x == r.x && l.y < r.y));
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
			SpawnFruit();
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

		if (_fruits.count(_snakeHead) > 0)
		{
			_fruits.erase(_snakeHead);
			SpawnFruit();
		} 
		else
		{
			_snakeBody.pop_back();
		}

		return true;
	}
	return false;
}
void GameController::ResizeWorld(int blocksX, int blocksY)
{
	_worldSizeX = blocksX;
	_worldSizeY = blocksY;

	// Create a new fruit foreach fruit that falls outside the new world
	vector<Position> toRemove;
	for (auto& fruit : _fruits)
	{
		auto& pos = fruit.first;
		if (pos.x >= _worldSizeX || pos.y >= _worldSizeY)
			toRemove.push_back(pos);
	}
	for (auto& pos : toRemove)
	{
		_fruits.erase(pos);
	}
	for (int i = 0; i < toRemove.size(); ++i)
	{
		SpawnFruit();
	}
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
void GameController::SpawnFruit()
{
	std::uniform_int_distribution<int> distX(0, _worldSizeX - 1);
	std::uniform_int_distribution<int> distY(0, _worldSizeY - 1);
	Position tentativePos{ distX(_rnd), distY(_rnd) };
	while (std::find(_snakeBody.begin(), _snakeBody.end(), tentativePos) != _snakeBody.end() || _fruits.count(tentativePos) > 0)
	{
		tentativePos = { distX(_rnd), distY(_rnd) };
	}
	_fruits.insert({ tentativePos, Fruit::GROWTH_FRUIT });
}