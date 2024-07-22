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
	InitializeSnake(2);
}
void GameController::ChangeState(CycleState newState)
{
	if ((_state == CycleState::STOPPED || _state == CycleState::PAUSED) && newState == CycleState::RUNNING)
	{
		if (_state == CycleState::STOPPED)
		{
			_baseSpeed = 1.0f;
			_specialFruitTimer = {};
			_lives = 1;
			InitializeSnake(2);
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
	if (_allowDirChange && _snakeDir != Opposite(newDirection))
	{
		_allowDirChange = false;
		_snakeDir = newDirection;
	}
}
void GameController::ChangeUserSpeed(float newUserSpeed)
{
	if (newUserSpeed > 0.5f)
		_userSpeed = newUserSpeed;
}
void GameController::Update()
{
	FILETIME now = {};
	GetSystemTimeAsFileTime(&now);
	REFERENCE_TIME elapsed = now - _lastUpdate;
	_lastUpdate = now;
	_appSecondTimer += elapsed;
	if (_appSecondTimer > MS_TO_100NS(1000))
	{
		_fps = MS_TO_100NS(1000) / elapsed;
		_appSecondTimer = {};
	}

	if (_state == CycleState::RUNNING)
	{
		_specialFruitTimer += elapsed;
		_playSecondTimer += elapsed;
		if (_playSecondTimer > MS_TO_100NS(1000))
		{
			_baseSpeed += 0.025f;
			_playSecondTimer = {};
		}
		if (_specialFruitTimer > MS_TO_100NS(30 * 1000))
		{
			std::uniform_int_distribution<int> dist(0, 100);
			Fruit type = dist(_rnd) < 35 ? Fruit::LIFE_FRUIT : Fruit::SLOW_FRUIT;
			SpawnFruit(type);
			_specialFruitTimer = {};
		}

		if (now - _lastMove < MS_TO_100NS(1000 / (_baseSpeed * _userSpeed)))
			return;
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

		if (CollisionTest(_snakeHead))
		{
			_lives--;
			if (_lives == 0)
			{
				_fruits.clear();
				ChangeState(CycleState::STOPPED);
				return;
			}
		}

		_snakeBody.push_front(_snakeHead);

		if (_fruits.count(_snakeHead) > 0)
		{
			Fruit type = _fruits[_snakeHead];
			if (type == Fruit::GROWTH_FRUIT)
			{
				SpawnFruit(); // spawn before erasing current fruit so the new is in a different location
			}
			else
			{
				if (type == Fruit::LIFE_FRUIT)
				{
					_lives++;
				}
				else if (type == Fruit::SLOW_FRUIT)
				{
					_baseSpeed = 1.0f;
				}
				_snakeBody.pop_back(); // these fruits should mantain snake size
			}
			_fruits.erase(_snakeHead);
		} 
		else
		{
			_snakeBody.pop_back();
		}

		_allowDirChange = true; // allow only after advancing one block to prevent inplace collitions
	}
}
void GameController::ResizeWorld(int blocksX, int blocksY)
{
	_worldSizeX = blocksX;
	_worldSizeY = blocksY;

	// Create a new fruit foreach fruit that falls outside the new world
	vector<Position> toRemove;
	vector<Fruit> toRemoveFruits;
	for (auto& fruit : _fruits)
	{
		auto& pos = fruit.first;
		if (pos.x >= _worldSizeX || pos.y >= _worldSizeY)
		{
			toRemove.push_back(pos);
			toRemoveFruits.push_back(fruit.second);
		}
	}
	for (auto& pos : toRemove)
	{
		_fruits.erase(pos);
	}
	for (int i = 0; i < toRemoveFruits.size(); ++i)
	{
		SpawnFruit(toRemoveFruits[i]);
	}
}
void GameController::InitializeSnake(int len)
{

	_snakeDir = Direction::RIGHT;
	bool _allowDirChange = true;
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
bool GameController::GameOver() const
{
	return _state == CycleState::STOPPED && _lives == 0;
}
float GameController::GetUpdateRate() const
{
	return _fps;
}
int GameController::GetLives() const
{
	return _lives;
}
GUIState GameController::GetGUIState() const
{
	GUIState st;
	st.Overlay = _showOverlay;
	st.Init = _state == CycleState::STOPPED;
	st.Paused = _state == CycleState::PAUSED;
	st.GameOver = st.Init && _lives == 0;
	return st;
}
void GameController::ChangeOverlayState()
{
	_showOverlay = !_showOverlay;
}
bool GameController::CollisionTest(Position pos)
{
	return std::find(_snakeBody.begin(), _snakeBody.end(), pos) != _snakeBody.end();
}
void GameController::SpawnFruit(Fruit type)
{
	std::uniform_int_distribution<int> distX(0, _worldSizeX - 1);
	std::uniform_int_distribution<int> distY(0, _worldSizeY - 1);
	Position tentativePos{ distX(_rnd), distY(_rnd) };
	while (CollisionTest(tentativePos) || _fruits.count(tentativePos) > 0)
	{
		tentativePos = { distX(_rnd), distY(_rnd) };
	}
	_fruits.insert({ tentativePos, type });
}