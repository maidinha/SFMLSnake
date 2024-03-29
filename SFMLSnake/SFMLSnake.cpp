#include "SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <random>

const int TILESIZE = 18;

std::random_device rd;
std::mt19937 randomNumbers(rd());

sf::RenderWindow renderWindow(sf::VideoMode(360, 360), "SFML Silly Snake");

int gameGrid[20][20];

sf::Vector2i vPlayerDir(0, 1);
std::list<sf::Vector2f> playerTilePositions;

sf::Vector2i vFoodPosition(10, 10);

sf::Texture tGameAtlas;
sf::Sprite sTile;

float moveDelay = 0.15f;
float gameTimer = 0;

sf::Vector2i pixelToGrid(sf::Vector2f &pixelCoordinates)
{
	return sf::Vector2i(
		floor(pixelCoordinates.x / TILESIZE),
		floor(pixelCoordinates.y / TILESIZE)
	);
}

sf::Vector2f gridToPixel(sf::Vector2i &gridCoordinates)
{
	return sf::Vector2f(
		gridCoordinates.x * TILESIZE,
		gridCoordinates.y * TILESIZE
	);
}

std::vector<sf::Vector2i> getAvailableCells()
{
	std::vector<sf::Vector2i> avaliableCells;

	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (gameGrid[i][j] == 0)
				avaliableCells.push_back(sf::Vector2i(j, i));
		}
	}

	return avaliableCells;
}

void spawnFood()
{
	std::vector<sf::Vector2i> avaliableCells = getAvailableCells();
	std::uniform_int_distribution<int> range(0, avaliableCells.size() - 1);

	int index = range(randomNumbers);

	vFoodPosition = avaliableCells[index];
	gameGrid[vFoodPosition.y][vFoodPosition.x] = 2;
}

void initializeGraphics()
{
	tGameAtlas.loadFromFile("assets/tiles.png");
	sTile.setTexture(tGameAtlas);
}

void initializePlayer()
{
	vPlayerDir.x = 0;
	vPlayerDir.y = 1;

	playerTilePositions.push_back(sf::Vector2f(0, 0));

	gameGrid[0][0] = 1;
}

void initializeGame()
{
	renderWindow.setFramerateLimit(60);
	
	playerTilePositions.clear();
	memset(gameGrid, 0, sizeof(gameGrid));
	
	spawnFood();
	initializePlayer();
	initializeGraphics();
}


void handleEvents()
{
	sf::Event event;

	while (renderWindow.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			renderWindow.close();
		}
		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::Left && vPlayerDir.x == 0)
			{
				vPlayerDir.x = -1;
				vPlayerDir.y = 0;
				gameTimer = moveDelay;
			}
			else if (event.key.code == sf::Keyboard::Right && vPlayerDir.x == 0)
			{
				vPlayerDir.x = 1;
				vPlayerDir.y = 0;
				gameTimer = moveDelay;
			}
			else if (event.key.code == sf::Keyboard::Down && vPlayerDir.y == 0)
			{
				vPlayerDir.x = 0;
				vPlayerDir.y = 1;
				gameTimer = moveDelay;
			}
			else if (event.key.code == sf::Keyboard::Up && vPlayerDir.y == 0)
			{
				vPlayerDir.x = 0;
				vPlayerDir.y = -1;
				gameTimer = moveDelay;
			}
		}
	}
}

bool hasEncounteredFood()
{
	sf::Vector2i headPosition = pixelToGrid(playerTilePositions.front());
	sf::Vector2i positionToCheck = headPosition + vPlayerDir;

	return positionToCheck.x >= 0 && positionToCheck.x < 20
		&& positionToCheck.y >= 0 && positionToCheck.y < 20
		&& gameGrid[positionToCheck.y][positionToCheck.x] == 2;
}

bool hasHitObstacle()
{
	sf::Vector2i headPosition = pixelToGrid(playerTilePositions.front());
	sf::Vector2i positionToCheck = headPosition + vPlayerDir;

	return positionToCheck.x < 0 || positionToCheck.x >= 20
		|| positionToCheck.y < 0 || positionToCheck.y >= 20
		|| gameGrid[positionToCheck.y][positionToCheck.x] == 1;
}

void movePlayer()
{
	sf::Vector2f moveDirection = gridToPixel(vPlayerDir);
	sf::Vector2f tilePosition = playerTilePositions.front() + moveDirection;
	sf::Vector2i gridPosition = pixelToGrid(tilePosition);

	gameGrid[gridPosition.y][gridPosition.x] = 1;

	playerTilePositions.push_front(tilePosition);

	sf::Vector2i positionToClear = pixelToGrid(playerTilePositions.back());
	playerTilePositions.pop_back();
	gameGrid[positionToClear.y][positionToClear.x] = 0;
}

void increasePlayerSize()
{
	sf::Vector2f tilePosition;
	sf::Vector2i gridPosition;

	if (playerTilePositions.size() == 1)
	{
		tilePosition = playerTilePositions.front() + (gridToPixel(vPlayerDir) * -1.f);
		gridPosition = pixelToGrid(tilePosition);
	}
	else
	{
		std::list<sf::Vector2f>::iterator it = playerTilePositions.begin();
		std::advance(it, playerTilePositions.size() - 2);

		tilePosition = playerTilePositions.back() + (playerTilePositions.back() - *it);
	}

	playerTilePositions.push_back(tilePosition);
	gameGrid[gridPosition.y][gridPosition.x] = 1;
}

void eatFood()
{
	increasePlayerSize();

	sf::Vector2i vLastFoodPosition = vFoodPosition;

	spawnFood();

	gameGrid[vLastFoodPosition.y][vLastFoodPosition.x] = 0;
}

void update()
{
	if (gameTimer > moveDelay)
	{
		if (hasHitObstacle())
		{
			initializeGame();
		}
		else
		{
			if (hasEncounteredFood())
			{
				eatFood();
			}
			movePlayer();
		}
		gameTimer = 0;
	}
}

void render()
{
	renderWindow.clear(sf::Color::White);

	sTile.setTextureRect(sf::IntRect(0, 0, TILESIZE, TILESIZE));

	for (auto &pos : playerTilePositions)
	{
		sTile.setPosition(pos);
		renderWindow.draw(sTile);
	}

	sTile.setTextureRect(sf::IntRect(18 * 2, 0, TILESIZE, TILESIZE));
	sTile.setPosition(gridToPixel(vFoodPosition));

	renderWindow.draw(sTile);

	renderWindow.display();
}

int main()
{
	initializeGame();

	sf::Clock clock;

	while (renderWindow.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		gameTimer += time;

		handleEvents();
		update();
		render();
	}

	return 0;
}