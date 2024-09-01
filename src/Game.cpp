#include "game.h"
#include "Utilities/resource_manager.h"
#include "sprite_renderer.h"

#include <iostream>


// Game-related State data
SpriteRenderer* Renderer;

Game::Game(unsigned int width, unsigned int height)
	: State(GAME_ACTIVE)
	, Keys()
	, Width(width)
	, Height(height) {
}

Game::~Game() {
	delete Renderer;
}

void Game::Init() {
	ResourceManager::LoadShader("Assets/Shaders/spriteShader.vert", "Assets/Shaders/spriteShader.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f
    );
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("Assets/Textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("Assets/Textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("Assets/Textures/block.png", false, "block");
    ResourceManager::LoadTexture("Assets/Textures/block_solid.png", false, "block_solid");
    // load levels
    GameLevel one;
    one.Load("Assets/Levels/one.lvl", this->Width, this->Height / 2);
    GameLevel two;
    two.Load("Assets/Levels/two.lvl", this->Width, this->Height / 2);
    GameLevel three;
    three.Load("Assets/Levels/three.lvl", this->Width, this->Height / 2);
    GameLevel four;
    four.Load("Assets/Levels/four.lvl", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->activeLevel = 0;
}

void Game::ProcessInput(float dt) {

}
void Game::Update(float dt) {

}

void Game::Render() {
    if (this->State == GAME_ACTIVE) {
        //draw background
        Renderer->DrawSprite(
            ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f),
            glm::vec2(this->Width, this->Height), 0, glm::vec3(1.0f, 1.0f, 1.0f)
        );
        // draw level
        this->Levels[this->activeLevel].Draw(*Renderer);
    }
}