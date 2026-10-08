#include <SFML/Graphics.hpp>
#include <iostream>

#include "config.hpp"

struct Player {

  // Player Variables
  sf::RectangleShape shape;
  float x;
  float y;
  float x_velocity;
  float y_velocity;

  // helper functions
  void setPos(float newX, float newY) {
    x = newX;
    y = newY;
    shape.setPosition({x,y});
  }

  void updatePos(float xv, float yv, dt) {
    x = x + x_velocity * dt;
    y = y + y_velocity * dt;
    shape.setPosition({x,y});
  }
};

int main() {
  sf::RenderWindow window(
    sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), 
    WINDOW_TITLE,
    sf::Style::Titlebar | sf::Style::Close
  );

  window.setFramerateLimit(FPS_LIMIT);

  // Deltatime Clock init
  sf::Clock deltaClock;
  
  // init the Player object
  Player square;
  square.shape.setSize({50.f, 50.f});
  square.shape.setFillColor(sf::Color::Yellow);
  square.setPos(390.f, 200.f);

  while (window.isOpen()) {

    float dt = deltaClock.restart().asSeconds();

    while (auto event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          window.close();
        }
      }
    }

    if (sf::Keyboard::KeyPressed(sf::Keyboard::Key::Up)) {
      // square.setPos(square.x, square.y - PLAYER_SPEED);
      square.y_velocity = -PLAYER_JUMP_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
      // square.setPos(square.x - PLAYER_SPEED, square.y);
      square.x_velocity = -PLAYER_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
      // square.setPos(square.x + PLAYER_SPEED, square.y);
      square.x_velocity = PLAYER_SPEED;
    }

    // Update player position each frame
    square.updatePos(square.x_velocity, square.y_velocity, dt);

    // Friction and gravity
    if (square.x_velocity > 0) {
      square.x_velocity = square.x_velocity - square.x_velocity/FRICTION; // Proportionately apply friction based on current velocity
    }
    if (square.x_velocity < 0) {
      square.x_velocity = square.x_velocity - square.x_velocity/FRICTION;
    }
    square.y_velocity = square.y_velocity + GRAVITY;

    window.clear(BACKGROUND_COLOR);
    window.draw(square.shape);
    window.display();
  }
  return 0;
}
