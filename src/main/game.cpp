#include <iostream>
#include <iomanip>
#include <vector>
#include "raylib.h"
#include "game.h"
#include "questionBank.h"

Image image1; 
Texture2D flappy1;
Image image2;
Texture2D flappy2;
Image image3;
Texture2D raindropTexture;
Image image4;
Texture2D cloudTexture;

Color sky_c = Color { 85, 111, 122, 255 };
Color cloud_c = Color { 94, 94, 94, 255 };
Color raindrop_c = Color { 177, 230, 54, 255 };

const int screenWidth = 1920;
const int screenHeight = 975;
const int maxRaindrops = 40;
const int maxClouds = 5;
bool pause = false;
float finalTime = 0.0f;
float currentTime = 0.0f;

class Cloud {
public:
    Vector2 position;
    Vector2 size;
    Color color;
    float speed;
};

std::vector<Cloud> clouds;
Cloud cloud;

class Raindrop {
public:
    Vector2 position;
    Color color;
    float speed;

    const Rectangle getCollisionRectangle() const {
        return { position.x, position.y, 1, 1 };
    }
};

std::vector<Raindrop> raindrops;
Raindrop raindrop;

class Bird : public Raindrop {
public:
    float x;
    float y;
    int speedX;
    float radius;

    const void update() {
        if (IsKeyDown(KEY_LEFT)) {
            x -= speedX;
        }
        else if (IsKeyDown(KEY_RIGHT)) {
            x += speedX;
        }

        if (x - radius <= 0) {
            x = radius;
        }
        else if (x + radius >= GetScreenWidth()) {
            x = GetScreenWidth() - radius;
        }
    }

    const void draw() const {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            DrawTextureEx(flappy2, Vector2{ x, y }, 0, -0.3f, WHITE);
        }
        else {
            DrawTextureEx(flappy1, Vector2{ x, y }, 0, -0.3f, WHITE);
        }
    }

    const Rectangle getBirdCollisionRectangle() const {
        return { x - radius, y - radius, radius * 2, radius * 2 };
    }
};

const bool checkCollisionBirdRain(Bird& bird, Raindrop& raindrop) {
    return CheckCollisionRecs(bird.getBirdCollisionRectangle(), raindrop.getCollisionRectangle());
}

Bird bird;

void game() {

    bird.x = screenWidth / 2;
    bird.y = screenHeight - 75;
    bird.speedX = 10;
    bird.radius = 25;

    InitWindow(screenWidth, screenHeight, "Flappy Quiz");

    image1 = LoadImage("sprites/flappy_1.png");
    flappy1 = LoadTextureFromImage(image1);
    UnloadImage(image1);
    image2 = LoadImage("sprites/flappy_2.png");
    flappy2 = LoadTextureFromImage(image2);
    UnloadImage(image2);
    image3 = LoadImage("sprites/raindrop.png");
    raindropTexture = LoadTextureFromImage(image3);
    UnloadImage(image3);
    image4 = LoadImage("sprites/cloud.png");
    cloudTexture = LoadTextureFromImage(image4);
    UnloadImage(image4);

    SetTargetFPS(60);

    for (size_t i = 0; i < maxClouds; i++) {
        cloud.position.x = GetRandomValue(0, screenWidth);
        cloud.position.y = GetRandomValue(0, screenHeight);
        cloud.color = cloud_c;
        cloud.speed = GetRandomValue(40, 50) * 0.1;
        clouds.push_back(cloud);
    }

    for (size_t i = 0; i < maxRaindrops; i++) {
        raindrop.position.x = GetRandomValue(0, screenWidth);
        raindrop.position.y = GetRandomValue(0, screenHeight);
        raindrop.color = raindrop_c;
        raindrop.speed = GetRandomValue(50, 55) * 0.1;
        raindrops.push_back(raindrop);
    }

    std::cout << std::endl;
    std::cout << "FLAPPY QUIZ" << std::endl;
    std::cout << std::endl;
    std::cout << "TEST YOUR KNOWLEDGE!" << std::endl;
    std::cout << "ANSWER A QUESTION TO SAVE YOURSELF!" << std::endl;
    std::cout << "THE QUESTION WILL APPEAR ON THE CONSOLE." << std::endl;

    while (!WindowShouldClose()) {

        BeginDrawing();

        ClearBackground(sky_c);

        DrawFPS(15, 20);

        bird.update();

		currentTime = GetTime();

        if (!pause) {
            for (size_t i = 0; i < clouds.size(); i++) {
                clouds[i].position.y += clouds[i].speed;

                if (clouds[i].position.y > screenHeight) {
                    clouds[i].position.y = 0;
                    clouds[i].position.x = GetRandomValue(0, screenWidth);
                    clouds[i].size.y = GetRandomValue(200, 250);
                    clouds[i].size.x = GetRandomValue(220, 270);
                }
                DrawTextureEx(cloudTexture, clouds[i].position, 0, -0.6f, cloud_c);
            }

            for (size_t i = 0; i < raindrops.size(); i++) {
                raindrops[i].position.y += raindrops[i].speed;

                if (raindrops[i].position.y > screenHeight) {
                    raindrops[i].position.y = 0;
                    raindrops[i].position.x = GetRandomValue(0, screenWidth);
                }

                if (checkCollisionBirdRain(bird, raindrops[i])) {
                    pause = true;
                    raindrops[i].position.y = 0;
                    raindrops[i].position.x = GetRandomValue(0, screenWidth);
                }
                DrawTextureEx(raindropTexture, raindrops[i].position, 0, -0.12f, raindrops[i].color);
            }
            DrawText(TextFormat("%.2f", currentTime), 30, 45, 100, YELLOW);
            finalTime = currentTime;
        }

        if (pause) {

            bool checkAnswer = false;
            question(checkAnswer);

            if (checkAnswer) {
                pause = false;
            }

            else {
                pause = true;
                std::cout << "Your final time is: " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << finalTime << " s." << std::endl;
                break;
            }
        }
        else {
            bird.draw();
        }

        EndDrawing();
    }

    UnloadTexture(flappy1);
    UnloadTexture(flappy2);
    UnloadTexture(raindropTexture);
    UnloadTexture(cloudTexture);

    CloseWindow();
}