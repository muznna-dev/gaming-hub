#include "TicTacToe.h"
#include "FileManager.h"
#include <iostream>

TicTacToe::TicTacToe()
{
    setupPhase = PHASE_NAMES;
    gameOver = false;
    drawMatch = false;
    returnToMenu = false;
    player1Turn = true;
    typingP1 = false;
    typingP2 = false;
    player1 = "";
    player2 = "";
    winnerName = "";
    sym1 = -1;
    sym2 = -1;

    symNames[0] = "Banana";
    symNames[1] = "Sad Face";
    symNames[2] = "X";
    symNames[3] = "Cat";
    symNames[4] = "O";

    reset();
    loadAssets();
    setupBackground();
    setupTexts();
    setupInputBoxes();
    setupButtons();
    setupSymbolPicker();
}

// TicTacToe destructor
TicTacToe::~TicTacToe()
{
    // SFML cleans up textures, fonts and sounds automatically
}

void TicTacToe::reset()
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            board[r][c] = 0;
    gameOver = false;
    drawMatch = false;
    winnerName = "";
    player1Turn = true;
}

void TicTacToe::loadAssets()
{
    if (!font.loadFromFile("assets/fonts/pixel.ttf"))
        std::cout << "Error: could not load font\n";
    if (!backgroundTexture.loadFromFile("assets/images/tictactoe1.png"))
        std::cout << "Error: could not load background\n";
    backgroundTexture.setSmooth(true);

    if (!winBuffer.loadFromFile("assets/sounds/win.wav"))
        std::cout << "Error: could not load win.wav\n";
    if (!drawBuffer.loadFromFile("assets/sounds/draw.wav"))
        std::cout << "Error: could not load draw.wav\n";

    // Only load image-based symbols (indices 0,1,3)
    // Index 2 = X and index 4 = O are drawn as text, no texture needed
    std::string symFiles[NUM_SYMBOLS] = {
        "assets/images/sym_banana.png", "assets/images/sym_sadface.png", "", // X — drawn as text
        "assets/images/sym_cat.png", ""    // O — drawn as text
    };
    for (int i = 0; i < NUM_SYMBOLS; i++)
    {
        if (symFiles[i].empty()) continue;
        if (!symTextures[i].loadFromFile(symFiles[i]))
            std::cout << "Error: could not load " << symFiles[i] << "\n";
        symTextures[i].setSmooth(true);
    }
}

void TicTacToe::setupBackground()
{
    backgroundSprite.setTexture(backgroundTexture);
    if (backgroundTexture.getSize().x > 0 && backgroundTexture.getSize().y > 0)
        backgroundSprite.setScale(1200.f / backgroundTexture.getSize().x, 700.f / backgroundTexture.getSize().y);
}

void TicTacToe::setupTexts()
{
    titleText.setFont(font);
    titleText.setString("TIC TAC TOE");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color(255, 230, 140));
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(4.f);
    centerText(titleText, 600.f, 60.f);

    subtitleText.setFont(font);
    subtitleText.setString("ENTER PLAYER NAMES");
    subtitleText.setCharacterSize(18);
    subtitleText.setFillColor(sf::Color(220, 220, 220));
    subtitleText.setOutlineColor(sf::Color::Black);
    subtitleText.setOutlineThickness(2.f);
    centerText(subtitleText, 600.f, 120.f);

    player1Label.setFont(font);
    player1Label.setString("PLAYER 1");
    player1Label.setCharacterSize(17);
    player1Label.setFillColor(sf::Color(255, 235, 180));
    player1Label.setOutlineColor(sf::Color::Black);
    player1Label.setOutlineThickness(2.f);
    player1Label.setPosition(340.f, 170.f);

    player2Label.setFont(font);
    player2Label.setString("PLAYER 2");
    player2Label.setCharacterSize(17);
    player2Label.setFillColor(sf::Color(255, 235, 180));
    player2Label.setOutlineColor(sf::Color::Black);
    player2Label.setOutlineThickness(2.f);
    player2Label.setPosition(340.f, 300.f);

    infoText.setFont(font);
    infoText.setString("CLICK A BOX TO TYPE");
    infoText.setCharacterSize(15);
    infoText.setFillColor(sf::Color(255, 235, 160));
    infoText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineThickness(2.f);
    centerText(infoText, 600.f, 430.f);

    pickPromptText.setFont(font);
    pickPromptText.setCharacterSize(28);
    pickPromptText.setFillColor(sf::Color(255, 230, 140));
    pickPromptText.setOutlineColor(sf::Color::Black);
    pickPromptText.setOutlineThickness(3.f);

    player1InputText.setFont(font);
    player1InputText.setCharacterSize(20);
    player1InputText.setFillColor(sf::Color(20, 20, 20));
    player1InputText.setPosition(350.f, 205.f);

    player2InputText.setFont(font);
    player2InputText.setCharacterSize(20);
    player2InputText.setFillColor(sf::Color(20, 20, 20));
    player2InputText.setPosition(350.f, 335.f);
}

void TicTacToe::setupInputBoxes()
{
    player1Box.setSize(sf::Vector2f(520.f, 48.f));
    player1Box.setPosition(340.f, 195.f);
    player1Box.setFillColor(sf::Color(240, 235, 220));
    player1Box.setOutlineThickness(3.f);
    player1Box.setOutlineColor(sf::Color(90, 50, 20));

    player2Box.setSize(sf::Vector2f(520.f, 48.f));
    player2Box.setPosition(340.f, 325.f);
    player2Box.setFillColor(sf::Color(240, 235, 220));
    player2Box.setOutlineThickness(3.f);
    player2Box.setOutlineColor(sf::Color(90, 50, 20));
}

void TicTacToe::setupButtons()
{
    sf::Color navyNormal(20, 50, 100);
    sf::Color navyMid(15, 40, 85);
    sf::Color navyDark(12, 30, 70);
    sf::Color rim(100, 160, 220);

    const float BTN_W = 155.f;
    const float BTN_H = 52.f;
    const float GAP = 20.f;
    const float TOTAL = 3 * BTN_W + 2 * GAP;
    const float LEFT_X = 600.f - TOTAL / 2.f;

    // NEXT button for names phase
    nextButton.setSize(sf::Vector2f(200.f, BTN_H));
    nextButton.setPosition(440.f, 490.f);
    nextButton.setFillColor(navyNormal);
    nextButton.setOutlineThickness(3.f);
    nextButton.setOutlineColor(rim);
    nextButtonText.setFont(font);
    nextButtonText.setString("NEXT  >");
    nextButtonText.setCharacterSize(22);
    nextButtonText.setFillColor(sf::Color::White);
    nextButtonText.setOutlineColor(sf::Color::Black);
    nextButtonText.setOutlineThickness(2.f);
    centerTextInButton(nextButtonText, nextButton);

    // PLAY | SCORES | BACK for symbol phases
    startButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    startButton.setPosition(LEFT_X, 560.f);
    startButton.setFillColor(navyNormal);
    startButton.setOutlineThickness(3.f);
    startButton.setOutlineColor(rim);
    startButtonText.setFont(font);
    startButtonText.setString("PLAY");
    startButtonText.setCharacterSize(20);
    startButtonText.setFillColor(sf::Color::White);
    startButtonText.setOutlineColor(sf::Color::Black);
    startButtonText.setOutlineThickness(2.f);
    centerTextInButton(startButtonText, startButton);

    leaderboardButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    leaderboardButton.setPosition(LEFT_X + BTN_W + GAP, 560.f);
    leaderboardButton.setFillColor(navyMid);
    leaderboardButton.setOutlineThickness(3.f);
    leaderboardButton.setOutlineColor(rim);
    leaderboardButtonText.setFont(font);
    leaderboardButtonText.setString("SCORES");
    leaderboardButtonText.setCharacterSize(20);
    leaderboardButtonText.setFillColor(sf::Color::White);
    leaderboardButtonText.setOutlineColor(sf::Color::Black);
    leaderboardButtonText.setOutlineThickness(2.f);
    centerTextInButton(leaderboardButtonText, leaderboardButton);

    backButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    backButton.setPosition(LEFT_X + 2 * (BTN_W + GAP), 560.f);
    backButton.setFillColor(navyDark);
    backButton.setOutlineThickness(3.f);
    backButton.setOutlineColor(rim);
    backButtonText.setFont(font);
    backButtonText.setString("BACK");
    backButtonText.setCharacterSize(20);
    backButtonText.setFillColor(sf::Color::White);
    backButtonText.setOutlineColor(sf::Color::Black);
    backButtonText.setOutlineThickness(2.f);
    centerTextInButton(backButtonText, backButton);
}

void TicTacToe::setupSymbolPicker()
{
    const float BOX_SZ = 100.f;
    const float GAP = 20.f;
    const float TOTAL_W = NUM_SYMBOLS * BOX_SZ + (NUM_SYMBOLS - 1) * GAP;
    const float START_X = 600.f - TOTAL_W / 2.f;
    const float ROW_Y = 295.f;

    for (int i = 0; i < NUM_SYMBOLS; i++)
    {
        float x = START_X + i * (BOX_SZ + GAP);
        symBoxes[i].setSize(sf::Vector2f(BOX_SZ, BOX_SZ));
        symBoxes[i].setPosition(x, ROW_Y);
        symBoxes[i].setFillColor(sf::Color(30, 30, 55));
        symBoxes[i].setOutlineThickness(3.f);
        symBoxes[i].setOutlineColor(sf::Color(100, 160, 220));

        // Indices 2 (X) and 4 (O) are text-only — skip texture loading
        if (i == 2 || i == 4) continue;

        symPreviews[i].setTexture(symTextures[i], true);
        sf::Vector2u ts = symTextures[i].getSize();
        if (ts.x > 0 && ts.y > 0)
        {
            float biggest = (float)(ts.x > ts.y ? ts.x : ts.y);
            float scale = (BOX_SZ - 10.f) / biggest;
            symPreviews[i].setScale(scale, scale);
        }
        sf::FloatRect sb = symPreviews[i].getLocalBounds();
        symPreviews[i].setOrigin(sb.width / 2.f, sb.height / 2.f);
        symPreviews[i].setPosition(x + BOX_SZ / 2.f, ROW_Y + BOX_SZ / 2.f);
    }
}

void TicTacToe::setupGameBoard()
{
    const float CELL = 140.f;
    const float BORDER = 4.f;
    const float GRID_X = 600.f - (CELL * 1.5f);
    const float GRID_Y = 700.f / 2.f - (CELL * 1.5f) + 20.f;

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
        {
            float x = GRID_X + c * CELL;
            float y = GRID_Y + r * CELL;
            cells[r][c].setSize(sf::Vector2f(CELL - BORDER, CELL - BORDER));
            cells[r][c].setPosition(x + BORDER / 2.f, y + BORDER / 2.f);
            cells[r][c].setFillColor(sf::Color(30, 30, 50));
            cells[r][c].setOutlineThickness(3.f);
            cells[r][c].setOutlineColor(sf::Color(100, 160, 220));
            cellSprites[r][c] = sf::Sprite();
            cellTexts[r][c] = sf::Text();
        }
}

void TicTacToe::setupGameUI()
{
    turnText.setFont(font);
    turnText.setCharacterSize(22);
    turnText.setFillColor(sf::Color(255, 220, 100));
    turnText.setOutlineColor(sf::Color::Black);
    turnText.setOutlineThickness(3.f);

    gamePlayer1Label.setFont(font);
    gamePlayer1Label.setCharacterSize(32);
    gamePlayer1Label.setFillColor(sf::Color::White);
    gamePlayer1Label.setOutlineColor(sf::Color::Black);
    gamePlayer1Label.setOutlineThickness(2.f);
    gamePlayer1Label.setPosition(30.f, 20.f);

    gamePlayer2Label.setFont(font);
    gamePlayer2Label.setCharacterSize(32);
    gamePlayer2Label.setFillColor(sf::Color::White);
    gamePlayer2Label.setOutlineColor(sf::Color::Black);
    gamePlayer2Label.setOutlineThickness(2.f);
}

void TicTacToe::setupResultScreen()
{
    sf::Color navyNormal(20, 50, 100);
    sf::Color navyMid(15, 40, 85);
    sf::Color navyDark(12, 30, 70);
    sf::Color rim(100, 160, 220);

    resultText.setFont(font);
    resultText.setCharacterSize(36);
    resultText.setFillColor(sf::Color(255, 220, 100));
    resultText.setOutlineColor(sf::Color::Black);
    resultText.setOutlineThickness(4.f);

    const float BTN_W = 155.f;
    const float BTN_H = 52.f;
    const float GAP = 20.f;
    const float BTN_Y = 375.f;
    const float TOTAL = 3 * BTN_W + 2 * GAP;
    const float LEFT_X = 600.f - TOTAL / 2.f;

    playAgainButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    playAgainButton.setPosition(LEFT_X, BTN_Y);
    playAgainButton.setFillColor(navyNormal);
    playAgainButton.setOutlineThickness(3.f);
    playAgainButton.setOutlineColor(rim);
    playAgainButtonText.setFont(font);
    playAgainButtonText.setString("PLAY AGAIN");
    playAgainButtonText.setCharacterSize(16);
    playAgainButtonText.setFillColor(sf::Color::White);
    playAgainButtonText.setOutlineColor(sf::Color::Black);
    playAgainButtonText.setOutlineThickness(2.f);
    centerTextInButton(playAgainButtonText, playAgainButton);

    resultLeaderboardButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    resultLeaderboardButton.setPosition(LEFT_X + BTN_W + GAP, BTN_Y);
    resultLeaderboardButton.setFillColor(navyMid);
    resultLeaderboardButton.setOutlineThickness(3.f);
    resultLeaderboardButton.setOutlineColor(rim);
    resultLeaderboardButtonText.setFont(font);
    resultLeaderboardButtonText.setString("SCORES");
    resultLeaderboardButtonText.setCharacterSize(16);
    resultLeaderboardButtonText.setFillColor(sf::Color::White);
    resultLeaderboardButtonText.setOutlineColor(sf::Color::Black);
    resultLeaderboardButtonText.setOutlineThickness(2.f);
    centerTextInButton(resultLeaderboardButtonText, resultLeaderboardButton);

    menuButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    menuButton.setPosition(LEFT_X + 2 * (BTN_W + GAP), BTN_Y);
    menuButton.setFillColor(navyDark);
    menuButton.setOutlineThickness(3.f);
    menuButton.setOutlineColor(rim);
    menuButtonText.setFont(font);
    menuButtonText.setString("MAIN MENU");
    menuButtonText.setCharacterSize(16);
    menuButtonText.setFillColor(sf::Color::White);
    menuButtonText.setOutlineColor(sf::Color::Black);
    menuButtonText.setOutlineThickness(2.f);
    centerTextInButton(menuButtonText, menuButton);
}

void TicTacToe::placeCellSprite(int r, int c, int symIdx)
{
    sf::FloatRect cb = cells[r][c].getGlobalBounds();
    float cx = cb.left + cb.width / 2.f;
    float cy = cb.top + cb.height / 2.f;

    if (symIdx == 2 || symIdx == 4)
    {
        // X or O — store as text in cellTexts
        cellTexts[r][c].setFont(font);
        cellTexts[r][c].setString(symIdx == 2 ? "X" : "O");
        cellTexts[r][c].setCharacterSize(64);
        cellTexts[r][c].setFillColor(symIdx == 2
            ? sf::Color(255, 255, 255)   // X = white
            : sf::Color(255, 220, 80));   // O = gold
        cellTexts[r][c].setOutlineColor(sf::Color::Black);
        cellTexts[r][c].setOutlineThickness(3.f);
        sf::FloatRect tb = cellTexts[r][c].getLocalBounds();
        cellTexts[r][c].setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        cellTexts[r][c].setPosition(cx, cy);
        return;
    }

    float cellSz = cb.width < cb.height ? cb.width : cb.height;
    float imgSz = cellSz - 16.f;

    cellSprites[r][c] = sf::Sprite();
    cellSprites[r][c].setTexture(symTextures[symIdx], true);
    sf::Vector2u ts = symTextures[symIdx].getSize();
    if (ts.x > 0 && ts.y > 0)
    {
        float biggest = (float)(ts.x > ts.y ? ts.x : ts.y);
        cellSprites[r][c].setScale(imgSz / biggest, imgSz / biggest);
    }
    sf::FloatRect sb = cellSprites[r][c].getLocalBounds();
    cellSprites[r][c].setOrigin(sb.width / 2.f, sb.height / 2.f);
    cellSprites[r][c].setPosition(cx, cy);
}

bool TicTacToe::checkWin(int playerSign)
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] * playerSign > 0 && board[i][1] * playerSign > 0 && board[i][2] * playerSign > 0) return true;
        if (board[0][i] * playerSign > 0 && board[1][i] * playerSign > 0 && board[2][i] * playerSign > 0) return true;
    }
    if (board[0][0] * playerSign > 0 && board[1][1] * playerSign > 0 && board[2][2] * playerSign > 0) return true;
    if (board[0][2] * playerSign > 0 && board[1][1] * playerSign > 0 && board[2][0] * playerSign > 0) return true;
    return false;
}

bool TicTacToe::checkDraw()
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (board[r][c] == 0) return false;
    return true;
}

void TicTacToe::handleBoardClick(sf::Vector2f mousePos)
{
    if (gameOver) return;
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (cells[r][c].getGlobalBounds().contains(mousePos) && board[r][c] == 0)
            {
                board[r][c] = player1Turn ? 1 : -1;
                placeCellSprite(r, c, player1Turn ? sym1 : sym2);

                if (checkWin(player1Turn ? 1 : -1))
                {
                    gameOver = true;
                    winnerName = player1Turn ? player1 : player2;
                    std::string loser = player1Turn ? player2 : player1;
                    FileManager::SaveEntry(winnerName, "TICTACTOE", 1);
                    FileManager::SaveEntry(loser, "TICTACTOE", 0);
                    gameSound.setBuffer(winBuffer);
                    gameSound.play();
                    resultText.setString(winnerName + " WINS!");
                    centerText(resultText, 600.f, 295.f);
                }
                else if (checkDraw())
                {
                    gameOver = true;
                    drawMatch = true;
                    FileManager::SaveEntry(player1, "TICTACTOE", 0);
                    FileManager::SaveEntry(player2, "TICTACTOE", 0);
                    gameSound.setBuffer(drawBuffer);
                    gameSound.play();
                    resultText.setString("IT'S A DRAW!");
                    centerText(resultText, 600.f, 295.f);
                }
                else
                    player1Turn = !player1Turn;
                return;
            }
}

void TicTacToe::drawNamesPhase(sf::RenderWindow& window)
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
    // Position back button beside NEXT for names phase
    backButton.setPosition(660.f, 490.f);
    centerTextInButton(backButtonText, backButton);
    window.draw(nextButton);
    window.draw(nextButtonText);
    window.draw(backButton);
    window.draw(backButtonText);
}

void TicTacToe::drawSymbolPicker(sf::RenderWindow& window, int playerNum)
{
    window.clear(sf::Color(15, 15, 30));
    window.draw(backgroundSprite);

    std::string prompt = (playerNum == 1)
        ? player1 + "  -  PICK YOUR SYMBOL"
        : player2 + "  -  PICK YOUR SYMBOL";
    pickPromptText.setString(prompt);
    centerText(pickPromptText, 600.f, 170.f);
    window.draw(pickPromptText);

    // Sub prompt showing which phase
    sf::Text subPrompt;
    subPrompt.setFont(font);
    subPrompt.setCharacterSize(16);
    subPrompt.setFillColor(sf::Color(255, 235, 160));
    subPrompt.setOutlineColor(sf::Color::Black);
    subPrompt.setOutlineThickness(2.f);
    subPrompt.setString(playerNum == 1 ? "STEP 1 OF 2" : "STEP 2 OF 2");
    centerText(subPrompt, 600.f, 215.f);
    window.draw(subPrompt);

    // Symbol name labels above boxes
    const float BOX_SZ = 100.f;
    const float GAP = 20.f;
    const float TOTAL_W = NUM_SYMBOLS * BOX_SZ + (NUM_SYMBOLS - 1) * GAP;
    const float START_X = 600.f - TOTAL_W / 2.f;
    const float ROW_Y = 295.f;

    for (int i = 0; i < NUM_SYMBOLS; i++)
    {
        bool takenByOther = (playerNum == 2 && i == sym1);
        float x = START_X + i * (BOX_SZ + GAP);

        bool isTextSym = (i == 2 || i == 4);

        // Taken text symbol (X/O) — skip entirely, no box no label
        if (takenByOther && isTextSym)
            continue;

        // Taken image symbol — show greyed box + faded image, unclickable
        symBoxes[i].setFillColor(takenByOther
            ? sf::Color(15, 15, 25)
            : sf::Color(30, 30, 55));
        window.draw(symBoxes[i]);

        if (isTextSym)
        {
            // Draw X or O as text — only if not taken (already skipped above)
            sf::Text symText;
            symText.setFont(font);
            symText.setString(i == 2 ? "X" : "O");
            symText.setCharacterSize(58);
            symText.setFillColor(i == 2
                ? sf::Color(255, 255, 255)
                : sf::Color(255, 220, 80));
            symText.setOutlineColor(sf::Color::Black);
            symText.setOutlineThickness(3.f);
            sf::FloatRect tb = symText.getLocalBounds();
            symText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
            float bx = symBoxes[i].getPosition().x + BOX_SZ / 2.f;
            float by = symBoxes[i].getPosition().y + BOX_SZ / 2.f;
            symText.setPosition(bx, by);
            window.draw(symText);
        }
        else
        {
            // Image symbol — always draw, but faded if taken
            symPreviews[i].setColor(takenByOther
                ? sf::Color(255, 255, 255, 80)   // faded
                : sf::Color(255, 255, 255, 255)); // full
            window.draw(symPreviews[i]);
        }

        // Name label under box
        sf::Text nameLabel;
        nameLabel.setFont(font);
        nameLabel.setCharacterSize(13);
        nameLabel.setFillColor(takenByOther
            ? sf::Color(100, 100, 100)
            : sf::Color(255, 235, 160));
        nameLabel.setOutlineColor(sf::Color::Black);
        nameLabel.setOutlineThickness(1.5f);
        nameLabel.setString(symNames[i]);
        sf::FloatRect nb = nameLabel.getLocalBounds();
        nameLabel.setOrigin(nb.left + nb.width / 2.f, 0.f);
        nameLabel.setPosition(x + BOX_SZ / 2.f, ROW_Y + BOX_SZ + 6.f);
        window.draw(nameLabel);
    }

    sf::Text hint;
    hint.setFont(font);
    hint.setString("CLICK A SYMBOL  |  THEN PRESS PLAY");
    hint.setCharacterSize(16);
    hint.setFillColor(sf::Color(255, 235, 160));
    hint.setOutlineColor(sf::Color::Black);
    hint.setOutlineThickness(2.f);
    centerText(hint, 600.f, 490.f);
    window.draw(hint);

    // Restore back button to symbol phase position
    const float SYM_BTN_W = 155.f;
    const float SYM_GAP = 20.f;
    const float SYM_TOTAL = 3 * SYM_BTN_W + 2 * SYM_GAP;
    const float SYM_LX = 600.f - SYM_TOTAL / 2.f;
    backButton.setPosition(SYM_LX + 2 * (SYM_BTN_W + SYM_GAP), 560.f);
    centerTextInButton(backButtonText, backButton);
    window.draw(startButton);
    window.draw(startButtonText);
    window.draw(leaderboardButton);
    window.draw(leaderboardButtonText);
    window.draw(backButton);
    window.draw(backButtonText);
}

void TicTacToe::drawGame(sf::RenderWindow& window)
{
    window.clear(sf::Color(15, 15, 30));

    gamePlayer1Label.setString(player1);
    gamePlayer2Label.setString(player2);

    sf::FloatRect p2b = gamePlayer2Label.getLocalBounds();
    gamePlayer2Label.setOrigin(p2b.left + p2b.width, 0.f);
    gamePlayer2Label.setPosition(1170.f, 20.f);

    gamePlayer1Label.setFillColor(player1Turn ? sf::Color(255, 220, 80) : sf::Color::White);
    gamePlayer2Label.setFillColor(!player1Turn ? sf::Color(255, 220, 80) : sf::Color::White);

    window.draw(gamePlayer1Label);
    window.draw(gamePlayer2Label);

    turnText.setString(player1Turn ? player1 + "'S TURN" : player2 + "'S TURN");
    centerText(turnText, 600.f, 660.f);
    window.draw(turnText);

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
        {
            window.draw(cells[r][c]);
            if (board[r][c] != 0)
            {
                int symIdx = (board[r][c] == 1) ? sym1 : sym2;
                if (symIdx == 2 || symIdx == 4)
                    window.draw(cellTexts[r][c]);
                else
                    window.draw(cellSprites[r][c]);
            }
        }
}

void TicTacToe::drawResult(sf::RenderWindow& window)
{
    drawGame(window);

    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(1200.f, 700.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    sf::RectangleShape panel;
    panel.setSize(sf::Vector2f(620.f, 230.f));
    panel.setPosition(290.f, 250.f);
    panel.setFillColor(sf::Color(15, 25, 55));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color(100, 160, 220));
    window.draw(panel);

    window.draw(resultText);
    window.draw(playAgainButton);
    window.draw(playAgainButtonText);
    window.draw(resultLeaderboardButton);
    window.draw(resultLeaderboardButtonText);
    window.draw(menuButton);
    window.draw(menuButtonText);
}

void TicTacToe::handleNamesClick(sf::Vector2f mousePos, sf::RenderWindow& window)
{
    if (player1Box.getGlobalBounds().contains(mousePos))
    {
        typingP1 = true;  typingP2 = false;
    }
    else if (player2Box.getGlobalBounds().contains(mousePos))
    {
        typingP1 = false; typingP2 = true;
    }
    else if (nextButton.getGlobalBounds().contains(mousePos))
    {
        if (!player1.empty() && !player2.empty())
            setupPhase = PHASE_P1SYM;
    }
    else if (backButton.getGlobalBounds().contains(mousePos))
        returnToMenu = true;
    updateInputBoxStyles();
}

void TicTacToe::handleSymbolClick(sf::Vector2f mousePos, int playerNum)
{
    for (int i = 0; i < NUM_SYMBOLS; i++)
    {
        bool takenByOther = (playerNum == 2 && i == sym1);
        if (takenByOther) continue;

        if (symBoxes[i].getGlobalBounds().contains(mousePos))
        {
            if (playerNum == 1) sym1 = i;
            else                sym2 = i;

            // Highlight selected, reset others
            for (int j = 0; j < NUM_SYMBOLS; j++)
            {
                symBoxes[j].setOutlineColor(sf::Color(100, 160, 220));
                symBoxes[j].setOutlineThickness(3.f);
            }
            symBoxes[i].setOutlineColor(sf::Color(255, 220, 80));
            symBoxes[i].setOutlineThickness(5.f);
            return;
        }
    }

    // PLAY — advance only if symbol chosen
    if (startButton.getGlobalBounds().contains(mousePos))
    {
        if (playerNum == 1 && sym1 != -1) setupPhase = PHASE_P2SYM;
        else if (playerNum == 2 && sym2 != -1) setupPhase = PHASE_GAME;
    }
}

void TicTacToe::handleTextInput(const sf::Event& event)
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

void TicTacToe::handleResultClick(sf::Vector2f mousePos, sf::RenderWindow& window)
{
    if (playAgainButton.getGlobalBounds().contains(mousePos))
    {
        reset();
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
            {
                cellSprites[r][c] = sf::Sprite();
                cellTexts[r][c] = sf::Text();
                cells[r][c].setFillColor(sf::Color(30, 30, 50));
            }
    }
    else if (resultLeaderboardButton.getGlobalBounds().contains(mousePos))
        showLeaderboard(window);
    else if (menuButton.getGlobalBounds().contains(mousePos))
        returnToMenu = true;
}

void TicTacToe::updateInputBoxStyles()
{
    player1Box.setOutlineColor(typingP1 ? sf::Color(255, 220, 80) : sf::Color(90, 50, 20));
    player1Box.setOutlineThickness(typingP1 ? 4.f : 3.f);
    player2Box.setOutlineColor(typingP2 ? sf::Color(255, 220, 80) : sf::Color(90, 50, 20));
    player2Box.setOutlineThickness(typingP2 ? 4.f : 3.f);
}

void TicTacToe::updateNamesHover(sf::Vector2f mp)
{
    nextButton.setFillColor(nextButton.getGlobalBounds().contains(mp)
        ? sf::Color(40, 80, 150) : sf::Color(20, 50, 100));
}

void TicTacToe::updateResultHover(sf::Vector2f mp)
{
    playAgainButton.setFillColor(playAgainButton.getGlobalBounds().contains(mp)
        ? sf::Color(40, 80, 150) : sf::Color(20, 50, 100));
    resultLeaderboardButton.setFillColor(resultLeaderboardButton.getGlobalBounds().contains(mp)
        ? sf::Color(35, 70, 135) : sf::Color(15, 40, 85));
    menuButton.setFillColor(menuButton.getGlobalBounds().contains(mp)
        ? sf::Color(25, 55, 115) : sf::Color(12, 30, 70));
}

void TicTacToe::centerText(sf::Text& text, float cx, float cy)
{
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    text.setPosition(cx, cy);
}

void TicTacToe::centerTextInButton(sf::Text& text, const sf::RectangleShape& btn)
{
    sf::FloatRect tb = text.getLocalBounds();
    sf::FloatRect bb = btn.getGlobalBounds();
    text.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    text.setPosition(bb.left + bb.width / 2.f, bb.top + bb.height / 2.f);
}

void TicTacToe::showLeaderboard(sf::RenderWindow& window)
{
    FileManager lb;
    lb.run(window, "TICTACTOE");
}

void TicTacToe::run(sf::RenderWindow& window)
{
    sf::Event event;

    // pre-compute symbol phase button positions
    const float SP_BTN_W = 155.f;
    const float SP_GAP = 20.f;
    const float SP_TOTAL = 3 * SP_BTN_W + 2 * SP_GAP;
    const float SP_LX = 600.f - SP_TOTAL / 2.f;
    startButton.setPosition(SP_LX, 560.f);
    leaderboardButton.setPosition(SP_LX + SP_BTN_W + SP_GAP, 560.f);
    backButton.setPosition(SP_LX + 2 * (SP_BTN_W + SP_GAP), 560.f);
    centerTextInButton(startButtonText, startButton);
    centerTextInButton(leaderboardButtonText, leaderboardButton);
    centerTextInButton(backButtonText, backButton);

    // outer setup loop — keeps cycling until game starts or exit
    while (window.isOpen() && setupPhase != PHASE_GAME && !returnToMenu)
    {
        // names phase
        if (setupPhase == PHASE_NAMES)
        {
            // Reposition back button for names phase
            backButton.setPosition(660.f, 490.f);
            centerTextInButton(backButtonText, backButton);

            while (window.isOpen() && setupPhase == PHASE_NAMES && !returnToMenu)
            {
                sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                updateNamesHover(mp);
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed) { window.close(); return; }
                    if (event.type == sf::Event::MouseButtonPressed &&
                        event.mouseButton.button == sf::Mouse::Left)
                        handleNamesClick(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)), window);
                    handleTextInput(event);
                }
                window.clear();
                drawNamesPhase(window);
                window.display();
            }
        }

        if (returnToMenu || !window.isOpen()) return;

        // p1 symbol phase
        if (setupPhase == PHASE_P1SYM)
        {
            // Reposition buttons for symbol phase
            backButton.setPosition(SP_LX + 2 * (SP_BTN_W + SP_GAP), 560.f);
            centerTextInButton(backButtonText, backButton);
            for (int i = 0; i < NUM_SYMBOLS; i++)
            {
                symBoxes[i].setOutlineColor(sf::Color(100, 160, 220)); symBoxes[i].setOutlineThickness(3.f);
            }

            while (window.isOpen() && setupPhase == PHASE_P1SYM && !returnToMenu)
            {
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed) { window.close(); return; }
                    if (event.type == sf::Event::MouseButtonPressed &&
                        event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::Vector2f click = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                        if (backButton.getGlobalBounds().contains(click)) { setupPhase = PHASE_NAMES; break; }
                        if (leaderboardButton.getGlobalBounds().contains(click)) { showLeaderboard(window); break; }
                        handleSymbolClick(click, 1);
                    }
                }
                window.clear();
                drawSymbolPicker(window, 1);
                window.display();
            }
        }

        if (returnToMenu || !window.isOpen()) return;

        // p2 symbol phase
        if (setupPhase == PHASE_P2SYM)
        {
            backButton.setPosition(SP_LX + 2 * (SP_BTN_W + SP_GAP), 560.f);
            centerTextInButton(backButtonText, backButton);
            for (int i = 0; i < NUM_SYMBOLS; i++)
            {
                symBoxes[i].setOutlineColor(sf::Color(100, 160, 220)); symBoxes[i].setOutlineThickness(3.f);
            }

            while (window.isOpen() && setupPhase == PHASE_P2SYM && !returnToMenu)
            {
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed) { window.close(); return; }
                    if (event.type == sf::Event::MouseButtonPressed &&
                        event.mouseButton.button == sf::Mouse::Left)
                    {
                        sf::Vector2f click = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                        if (backButton.getGlobalBounds().contains(click)) { setupPhase = PHASE_P1SYM; break; }
                        if (leaderboardButton.getGlobalBounds().contains(click)) { showLeaderboard(window); break; }
                        handleSymbolClick(click, 2);
                    }
                }
                window.clear();
                drawSymbolPicker(window, 2);
                window.display();
            }
        }

        if (returnToMenu || !window.isOpen()) return;
    }

    if (returnToMenu || !window.isOpen()) return;

    // Phase 4 & 5: Game + Result
    setupGameBoard();
    setupGameUI();
    setupResultScreen();

    while (window.isOpen() && !returnToMenu)
    {
        sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        if (gameOver)
            updateResultHover(mp);
        else
            for (int r = 0; r < 3; r++)
                for (int c = 0; c < 3; c++)
                {
                    if (board[r][c] == 0 && cells[r][c].getGlobalBounds().contains(mp))
                        cells[r][c].setFillColor(sf::Color(45, 45, 75));
                    else if (board[r][c] == 0)
                        cells[r][c].setFillColor(sf::Color(30, 30, 50));
                }

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { window.close(); return; }
            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f click = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (gameOver) handleResultClick(click, window);
                else          handleBoardClick(click);
            }
        }

        window.clear();
        if (gameOver) drawResult(window);
        else          drawGame(window);
        window.display();
    }
}

bool TicTacToe::shouldReturnToMenu() const { return returnToMenu; }
std::string TicTacToe::getPlayer1Name()     const { return player1; }
std::string TicTacToe::getPlayer2Name()     const { return player2; }