#pragma once
#include <SFML/Graphics.hpp>
#include <string>

const int MAX_LEADERBOARD_ENTRIES = 50;
const int MAX_DISPLAY_ROWS = 60;

// data record
struct LeaderboardEntry
{
    std::string playerName;
    std::string game;
    int score;
};

// Leaderboard — serves as the LeaderboardManager / FileManager
// Handles all file I/O, persistent storage, and leaderboard display
// as required by the assignment specification.
class FileManager
{
private:
    sf::Font    font;
    sf::Texture backgroundTexture;
    sf::Sprite  backgroundSprite;

    LeaderboardEntry entries[MAX_LEADERBOARD_ENTRIES];
    int entryCount;

    sf::Text titleText;
    sf::Text noEntriesText;

    sf::RectangleShape backButton;
    sf::Text           backButtonText;

    sf::Text rowTexts[MAX_DISPLAY_ROWS];
    int rowCount;

    std::string filterGame;   // if set, only show entries for this game

    const std::string FILE_PATH = "assets/leaderboard.txt";

    void loadAssets();
    void setupBackground();
    void setupUI();
    void buildRowTexts();

    void centerText(sf::Text& text, float cx, float cy);
    void centerTextInButton(sf::Text& text, const sf::RectangleShape& btn);

public:
    FileManager();
    ~FileManager();

    // required methods per assignment spec
    void AddRecord(const std::string& playerName, const std::string& game, int score);

    void LoadFromFile();
    void LoadFromFileFiltered(const std::string& game); // loads only entries for one game
    void SaveToFile();
    void SortRecords();
    void DisplayLeaderboard(sf::RenderWindow& window);

    // static convenience — call from any game
    static void SaveEntry(const std::string& playerName, const std::string& game, int score);

    // run() with optional game filter
    // pass "" for global leaderboard, or "TICTACTOE" etc for per-game
    void run(sf::RenderWindow& window, const std::string& gameFilter);
};