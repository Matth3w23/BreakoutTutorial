#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"

class ResourceManager {
public:
	// resource storage
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;

	//given a vertex shader file, a fragment shader file and optionally a geomtry shader file, loads and generates a shader program.
	static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);
	// retrieves a stored shader
	static Shader GetShader(std::string name);

	//loads and generates a texture from a file
	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);
	//retrieves a stored shader
	static Texture2D GetTexture(std::string name);

	//de-allocates all loaded resources
	static void Clear();
private:
	ResourceManager() {}
	//load and generate a shader from files
	static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);
	//load a single texture from a file
	static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif // !RESOURCE_MANAGER_H