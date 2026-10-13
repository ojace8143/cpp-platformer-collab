#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

// Window
#define FPS_LIMIT 60
#define USE_VSYNC true
#define WINDOW_TITLE  "Platformer"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

// Player
#define PLAYER_SPEED 300.f
#define PLAYER_JUMP_SPEED 500.f
#define PLAYER_MAX_FALL_SPEED 500.f

#define PLAYER_INITIAL_X 400.f
#define PLAYER_INITIAL_Y 50.f

#define PLAYER_WIDTH 50.f
#define PLAYER_HEIGHT 50.f

#define PLAYER_COLOR sf::Color::Yellow

#define PLAYER_INFINITE_JUMP false

// Camera
#define CAMERA_SPEED 400.f
#define CAMERA_FOLLOW_PLAYER true
#define CAMERA_SMOOTHING 3.f

// Globals
#define GRAVITY  12.f  // Hihger the number, higher the gravity
#define FRICTION 11.f  // Higher the number, lower the friction. Friction is velocity - velocity/friction

// Colors
#define BACKGROUND_COLOR sf::Color::Black

// Assets
#define TEXT_FONT_PATH "assets/fonts/Silkscreen-Bold.ttf"
#define END_FLAG_PATH "assets/sprites/end_flag.png"
