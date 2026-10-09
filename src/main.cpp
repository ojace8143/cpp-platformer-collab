#include <SFML/Graphics.hpp>
#include <iostream>

#include "config.hpp"

class Player {
  private:
    // Player Variables
    sf::RectangleShape shape;
    float x;
    float y;
    float old_x;
    float old_y;
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
      old_x = x;
      old_y = y;

      if (x_velocity != 0.f) {
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

    void checkCollision(sf::FloatRect hitbox) {
      // Check x axis, against old y. only x is zeroed here, so it wont interfere with y motion.
      shape.setPosition({x, old_y});
      if (shape.getGlobalBounds().findIntersection(hitbox)) {
        x = old_x;
        x_velocity = 0.f;
      }

      // check y axis: uses the corrected x to test the new y. only y is zeroed like x, therefore wall contact still lets you fall.
      shape.setPosition({x, y});
      if (shape.getGlobalBounds().findIntersection(hitbox)) {
        y = old_y;
        y_velocity = 0.f;
      }

      // checking x before y is important here.

      shape.setPosition({x, y});
    } 
};

struct Platform {

  // Platform stuff
  sf::RectangleShape shape;
  float x;
  float y;

  sf::FloatRect bounds = shape.getGlobalBounds();

  sf::FloatRect getBounds() {
    return bounds = shape.getGlobalBounds();
  }

  void setPos(float newX, float newY) {
    x = newX;
    y = newY;
    shape.setPosition({x,y});
  }

  void init(float x, float y, float width, float height, sf::Color color) {
    shape.setPosition({x,y});
    shape.setSize({width, height});
    shape.setFillColor(color);
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
  platform1.init(250.f, 500.f, 300.f, 30.f, sf::Color::White);

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
    square.checkCollision(platform1.getBounds());

    window.clear(BACKGROUND_COLOR);

    window.draw(platform1.shape);
    square.drawPlayer(window);

    window.display();
  }
  return 0;
}
