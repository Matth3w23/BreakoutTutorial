#include "game.h"
#include "Utilities/resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"

#include <glm/glm.hpp>
#include <iostream>


// Game-related State data
SpriteRenderer* Renderer;
GameObject* Player;

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
    this->DoCollisions();
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

bool CheckCollision(GameObject& one, GameObject& two);
bool CheckCollision(BallObject& ball, GameObject& box);

void Game::DoCollisions() {
    for (GameObject& box : this->Levels[this->activeLevel].Bricks) {
        if (!box.Destroyed) {
            if (CheckCollision(*Ball, box)) {
                if (!box.IsSolid) {
                    box.Destroyed = true;
                }
            }
        }
    }
}

//check if axis aligned BB collisions between two GameObjects
bool CheckCollision(GameObject& one, GameObject& two) {
    // x axis
    bool collisionX =
        one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // y axis
    bool collisionY =
        one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision if both
    return collisionX && collisionY;
}

//collision between a Ball (Circle) and a GameObject (AABB)
bool CheckCollision(BallObject& ball, GameObject& box) {
    //calculate info about objects
    glm::vec2 ballCenter(ball.Position + ball.Radius);
    glm::vec2 boxHalfExtents(box.Size.x / 2.0f, box.Size.y / 2.0f);
    glm::vec2 boxCenter(
        box.Position.x + boxHalfExtents.x,
        box.Position.y + boxHalfExtents.y
    );
    // vector between object centers
    glm::vec2 difference = ballCenter - boxCenter;

    //clamping to box, and adding to box center gives closest position on the box to the ball
    glm::vec2 clamped = glm::clamp(difference, -boxHalfExtents, boxHalfExtents);
    glm::vec2 closest = boxCenter + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - ballCenter;
    return glm::length(difference) < ball.Radius;
}