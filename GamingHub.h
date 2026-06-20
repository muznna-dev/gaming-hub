#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// screen constants
const int SCREEN_MENU = 0;
const int SCREEN_TICTACTOE = 1;
const int SCREEN_PONG = 2;
const int SCREEN_CARDS = 3;
const int SCREEN_LEADERBOARD = 4;

// menubutton struct
struct MenuButton
{
    sf::Sprite  sprite;
    sf::Text    text;
    std::string label;
    int targetScreen;
};

const int MAX_MENU_BUTTONS = 5;

class GamingHub
{
private:
    sf::RenderWindow window;
    int currentScreen;

    sf::Font    font;
    sf::Texture backgroundTexture;
    sf::Sprite  backgroundSprite;
    sf::Texture buttonTexture;
    sf::Text    titleText;

    MenuButton  menuButtons[MAX_MENU_BUTTONS];
    int buttonCount;

    void loadAssets();
    void setupBackground();
    void setupTitle();
    void setupMenu();

    void centerTextInRect(sf::Text& text, float rx, float ry, float rw, float rh);
    void updateHoverEffects();
    void handleMouseClick(const sf::Vector2f& mousePos);
    void handleEvents();
    void drawMenu();
    void drawPlaceholderScreen(const std::string& screenName);

public:
    GamingHub();
    ~GamingHub();
    void run();
};