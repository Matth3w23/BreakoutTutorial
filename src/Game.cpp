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

    if (Ball->Position.y >= this->Height) {
        this->ResetLevel();
        this->ResetPlayer();
    }
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
Collision CheckCollision(BallObject& ball, GameObject& box);
Direction VectorDirection(glm::vec2 target);

void Game::DoCollisions() {
    //boxes
    for (GameObject& box : this->Levels[this->activeLevel].Bricks) {
        if (!box.Destroyed) {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) {
                if (!box.IsSolid) {
                    box.Destroyed = true;
                }

                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) { //horizontal collision
                    Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT) {
                        Ball->Position.x += penetration;
                    } else {
                        Ball->Position.x -= penetration;
                    }
                } else { //vertical
                    Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP) {
                        Ball->Position.y -= penetration;
                    } else {
                        Ball->Position.y += penetration;
                    }
                }
            }
        }
    }

    //player
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result)) {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
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
Collision CheckCollision(BallObject& ball, GameObject& box) {
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
    // retrieve vector between center circle and closest point AABB
    difference = closest - ballCenter;

    //check if length <= radius and return collision
    if (glm::length(difference) <= ball.Radius) {
        return std::make_tuple(true, VectorDirection(difference), difference);
    } else {
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
    }
}

Direction VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++) {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}



void Game::ResetLevel() {
    if (this->activeLevel == 0) {
        this->Levels[0].Load("Assets/Levels/one.lvl", this->Width, this->Height / 2);
    } else if (this->activeLevel == 1) {
            this->Levels[1].Load("Assets/Levels/one.lvl", this->Width, this->Height / 2);
    } else if (this->activeLevel == 2) {
        this->Levels[2].Load("Assets/Levels/one.lvl", this->Width, this->Height / 2);
    } else if (this->activeLevel == 3) {
        this->Levels[3].Load("Assets/Levels/one.lvl", this->Width, this->Height / 2);
    }
}

void Game::ResetPlayer() {
    // reset player and ball states
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}