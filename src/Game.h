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
};

#endif