#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

const int MAX_CARDS = 16;  // maximum cards (hard mode)

// difficulty constants
const int CARDS_DIFF_EASY = 0;
const int CARDS_DIFF_HARD = 1;

// single card data
struct Card
{
    int pairId;       // 1-8, two cards share the same pairId
    bool faceUp;
    bool matched;

    sf::Sprite      frontSprite;
    sf::Sprite      backSprite;
    sf::RectangleShape highlight; // hover/selection outline
};

class Cards
{
private:
    // game flow flags
    bool gameStarted;
    bool gameOver;
    bool returnToMenu;
    bool player1Turn;

    // difficulty
    int difficulty;      // CARDS_DIFF_EASY or CARDS_DIFF_HARD
    int gridRows;
    int gridCols;
    int activeCards;     // 8 (easy) or 16 (hard)

    // name input focus
    bool typingP1;
    bool typingP2;

    // player data
    std::string player1;
    std::string player2;
    std::string winnerName;

    sf::SoundBuffer winBuffer;
    sf::SoundBuffer drawBuffer;
    sf::Sound       gameSound;
    int score1;
    int score2;

    // card flip state
    int firstFlip;    // index of first flipped card (-1 if none)
    int secondFlip;   // index of second flipped card (-1 if none)
    bool waitingToHide;
    sf::Clock flipClock; // pause before hiding unmatched pair

    // board
    Card cards[MAX_CARDS];

    // assets
    sf::Font    font;
    sf::Texture backgroundTexture;
    sf::Sprite  backgroundSprite;
    sf::Texture cardTextures[8];   // front images card1..card8
    sf::Texture backTexture;       // card back image

    // difficulty buttons
    sf::RectangleShape diffButtons[2];
    sf::Text           diffButtonTexts[2];

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

    sf::RectangleShape startButton;
    sf::Text           startButtonText;
    sf::RectangleShape leaderboardButton;
    sf::Text           leaderboardButtonText;
    sf::RectangleShape backButton;
    sf::Text           backButtonText;

    // game screen ui
    sf::Text gamePlayer1Label;
    sf::Text gamePlayer2Label;
    sf::Text score1Text;
    sf::Text score2Text;
    sf::Text turnText;

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
    void setupDiffButtons();
    void setupGameUI();
    void setupResultUI();
    void initBoard();
    void shuffleCards();

    // game logic
    void handleCardClick(sf::Vector2f mousePos);
    void checkMatch();
    void checkGameOver();

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

    // utility
    void centerText(sf::Text& text, float cx, float cy);
    void centerTextInButton(sf::Text& text, const sf::RectangleShape& btn);
    void showLeaderboard(sf::RenderWindow& window);
    void reset();

public:
    Cards();
    ~Cards();
    void run(sf::RenderWindow& window);
};