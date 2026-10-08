#include <SFML/Graphics.hpp>
#include <iostream>

#include "config.hpp"

struct Player {
  sf::RectangleShape shape;
  float x;
  float y;

  // helper functions
  void setPos(float newX, float newY) {
    x = newX;
    y = newY;
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
        std::cout << "Keypress";
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
          window.close();
        }
        if (keyPressed->code == sf::Keyboard::Key::Up) {
          square.setPos(square.x, square.y - PLAYER_SPEED);
        }
        if (keyPressed->code == sf::Keyboard::Key::Down) {
          square.setPos(square.x, square.y + PLAYER_SPEED);
        }
        if (keyPressed->code == sf::Keyboard::Key::Left) {
          square.setPos(square.x - PLAYER_SPEED, square.y); 
        }
        if (keyPressed->code == sf::Keyboard::Key::Right) {
          square.setPos(square.x + PLAYER_SPEED, square.y);
        }
      }
    }

    window.clear(BACKGROUND_COLOR);
    window.draw(square.shape);
    window.display();
  }

  return 0;
}
