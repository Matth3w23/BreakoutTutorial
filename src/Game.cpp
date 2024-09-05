#include "game.h"
#include "Utilities/resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"

#include <iostream>


// Game-related State data
SpriteRenderer* Renderer;
GameObject* Player;

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

BallObject* Ball;

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

    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    // load textures
    ResourceManager::LoadTexture("Assets/Textures/awesomeface.png", true, "face");
    ResourceManager::LoadTexture("Assets/Textures/background.jpg", false, "background");
    ResourceManager::LoadTexture("Assets/Textures/block.png", false, "block");
    ResourceManager::LoadTexture("Assets/Textures/block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("Assets/Textures/paddle.png", false, "paddle");

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

    //initialise player
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2.0f,
        this->Height - PLAYER_SIZE.y
    );
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    //initialise ball
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::ProcessInput(float dt) {
    if (this->State == GAME_ACTIVE) {
        float velocity = PLAYER_VELOCITY * dt;

        if (this->Keys[GLFW_KEY_A]) {
            if (Player->Position.x >= 0.0f) {
                Player->Position.x -= velocity;
                if (Ball->Stuck) {
                    Ball->Position.x -= velocity;
                }
            }
        }

        if (this->Keys[GLFW_KEY_D]) {
            if (Player->Position.x <= this->Width - Player->Size.x) {
                Player->Position.x += velocity;
                if (Ball->Stuck) {
                    Ball->Position.x += velocity;
                }
            }
        }

        if (this->Keys[GLFW_KEY_SPACE]) {
            Ball->Stuck = false;
        }
    }
}

void Game::Update(float dt) {
    Ball->Move(dt, this->Width);
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

        //draw paddle
        Player->Draw(*Renderer);

        //draw ball
        Ball->Draw(*Renderer);
    }
}