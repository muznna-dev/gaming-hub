#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

// difficulty constants
const int DIFF_EASY = 0;
const int DIFF_MEDIUM = 1;
const int DIFF_HARD = 2;

class Pong
{
private:
    // game flow flags
    bool gameStarted;
    bool gameOver;
    bool returnToMenu;
    bool ballLaunched;

    // name input focus
    bool typingP1;
    bool typingP2;

    // difficulty
    int difficulty;

    // player data
    std::string player1;
    std::string player2;
    std::string winnerName;

    sf::SoundBuffer winBuffer;
    sf::Sound       winSound;
    int score1;
    int score2;
    int winScore;   // 5, 7, or 10 depending on difficulty

    // ball
    sf::RectangleShape ball;
    float ballVX;
    float ballVY;
    float ballSpeed;

    // paddles
    sf::RectangleShape paddle1;   // left  — P1
    sf::RectangleShape paddle2;   // right — P2
    float paddleSpeed;

    // court
    sf::RectangleShape centerLine;  // dashed centre divider (drawn as rects)
    sf::RectangleShape dashRects[20];
    int dashCount;

    // assets
    sf::Font    font;
    sf::Texture backgroundTexture;
    sf::Sprite  backgroundSprite;

    // setup screen ui
    sf::Text titleText;
    sf::Text subtitleText;
    sf::Text player1Label;
    sf::Text player2Label;
    sf::Text infoText;

    sf::RectangleShape player1Box;
    sf::RectangleShape player2Box;
    sf::Text           player1InputText;
    sf::Text           player2InputText;

    // Difficulty buttons
    sf::RectangleShape diffButtons[3];
    sf::Text           diffButtonTexts[3];

    // Bottom buttons: PLAY | SCORES | BACK
    sf::RectangleShape startButton;
    sf::Text           startButtonText;
    sf::RectangleShape leaderboardButton;
    sf::Text           leaderboardButtonText;
    sf::RectangleShape backButton;
    sf::Text           backButtonText;

    // game screen ui
    sf::Text score1Text;
    sf::Text score2Text;
    sf::Text player1GameLabel;
    sf::Text player2GameLabel;
    sf::Text launchHint;

    // result screen ui
    sf::Text           resultText;
    sf::RectangleShape playAgainButton;
    sf::Text           playAgainButtonText;
    sf::RectangleShape resultLeaderboardButton;
    sf::Text           resultLeaderboardButtonText;
    sf::RectangleShape menuButton;
    sf::Text           menuButtonText;

    // setup helpers
    void loadAssets();
    void setupBackground();
    void setupSetupUI();
    void setupGameUI();
    void setupResultUI();
    void setupCourt();
    void applyDifficulty();
    void resetBall();
    void reset();

    // game logic
    void updateGame(float dt);
    void movePaddles(float dt);

    // draw phases
    void drawSetup(sf::RenderWindow& window);
    void drawGame(sf::RenderWindow& window);
    void drawResult(sf::RenderWindow& window);

    // input handling
    void handleSetupClick(sf::Vector2f mousePos, sf::RenderWindow& window);
    void handleResultClick(sf::Vector2f mousePos, sf::RenderWindow& window);
    void handleTextInput(const sf::Event& event);

    // hover helpers
    void updateInputBoxStyles();
    void updateSetupHover(sf::Vector2f mp);
    void updateResultHover(sf::Vector2f mp);
    void updateDiffHover(sf::Vector2f mp);

    // utility
    void centerText(sf::Text& text, float cx, float cy);
    void centerTextInButton(sf::Text& text, const sf::RectangleShape& btn);
    void showLeaderboard(sf::RenderWindow& window);

    // score helpers
    std::string intToStr(int n);

public:
    Pong();
    ~Pong();
    void run(sf::RenderWindow& window);
};