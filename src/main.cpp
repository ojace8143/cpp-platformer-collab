#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <thread>

#include "config.hpp"

// custom data types to track game state, rather than a variable with a number. less confusion, more better.
enum class GameState {
  MainMenu,
  Playing,
  Paused,
  GameOver,
  SelectedLevelEasy,
  SelectedLevelHard,
  LevelComplete
};

// data types for types of collisions so i dont have to do like 40 booleans for my collide player function
enum class CollisionType {
  Platform,
  Spike,
  LevelSelectorEasy,
  LevelSelectorHard,
  LevelGoal
};

enum class Level {
  Level1,
  Level2,
  LevelSelect
};

class Camera {
  private:
    // Variables
    float x, y;
    float speed = CAMERA_SPEED;
    sf::View view;

  public:
    void init(float width, float height) {
      view.setSize({width, height});
      x = width / 2.f;
      y = height / 2.f;
      view.setCenter({x,y});
    }

    
    void handleInputAndUpdate(float dt, float playerX, float playerY) {
      if (!CAMERA_FOLLOW_PLAYER) {
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
      else if (CAMERA_FOLLOW_PLAYER) {

        float t = dt * CAMERA_SMOOTHING;
        if (t > 1.f) { // clamp so the camera doesent overshoot
          t = 1.f;
        }

        x = x + (playerX - x) * t;
        y = y + (playerY - y) * t;
      
        view.setCenter({x, y});
      }
    }

    sf::View getView() { return view; }
    float    getX()    { return x; }
    float    getY()    { return y; }

    void setCenter(float newX, float newY) {
      x = newX;
      y = newY;
      view.setCenter({x,y});
    }
};

class Player {
  private:
    // Player Variables
    sf::RectangleShape shape;
    bool onGround = false;
    float x, y, old_x, old_y;

    float x_velocity = 0.f;
    float y_velocity = 0.f;

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

    void handleInput(float dt) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && (onGround || PLAYER_INFINITE_JUMP)) {
        y_velocity = -PLAYER_JUMP_SPEED;
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        x_velocity = x_velocity - (PLAYER_SPEED/PLAYER_ACCELERATION) * dt * 60.f;
        if (x_velocity < -PLAYER_SPEED) {
          x_velocity = -PLAYER_SPEED;
        }
      }
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        x_velocity = x_velocity + (PLAYER_SPEED/PLAYER_ACCELERATION) * dt * 60.f;
        if (x_velocity > PLAYER_SPEED) {
          x_velocity = PLAYER_SPEED;
        }
      }
    }

    // get() functions for use in other classes/structs, or just for when i need it
    sf::FloatRect getBounds() { return bounds = shape.getGlobalBounds(); }
    float         getX()      { return x; }
    float         getY()      { return y; }

    void drawPlayer(sf::RenderWindow& window) {
      window.draw(shape);
    }

    void softReset() {
      x_velocity = 0;
      y_velocity = 0;
    }

    bool canJump() {
      return onGround;
    }

    GameState checkCollision(sf::FloatRect hitbox, CollisionType collisionType) {

      if (collisionType == CollisionType::LevelSelectorEasy) {
        if (shape.getGlobalBounds().findIntersection(hitbox)) {
         return GameState::SelectedLevelEasy;
        }  
      } else if (collisionType == CollisionType::LevelSelectorHard) {
        if (shape.getGlobalBounds().findIntersection(hitbox)) {
         return GameState::SelectedLevelHard;
        }
      }

      // Check x axis, against old y. only x is zeroed here, so it wont interfere with y motion.
      shape.setPosition({x, old_y});
      if (shape.getGlobalBounds().findIntersection(hitbox)) {
        x = old_x;
        x_velocity = 0.f;

        if (collisionType == CollisionType::Spike) { // if you're colliding with a spike set game state to game over
          return GameState::GameOver;
        }
        if (collisionType == CollisionType::LevelGoal) {
          return GameState::LevelComplete;
        }
      }

      // check y axis: uses the corrected x to test the new y. only y is zeroed like x, therefore wall contact still lets you fall.
      shape.setPosition({x, y});
      if (auto intersection = shape.getGlobalBounds().findIntersection(hitbox)) {

        if (collisionType == CollisionType::Spike) { // if you're colliding with a spike set game state to game over
          return GameState::GameOver;
        }
        if (collisionType == CollisionType::LevelGoal) {
          return GameState::LevelComplete;
        }

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
      return GameState::Playing;
    } 
};

class LevelGoal {
  private:
    float x, y;
    sf::Texture texture;
    sf::Sprite sprite{texture};
    sf::FloatRect bounds;

  public:
    void init(float startX, float startY) {

      if (!texture.loadFromFile("assets/sprites/end_flag.png")) {
        std::cout << "error: no sprite asset found";
      }

      sprite.setTexture(texture, true);

      x = startX;
      y = startY;

      sprite.setPosition({x,y});

      bounds = sprite.getGlobalBounds();
    }

    void drawGoal(sf::RenderWindow& window) {
      window.draw(sprite);
    }

    sf::FloatRect getBounds() {
      return bounds = sprite.getGlobalBounds();
    }
};

struct LevelSelector {
  float x, y;
  sf::RectangleShape shape;
  sf::Color color;

  sf::FloatRect bounds = shape.getGlobalBounds();

  void init(float startX, float startY) {
    x = startX;
    y = startY;

    shape.setPosition({x, y});
    shape.setSize({35.f, 35.f});
    shape.setFillColor(sf::Color::Cyan);
  }

  sf::FloatRect getBounds() {
    return bounds = shape.getGlobalBounds();
  }

  Level setLevel(Level level) {
    return level;
  }
};

struct Platform {

  // Platform stuff
  sf::RectangleShape shape;
  sf::Color color;
  float x, y, width, height;

  sf::FloatRect bounds = shape.getGlobalBounds();

  sf::FloatRect getBounds() {
    return bounds = shape.getGlobalBounds();
  }

  void setPos(float newX, float newY) {
    x = newX;
    y = newY;
    shape.setPosition({x,y});
  }

  void init(float startX, float startY, float startWidth, float startHeight, sf::Color startColor) {
    x = startX;
    y = startY;
    width = startWidth;
    height = startHeight;
    color = startColor;

    shape.setPosition({x,y});
    shape.setSize({width, height});
    shape.setFillColor(color);
  }
};

struct Spike {

  // Variables
  sf::CircleShape shape; // We make a CircleShape, but give it 3 points to make it a triangle
  sf::Color color;
  float x, y, size, rotation; // x/y is the CENTER of the spike, rotation is in degrees

  sf::FloatRect bounds = shape.getGlobalBounds();

  sf::FloatRect getBounds() {
    // for gd styled spikes: we just shrink the hitbox to a little square inside of the actual spike.
    float side = SPIKE_HITBOX_SCALE * 2.f * size;

    // Ask the shape where its middle ended up in the world, so the hitbox follows the rotation.
    // A square is symmetric every 90 degrees, so this stays exact for floor/wall/ceiling spikes.
    sf::Vector2f center = shape.getTransform().transformPoint(shape.getGeometricCenter());

    return bounds = sf::FloatRect({center.x - side / 2.f, center.y - side / 2.f}, {side, side});
  }

  void setPos(float newX, float newY) {
    x = newX;
    y = newY;
    shape.setPosition({x,y});
  }

  // x, y = center of the spike. rotation in degrees: 0 = points up, 90 = points right, 180 = points down, 270 = points left
  void init(float startX, float startY, float startSize, float startRotation, sf::Color startColor) {
    x = startX;
    y = startY;
    color = startColor;
    size = startSize;
    rotation = startRotation;

    shape.setPointCount(3);
    shape.setRadius(size);
    shape.setOrigin(shape.getGeometricCenter()); // rotate around the middle of the spike, not its corner
    shape.setPosition({x,y});
    shape.setRotation(sf::degrees(rotation));
    shape.setFillColor(color);
  }
};

struct Text {
  
  // Variables
  float x, y;
  sf::Font font;
  sf::Text* text = nullptr;

  void init(float startX, float startY, const std::string& startText, sf::Color startColor) {
    x = startX;
    y = startY;

    // set up the text
    if (!font.openFromFile(TEXT_FONT_PATH)) {
      std::cout << "ERROR: could not load font file;\n";
    }

    text = new sf::Text(font, startText, 24);
    text->setFillColor(startColor);
    text->setPosition({x,y});
  }
  
  void draw(sf::RenderWindow& window) {
    if (text != nullptr) {
      window.draw(*text);
    }
  }

  void setPos(float newX, float newY) {
    x = newX;
    y = newY;
    text->setPosition({x,y});
  }
};


int main() {
  sf::RenderWindow window(
    sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), 
    WINDOW_TITLE,
    sf::Style::Titlebar | sf::Style::Close
  );

  // GameState gameState = GameState::Playing;
  GameState gameState = GameState::MainMenu;
  Level level = Level::LevelSelect;

  if (USE_VSYNC) {
    window.setVerticalSyncEnabled(true);
  } else {
    window.setFramerateLimit(FPS_LIMIT);
  }

  // Deltatime Clock init
  sf::Clock deltaClock;
 
  Camera camera;
  camera.init(WINDOW_WIDTH, WINDOW_HEIGHT);
  // init the Player object
  Player square;
  square.init(PLAYER_INITIAL_X, PLAYER_INITIAL_Y, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_COLOR);

  // Create a vector (vectorrr oh yeahhhhh) with the platforms
  std::vector<Platform> level1Platforms;
  std::vector<Platform> level2Platforms;

  std::vector<Spike>    level1Spikes;
  std::vector<Spike>    level2Spikes;

  std::vector<Text>     mainMenuText;
  std::vector<Text>     levelSelectText;

  // Level 1
  Platform level1Platform1;
  level1Platform1.init(150.f, 500.f, 300.f, 30.f, sf::Color::White);
  level1Platforms.push_back(level1Platform1);

  Platform level1Platform2;
  level1Platform2.init(200.f, 700.f, 200.f, 30.f, sf::Color::White);
  level1Platforms.push_back(level1Platform2);

  Platform level1Platform3;
  level1Platform3.init(-100.f, 600.f, 400.f, 30.f, sf::Color::White);
  level1Platforms.push_back(level1Platform3);

  Platform level1Platform4;
  level1Platform4.init(-600.f, 700.f, 500.f, 50.f, sf::Color::White);
  level1Platforms.push_back(level1Platform4);

  Platform level1Platform5;
  level1Platform5.init(550.f, 600.f, 700.f, 30.f, sf::Color::White);
  level1Platforms.push_back(level1Platform5);

  Platform level1Platform6;
  level1Platform6.init(200.f, 900.f, 500.f, 30.f, sf::Color::White);
  level1Platforms.push_back(level1Platform6);

  Platform level1Platform7;
  level1Platform7.init(500.f, 800.f, 300.f, 30.f, sf::Color::White);
  level1Platforms.push_back(level1Platform7);

  // spikes
  Spike level1Spike1;
  level1Spike1.init(60.f, 583.f, 35.f, 0.f, sf::Color::Red);
  level1Spikes.push_back(level1Spike1);

  Spike level1Spike2;
  level1Spike2.init(285.f, 685.f, 35.f, 0.f, sf::Color::Red);
  level1Spikes.push_back(level1Spike2);

  // level 2
  constexpr float L2_SCALE = 1.62f;
  constexpr float L2_Y0    = 400.f; // the world y that stays put while scaling
  auto l2x = [](float v) { return v * L2_SCALE; };                    // sketch x -> world x
  auto l2y = [](float v) { return L2_Y0 + (v - L2_Y0) * L2_SCALE; };  // sketch y -> world y
  auto l2s = [](float v) { return v * L2_SCALE; };                    // sketch width/height/spike size -> world

  Platform level2Platform1;
  level2Platform1.init(l2x(0.f), l2y(481.f), l2s(102.f), l2s(22.f), sf::Color::White);
  level2Platforms.push_back(level2Platform1);

  Platform level2Platform2;
  level2Platform2.init(l2x(131.f), l2y(519.f), l2s(125.f), l2s(22.f), sf::Color::White);
  level2Platforms.push_back(level2Platform2);

  Platform level2Platform3; // roof over platform 4
  level2Platform3.init(l2x(302.f), l2y(432.f), l2s(250.f), l2s(22.f), sf::Color::White);
  level2Platforms.push_back(level2Platform3);

  Platform level2Platform4;
  level2Platform4.init(l2x(305.f), l2y(515.f), l2s(253.f), l2s(22.f), sf::Color::White);
  level2Platforms.push_back(level2Platform4);

  Platform level2Platform5;
  level2Platform5.init(l2x(582.f), l2y(508.f), l2s(105.f), l2s(38.f), sf::Color::White);
  level2Platforms.push_back(level2Platform5);

  Platform level2Platform6;
  level2Platform6.init(l2x(772.f), l2y(509.f), l2s(41.f), l2s(28.f), sf::Color::White);
  level2Platforms.push_back(level2Platform6);

  Platform level2Platform7;
  level2Platform7.init(l2x(878.f), l2y(483.f), l2s(16.f), l2s(26.f), sf::Color::White);
  level2Platforms.push_back(level2Platform7);

  Platform level2Platform8; // left tall bar
  level2Platform8.init(l2x(965.f), l2y(484.f), l2s(19.f), l2s(177.f), sf::Color::White);
  level2Platforms.push_back(level2Platform8);

  Platform level2Platform9; // right tall bar
  level2Platform9.init(l2x(1022.f), l2y(484.f), l2s(20.f), l2s(177.f), sf::Color::White);
  level2Platforms.push_back(level2Platform9);

  Platform level2Platform10;
  level2Platform10.init(l2x(1318.f), l2y(522.f), l2s(29.f), l2s(49.f), sf::Color::White);
  level2Platforms.push_back(level2Platform10);

  Platform level2Platform11;
  level2Platform11.init(l2x(1325.f), l2y(637.f), l2s(13.f), l2s(53.f), sf::Color::White);
  level2Platforms.push_back(level2Platform11);

  Platform level2Platform12;
  level2Platform12.init(l2x(1142.f), l2y(659.f), l2s(158.f), l2s(24.f), sf::Color::White);
  level2Platforms.push_back(level2Platform12);

  Platform level2Platform13;
  level2Platform13.init(l2x(957.f), l2y(746.f) - 30.f, l2s(154.f), l2s(24.f), sf::Color::White);
  level2Platforms.push_back(level2Platform13);

  Platform level2Platform14; // final platform, holds the goal
  level2Platform14.init(l2x(1353.f), l2y(703.f), l2s(90.f), l2s(11.f), sf::Color::White);
  level2Platforms.push_back(level2Platform14);

  // level 2 spikes
  Spike level2Spike1; // hangs under platform 3
  level2Spike1.init(l2x(442.f), l2y(464.f), l2s(20.f), 180.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike1);

  Spike level2Spike2; // on platform 4, left
  level2Spike2.init(l2x(321.f), l2y(505.f), l2s(20.f), 0.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike2);

  Spike level2Spike3; // on platform 4, right
  level2Spike3.init(l2x(535.f), l2y(505.f), l2s(20.f), 0.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike3);

  Spike level2Spike4; // in the gap between the two tall platforms, pointing right
  level2Spike4.init(l2x(990.f), l2y(504.f), l2s(12.f), 90.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike4);

  Spike level2Spike5; // in the gap, pointing left
  level2Spike5.init(l2x(1016.f), l2y(554.f), l2s(12.f), 270.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike5);

  Spike level2Spike6; // in the gap, pointing right
  level2Spike6.init(l2x(990.f), l2y(624.f), l2s(12.f), 90.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike6);

  Spike level2Spike7; // hangs under the right bar
  level2Spike7.init(l2x(1333.f), l2y(581.f), l2s(20.f), 180.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike7);

  // some spikes removed for balance

  Spike level2Spike9; // platform 12, left
  level2Spike9.init(l2x(1162.f), l2y(652.f), l2s(14.f), 0.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike9);

  Spike level2Spike10; // platform 12, middle
  level2Spike10.init(l2x(1221.f), l2y(652.f), l2s(14.f), 0.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike10);

  Spike level2Spike13;
  level2Spike13.init(l2x(1386.f), l2y(697.f), l2s(12.f), 0.f, sf::Color::Red);
  level2Spikes.push_back(level2Spike13);

  // Level for selecting level or something like that
  Platform levelSelectPlatform1;
  levelSelectPlatform1.init(0.f, 0.f, 500.f, 40.f, sf::Color::White);

  LevelSelector selectLevelHard;
  selectLevelHard.init(75.f, -100.f);

  LevelSelector selectLevelEasy;
  selectLevelEasy.init(350.f, -100.f);

    // the text objects
  Text levelSelectInstructions;
  levelSelectInstructions.init(105.f, -250.f, "CHOOSE A LEVEL", sf::Color::White);
  levelSelectText.push_back(levelSelectInstructions);

  Text levelSelectHard;
  levelSelectHard.init(55.f, -150.f, "HARD", sf::Color::Red);
  levelSelectText.push_back(levelSelectHard);

  Text levelSelectEasy;
  levelSelectEasy.init(325.f, -150.f, "EASY", sf::Color::Green);
  levelSelectText.push_back(levelSelectEasy);

  //

  // Game state text
  Text pauseText;
  pauseText.init(300.f, 400.f, "PAUSED", sf::Color::White);
  
  Text gameOverText;
  gameOverText.init(300.f, 400.f, "LOL UR BAD", sf::Color::Red);

  Text gameOverPlayAgainText;
  gameOverPlayAgainText.init(-50.f, 500.f, "PRESS Q TO PLAY AGAIN!", sf::Color::White);

  Text levelCompleteText;
  levelCompleteText.init(300.f, 400.f, "YOU FOUND THE GOAL", sf::Color::Green);


  // Main Menu objects
  Text mainMenuTitle;
  mainMenuTitle.init(300.f, 200.f, "CPP PLATFORMER", sf::Color::White);
  mainMenuText.push_back(mainMenuTitle);

  Text mainMenuInstructions;
  mainMenuInstructions.init(25.f, 500.f, "Press Q to play/pause, arrows to move.", sf::Color::Green);
  mainMenuText.push_back(mainMenuInstructions);

  Text mainMenuCredits;
  mainMenuCredits.init(300.f, 250.f, "By Jace and Toby", sf::Color::Red);
  mainMenuText.push_back(mainMenuCredits);

  // make level goal objects
  LevelGoal level1Goal;
  level1Goal.init(900.f, 575.f);

  LevelGoal level2Goal; // floats above the final platform, as drawn in the sketch
  // dropped 32px from the sketch height so a plain walk-off the end of platform 12 touches the
  // flag instead of dropping into the spike carpet below it
  level2Goal.init(l2x(1386.f), l2y(632.f) + 32.f);

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
          } else if (gameState == GameState::Paused) {
            gameState = GameState::Playing;
          } else if (gameState == GameState::MainMenu) {
            gameState = GameState::Playing;
          } else if (gameState == GameState::GameOver || gameState == GameState::LevelComplete) {
            square.softReset();
            square.setPos(PLAYER_INITIAL_X, PLAYER_INITIAL_Y);
            level = Level::LevelSelect;
            gameState = GameState::Playing;
          }
        }
      }
    }

    window.clear(BACKGROUND_COLOR);

    if (gameState == GameState::MainMenu) {
      for (auto& text : mainMenuText) {
        text.draw(window);
      }
    }

    if (gameState == GameState::Playing) {
      // Update player, check collisions
      square.handleInput(dt);
      square.update(dt);
      if (level == Level::Level1) {
        for (auto& platform : level1Platforms) { // for every platform in vector level1Platforms, check the collision
          square.checkCollision(platform.getBounds(), CollisionType::Platform);
        }
        for (auto& spike : level1Spikes) { // Check all collision with spikes in the level1Spikes vector
          GameState s = square.checkCollision(spike.getBounds(), CollisionType::Spike);
          if (s != GameState::Playing) {
            gameState = s;
            break;
          }
        }

        if (gameState == GameState::Playing) { // only check the goal if still playing, otherwise this overwrites a spike's GameOver. we cant have it just in there, because the state can change after we check spike collision and that can break stuff.
          gameState = square.checkCollision(level1Goal.getBounds(), CollisionType::LevelGoal);
        }

        for (auto& platform : level1Platforms) { // for every platform in the level1Platforms vector, draw it
          window.draw(platform.shape);
        } 
        for (auto& spike : level1Spikes) {
          window.draw(spike.shape);

          if (SHOW_HITBOXES) {
            sf::FloatRect hitbox = spike.getBounds();
            sf::RectangleShape debug(hitbox.size);
            debug.setPosition(hitbox.position);
            debug.setFillColor(sf::Color::Transparent); // so its an outline and not a green box
            debug.setOutlineColor(sf::Color::Green); // would be red but the spike is red darn
            debug.setOutlineThickness(1.f);
            window.draw(debug);
          }
        }

        level1Goal.drawGoal(window);
      } else if (level == Level::Level2) {
         for (auto& platform : level2Platforms) { // for every platform in vector level1Platforms, check the collision
          square.checkCollision(platform.getBounds(), CollisionType::Platform);
        }
        for (auto& spike : level2Spikes) { // Check all collision with spikes in the level1Spikes vector
          GameState s = square.checkCollision(spike.getBounds(), CollisionType::Spike);
          if (s != GameState::Playing) {
            gameState = s;
            break;
          }
        }

        if (gameState == GameState::Playing) { // only check the goal if still playing, otherwise this overwrites a spike's GameOver. we cant have it just in there, because the state can change after we check spike collision and that can break stuff.
          gameState = square.checkCollision(level2Goal.getBounds(), CollisionType::LevelGoal);
        }

        for (auto& platform : level2Platforms) { // for every platform in the level1Platforms vector, draw it
          window.draw(platform.shape);
        } 
        for (auto& spike : level2Spikes) {
          window.draw(spike.shape);

          if (SHOW_HITBOXES) {
            sf::FloatRect hitbox = spike.getBounds();
            sf::RectangleShape debug(hitbox.size);
            debug.setPosition(hitbox.position);
            debug.setFillColor(sf::Color::Transparent);
            debug.setOutlineColor(sf::Color::Green);
            debug.setOutlineThickness(1.f);
            window.draw(debug);
          }
        }   

        level2Goal.drawGoal(window);
      } else if (level == Level::LevelSelect) {
        square.checkCollision(levelSelectPlatform1.getBounds(), CollisionType::Platform);
        window.draw(levelSelectPlatform1.shape);

        window.draw(selectLevelEasy.shape);
        window.draw(selectLevelHard.shape);

        for (auto& text : levelSelectText) {
          text.draw(window);
        }

        GameState s;

        s = square.checkCollision(selectLevelEasy.getBounds(), CollisionType::LevelSelectorEasy);
        if (s == GameState::SelectedLevelEasy) { 
          level = Level::Level1; 
          square.setPos(200.f, 450.f); 
          camera.setCenter(200.f, 450.f);
        }

        s = square.checkCollision(selectLevelHard.getBounds(), CollisionType::LevelSelectorHard);
        if (s == GameState::SelectedLevelHard) { 
          level = Level::Level2; 
          square.setPos(l2x(50.f), l2y(440.f)); // drop onto the first platform of the hard level
          camera.setCenter(l2x(50.f), l2y(440.f));
        }
      }

      // kill player if they fall out of world
      if (square.getY() > PLAYER_FALL_DEATH_Y) {
        gameState = GameState::GameOver;
      }

      square.drawPlayer(window);
      camera.handleInputAndUpdate(dt, square.getX(), square.getY());
    }

    if (gameState == GameState::Paused) {
      pauseText.setPos(camera.getX(), camera.getY());
      pauseText.draw(window);
    }

    if (gameState == GameState::LevelComplete) {
      levelCompleteText.setPos(camera.getX(), camera.getY());
      levelCompleteText.draw(window);
      gameOverPlayAgainText.setPos(camera.getX() - 200, camera.getY() + 100);
      gameOverPlayAgainText.draw(window);
    }

    if (gameState == GameState::GameOver) {
      gameOverText.setPos(camera.getX(), camera.getY());
      gameOverPlayAgainText.setPos(camera.getX() - 200, camera.getY() + 100);
      gameOverText.draw(window);
      gameOverPlayAgainText.draw(window);
    }

    window.setView(camera.getView()); // Update window to show camera view

    // Draw the window and everything
    window.display();
  }
  return 0;
}
