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

  void updatePos(float x_velocity, float y_velocity) {
    x = x + x_velocity;
    y = y + y_velocity;
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

  // init the Player object
  Player square;
  square.shape.setSize({50.f, 50.f});
  square.shape.setFillColor(sf::Color::Yellow);
  square.setPos(390.f, 200.f);

  while (window.isOpen()) {
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
      // square.setPos(square.x, square.y - PLAYER_SPEED);
      square.y_velocity = -PLAYER_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
      // square.setPos(square.x, square.y + PLAYER_SPEED);
      square.y_velocity = PLAYER_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
      // square.setPos(square.x - PLAYER_SPEED, square.y); 
      square.x_velocity = -PLAYER_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
      // square.setPos(square.x + PLAYER_SPEED, square.y);
      square.x_velocity = PLAYER_SPEED;
    }

    square.updatePos(square.x_velocity, square.y_velocity);

    // Friction and gravity
    if (square.x_velocity > 0) {
      square.x_velocity = square.x_velocity - square.x_velocity/2; // Proportionately apply friction based on current velocity
    }
    if (square.x_velocity < 0) {
      square.x_velocity = square.x_velocity - square.x_velocity*2;
    }
    square.y_velocity = square.y_velocity + 0.1;

    window.clear(BACKGROUND_COLOR);
    window.draw(square.shape);
    window.display();
  }
  return 0;
}
