#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

const int NUM_SYMBOLS = 5;

const int PHASE_NAMES = 0;
const int PHASE_P1SYM = 1;
const int PHASE_P2SYM = 2;
const int PHASE_GAME = 3;
const int PHASE_RESULT = 4;

class TicTacToe
{
private:
    int setupPhase;
    bool gameOver;
    bool drawMatch;
    bool returnToMenu;
    bool player1Turn;
    bool typingP1;
    bool typingP2;

    std::string player1;
    std::string player2;
    std::string winnerName;

    sf::SoundBuffer winBuffer;
    sf::SoundBuffer drawBuffer;
    sf::Sound       gameSound;
    int sym1;
    int sym2;

    int board[3][3];

    sf::Font    font;
    sf::Texture backgroundTexture;
    sf::Sprite  backgroundSprite;

    sf::Texture symTextures[NUM_SYMBOLS];
    std::string symNames[NUM_SYMBOLS];

    sf::RectangleShape symBoxes[NUM_SYMBOLS];
    sf::Sprite         symPreviews[NUM_SYMBOLS];

    sf::Text titleText;
    sf::Text subtitleText;
    sf::Text player1Label;
    sf::Text player2Label;
    sf::Text infoText;
    sf::Text pickPromptText;

    sf::RectangleShape player1Box;
    sf::RectangleShape player2Box;
    sf::Text           player1InputText;
    sf::Text           player2InputText;

    sf::RectangleShape nextButton;
    sf::Text           nextButtonText;
    sf::RectangleShape startButton;
    sf::Text           startButtonText;
    sf::RectangleShape leaderboardButton;
    sf::Text           leaderboardButtonText;
    sf::RectangleShape backButton;
    sf::Text           backButtonText;

    sf::RectangleShape cells[3][3];
    sf::Sprite         cellSprites[3][3];
    sf::Text           cellTexts[3][3];
    sf::Text           turnText;
    sf::Text           gamePlayer1Label;
    sf::Text           gamePlayer2Label;

    sf::Text           resultText;
    sf::RectangleShape playAgainButton;
    sf::Text           playAgainButtonText;
    sf::RectangleShape resultLeaderboardButton;
    sf::Text           resultLeaderboardButtonText;
    sf::RectangleShape menuButton;
    sf::Text           menuButtonText;

    void loadAssets();
    void setupBackground();
    void setupTexts();
    void setupInputBoxes();
    void setupButtons();
    void setupSymbolPicker();
    void setupGameBoard();
    void setupGameUI();
    void setupResultScreen();

    bool checkWin(int playerSign);
    bool checkDraw();
    void handleBoardClick(sf::Vector2f mousePos);
    void placeCellSprite(int r, int c, int symIdx);

    void drawNamesPhase(sf::RenderWindow& window);
    void drawSymbolPicker(sf::RenderWindow& window, int playerNum);
    void drawGame(sf::RenderWindow& window);
    void drawResult(sf::RenderWindow& window);

    void handleNamesClick(sf::Vector2f mousePos, sf::RenderWindow& window);
    void handleSymbolClick(sf::Vector2f mousePos, int playerNum);
    void handleTextInput(const sf::Event& event);
    void handleResultClick(sf::Vector2f mousePos, sf::RenderWindow& window);

    void updateInputBoxStyles();
    void updateNamesHover(sf::Vector2f mp);
    void updateResultHover(sf::Vector2f mp);
    void centerText(sf::Text& text, float cx, float cy);
    void centerTextInButton(sf::Text& text, const sf::RectangleShape& btn);
    void showLeaderboard(sf::RenderWindow& window);

public:
    TicTacToe();
    ~TicTacToe();
    void reset();
    void run(sf::RenderWindow& window);
    bool shouldReturnToMenu() const;
    std::string getPlayer1Name()     const;
    std::string getPlayer2Name()     const;
};