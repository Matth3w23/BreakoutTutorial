#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Utilities/texture.h"
#include "sprite_renderer.h"

//Object holding the minimum state required for an object to be in the game.
class GameObject {
public:
	//object state
	glm::vec2 Position, Size, Velocity;
	glm::vec3 Color;
	float Rotation;
	bool IsSolid; //check usage
	bool Destroyed;

	//render state
	Texture2D Sprite;

	//constructors
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	//draw
	virtual void Draw(SpriteRenderer& renderer);
};

#endif // !GAMEOBJECT_H