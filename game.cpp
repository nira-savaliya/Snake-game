#include <iostream>
#include <fstream>
#include <vector>
#include <conio.h>   
#include <cstdlib>   
#include <ctime>     
#include <windows.h> 

using namespace std;

int speed = 275;

// Directions
enum Direction
{
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Function to set the cursor position in the console
void setCursorPosition(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Function to display rulebook
void displayRulebook()
{
    system("cls"); // Clear screen
    setColor(14);  // Yellow color
    cout << "======================== SNAKE GAME RULEBOOK ========================\n\n";
    setColor(7); // White color

    cout << "🐍 Objective:\n";
    cout << "   - Eat the apples 🍎 to grow your snake and increase your score.\n";
    cout << "   - Avoid crashing into the walls 🧱 or yourself!\n\n";

    cout << "🎮 Controls:\n"
    ;
    cout << "   - Move using Arrow Keys (⬆  ⬇  ⬅  ➡  ) or WASD.\n";
    cout << "   - Press 'X' to quit the game at any time.\n\n";

    cout << "⚡ Speed:\n";
    cout << "   - The snake moves faster as you eat more apples.\n";
    cout << "   - Every 3 apples, speed increases!\n\n";

    cout << "🏆 Scoring:\n";
    cout << "   - Each apple = +10 points.\n";
    cout << "   - Your high score is saved automatically.\n\n";

    setColor(11); // Cyan
    cout << "Press any key to start the game...";

    _getch();      // Wait for key press
    system("cls"); // Clear screen before starting
    setColor(7);   // Reset to default color
}
// Function to hide the cursor in the console
void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false; // Set cursor visibility to false
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

// Function to show the cursor (optional, for cleanup after the game ends)
void showCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = true; // Set cursor visibility to true
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

class SnakeGame
{
private:
    int width, height;
    bool gameOver;
    int x, y; // Snake's head position
    int foodX, foodY;
    int score;
    int highScore; // High score variable
    Direction dir;
    vector<pair<int, int>> snakeBody; // Stores the snake's body (x, y)

public:
    SnakeGame(int w, int h) : width(w), height(h), gameOver(false), score(0), dir(STOP)
    {
        x = width / 2;
        y = height / 2;
        generateFood();
        snakeBody.push_back({x - 1, y});
        snakeBody.push_back({x - 2, y});
        highScore = loadHighScore(); // Load high score from file
    }

    void generateFood()
    {
        bool validPosition = false;
        while (!validPosition)
        {
            foodX = rand() % width;
            foodY = rand() % height;
            validPosition = true;

            if (x == foodX && y == foodY)
                validPosition = false;
            else
            {
                // Check if the food position overlaps with the snake's body
                for (const auto &bodyPart : snakeBody)
                {
                    if (bodyPart.first == foodX && bodyPart.second == foodY)
                    {
                        validPosition = false;
                        break;
                    }
                }
            }
        }
    }

    void draw()
    {
        setCursorPosition(0, 0); // Reset the cursor to the top-left corner

        // Top border
        for (int i = 0; i < width + 2; ++i)
        {
            cout << "🧱";
        }
        cout << endl;

        // Game area printing
        for (int i = 0; i < height; ++i)
        {
            for (int j = 0; j < width + 2; ++j)
            {
                if (j == 0 || j == width + 1)
                {
                    cout << "🧱";
                }
                else if (i == y && j == x + 1)
                {
                    cout << "🐲"; // Snake's head
                }
                else if (i == foodY && j == foodX + 1)
                {
                    cout << "🍎"; 
                }
                else
                {
                    bool isBody = false;
                    for (const auto &bodyPart : snakeBody)
                    {
                        if (bodyPart.first == j - 1 && bodyPart.second == i)
                        {
                            cout << "🟢"; // Snake's body
                            isBody = true;
                            break;
                        }
                    }
                    if (!isBody)
                    {
                        cout << "  "; // Empty space
                    }
                }
            }
            cout << endl;
        }

        // Bottom border
        for (int i = 0; i < width + 2; ++i)
        {
            cout << "🧱";
        }
        cout << "\nScore: " << score << "\tHigh Score: " << highScore << endl;
    }

    void input()
    {
        if (_kbhit())
        {                       // Check if a key is pressed
            int key = _getch(); // Getting the pressed key

            if (key == 224) // special key code for arrow keys
            {                  
                key = _getch(); // Getting the actual arrow key 
                switch (key)
                {
                case 75:
                    if (dir != RIGHT && dir != STOP)
                        dir = LEFT;
                    break; 
                case 77:
                    if (dir != LEFT)
                        dir = RIGHT;
                    break; 
                case 72:
                    if (dir != DOWN)
                        dir = UP;
                    break; 
                case 80:
                    if (dir != UP)
                        dir = DOWN;
                    break; 
                }
            }
            else
            {
                switch (key)
                {
                case 'a':
                    if (dir != RIGHT && dir != STOP)
                        dir = LEFT;
                    break;
                case 'd':
                    if (dir != LEFT)
                        dir = RIGHT;
                    break;
                case 'w':
                    if (dir != DOWN)
                        dir = UP;
                    break;
                case 's':
                    if (dir != UP)
                        dir = DOWN;
                    break;
                case 'x':
                    gameOver = true;
                    break; // Game over
                }
            }
        }
    }

    
    void logic()
    {
        // Store the previous head position
        int prevX = x;
        int prevY = y;

        // Move the head
        switch (dir)
        {
        case LEFT:
            --x;
            break;
        case RIGHT:
            ++x;
            break;
        case UP:
            --y;
            break;
        case DOWN:
            ++y;
            break;
        default:
            break;
        }

        // Check for collisions with walls
        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            gameOver = true;
            return;
        }

        // Check for collisions with itself
        for (const auto &bodyPart : snakeBody)
        {
            if (bodyPart.first == x && bodyPart.second == y)
            {
                gameOver = true;
                return;
            }
        }

        //Shift every part to the previous one's position to move body
        if (!snakeBody.empty())
        {
            for (size_t i = snakeBody.size() - 1; i > 0; --i)
            {
                snakeBody[i] = snakeBody[i - 1];
            }
            snakeBody[0] = {prevX, prevY}; // Place previous head position as first body part
        }

        // Check if food is eaten
        if (x == foodX && y == foodY)
        {
            score += 10;
            if (score % 30 == 0 && speed > 50)
                speed -= 25;

            // Add new body part (extend tail)
            snakeBody.push_back(snakeBody.back());

            generateFood();
        }
    }

    void run()
    {

        hideCursor(); // Hide the cursor to stop cursor related glitches
        while (!gameOver)
        {
            draw();
            input();
            if (dir != STOP)
                logic();  // Snake moves only after the first input
            Sleep(speed); // Manage snake's speed
        }
        showCursor(); // Show the cursor after the game ends

        if (score > highScore)
        {
            highScore = score;
            saveHighScore();
            cout << "Congratulations!! \nYou made a NEW HIGH SCORE!! 🥳🥳\n"; // Save new high score to file
        }

        cout << "Game Over! Final Score: " << score << endl;
    }

    // Function to load high score from file
    int loadHighScore()
    {
        ifstream file("HighScore.txt");
        int savedHighScore = 0;
        if (file.is_open())
        {
            file >> savedHighScore;
            file.close();
        }
        return savedHighScore;
    }

    // Function to save high score to file
    void saveHighScore()
    {
        ofstream file("HighScore.txt");
        if (file.is_open())
        {
            file << highScore;
            file.close();
        }
    }
};

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    displayRulebook();
    srand(static_cast<unsigned>(time(0))); // Seed random number generator
    while (1)
    {
        SnakeGame game(25, 25);
        game.run();

        cout << "\n Press any key to Play Again  or Press X to exit the game.\n";
        char ch = _getch();
        if (ch == 'x' || ch == 'X')
            break;
        system("cls");
    }
    return 0;
}