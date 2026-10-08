#pragma once

#include <SFML/Graphics.hpp>

// Window
#define FPS_LIMIT 60
#define WINDOW_TITLE  "Platformer"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

// Player
#define PLAYER_SPEED 10.f
#define PLAYER_JUMP_SPEED 15.f

// Globals
#define GRAVITY 1.5f   // Hihger the number, higher the gravity
#define FRICTION 3.f  // Higher the number, lower the friction. Friction is: velocity - velocity/friction

// Colors
#define BACKGROUND_COLOR sf::Color::Black
