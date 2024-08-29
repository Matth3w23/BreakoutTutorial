#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

//are these needed in header?
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "Utilities/resource_manager.h"

//holds tiles for a level of the game, as well as loading from a file.
//should loading and storing/rendering be split?
class GameLevel {
public:
	//level state
	std::vector<GameObject> Bricks;
	//constructor
	GameLevel() {}
	//load level from given file
	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);
	//render loaded level
	void Draw(SpriteRenderer& renderer);
	//check if level completed/all destroyed
	bool IsCompleted();
private:
	//initialises a level from read tile data
	void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif // !GAMELEVEL_H
