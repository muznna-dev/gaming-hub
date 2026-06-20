#include "FileManager.h"
#include <iostream>
#include <fstream>

// simple helpers to convert between int and string without using libraries
static std::string intToStr(int n)
{
    if (n == 0) return "0";

    std::string result = "";
    bool negative = (n < 0);
    if (negative) n = -n;

    while (n > 0)
    {
        char digit = '0' + (n % 10);
        result = digit + result;
        n /= 10;
    }
    if (negative) result = "-" + result;
    return result;
}

static int strToInt(const std::string& s)
{
    int result = 0;
    int i = 0;
    bool negative = false;

    if (!s.empty() && s[0] == '-') { negative = true; i = 1; }

    for (; i < (int)s.size(); i++)
    {
        if (s[i] < '0' || s[i] > '9') break;
        result = result * 10 + (s[i] - '0');
    }
    return negative ? -result : result;
}

// initialise window and load everything
FileManager::FileManager()
    : entryCount(0), rowCount(0), filterGame("")
{
    loadAssets();
    setupBackground();
    setupUI();
}

FileManager::~FileManager()
{
    // nothing to manually clean up - SFML handles its own resources
}

// load fonts, textures, sounds
void FileManager::loadAssets()
{
    if (!font.loadFromFile("assets/fonts/pixel.ttf"))
        std::cout << "Error: could not load font\n";

    if (!backgroundTexture.loadFromFile("assets/images/hub_bg.jpg"))
        std::cout << "Error: could not load leaderboard background\n";

    backgroundTexture.setSmooth(true);
}

void FileManager::setupBackground()
{
    backgroundSprite.setTexture(backgroundTexture);
    if (backgroundTexture.getSize().x > 0 && backgroundTexture.getSize().y > 0)
    {
        backgroundSprite.setScale(1200.0f / backgroundTexture.getSize().x, 700.0f / backgroundTexture.getSize().y
        );
    }
}

void FileManager::setupUI()
{
    titleText.setFont(font);
    titleText.setString("LEADERBOARD");
    titleText.setCharacterSize(52);
    titleText.setFillColor(sf::Color(255, 220, 100));
    titleText.setOutlineColor(sf::Color(60, 30, 5));
    titleText.setOutlineThickness(5.0f);
    centerText(titleText, 600.0f, 60.0f);

    noEntriesText.setFont(font);
    noEntriesText.setString("NO ENTRIES YET.\nPLAY A GAME FIRST!");
    noEntriesText.setCharacterSize(24);
    noEntriesText.setFillColor(sf::Color(200, 210, 255));
    noEntriesText.setOutlineColor(sf::Color::Black);
    noEntriesText.setOutlineThickness(3.0f);
    centerText(noEntriesText, 600.0f, 340.0f);

    backButton.setSize(sf::Vector2f(200.0f, 52.0f));
    backButton.setPosition(500.0f, 625.0f);
    backButton.setFillColor(sf::Color(12, 30, 70));
    backButton.setOutlineThickness(3.0f);
    backButton.setOutlineColor(sf::Color(100, 160, 220));

    backButtonText.setFont(font);
    backButtonText.setString("BACK");
    backButtonText.setCharacterSize(22);
    backButtonText.setFillColor(sf::Color(180, 210, 245));
    backButtonText.setOutlineColor(sf::Color::Black);
    backButtonText.setOutlineThickness(2.0f);
    centerTextInButton(backButtonText, backButton);
}

// adds a new score or updates existing one for the same player and game
void FileManager::AddRecord(const std::string& playerName, const std::string& game, int score)
{
    for (int i = 0; i < entryCount; i++)
    {
        if (entries[i].playerName == playerName && entries[i].game == game)
        {
            entries[i].score += score;
            return;
        }
    }

    if (entryCount < MAX_LEADERBOARD_ENTRIES)
    {
        entries[entryCount].playerName = playerName;
        entries[entryCount].game = game;
        entries[entryCount].score = score;
        entryCount++;
    }
}

// internal helper - reads file and optionally filters by game name
static void parseFile(const std::string& filePath, LeaderboardEntry* entries, int& entryCount, const std::string& filter)
{
    entryCount = 0;

    std::ifstream file(filePath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line) && entryCount < MAX_LEADERBOARD_ENTRIES)
    {
        if (line.empty()) continue;

        // Manual comma-split
        std::string name = "", game = "", scoreStr = "";
        int field = 0;
        for (int ci = 0; ci < (int)line.size(); ci++)
        {
            if (line[ci] == ',') { field++; continue; }
            if (field == 0)      name += line[ci];
            else if (field == 1) game += line[ci];
            else                 scoreStr += line[ci];
        }

        if (name.empty() || game.empty() || scoreStr.empty()) continue;

        // Apply filter — skip if doesn't match
        if (!filter.empty() && game != filter) continue;

        entries[entryCount].playerName = name;
        entries[entryCount].game = game;
        entries[entryCount].score = strToInt(scoreStr);
        entryCount++;
    }
    file.close();
}

// reads all saved scores from the file
void FileManager::LoadFromFile()
{
    filterGame = "";
    parseFile(FILE_PATH, entries, entryCount, "");
}

// reads scores for a specific game only
void FileManager::LoadFromFileFiltered(const std::string& game)
{
    filterGame = game;

    // Update title to show which game's leaderboard this is
    titleText.setString(game + "\nLEADERBOARD");
    centerText(titleText, 600.0f, 55.0f);

    parseFile(FILE_PATH, entries, entryCount, game);
}

// saves everything back to disk, merging filtered entries with the rest
void FileManager::SaveToFile()
{
    // If no filter was active, just write directly
    if (filterGame.empty())
    {
        std::ofstream file(FILE_PATH);
        if (!file.is_open())
        {
            std::cout << "Error: could not open leaderboard.txt for writing\n";
            return;
        }
        for (int i = 0; i < entryCount; i++)
            file << entries[i].playerName << ","
            << entries[i].game << ","
            << entries[i].score << "\n";
        file.close();
        return;
    }

    // Filter was active — read all OTHER game entries first
    LeaderboardEntry others[MAX_LEADERBOARD_ENTRIES];
    int otherCount = 0;
    parseFile(FILE_PATH, others, otherCount, "");

    // Remove entries that belong to filterGame (we'll replace with updated ones)
    LeaderboardEntry merged[MAX_LEADERBOARD_ENTRIES];
    int mergedCount = 0;

    for (int i = 0; i < otherCount; i++)
    {
        if (others[i].game != filterGame && mergedCount < MAX_LEADERBOARD_ENTRIES)
        {
            merged[mergedCount] = others[i];
            mergedCount++;
        }
    }

    // Append our updated filtered entries
    for (int i = 0; i < entryCount && mergedCount < MAX_LEADERBOARD_ENTRIES; i++)
    {
        merged[mergedCount] = entries[i];
        mergedCount++;
    }

    std::ofstream file(FILE_PATH);
    if (!file.is_open())
    {
        std::cout << "Error: could not open leaderboard.txt for writing\n";
        return;
    }
    for (int i = 0; i < mergedCount; i++)
        file << merged[i].playerName << ","
        << merged[i].game << ","
        << merged[i].score << "\n";
    file.close();
}

// sorts scores from highest to lowest using bubble sort
void FileManager::SortRecords()
{
    for (int i = 0; i < entryCount - 1; i++)
    {
        for (int j = 0; j < entryCount - i - 1; j++)
        {
            if (entries[j].score < entries[j + 1].score)
            {
                LeaderboardEntry temp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = temp;
            }
        }
    }
}

// Static SaveEntry — full pipeline: load ? add ? sort ? save
void FileManager::SaveEntry(const std::string& playerName, const std::string& game, int score)
{
    FileManager lb;
    lb.LoadFromFile();
    lb.AddRecord(playerName, game, score);
    lb.SortRecords();
    lb.SaveToFile();
}

// builds the text rows that will be drawn on screen
void FileManager::buildRowTexts()
{
    rowCount = 0;

    // Header
    rowTexts[rowCount].setFont(font);
    rowTexts[rowCount].setString("  RANK   PLAYER              GAME          WINS");
    rowTexts[rowCount].setCharacterSize(22);
    rowTexts[rowCount].setFillColor(sf::Color(255, 220, 100));
    rowTexts[rowCount].setOutlineColor(sf::Color::Black);
    rowTexts[rowCount].setOutlineThickness(2.0f);
    rowTexts[rowCount].setPosition(100.0f, 115.0f);
    rowCount++;

    // Divider
    rowTexts[rowCount].setFont(font);
    rowTexts[rowCount].setString("  ------------------------------------------------");
    rowTexts[rowCount].setCharacterSize(22);
    rowTexts[rowCount].setFillColor(sf::Color(100, 100, 120));
    rowTexts[rowCount].setPosition(100.0f, 150.0f);
    rowCount++;

    const int MAX_SHOW = 10;
    int count = entryCount < MAX_SHOW ? entryCount : MAX_SHOW;

    for (int i = 0; i < count && rowCount < MAX_DISPLAY_ROWS; i++)
    {
        std::string rank = intToStr(i + 1);
        while ((int)rank.size() < 4) rank += " ";

        std::string name = entries[i].playerName;
        while ((int)name.size() < 20) name += " ";

        std::string game = entries[i].game;
        while ((int)game.size() < 14) game += " ";

        std::string line = "  " + rank + "   " + name + game + intToStr(entries[i].score);

        rowTexts[rowCount].setFont(font);
        rowTexts[rowCount].setString(line);
        rowTexts[rowCount].setCharacterSize(22);

        if (i == 0) rowTexts[rowCount].setFillColor(sf::Color(255, 215, 0));
        else if (i == 1) rowTexts[rowCount].setFillColor(sf::Color(192, 192, 192));
        else if (i == 2) rowTexts[rowCount].setFillColor(sf::Color(205, 127, 50));
        else             rowTexts[rowCount].setFillColor(sf::Color(220, 230, 255));

        rowTexts[rowCount].setOutlineColor(sf::Color::Black);
        rowTexts[rowCount].setOutlineThickness(2.0f);
        rowTexts[rowCount].setPosition(100.0f, 185.0f + i * 40.0f);
        rowCount++;
    }
}

// DisplayLeaderboard
void FileManager::DisplayLeaderboard(sf::RenderWindow& window)
{
    window.draw(backgroundSprite);

    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(1200.0f, 700.0f));
    overlay.setFillColor(sf::Color(0, 0, 0, 110));
    window.draw(overlay);

    window.draw(titleText);

    if (entryCount == 0)
        window.draw(noEntriesText);
    else
        for (int i = 0; i < rowCount; i++)
            window.draw(rowTexts[i]);

    window.draw(backButton);
    window.draw(backButtonText);
}

// utility functions
void FileManager::centerText(sf::Text& text, float cx, float cy)
{
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
    text.setPosition(cx, cy);
}

void FileManager::centerTextInButton(sf::Text& text, const sf::RectangleShape& btn)
{
    sf::FloatRect tb = text.getLocalBounds();
    sf::FloatRect bb = btn.getGlobalBounds();
    text.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    text.setPosition(bb.left + bb.width / 2.0f, bb.top + bb.height / 2.0f);
}

// main loop - shows the leaderboard until user goes back
void FileManager::run(sf::RenderWindow& window, const std::string& gameFilter)
{
    if (gameFilter.empty())
        LoadFromFile();
    else
        LoadFromFileFiltered(gameFilter);

    SortRecords();
    buildRowTexts();

    sf::Color navyHover(40, 80, 150);
    sf::Color navyDark(12, 30, 70);

    sf::Event event;
    while (window.isOpen())
    {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        backButton.setFillColor(backButton.getGlobalBounds().contains(mousePos) ? navyHover : navyDark);

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
                return;

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f click = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (backButton.getGlobalBounds().contains(click))
                    return;
            }
        }

        window.clear();
        DisplayLeaderboard(window);
        window.display();
    }
}