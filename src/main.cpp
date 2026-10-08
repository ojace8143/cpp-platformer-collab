#include <SFML/Graphics.hpp>
#include <iostream>

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
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Platformer");
  window.setFramerateLimit(60);

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
          square.setPos(square.x, square.y - 10);
        }
        if (keyPressed->code == sf::Keyboard::Key::Down) {
          square.setPos(square.x, square.y +10);
        }
        if (keyPressed->code == sf::Keyboard::Key::Left) {
          square.setPos(square.x - 10, square.y); 
        }
        if (keyPressed->code == sf::Keyboard::Key::Right) {
          square.setPos(square.x + 10, square.y);
        }
      }
    }

    window.clear(sf::Color::Black);
    window.draw(square.shape);
    window.display();
  }

  return 0;
}
