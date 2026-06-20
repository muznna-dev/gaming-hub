#include "Pong.h"
#include "FileManager.h"
#include <iostream>

// initialise window and load everything
Pong::Pong()
{
    gameStarted = false;
    gameOver = false;
    returnToMenu = false;
    ballLaunched = false;
    typingP1 = false;
    typingP2 = false;
    difficulty = DIFF_EASY;
    player1 = "";
    player2 = "";
    winnerName = "";
    score1 = 0;
    score2 = 0;
    winScore = 5;
    ballVX = 0.f;
    ballVY = 0.f;
    ballSpeed = 0.f;
    paddleSpeed = 0.f;
    dashCount = 0;

    loadAssets();
    setupBackground();
    setupSetupUI();
}

// Pong destructor
Pong::~Pong()
{
    // SFML cleans up textures, fonts and sounds automatically
}


// intToStr — manual int to string, no std::to_string
std::string Pong::intToStr(int n)
{
    if (n == 0) return "0";
    std::string result = "";
    bool negative = (n < 0);
    if (negative) n = -n;
    while (n > 0)
    {
        result = (char)('0' + n % 10) + result;
        n /= 10;
    }
    if (negative) result = "-" + result;
    return result;
}

// load fonts, textures, sounds
void Pong::loadAssets()
{
    if (!font.loadFromFile("assets/fonts/pixel.ttf"))
        std::cout << "Error: could not load font\n";

    if (!backgroundTexture.loadFromFile("assets/images/pong_bg.png"))
        if (!backgroundTexture.loadFromFile("assets/images/hub_bg.jpg"))
            std::cout << "Error: could not load pong background\n";

    backgroundTexture.setSmooth(true);

    if (!winBuffer.loadFromFile("assets/sounds/win.wav"))
        std::cout << "Error: could not load win.wav\n";
}

void Pong::setupBackground()
{
    backgroundSprite.setTexture(backgroundTexture);
    if (backgroundTexture.getSize().x > 0 && backgroundTexture.getSize().y > 0)
    {
        backgroundSprite.setScale(1200.0f / backgroundTexture.getSize().x, 700.0f / backgroundTexture.getSize().y
        );
    }
}

// applyDifficulty — sets ball speed, paddle size, win score
void Pong::applyDifficulty()
{
    if (difficulty == DIFF_EASY)
    {
        ballSpeed = 350.f;
        paddleSpeed = 420.f;
        winScore = 5;
        paddle1.setSize(sf::Vector2f(14.f, 110.f));
        paddle2.setSize(sf::Vector2f(14.f, 110.f));
    }
    else if (difficulty == DIFF_MEDIUM)
    {
        ballSpeed = 480.f;
        paddleSpeed = 480.f;
        winScore = 7;
        paddle1.setSize(sf::Vector2f(14.f, 85.f));
        paddle2.setSize(sf::Vector2f(14.f, 85.f));
    }
    else // HARD
    {
        ballSpeed = 620.f;
        paddleSpeed = 540.f;
        winScore = 10;
        paddle1.setSize(sf::Vector2f(14.f, 60.f));
        paddle2.setSize(sf::Vector2f(14.f, 60.f));
    }
}

// resetBall — centres ball and gives it a random direction
void Pong::resetBall()
{
    ball.setPosition(600.f - 8.f, 350.f - 8.f);
    ballLaunched = false;
    ballVX = 0.f;
    ballVY = 0.f;
}

// reset — full game restart keeping player names
void Pong::reset()
{
    score1 = 0;
    score2 = 0;
    gameOver = false;
    winnerName = "";

    applyDifficulty();

    // Reset paddle positions
    paddle1.setPosition(30.f, 350.f - paddle1.getSize().y / 2.f);
    paddle2.setPosition(1156.f, 350.f - paddle2.getSize().y / 2.f);

    resetBall();
}

// setupCourt — centre dashes and paddles
void Pong::setupCourt()
{
    // ball shape and velocity
    ball.setSize(sf::Vector2f(16.f, 16.f));
    ball.setFillColor(sf::Color::White);

    // left and right paddles
    paddle1.setSize(sf::Vector2f(14.f, 110.f));
    paddle1.setFillColor(sf::Color(120, 200, 255));
    paddle1.setPosition(30.f, 350.f - 55.f);

    paddle2.setSize(sf::Vector2f(14.f, 110.f));
    paddle2.setFillColor(sf::Color(180, 100, 255));
    paddle2.setPosition(1156.f, 350.f - 55.f);

    // Centre dashes
    dashCount = 0;
    float dashH = 20.f;
    float dashGap = 14.f;
    float y = 10.f;
    while (y < 700.f && dashCount < 20)
    {
        dashRects[dashCount].setSize(sf::Vector2f(4.f, dashH));
        dashRects[dashCount].setFillColor(sf::Color(180, 180, 180, 160));
        dashRects[dashCount].setPosition(598.f, y);
        dashCount++;
        y += dashH + dashGap;
    }

    resetBall();
}

// all the UI for the name entry screen
void Pong::setupSetupUI()
{
    // layout constants — everything derived from these
    // Image has "WELCOME TO PONG" filling top ~200px and court in middle
    // We place UI in the bottom portion so it sits inside the court area
    const float BOX_W = 460.f;
    const float BOX_H = 40.f;
    const float BOX_X = 370.f;
    const float START_Y = 200.f;      // moved up

    const float P1_LABEL_Y = START_Y;
    const float P1_BOX_Y = START_Y + 26.f;
    const float P2_LABEL_Y = P1_BOX_Y + BOX_H + 40.f;  // more spacing
    const float P2_BOX_Y = P2_LABEL_Y + 26.f;
    const float INFO_Y = P2_BOX_Y + BOX_H + 28.f; // more spacing
    const float DIFF_Y = INFO_Y + 38.f;          // more spacing
    const float BTN_Y = DIFF_Y + 62.f;          // more spacing

    // player labels
    player1Label.setFont(font);
    player1Label.setString("PLAYER 1  (Left paddle)");
    player1Label.setCharacterSize(16);
    player1Label.setFillColor(sf::Color(120, 200, 255));
    player1Label.setOutlineColor(sf::Color::Black);
    player1Label.setOutlineThickness(2.f);
    player1Label.setPosition(BOX_X, P1_LABEL_Y);

    player2Label.setFont(font);
    player2Label.setString("PLAYER 2  (Right paddle)");
    player2Label.setCharacterSize(16);
    player2Label.setFillColor(sf::Color(180, 100, 255));
    player2Label.setOutlineColor(sf::Color::Black);
    player2Label.setOutlineThickness(2.f);
    player2Label.setPosition(BOX_X, P2_LABEL_Y);

    // input boxes
    player1Box.setSize(sf::Vector2f(BOX_W, BOX_H));
    player1Box.setPosition(BOX_X, P1_BOX_Y);
    player1Box.setFillColor(sf::Color(240, 235, 220));
    player1Box.setOutlineThickness(3.f);
    player1Box.setOutlineColor(sf::Color(90, 50, 20));

    player2Box.setSize(sf::Vector2f(BOX_W, BOX_H));
    player2Box.setPosition(BOX_X, P2_BOX_Y);
    player2Box.setFillColor(sf::Color(240, 235, 220));
    player2Box.setOutlineThickness(3.f);
    player2Box.setOutlineColor(sf::Color(90, 50, 20));

    player1InputText.setFont(font);
    player1InputText.setCharacterSize(20);
    player1InputText.setFillColor(sf::Color(20, 20, 20));
    player1InputText.setPosition(BOX_X + 10.f, P1_BOX_Y + 8.f);

    player2InputText.setFont(font);
    player2InputText.setCharacterSize(20);
    player2InputText.setFillColor(sf::Color(20, 20, 20));
    player2InputText.setPosition(BOX_X + 10.f, P2_BOX_Y + 8.f);

    // info text
    infoText.setFont(font);
    infoText.setString("CLICK A BOX TO TYPE");
    infoText.setCharacterSize(14);
    infoText.setFillColor(sf::Color::White);
    infoText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineThickness(2.f);
    centerText(infoText, 600.f, INFO_Y);

    // difficulty buttons
    std::string diffLabels[3] = { "EASY  (5 pts)", "MEDIUM  (7 pts)", "HARD  (10 pts)" };
    sf::Color diffColors[3] = {
        sf::Color(55, 10, 90), sf::Color(70, 15, 110), sf::Color(90, 20, 140)
    };

    const float DBW = 210.f;
    const float DBH = 44.f;
    const float DGAP = 18.f;
    const float DTOTAL = 3 * DBW + 2 * DGAP;
    const float DLX = 600.f - DTOTAL / 2.f;

    for (int i = 0; i < 3; i++)
    {
        diffButtons[i].setSize(sf::Vector2f(DBW, DBH));
        diffButtons[i].setPosition(DLX + i * (DBW + DGAP), DIFF_Y);
        diffButtons[i].setFillColor(diffColors[i]);
        diffButtons[i].setOutlineThickness(3.f);
        diffButtons[i].setOutlineColor(sf::Color(200, 200, 200));

        diffButtonTexts[i].setFont(font);
        diffButtonTexts[i].setString(diffLabels[i]);
        diffButtonTexts[i].setCharacterSize(15);
        diffButtonTexts[i].setFillColor(sf::Color::White);
        diffButtonTexts[i].setOutlineColor(sf::Color::Black);
        diffButtonTexts[i].setOutlineThickness(2.f);
        centerTextInButton(diffButtonTexts[i], diffButtons[i]);
    }

    // Mark EASY as selected by default
    diffButtons[0].setOutlineColor(sf::Color::White);
    diffButtons[0].setOutlineThickness(4.f);

    // bottom buttons: play | scores | back
    sf::Color navyNormal(20, 50, 100);
    sf::Color rim(100, 160, 220);

    const float BTN_W = 155.f;
    const float BTN_H = 48.f;
    const float GAP = 20.f;
    const float BTOTAL = 3 * BTN_W + 2 * GAP;
    const float BLX = 600.f - BTOTAL / 2.f;

    startButton.setSize(sf::Vector2f(BTN_W, BTN_H));
    startButton.setPosition(BLX, BTN_Y);
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
    leaderboardButton.setPosition(BLX + BTN_W + GAP, BTN_Y);
    leaderboardButton.setFillColor(navyNormal);
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
    backButton.setPosition(BLX + 2 * (BTN_W + GAP), BTN_Y);
    backButton.setFillColor(navyNormal);
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

// Game screen UI
void Pong::setupGameUI()
{
    score1Text.setFont(font);
    score1Text.setCharacterSize(52);
    score1Text.setFillColor(sf::Color(120, 200, 255));
    score1Text.setOutlineColor(sf::Color::Black);
    score1Text.setOutlineThickness(4.f);

    score2Text.setFont(font);
    score2Text.setCharacterSize(52);
    score2Text.setFillColor(sf::Color(180, 100, 255));
    score2Text.setOutlineColor(sf::Color::Black);
    score2Text.setOutlineThickness(4.f);

    player1GameLabel.setFont(font);
    player1GameLabel.setCharacterSize(16);
    player1GameLabel.setFillColor(sf::Color::White);
    player1GameLabel.setOutlineColor(sf::Color::Black);
    player1GameLabel.setOutlineThickness(2.f);

    player2GameLabel.setFont(font);
    player2GameLabel.setCharacterSize(16);
    player2GameLabel.setFillColor(sf::Color::White);
    player2GameLabel.setOutlineColor(sf::Color::Black);
    player2GameLabel.setOutlineThickness(2.f);

    launchHint.setFont(font);
    launchHint.setCharacterSize(16);
    launchHint.setFillColor(sf::Color(220, 220, 220, 200));
    launchHint.setOutlineColor(sf::Color::Black);
    launchHint.setOutlineThickness(2.f);
    launchHint.setString("PRESS SPACE TO LAUNCH");
    centerText(launchHint, 600.f, 660.f);
}

// buttons and text shown after game ends
void Pong::setupResultUI()
{
    sf::Color navyNormal(55, 10, 90);
    sf::Color rim(180, 100, 255);

    resultText.setFont(font);
    resultText.setCharacterSize(36);
    resultText.setFillColor(sf::Color::White);
    resultText.setOutlineColor(sf::Color::Black);
    resultText.setOutlineThickness(4.f);

    const float BTN_W = 155.f;
    const float BTN_H = 52.f;
    const float GAP = 20.f;
    const float BTN_Y = 390.f;
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
    resultLeaderboardButton.setFillColor(navyNormal);
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
    menuButton.setFillColor(navyNormal);
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

// Game update — ball movement, collisions, scoring
void Pong::updateGame(float dt)
{
    if (!ballLaunched || gameOver) return;

    // Move ball
    ball.move(ballVX * dt, ballVY * dt);

    sf::FloatRect ballB = ball.getGlobalBounds();
    sf::FloatRect p1B = paddle1.getGlobalBounds();
    sf::FloatRect p2B = paddle2.getGlobalBounds();

    // Top / bottom wall bounce
    if (ballB.top <= 0.f)
    {
        ball.setPosition(ballB.left, 0.f);
        ballVY = (ballVY < 0.f) ? -ballVY : ballVY;
    }
    if (ballB.top + ballB.height >= 700.f)
    {
        ball.setPosition(ballB.left, 700.f - ballB.height);
        ballVY = (ballVY > 0.f) ? -ballVY : ballVY;
    }

    // Paddle 1 collision
    if (ballVX < 0.f && ballB.intersects(p1B))
    {
        ball.setPosition(p1B.left + p1B.width + 1.f, ballB.top);
        // Angle based on where ball hits paddle
        float hitPos = (ballB.top + ballB.height / 2.f) - (p1B.top + p1B.height / 2.f);
        float norm = hitPos / (p1B.height / 2.f);
        ballVY = norm * ballSpeed * 0.75f;
        ballVX = (ballVX < 0.f) ? -ballVX : ballVX;
    }

    // Paddle 2 collision
    if (ballVX > 0.f && ballB.intersects(p2B))
    {
        ball.setPosition(p2B.left - ballB.width - 1.f, ballB.top);
        float hitPos = (ballB.top + ballB.height / 2.f) - (p2B.top + p2B.height / 2.f);
        float norm = hitPos / (p2B.height / 2.f);
        ballVY = norm * ballSpeed * 0.75f;
        ballVX = (ballVX > 0.f) ? -ballVX : ballVX;
    }

    // Score — ball exits left
    if (ballB.left + ballB.width < 0.f)
    {
        score2++;
        if (score2 >= winScore)
        {
            gameOver = true;
            winnerName = player2;
            winSound.setBuffer(winBuffer);
            winSound.play();
            FileManager::SaveEntry(player2, "PONG", 1);
            FileManager::SaveEntry(player1, "PONG", 0);

            std::string s1 = intToStr(score1);
            std::string s2 = intToStr(score2);
            resultText.setString(winnerName + " WINS!\n" + player1 + ": " + s1 + "  |  " + player2 + ": " + s2);
            centerText(resultText, 600.f, 305.f);
        }
        else resetBall();
    }

    // Score — ball exits right
    if (ballB.left > 1200.f)
    {
        score1++;
        if (score1 >= winScore)
        {
            gameOver = true;
            winnerName = player1;
            winSound.setBuffer(winBuffer);
            winSound.play();
            FileManager::SaveEntry(player1, "PONG", 1);
            FileManager::SaveEntry(player2, "PONG", 0);

            std::string s1 = intToStr(score1);
            std::string s2 = intToStr(score2);
            resultText.setString(winnerName + " WINS!\n" + player1 + ": " + s1 + "  |  " + player2 + ": " + s2);
            centerText(resultText, 600.f, 305.f);
        }
        else resetBall();
    }
}

// movePaddles — keyboard + mouse
void Pong::movePaddles(float dt)
{
    float p1Top = paddle1.getPosition().y;
    float p1Bottom = p1Top + paddle1.getSize().y;
    float p2Top = paddle2.getPosition().y;
    float p2Bottom = p2Top + paddle2.getSize().y;

    // player 1: w/s keys
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && p1Top > 0.f)
        paddle1.move(0.f, -paddleSpeed * dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && p1Bottom < 700.f)
        paddle1.move(0.f, paddleSpeed * dt);

    // player 2: up/down keys
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && p2Top > 0.f)
        paddle2.move(0.f, -paddleSpeed * dt);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && p2Bottom < 700.f)
        paddle2.move(0.f, paddleSpeed * dt);
}

// Drawing
void Pong::drawSetup(sf::RenderWindow& window)
{
    window.clear(sf::Color(15, 15, 30));
    window.draw(backgroundSprite);

    // big "pong" title — blue shadow + purple on top
    sf::Text titleShadow;
    titleShadow.setFont(font);
    titleShadow.setString("PONG");
    titleShadow.setCharacterSize(80);
    titleShadow.setFillColor(sf::Color(60, 120, 255, 200));   // blue
    titleShadow.setOutlineColor(sf::Color::Black);
    titleShadow.setOutlineThickness(6.f);
    sf::FloatRect sb = titleShadow.getLocalBounds();
    titleShadow.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
    titleShadow.setPosition(603.f, 73.f);
    window.draw(titleShadow);

    sf::Text titleMain;
    titleMain.setFont(font);
    titleMain.setString("PONG");
    titleMain.setCharacterSize(80);
    titleMain.setFillColor(sf::Color(180, 80, 255, 230));     // purple
    titleMain.setOutlineColor(sf::Color::Black);
    titleMain.setOutlineThickness(3.f);
    sf::FloatRect mb = titleMain.getLocalBounds();
    titleMain.setOrigin(mb.left + mb.width / 2.f, mb.top + mb.height / 2.f);
    titleMain.setPosition(600.f, 70.f);
    window.draw(titleMain);

    window.draw(player1Label);
    window.draw(player1Box);
    window.draw(player1InputText);
    window.draw(player2Label);
    window.draw(player2Box);
    window.draw(player2InputText);
    window.draw(infoText);

    for (int i = 0; i < 3; i++)
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

void Pong::drawGame(sf::RenderWindow& window)
{
    window.clear(sf::Color(10, 10, 20));
    window.draw(backgroundSprite);

    sf::RectangleShape tint;
    tint.setSize(sf::Vector2f(1200.f, 700.f));
    tint.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(tint);

    // Centre dashes
    for (int i = 0; i < dashCount; i++)
        window.draw(dashRects[i]);

    // Scores
    std::string s1 = intToStr(score1);
    std::string s2 = intToStr(score2);
    score1Text.setString(s1);
    score2Text.setString(s2);
    centerText(score1Text, 300.f, 50.f);
    centerText(score2Text, 900.f, 50.f);
    window.draw(score1Text);
    window.draw(score2Text);

    // Player name labels
    player1GameLabel.setString(player1 + "  [W/S]");
    centerText(player1GameLabel, 300.f, 90.f);
    window.draw(player1GameLabel);

    player2GameLabel.setString("[UP/DOWN]  " + player2);
    centerText(player2GameLabel, 900.f, 90.f);
    window.draw(player2GameLabel);

    // Launch hint if (!ballLaunched)
    window.draw(launchHint);

    window.draw(paddle1);
    window.draw(paddle2);
    window.draw(ball);
}

void Pong::drawResult(sf::RenderWindow& window)
{
    drawGame(window);

    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(1200.f, 700.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(overlay);

    sf::RectangleShape panel;
    panel.setSize(sf::Vector2f(660.f, 240.f));
    panel.setPosition(270.f, 255.f);
    panel.setFillColor(sf::Color(75, 15, 120));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color(220, 150, 255));
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
void Pong::handleSetupClick(sf::Vector2f mousePos, sf::RenderWindow& window)
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
        // Difficulty buttons
        for (int i = 0; i < 3; i++)
        {
            if (diffButtons[i].getGlobalBounds().contains(mousePos))
            {
                difficulty = i;
                // Reset all outlines then highlight selected
                for (int j = 0; j < 3; j++)
                {
                    diffButtons[j].setOutlineThickness(3.f);
                    diffButtons[j].setOutlineColor(sf::Color(200, 200, 200));
                }
                diffButtons[i].setOutlineColor(sf::Color::White);
                diffButtons[i].setOutlineThickness(5.f);
                break;
            }
        }
    }

    updateInputBoxStyles();
}

void Pong::handleTextInput(const sf::Event& event)
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

void Pong::handleResultClick(sf::Vector2f mousePos, sf::RenderWindow& window)
{
    if (playAgainButton.getGlobalBounds().contains(mousePos))
        reset();
    else if (resultLeaderboardButton.getGlobalBounds().contains(mousePos))
        showLeaderboard(window);
    else if (menuButton.getGlobalBounds().contains(mousePos))
        returnToMenu = true;
}

// update button colors on hover
void Pong::updateInputBoxStyles()
{
    player1Box.setOutlineColor(typingP1 ? sf::Color(255, 220, 80) : sf::Color(90, 50, 20));
    player1Box.setOutlineThickness(typingP1 ? 4.f : 3.f);
    player2Box.setOutlineColor(typingP2 ? sf::Color(255, 220, 80) : sf::Color(90, 50, 20));
    player2Box.setOutlineThickness(typingP2 ? 4.f : 3.f);
}

void Pong::updateSetupHover(sf::Vector2f mp)
{
    startButton.setFillColor(startButton.getGlobalBounds().contains(mp)
        ? sf::Color(40, 80, 150) : sf::Color(20, 50, 100));
    leaderboardButton.setFillColor(leaderboardButton.getGlobalBounds().contains(mp)
        ? sf::Color(40, 80, 150) : sf::Color(20, 50, 100));
    backButton.setFillColor(backButton.getGlobalBounds().contains(mp)
        ? sf::Color(40, 80, 150) : sf::Color(20, 50, 100));
}

void Pong::updateDiffHover(sf::Vector2f mp)
{
    sf::Color diffColors[3] = {
        sf::Color(55, 10, 90), sf::Color(70, 15, 110), sf::Color(90, 20, 140)
    };
    sf::Color diffHover[3] = {
        sf::Color(80, 20, 120), sf::Color(100, 30, 150), sf::Color(120, 40, 170)
    };

    for (int i = 0; i < 3; i++)
    {
        if (i != difficulty) // don't override selected
            diffButtons[i].setFillColor(diffButtons[i].getGlobalBounds().contains(mp)
                ? diffHover[i] : diffColors[i]);
    }
}

void Pong::updateResultHover(sf::Vector2f mp)
{
    playAgainButton.setFillColor(playAgainButton.getGlobalBounds().contains(mp)
        ? sf::Color(90, 20, 140) : sf::Color(55, 10, 90));
    resultLeaderboardButton.setFillColor(resultLeaderboardButton.getGlobalBounds().contains(mp)
        ? sf::Color(90, 20, 140) : sf::Color(55, 10, 90));
    menuButton.setFillColor(menuButton.getGlobalBounds().contains(mp)
        ? sf::Color(90, 20, 140) : sf::Color(55, 10, 90));
}

// small helper functions
void Pong::centerText(sf::Text& text, float cx, float cy)
{
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    text.setPosition(cx, cy);
}

void Pong::centerTextInButton(sf::Text& text, const sf::RectangleShape& btn)
{
    sf::FloatRect tb = text.getLocalBounds();
    sf::FloatRect bb = btn.getGlobalBounds();
    text.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    text.setPosition(bb.left + bb.width / 2.f, bb.top + bb.height / 2.f);
}

void Pong::showLeaderboard(sf::RenderWindow& window)
{
    FileManager lb;
    lb.run(window, "PONG");
}

// Main run loop
void Pong::run(sf::RenderWindow& window)
{
    sf::Event event;

    // phase 1: setup
    while (window.isOpen() && !gameStarted && !returnToMenu)
    {
        sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        updateSetupHover(mp);
        updateDiffHover(mp);

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
    setupCourt();
    applyDifficulty();
    paddle1.setPosition(30.f, 350.f - paddle1.getSize().y / 2.f);
    paddle2.setPosition(1156.f, 350.f - paddle2.getSize().y / 2.f);
    setupGameUI();
    setupResultUI();

    sf::Clock gameClock;

    while (window.isOpen() && !returnToMenu)
    {
        float dt = gameClock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f; // cap delta time

        sf::Vector2f mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        // Mouse control — P1 left paddle, P2 right paddle
        if (!gameOver)
        {
            // Mouse only moves a paddle if cursor is near it (within 80px horizontally)
            // This prevents mouse from overriding keyboard controls
            sf::FloatRect p1b = paddle1.getGlobalBounds();
            sf::FloatRect p2b = paddle2.getGlobalBounds();

            if (mp.x < 600.f && mp.x > p1b.left - 80.f && mp.x < p1b.left + p1b.width + 80.f)
            {
                float targetY = mp.y - paddle1.getSize().y / 2.f;
                if (targetY < 0.f) targetY = 0.f;
                if (targetY + paddle1.getSize().y > 700.f)
                    targetY = 700.f - paddle1.getSize().y;
                paddle1.setPosition(paddle1.getPosition().x, targetY);
            }
            if (mp.x >= 600.f && mp.x > p2b.left - 80.f && mp.x < p2b.left + p2b.width + 80.f)
            {
                float targetY = mp.y - paddle2.getSize().y / 2.f;
                if (targetY < 0.f) targetY = 0.f;
                if (targetY + paddle2.getSize().y > 700.f)
                    targetY = 700.f - paddle2.getSize().y;
                paddle2.setPosition(paddle2.getPosition().x, targetY);
            }

            movePaddles(dt);
            updateGame(dt);
        }

        if (gameOver) updateResultHover(mp);

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { window.close(); return; }

            if (event.type == sf::Event::KeyPressed)
            {
                // Space launches ball
                if (event.key.code == sf::Keyboard::Space && !ballLaunched && !gameOver)
                {
                    ballLaunched = true;
                    ballVX = ballSpeed;
                    ballVY = ballSpeed * 0.5f;
                }
            }

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f click = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                if (gameOver) handleResultClick(click, window);
            }
        }

        window.clear();
        if (gameOver) drawResult(window);
        else          drawGame(window);
        window.display();
    }
}