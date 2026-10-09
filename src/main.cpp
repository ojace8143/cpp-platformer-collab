#include <SFML/Graphics.hpp>
#include <iostream>

#include "config.hpp"

class Player {
  private:
    // Player Variables
    sf::RectangleShape shape;
    float x;
    float y;
    float x_velocity;
    float y_velocity;

    sf::FloatRect bounds = shape.getGlobalBounds();

  public:
    // helper functions
    void init(float startX, float startY, float width, float height, sf::Color color) {
      x = startX;
      y = startY;
      shape.setSize({width, height});
      shape.setFillColor(color);
      shape.setPosition({x,y});
    }

    void setPos(float newX, float newY) {
      x = newX;
      y = newY;
      shape.setPosition({x,y});
    }

    void update(float dt) {
      
      if (x != 0.f) {
        x_velocity = x_velocity - (x_velocity/FRICTION); // Proportionately apply friction based on current velocity
      }

      x = x + x_velocity * dt;
      y = y + y_velocity * dt;

      y_velocity = y_velocity + GRAVITY;

      shape.setPosition({x,y});
    }

    void handleInput() {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        y_velocity = -PLAYER_JUMP_SPEED;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        x_velocity = -PLAYER_SPEED;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        x_velocity = PLAYER_SPEED;
      }
    }

    sf::FloatRect getBounds() {
      return bounds = shape.getGlobalBounds();
    }
    
    void drawPlayer(sf::RenderWindow& window) {
      window.draw(shape);
    }
};

struct Platform {

  // Platform stuff
  sf::RectangleShape shape;
  float x;
  float y;

  void setPos(float newX, float newY) {
    x = newX;
    y = newY;
    shape.setPosition({x,y});
  }

  void init(float x, float y, float width, float height) {
    shape.setPosition({x,y});
    shape.setSize({width, height});
    shape.setFillColor(sf::Color::White);
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
  square.init(0.f, 0.f, 50.f, 50.f, sf::Color::Yellow);

  // make a platform
  Platform platform1;
  platform1.init(250.f, 500.f, 300.f, 30.f);

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

    // Update player
    square.handleInput();
    square.update(dt);

    window.clear(BACKGROUND_COLOR);

    window.draw(platform1.shape);
    square.drawPlayer(window);

    window.display();
  }
  return 0;
}
