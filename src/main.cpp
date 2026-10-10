#include <SFML/Graphics.hpp>
#include <iostream>

#include "config.hpp"

// custom data types to track game state, rather than a variable with a number. less confusion, more better.
enum class GameState {
  MainMenu,
  Playing,
  Paused,
};

class Camera {
  private:
    // Variables
    float x;
    float y;
    float speed = CAMERA_SPEED;
    sf::View view;

  public:
    void init(float width, float height) {
      view.setSize({width, height});
      x = width / 2.f;
      y = height / 2.f;
      view.setCenter({x,y});
    }

    
    void handleInputAndUpdate(float dt) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        y = y - (speed * dt);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        y = y + (speed * dt);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        x = x - (speed * dt);
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        x = x + (speed * dt);
      }

      view.setCenter({x,y});
    }

    sf::View getView() {
      return view;
    }
};

class Player {
  private:
    // Player Variables
    sf::RectangleShape shape;
    bool onGround = false;
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
      onGround = false;

      if (x_velocity != 0.f) {
        x_velocity = x_velocity - (x_velocity/FRICTION) * dt * 60.f; // Proportionately apply friction based on current velocity.
      }

      x = x + x_velocity * dt;
      y = y + y_velocity * dt;

      y_velocity = y_velocity + (GRAVITY * dt * 60.f);
      if (y_velocity > PLAYER_MAX_FALL_SPEED) {
        y_velocity = PLAYER_MAX_FALL_SPEED;
      }

      shape.setPosition({x,y});
    }

    void handleInput() {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && (onGround || PLAYER_INFINITE_JUMP)) {
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

    bool canJump() {
      return onGround;
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
      if (auto intersection = shape.getGlobalBounds().findIntersection(hitbox)) {
        if (y_velocity > 0.f) {
          onGround = true;
          y = hitbox.position.y - shape.getGlobalBounds().size.y;
          y_velocity = 0.f;
        }
        else if (y_velocity < 0.f ) {
          y = hitbox.position.y + hitbox.size.y;
          y_velocity = 0.f;
        }
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

  GameState gameState = GameState::Playing;

  window.setFramerateLimit(FPS_LIMIT);

  // Deltatime Clock init
  sf::Clock deltaClock;
  
  Camera camera;
  camera.init(WINDOW_WIDTH, WINDOW_HEIGHT);
  // init the Player object
  Player square;
  square.init(PLAYER_INITIAL_X, PLAYER_INITIAL_Y, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_COLOR);

  // Create a vector (vectorrr oh yeahhhhh) with the platforms
  std::vector<Platform> platforms;

  // make platforms, add them to the vector platforms as we make them
  Platform platform1;
  platform1.init(100.f, 500.f, 300.f, 30.f, sf::Color::White);
  platforms.push_back(platform1);

  Platform platform2;
  platform2.init(200.f, 300.f, 200.f, 30.f, sf::Color::White);
  platforms.push_back(platform2);

  Platform platform3;
  platform3.init(-100.f, 600.f, 400.f, 30.f, sf::Color::White);
  platforms.push_back(platform3);

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

        if (keyPressed->code == sf::Keyboard::Key::Q) {
          if (gameState == GameState::Playing) {
            gameState = GameState::Paused;
          }
          else if (gameState == GameState::Paused) {
            gameState = GameState::Playing;
          }
        }
      }
    }

    window.clear(BACKGROUND_COLOR);

    if (gameState == GameState::Playing) {
      // Update player, check collisions
      square.handleInput();
      square.update(dt);
      for (auto& platform : platforms) { // for every platform in vector platforms, check the collision
        square.checkCollision(platform.getBounds());
      }

      for (auto& platform : platforms) { // for every platform in the vector platforms, draw it
        window.draw(platform.shape);
      } 

      square.drawPlayer(window);
      camera.handleInputAndUpdate(dt);
    }

    if (gameState == GameState::Paused) {
      // pass
    }

    window.setView(camera.getView()); // Update window to show camera view

    // Draw the window and everything
    window.display();
  }
  return 0;
}
