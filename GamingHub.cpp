#include "GamingHub.h"
#include "TicTacToe.h"
#include "Cards.h"
#include "Pong.h"
#include "FileManager.h"
#include <iostream>

GamingHub::GamingHub()
    : window(sf::VideoMode(1200, 700), "Gaming Hub"), currentScreen(SCREEN_MENU), buttonCount(0)
{
    window.setFramerateLimit(60);
    loadAssets();
    setupBackground();
    setupTitle();
    setupMenu();
}

// GamingHub destructor
GamingHub::~GamingHub()
{
    // window is closed via window.close() in the run loop
}


void GamingHub::loadAssets()
{
    if (!font.loadFromFile("assets/fonts/pixel.ttf"))
        std::cout << "Error: could not load font\n";

    if (!backgroundTexture.loadFromFile("assets/images/hub_bg.jpg"))
        std::cout << "Error: could not load background\n";

    if (!buttonTexture.loadFromFile("assets/images/wood_button.png"))
        std::cout << "Error: could not load button texture\n";

    backgroundTexture.setSmooth(true);
    buttonTexture.setSmooth(false);
}

void GamingHub::setupBackground()
{
    backgroundSprite.setTexture(backgroundTexture);
    if (backgroundTexture.getSize().x > 0 && backgroundTexture.getSize().y > 0)
    {
        backgroundSprite.setScale(1200.0f / backgroundTexture.getSize().x, 700.0f / backgroundTexture.getSize().y
        );
    }
}

void GamingHub::setupTitle()
{
    titleText.setFont(font);
    titleText.setString("GAMING HUB");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color(255, 220, 100));
    titleText.setOutlineColor(sf::Color(60, 30, 5));
    titleText.setOutlineThickness(5.0f);

    sf::FloatRect b = titleText.getLocalBounds();
    titleText.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    titleText.setPosition(600.f, 65.f);
}

void GamingHub::centerTextInRect(sf::Text& text, float rx, float ry, float rw, float rh)
{
    sf::FloatRect tb = text.getLocalBounds();
    text.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    text.setPosition(rx + rw / 2.f, ry + rh / 2.f);
}

void GamingHub::setupMenu()
{
    buttonCount = 0;

    const float BOX_W = 480.f;
    const float BOX_H = 80.f;
    const float GAP_Y = 35.f;
    const float START_X = (1200.f - BOX_W) / 2.f;
    const float START_Y = 130.f;

    // Button data — labels and target screens
    const int TOTAL = 4;
    std::string labels[TOTAL] = { "TIC TAC TOE", "PONG", "MATCH CARDS", "EXIT" };
    int screens[TOTAL] = { SCREEN_TICTACTOE, SCREEN_PONG, SCREEN_CARDS, SCREEN_MENU };

    sf::Vector2u ts = buttonTexture.getSize();

    for (int i = 0; i < TOTAL; i++)
    {
        float px = START_X;
        float py = START_Y + i * (BOX_H + GAP_Y);

        menuButtons[i].label = labels[i];
        menuButtons[i].targetScreen = screens[i];

        menuButtons[i].sprite.setTexture(buttonTexture);
        if (ts.x > 0 && ts.y > 0)
            menuButtons[i].sprite.setScale(BOX_W / ts.x, BOX_H / ts.y);
        menuButtons[i].sprite.setPosition(px, py);

        menuButtons[i].text.setFont(font);
        menuButtons[i].text.setString(labels[i]);
        menuButtons[i].text.setCharacterSize(32);
        menuButtons[i].text.setLetterSpacing(2.5f);
        menuButtons[i].text.setFillColor(sf::Color(255, 235, 180));
        menuButtons[i].text.setOutlineColor(sf::Color(40, 20, 5));
        menuButtons[i].text.setOutlineThickness(2.f);

        centerTextInRect(menuButtons[i].text, px, py, BOX_W, BOX_H);
    }

    buttonCount = TOTAL;
}

void GamingHub::updateHoverEffects()
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    for (int i = 0; i < buttonCount; i++)
    {
        if (menuButtons[i].sprite.getGlobalBounds().contains(mousePos))
        {
            menuButtons[i].sprite.setColor(sf::Color(255, 230, 160));
            menuButtons[i].text.setFillColor(sf::Color(255, 255, 210));
        }
        else
        {
            menuButtons[i].sprite.setColor(sf::Color::White);
            menuButtons[i].text.setFillColor(sf::Color(255, 235, 180));
        }
    }
}

void GamingHub::handleMouseClick(const sf::Vector2f& mousePos)
{
    for (int i = 0; i < buttonCount; i++)
    {
        if (menuButtons[i].sprite.getGlobalBounds().contains(mousePos))
        {
            if (menuButtons[i].label == "EXIT")
            {
                window.close();
                return;
            }
            currentScreen = menuButtons[i].targetScreen;
            return;
        }
    }
}

void GamingHub::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();

        if (currentScreen == SCREEN_MENU)
        {
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mp = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                handleMouseClick(mp);
            }
        }
        else
        {
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
                currentScreen = SCREEN_MENU;
        }
    }
}

void GamingHub::drawMenu()
{
    window.clear();
    window.draw(backgroundSprite);
    window.draw(titleText);

    for (int i = 0; i < buttonCount; i++)
    {
        window.draw(menuButtons[i].sprite);
        window.draw(menuButtons[i].text);
    }

    window.display();
}

void GamingHub::drawPlaceholderScreen(const std::string& screenName)
{
    window.clear(sf::Color(15, 15, 30));

    sf::Text screenText;
    screenText.setFont(font);
    screenText.setCharacterSize(32);
    screenText.setFillColor(sf::Color(255, 220, 100));
    screenText.setOutlineColor(sf::Color(40, 20, 5));
    screenText.setOutlineThickness(3.f);
    screenText.setString(screenName + "\n\nCOMING SOON\n\nPress ESC to return");

    sf::FloatRect b = screenText.getLocalBounds();
    screenText.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    screenText.setPosition(600.f, 350.f);

    window.draw(screenText);
    window.display();
}

void GamingHub::run()
{
    while (window.isOpen())
    {
        // tictactoe
        if (currentScreen == SCREEN_TICTACTOE)
        {
            TicTacToe ticTacToe;
            ticTacToe.run(window);
            if (!window.isOpen()) break;
            currentScreen = SCREEN_MENU;
            continue;
        }

        // leaderboard
        if (currentScreen == SCREEN_LEADERBOARD)
        {
            FileManager lb;
            lb.run(window, "");
            if (!window.isOpen()) break;
            currentScreen = SCREEN_MENU;
            continue;
        }

        // pong
        if (currentScreen == SCREEN_PONG)
        {
            Pong pong;
            pong.run(window);
            if (!window.isOpen()) break;
            currentScreen = SCREEN_MENU;
            continue;
        }

        // cards
        else if (currentScreen == SCREEN_CARDS)
        {
            Cards cardsGame;
            cardsGame.run(window);
            if (!window.isOpen()) break;
            currentScreen = SCREEN_MENU;
            continue;
        }

        // menu
        else
        {
            handleEvents();
            if (!window.isOpen()) break;
            updateHoverEffects();
            drawMenu();
        }
    }
}