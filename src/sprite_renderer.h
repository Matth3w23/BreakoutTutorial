#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "Utilities/shader.h"
#include "Utilities/texture.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SpriteRenderer {
public:
	//constructor, destructor
	SpriteRenderer(Shader& shader);
	~SpriteRenderer();


	void DrawSprite(Texture2D &texture, glm::vec2 position,
		glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));

private:
	Shader shader;
	unsigned int quadVAO;

	//initialises the quad's buffer and vertex array
	void initRenderData();
};
#endif // !SPRITE_RENDERER_H

