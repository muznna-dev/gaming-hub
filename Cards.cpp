#include "Cards.h"
#include "FileManager.h"
#include <ctime>
#include <iostream>

// initialise window and load everything
Cards::Cards()
{
    gameStarted = false;
    gameOver = false;
    returnToMenu = false;
    player1Turn = true;
    typingP1 = false;
    typingP2 = false;
    difficulty = CARDS_DIFF_EASY;
    gridRows = 2;
    gridCols = 4;
    activeCards = 8;
    player1 = "";
    player2 = "";
    winnerName = "";
    score1 = 0;
    score2 = 0;
    firstFlip = -1;
    secondFlip = -1;
    waitingToHide = false;

    loadAssets();
    setupBackground();
    setupSetupUI();
    setupDiffButtons();
}

// Cards destructor
Cards::~Cards()
{
    // SFML resources released automatically
    // Card sprites and textures cleaned up by SFML
}


// reset() — restarts board keeping player names
void Cards::reset()
{
    score1 = 0;
    score2 = 0;
    firstFlip = -1;
    secondFlip = -1;
    waitingToHide = false;
    gameOver = false;
    player1Turn = true;
    winnerName = "";

    // Apply difficulty settings
    if (difficulty == CARDS_DIFF_EASY)
    {
        gridRows = 2;
        gridCols = 4;
        activeCards = 8;
    }
    else
    {
        gridRows = 4;
        gridCols = 4;
        activeCards = 16;
    }

    initBoard();
}

// load fonts, textures, sounds
void Cards::loadAssets()
{
    if (!font.loadFromFile("assets/fonts/pixel.ttf"))
        std::cout << "Error: could not load font\n";

    if (!backgroundTexture.loadFromFile("assets/images/cards_bg.png"))
        if (!backgroundTexture.loadFromFile("assets/images/hub_bg.jpg"))
            std::cout << "Error: could not load cards background\n";

    backgroundTexture.setSmooth(true);

    if (!winBuffer.loadFromFile("assets/sounds/win.wav"))
        std::cout << "Error: could not load win.wav\n";
    if (!drawBuffer.loadFromFile("assets/sounds/draw.wav"))
        std::cout << "Error: could not load draw.wav\n";

    // Load card front images card1.png ... card8.png
    for (int i = 0; i < 8; i++)
    {
        std::string path = "assets/images/card";

        // Manual int-to-string
        path += (char)('1' + i);
        path += ".png";

        if (!cardTextures[i].loadFromFile(path))
            std::cout << "Error: could not load " << path << "\n";

        cardTextures[i].setSmooth(false);
    }

    if (!backTexture.loadFromFile("assets/images/card_back.png"))
        std::cout << "Error: could not load card_back.png\n";

    backTexture.setSmooth(false);
}

void Cards::setupBackground()
{
    backgroundSprite.setTexture(backgroundTexture);
    if (backgroundTexture.getSize().x > 0 && backgroundTexture.getSize().y > 0)
    {
        backgroundSprite.setScale(1200.0f / backgroundTexture.getSize().x, 700.0f / backgroundTexture.getSize().y
        );
    }
}

// initBoard — places 8 pairs into 16 slots then shuffles
void Cards::initBoard()
{
    // Assign pair IDs: two cards per pair
    for (int i = 0; i < activeCards; i++)
    {
        cards[i].pairId = (i / 2) + 1;
        cards[i].faceUp = false;
        cards[i].matched = false;
    }

    shuffleCards();

    // Card size depends on difficulty
    const float CARD_W = (difficulty == CARDS_DIFF_EASY) ? 140.0f : 90.0f;
    const float CARD_H = (difficulty == CARDS_DIFF_EASY) ? 180.0f : 130.0f;
    const float GAP_X = (difficulty == CARDS_DIFF_EASY) ? 20.0f : 14.0f;
    const float GAP_Y = (difficulty == CARDS_DIFF_EASY) ? 20.0f : 10.0f;
    const float GRID_W = gridCols * CARD_W + (gridCols - 1) * GAP_X;
    const float GRID_H = gridRows * CARD_H + (gridRows - 1) * GAP_Y;
    const float START_X = (1200.0f - GRID_W) / 2.0f;
    const float START_Y = (700.0f - GRID_H) / 2.0f - 30.0f;

    for (int i = 0; i < activeCards; i++)
    {
        int row = i / gridCols;
        int col = i % gridCols;

        float x = START_X + col * (CARD_W + GAP_X);
        float y = START_Y + row * (CARD_H + GAP_Y);

        int texIdx = cards[i].pairId - 1; // 0..7

        // Scale front sprite to card size — resetRect=true prevents bleed on replay
        cards[i].frontSprite = sf::Sprite();
        cards[i].frontSprite.setTexture(cardTextures[texIdx], true);
        sf::Vector2u fs = cardTextures[texIdx].getSize();
        if (fs.x > 0 && fs.y > 0)
            cards[i].frontSprite.setScale(CARD_W / fs.x, CARD_H / fs.y);
        cards[i].frontSprite.setPosition(x, y);

        // Scale back sprite to card size — resetRect=true prevents bleed on replay
        cards[i].backSprite = sf::Sprite();
        cards[i].backSprite.setTexture(backTexture, true);
        sf::Vector2u bs = backTexture.getSize();
        if (bs.x > 0 && bs.y > 0)
            cards[i].backSprite.setScale(CARD_W / bs.x, CARD_H / bs.y);
        cards[i].backSprite.setPosition(x, y);

        // Highlight outline
        cards[i].highlight.setSize(sf::Vector2f(CARD_W + 6.0f, CARD_H + 6.0f));
        cards[i].highlight.setPosition(x - 3.0f, y - 3.0f);
        cards[i].highlight.setFillColor(sf::Color::Transparent);
        cards[i].highlight.setOutlineThickness(4.0f);
        cards[i].highlight.setOutlineColor(sf::Color::Transparent);
    }
}

// shuffleCards — simple Fisher-Yates using rand()
void Cards::shuffleCards()
{
    // Seed using time
    srand((unsigned int)time(nullptr));

    for (int i = activeCards - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int tmp = cards[i].pairId;
        cards[i].pairId = cards[j].pairId;
        cards[j].pairId = tmp;
    }
}

// all the UI for the name entry screen
// setupDiffButtons
void Cards::setupDiffButtons()
{
    std::string labels[2] = { "EASY  (8 cards)", "HARD  (16 cards)" };
    sf::Color   colors[2] = { sf::Color(20, 20, 20), sf::Color(20, 20, 20) };

    const float DBW = 220.f;
    const float DBH = 44.f;
    const float DGAP = 24.f;
    const float DTOTAL = 2 * DBW + DGAP;
    const float DLX = 600.f - DTOTAL / 2.f;
    const float DY = 430.f;

    for (int i = 0; i < 2; i++)
    {
        diffButtons[i].setSize(sf::Vector2f(DBW, DBH));
        diffButtons[i].setPosition(DLX + i * (DBW + DGAP), DY);
        diffButtons[i].setFillColor(colors[i]);
        diffButtons[i].setOutlineThickness(3.f);
        diffButtons[i].setOutlineColor(sf::Color(150, 150, 150));

        diffButtonTexts[i].setFont(font);
        diffButtonTexts[i].setString(labels[i]);
        diffButtonTexts[i].setCharacterSize(16);
        diffButtonTexts[i].setFillColor(sf::Color::White);
        diffButtonTexts[i].setOutlineColor(sf::Color::Black);
        diffButtonTexts[i].setOutlineThickness(2.f);

        sf::FloatRect tb = diffButtonTexts[i].getLocalBounds();
        sf::FloatRect bb = diffButtons[i].getGlobalBounds();
        diffButtonTexts[i].setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        diffButtonTexts[i].setPosition(bb.left + bb.width / 2.f, bb.top + bb.height / 2.f);
    }

    // Highlight EASY as default — same rim for both, selected gets white
    diffButtons[0].setOutlineColor(sf::Color(150, 150, 150));
    diffButtons[0].setOutlineThickness(3.f);
    diffButtons[1].setOutlineColor(sf::Color(150, 150, 150));
    diffButtons[1].setOutlineThickness(3.f);
    diffButtons[difficulty].setOutlineColor(sf::Color(150, 150, 150));
}

void Cards::setupSetupUI()
{
    titleText.setFont(font);
    titleText.setString("MATCH CARDS");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color(255, 230, 140));
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(4.0f);
    centerText(titleText, 600.0f, 50.0f);

    subtitleText.setFont(font);
    subtitleText.setString("ENTER PLAYER NAMES");
    subtitleText.setCharacterSize(18);
    subtitleText.setFillColor(sf::Color(220, 220, 220));
    subtitleText.setOutlineColor(sf::Color::Black);
    subtitleText.setOutlineThickness(2.0f);
    centerText(subtitleText, 600.0f, 105.0f);

    player1Label.setFont(font);
    player1Label.setString("PLAYER 1");
    player1Label.setCharacterSize(17);
    player1Label.setFillColor(sf::Color(255, 235, 180));
    player1Label.setOutlineColor(sf::Color::Black);
    player1Label.setOutlineThickness(2.0f);
    player1Label.setPosition(340.0f, 148.0f);

    player2Label.setFont(font);
    player2Label.setString("PLAYER 2");
    player2Label.setCharacterSize(17);
    player2Label.setFillColor(sf::Color(255, 235, 180));
    player2Label.setOutlineColor(sf::Color::Black);
    player2Label.setOutlineThickness(2.0f);
    player2Label.setPosition(340.0f, 268.0f);

    infoText.setFont(font);
    infoText.setString("CLICK A BOX TO TYPE");
    infoText.setCharacterSize(15);
    infoText.setFillColor(sf::Color::White);
    infoText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineThickness(2.0f);
    centerText(infoText, 600.0f, 390.0f);

    player1InputText.setFont(font);
    player1InputText.setCharacterSize(20);
    player1InputText.setFillColor(sf::Color(20, 20, 20));
    player1InputText.setPosition(350.0f, 177.0f);

    player2InputText.setFont(font);
    player2InputText.setCharacterSize(20);
    player2InputText.setFillColor(sf::Color(20, 20, 20));
    player2InputText.setPosition(350.0f, 297.0f);

    // Input boxes
    const float BOX_W = 520.0f;
    const float BOX_H = 48.0f;
    const float BOX_X = 340.0f;

    player1Box.setSize(sf::Vector2f(BOX_W, BOX_H));
    player1Box.setPosition(BOX_X, 170.0f);
    player1Box.setFillColor(sf::Color(240, 235, 220));
    player1Box.setOutlineThickness(3.0f);
    player1Box.setOutlineColor(sf::Color(90, 50, 20));

    player2Box.setSize(sf::Vector2f(BOX_W, BOX_H));
    player2Box.setPosition(BOX_X, 290.0f);
    player2Box.setFillColor(sf::Color(240, 235, 220));
    player2Box.setOutlineThickness(3.0f);
    player2Box.setOutlineColor(sf::Color(90, 50, 20));

    // Three buttons: PLAY | SCORES | BACK
    sf::Color navyNormal(20, 20, 20);  // black
    sf::Color navyMid(30, 30, 30);  // black mid
    sf::Color navyDark(10, 10, 10);  // black dark
    sf::Color rim(80, 80, 80);  // grey rim

    const float BTN_W = 155.0f;
    const float BTN_H = 52.0f;
    const float GAP = 20.0f;
    const float BTN_Y = 510.0f;
    const float TOTAL_W = 3 * BTN_W + 2 * GAP;
    const float LEFT_X = 600.0f - TOTAL_W / 2.0f;

    startButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    startButton.setPosition(LEFT_X, BTN_Y);
    startButton.setFillColor(navyNormal);
    startButton.setOutlineThickness(3.0f);
    startButton.setOutlineColor(rim);

    startButtonText.setFont(font);
    startButtonText.setString("PLAY");
    startButtonText.setCharacterSize(20);
    startButtonText.setFillColor(sf::Color::White);
    startButtonText.setOutlineColor(sf::Color(60, 60, 60));
    startButtonText.setOutlineThickness(2.0f);
    centerTextInButton(startButtonText, startButton);

    leaderboardButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    leaderboardButton.setPosition(LEFT_X + BTN_W + GAP, BTN_Y);
    leaderboardButton.setFillColor(navyMid);
    leaderboardButton.setOutlineThickness(3.0f);
    leaderboardButton.setOutlineColor(rim);

    leaderboardButtonText.setFont(font);
    leaderboardButtonText.setString("SCORES");
    leaderboardButtonText.setCharacterSize(20);
    leaderboardButtonText.setFillColor(sf::Color::White);
    leaderboardButtonText.setOutlineColor(sf::Color(60, 60, 60));
    leaderboardButtonText.setOutlineThickness(2.0f);
    centerTextInButton(leaderboardButtonText, leaderboardButton);

    backButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    backButton.setPosition(LEFT_X + 2 * (BTN_W + GAP), BTN_Y);
    backButton.setFillColor(navyDark);
    backButton.setOutlineThickness(3.0f);
    backButton.setOutlineColor(rim);

    backButtonText.setFont(font);
    backButtonText.setString("BACK");
    backButtonText.setCharacterSize(20);
    backButtonText.setFillColor(sf::Color::White);
    backButtonText.setOutlineColor(sf::Color::Black);
    backButtonText.setOutlineThickness(2.0f);
    centerTextInButton(backButtonText, backButton);
}

// Game screen UI (score + turn labels)
void Cards::setupGameUI()
{
    gamePlayer1Label.setFont(font);
    gamePlayer1Label.setCharacterSize(28);
    gamePlayer1Label.setFillColor(sf::Color(255, 235, 210));
    gamePlayer1Label.setOutlineColor(sf::Color::Black);
    gamePlayer1Label.setOutlineThickness(2.0f);
    gamePlayer1Label.setPosition(30.0f, 15.0f);

    gamePlayer2Label.setFont(font);
    gamePlayer2Label.setCharacterSize(28);
    gamePlayer2Label.setFillColor(sf::Color(255, 235, 210));
    gamePlayer2Label.setOutlineColor(sf::Color::Black);
    gamePlayer2Label.setOutlineThickness(2.0f);

    score1Text.setFont(font);
    score1Text.setCharacterSize(24);
    score1Text.setFillColor(sf::Color(255, 235, 210));
    score1Text.setOutlineColor(sf::Color::Black);
    score1Text.setOutlineThickness(2.0f);
    score1Text.setPosition(30.0f, 40.0f);

    score2Text.setFont(font);
    score2Text.setCharacterSize(24);
    score2Text.setFillColor(sf::Color(255, 235, 210));
    score2Text.setOutlineColor(sf::Color::Black);
    score2Text.setOutlineThickness(2.0f);

    turnText.setFont(font);
    turnText.setCharacterSize(20);
    turnText.setFillColor(sf::Color(255, 220, 100));
    turnText.setOutlineColor(sf::Color::Black);
    turnText.setOutlineThickness(3.0f);
}

// buttons and text shown after game ends
void Cards::setupResultUI()
{
    sf::Color maroonNormal(60, 8, 18);
    sf::Color maroonDark(60, 8, 18);
    sf::Color rim(200, 80, 100);

    resultText.setFont(font);
    resultText.setCharacterSize(36);
    resultText.setFillColor(sf::Color(255, 235, 210));
    resultText.setOutlineColor(sf::Color::Black);
    resultText.setOutlineThickness(4.0f);

    const float BTN_W = 155.0f;
    const float BTN_H = 52.0f;
    const float GAP = 20.0f;
    const float BTN_Y = 390.0f;
    const float TOTAL_W = 3 * BTN_W + 2 * GAP;
    const float LEFT_X = 600.0f - TOTAL_W / 2.0f;

    playAgainButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    playAgainButton.setPosition(LEFT_X, BTN_Y);
    playAgainButton.setFillColor(maroonNormal);
    playAgainButton.setOutlineThickness(3.0f);
    playAgainButton.setOutlineColor(rim);

    playAgainButtonText.setFont(font);
    playAgainButtonText.setString("PLAY AGAIN");
    playAgainButtonText.setCharacterSize(16);
    playAgainButtonText.setFillColor(sf::Color(255, 235, 210));
    playAgainButtonText.setOutlineColor(sf::Color::Black);
    playAgainButtonText.setOutlineThickness(2.0f);
    centerTextInButton(playAgainButtonText, playAgainButton);

    resultLeaderboardButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    resultLeaderboardButton.setPosition(LEFT_X + BTN_W + GAP, BTN_Y);
    resultLeaderboardButton.setFillColor(maroonDark);
    resultLeaderboardButton.setOutlineThickness(3.0f);
    resultLeaderboardButton.setOutlineColor(rim);

    resultLeaderboardButtonText.setFont(font);
    resultLeaderboardButtonText.setString("SCORES");
    resultLeaderboardButtonText.setCharacterSize(16);
    resultLeaderboardButtonText.setFillColor(sf::Color(255, 235, 210));
    resultLeaderboardButtonText.setOutlineColor(sf::Color::Black);
    resultLeaderboardButtonText.setOutlineThickness(2.0f);
    centerTextInButton(resultLeaderboardButtonText, resultLeaderboardButton);

    menuButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    menuButton.setPosition(LEFT_X + 2 * (BTN_W + GAP), BTN_Y);
    menuButton.setFillColor(maroonDark);
    menuButton.setOutlineThickness(3.0f);
    menuButton.setOutlineColor(rim);

    menuButtonText.setFont(font);
    menuButtonText.setString("MAIN MENU");
    menuButtonText.setCharacterSize(16);
    menuButtonText.setFillColor(sf::Color(255, 235, 210));
    menuButtonText.setOutlineColor(sf::Color::Black);
    menuButtonText.setOutlineThickness(2.0f);
    centerTextInButton(menuButtonText, menuButton);
}

// Card click logic
void Cards::handleCardClick(sf::Vector2f mousePos)
{
    if (waitingToHide || gameOver) return;

    for (int i = 0; i < activeCards; i++)
    {
        if (cards[i].matched || cards[i].faceUp) continue;

        if (cards[i].backSprite.getGlobalBounds().contains(mousePos))
        {
            cards[i].faceUp = true;

            if (firstFlip == -1)
            {
                firstFlip = i;
            }
            else if (secondFlip == -1 && i != firstFlip)
            {
                secondFlip = i;
                checkMatch();
            }
            return;
        }
    }
}

void Cards::checkMatch()
{
    if (firstFlip == -1 || secondFlip == -1) return;

    if (cards[firstFlip].pairId == cards[secondFlip].pairId)
    {
        // Match!
        cards[firstFlip].matched = true;
        cards[secondFlip].matched = true;

        // Highlight matched pair green
        cards[firstFlip].highlight.setOutlineColor(sf::Color(80, 220, 80));
        cards[secondFlip].highlight.setOutlineColor(sf::Color(80, 220, 80));

        if (player1Turn) score1++;
        else             score2++;

        firstFlip = -1;
        secondFlip = -1;

        checkGameOver();
        // Same player gets another turn on match
    }
    else
    {
        // No match — wait 1 second then flip back
        waitingToHide = true;
        flipClock.restart();
        // Turn switches after hiding
    }
}

void Cards::checkGameOver()
{
    int matched = 0;
    for (int i = 0; i < activeCards; i++)
        if (cards[i].matched) matched++;

    if (matched == activeCards)
    {
        gameOver = true;

        if (score1 > score2)
        {
            winnerName = player1;
            resultText.setString(player1 + " WINS!\n" + player1 + ": " + player2 + " pairs vs ");
        }
        else if (score2 > score1)
            winnerName = player2;
        else
            winnerName = "DRAW";

        // Build result string manually
        std::string s1 = "", s2 = "";
        int tmp = score1;
        if (tmp == 0) s1 = "0";
        while (tmp > 0) { s1 = (char)('0' + tmp % 10) + s1; tmp /= 10; }
        tmp = score2;
        if (tmp == 0) s2 = "0";
        while (tmp > 0) { s2 = (char)('0' + tmp % 10) + s2; tmp /= 10; }

        if (winnerName == "DRAW")
        {
            gameSound.setBuffer(drawBuffer);
            gameSound.play();
        }
        else
        {
            gameSound.setBuffer(winBuffer);
            gameSound.play();
        }

        if (winnerName == "DRAW")
            resultText.setString("IT'S A DRAW!\n" + player1 + ": " + s1 + "  |  " + player2 + ": " + s2);
        else
            resultText.setString(winnerName + " WINS!\n" + player1 + ": " + s1 + "  |  " + player2 + ": " + s2);

        centerText(resultText, 600.0f, 300.0f);

        // Save to leaderboard
        if (winnerName == "DRAW")
        {
            FileManager::SaveEntry(player1, "CARDS", 0);
            FileManager::SaveEntry(player2, "CARDS", 0);
        }
        else
        {
            std::string loser = (winnerName == player1) ? player2 : player1;
            FileManager::SaveEntry(winnerName, "CARDS", 1);
            FileManager::SaveEntry(loser, "CARDS", 0);
        }
    }
}

// Drawing
void Cards::drawSetup(sf::RenderWindow& window)
{
    window.clear(sf::Color(15, 15, 30));
    window.draw(backgroundSprite);
    window.draw(titleText);
    window.draw(subtitleText);
    window.draw(player1Label);
    window.draw(player1Box);
    window.draw(player1InputText);
    window.draw(player2Label);
    window.draw(player2Box);
    window.draw(player2InputText);
    window.draw(infoText);
    for (int i = 0; i < 2; i++)
    {
        window.draw(diffButtons[i]);
        window.draw(diffButtonTexts[i]);
    }
    window.draw(startButton);
    window.draw(startButtonText);
    window.draw(leaderboardButton);
    window.draw(leaderboardButtonText);
    window.draw(backButton);
    window.draw(backButtonText);
}

void Cards::drawGame(sf::RenderWindow& window)
{
    // Dark-tinted background
    window.clear(sf::Color(15, 15, 30));
    window.draw(backgroundSprite);

    sf::RectangleShape tint;
    tint.setSize(sf::Vector2f(1200.0f, 700.0f));
    tint.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(tint);

    // Player labels and scores
    gamePlayer1Label.setString(player1);
    gamePlayer1Label.setFillColor(sf::Color(255, 235, 210));
    window.draw(gamePlayer1Label);

    // Build score strings manually
    std::string s1 = "", s2 = "";
    int tmp = score1;
    if (tmp == 0) s1 = "0";
    while (tmp > 0) { s1 = (char)('0' + tmp % 10) + s1; tmp /= 10; }
    tmp = score2;
    if (tmp == 0) s2 = "0";
    while (tmp > 0) { s2 = (char)('0' + tmp % 10) + s2; tmp /= 10; }

    score1Text.setString("Pairs: " + s1);
    window.draw(score1Text);

    gamePlayer2Label.setString(player2);
    gamePlayer2Label.setFillColor(sf::Color(255, 235, 210));
    sf::FloatRect p2b = gamePlayer2Label.getLocalBounds();
    gamePlayer2Label.setOrigin(p2b.left + p2b.width, 0.f);
    gamePlayer2Label.setPosition(1170.0f, 15.0f);
    window.draw(gamePlayer2Label);

    score2Text.setString("Pairs: " + s2);
    sf::FloatRect s2b = score2Text.getLocalBounds();
    score2Text.setOrigin(s2b.left + s2b.width, 0.f);
    score2Text.setPosition(1170.0f, 40.0f);
    window.draw(score2Text);

    // Turn text
    std::string turnStr = player1Turn
        ? player1 + "'S TURN"
        : player2 + "'S TURN";
    turnText.setString(turnStr);
    centerText(turnText, 600.0f, 660.0f);
    window.draw(turnText);

    // Draw cards
    for (int i = 0; i < activeCards; i++)
    {
        if (cards[i].matched || cards[i].faceUp)
            window.draw(cards[i].frontSprite);
        else
            window.draw(cards[i].backSprite);

        window.draw(cards[i].highlight);
    }
}

void Cards::drawResult(sf::RenderWindow& window)
{
    drawGame(window);

    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(1200.0f, 700.0f));
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(overlay);

    sf::RectangleShape panel;
    panel.setSize(sf::Vector2f(680.0f, 250.0f));
    panel.setPosition(260.0f, 245.0f);
    panel.setFillColor(sf::Color(80, 5, 18));
    panel.setOutlineThickness(3.0f);
    panel.setOutlineColor(sf::Color(180, 60, 80));
    window.draw(panel);

    window.draw(resultText);
    window.draw(playAgainButton);
    window.draw(playAgainButtonText);
    window.draw(resultLeaderboardButton);
    window.draw(resultLeaderboardButtonText);
    window.draw(menuButton);
    window.draw(menuButtonText);
}

// handle mouse clicks and keyboard input
void Cards::handleSetupClick(sf::Vector2f mousePos, sf::RenderWindow& window)
{
    if (player1Box.getGlobalBounds().contains(mousePos))
    {
        typingP1 = true;  typingP2 = false;
    }
    else if (player2Box.getGlobalBounds().contains(mousePos))
    {
        typingP1 = false; typingP2 = true;
    }
    else if (startButton.getGlobalBounds().contains(mousePos))
    {
        if (!player1.empty() && !player2.empty())
            gameStarted = true;
    }
    else if (leaderboardButton.getGlobalBounds().contains(mousePos))
        showLeaderboard(window);
    else if (backButton.getGlobalBounds().contains(mousePos))
        returnToMenu = true;
    else
    {
        for (int i = 0; i < 2; i++)
        {
            if (diffButtons[i].getGlobalBounds().contains(mousePos))
            {
                difficulty = i;
                diffButtons[0].setOutlineColor(sf::Color(150, 150, 150));
                diffButtons[0].setOutlineThickness(3.f);
                diffButtons[1].setOutlineColor(sf::Color(150, 150, 150));
                diffButtons[1].setOutlineThickness(3.f);
                break;
            }
        }
    }

    updateInputBoxStyles();
}

void Cards::handleTextInput(const sf::Event& event)
{
    if (event.type != sf::Event::TextEntered) return;
    unsigned int u = event.text.unicode;

    if (u == 8)
    {
        if (typingP1 && !player1.empty())      player1.pop_back();
        else if (typingP2 && !player2.empty()) player2.pop_back();
    }
    else if (u == 13) { return; }
    else if (u >= 32 && u <= 126)
    {
        char ch = static_cast<char>(u);
        if (typingP1 && (int)player1.size() < 15) player1 += ch;
        else if (typingP2 && (int)player2.size() < 15) player2 += ch;
    }

    player1InputText.setString(player1);
    player2InputText.setString(player2);
}

void Cards::handleResultClick(sf::Vector2f mousePos, sf::RenderWindow& window)
{
    if (playAgainButton.getGlobalBounds().contains(mousePos))
    {
        reset();
    }
    else if (resultLeaderboardButton.getGlobalBounds().contains(mousePos))
        showLeaderboard(window);
    else if (menuButton.getGlobalBounds().contains(mousePos))
        returnToMenu = true;
}

// update button colors on hover
void Cards::updateInputBoxStyles()
{
    player1Box.setOutlineColor(typingP1 ? sf::Color(255, 220, 80) : sf::Color(90, 50, 20));
    player1Box.setOutlineThickness(typingP1 ? 4.0f : 3.0f);
    player2Box.setOutlineColor(typingP2 ? sf::Color(255, 220, 80) : sf::Color(90, 50, 20));
    player2Box.setOutlineThickness(typingP2 ? 4.0f : 3.0f);
}

void Cards::updateSetupHover(sf::Vector2f mp)
{
    startButton.setFillColor(startButton.getGlobalBounds().contains(mp)
        ? sf::Color(60, 60, 60) : sf::Color(20, 20, 20));
    leaderboardButton.setFillColor(leaderboardButton.getGlobalBounds().contains(mp)
        ? sf::Color(60, 60, 60) : sf::Color(20, 20, 20));
    backButton.setFillColor(backButton.getGlobalBounds().contains(mp)
        ? sf::Color(60, 60, 60) : sf::Color(20, 20, 20));

    // Diff button hover
    for (int i = 0; i < 2; i++)
    {
        if (i != difficulty)
            diffButtons[i].setFillColor(diffButtons[i].getGlobalBounds().contains(mp)
                ? sf::Color(50, 50, 50) : sf::Color(20, 20, 20));
    }
}

void Cards::updateResultHover(sf::Vector2f mp)
{
    playAgainButton.setFillColor(playAgainButton.getGlobalBounds().contains(mp)
        ? sf::Color(100, 15, 30) : sf::Color(60, 8, 18));
    resultLeaderboardButton.setFillColor(resultLeaderboardButton.getGlobalBounds().contains(mp)
        ? sf::Color(100, 15, 30) : sf::Color(60, 8, 18));
    menuButton.setFillColor(menuButton.getGlobalBounds().contains(mp)
        ? sf::Color(100, 15, 30) : sf::Color(60, 8, 18));
}

// small helper functions
void Cards::centerText(sf::Text& text, float cx, float cy)
{
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    text.setPosition(cx, cy);
}

void Cards::centerTextInButton(sf::Text& text, const sf::RectangleShape& btn)
{
    sf::FloatRect tb = text.getLocalBounds();
    sf::FloatRect bb = btn.getGlobalBounds();
    text.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    text.setPosition(bb.left + bb.width / 2.f, bb.top + bb.height / 2.f);
}

void Cards::showLeaderboard(sf::RenderWindow& window)
{
    FileManager lb;
    lb.run(window, "CARDS");
}

// Main run loop
void Cards::run(sf::RenderWindow& window)
{
    sf::Event event;

    // phase 1: setup
    while (window.isOpen() && !gameStarted && !returnToMenu)
    {
        sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        updateSetupHover(mp);

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { window.close(); return; }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
                handleSetupClick(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)), window);

            handleTextInput(event);
        }

        window.clear();
        drawSetup(window);
        window.display();
    }

    if (returnToMenu || !window.isOpen()) return;

    // phase 2 & 3: game + result
    // Apply difficulty settings before building the board
    if (difficulty == CARDS_DIFF_EASY)
    {
        gridRows = 2;
        gridCols = 4;
        activeCards = 8;
    }
    else
    {
        gridRows = 4;
        gridCols = 4;
        activeCards = 16;
    }
    initBoard();
    setupGameUI();
    setupResultUI();

    while (window.isOpen() && !returnToMenu)
    {
        sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // Handle delayed flip-back
        if (waitingToHide && flipClock.getElapsedTime().asSeconds() >= 1.0f)
        {
            cards[firstFlip].faceUp = false;
            cards[secondFlip].faceUp = false;
            firstFlip = -1;
            secondFlip = -1;
            waitingToHide = false;
            player1Turn = !player1Turn; // switch turn on mismatch
        }

        // Hover highlight on unflipped cards
        if (!gameOver && !waitingToHide)
        {
            for (int i = 0; i < activeCards; i++)
            {
                if (!cards[i].matched && !cards[i].faceUp &&
                    cards[i].backSprite.getGlobalBounds().contains(mp))
                    cards[i].highlight.setOutlineColor(sf::Color(255, 220, 80));
                else if (!cards[i].matched)
                    cards[i].highlight.setOutlineColor(sf::Color::Transparent);
            }
        }

        if (gameOver) updateResultHover(mp);

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { window.close(); return; }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f click = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (gameOver) handleResultClick(click, window);
                else          handleCardClick(click);
            }
        }

        window.clear();
        if (gameOver) drawResult(window);
        else          drawGame(window);
        window.display();
    }
}