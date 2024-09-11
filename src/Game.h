#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "game_level.h"

//represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Represents the four possible collision directions
enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

//tuple for returning collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision;

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

class Game {
public:
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;

	std::vector<GameLevel> Levels;
	unsigned int activeLevel;

	Game(unsigned int width, unsigned int height);
	~Game();

	void Init();

	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();

	void DoCollisions();

	void ResetLevel();
	void ResetPlayer();
};

#endif