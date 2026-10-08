#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Platformer");
  window.setFramerateLimit(60);

  // Make circle object
  sf::CircleShape shape(50.f);
  shape.setFillColor(sf::Color::Green);
  shape.setPosition({375.f, 275.f});

  // Make a regtangle object
  sf::RectangleShape shape2({50.f, 50.f});
  shape2.setFillColor(sf::Color::Yellow);
  shape2.setPosition({390.f, 200.f});

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
        if (keyPressed->code == sf::Keyboard::Key::Space) {
          shape2.setPosition({400.f, 200.f});
        }
      }
    }

    window.clear(sf::Color::Black);
    window.draw(shape);
    window.draw(shape2);
    window.display();
  }

  return 0;
}
