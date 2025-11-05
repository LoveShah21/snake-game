#include <iostream>
#include <deque>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <fcntl.h>
#include <fstream>
#include <sys/ioctl.h>
#include <sstream>
using namespace std;

struct Point
{
    int x, y;

    Point(int x = 0, int y = 0) : x(x), y(y) {}

    bool operator==(const Point &other) const
    {
        return x == other.x && y == other.y;
    }
};

// ============ Obstacle Class ============
class Obstacle
{
private:
    vector<Point> positions;

public:
    Obstacle() {}

    void generateObstacles(int gridWidth, int gridHeight, const Point &snakeStart)
    {
        positions.clear();
        int numObstacles = (gridWidth * gridHeight) / 50;

        for (int i = 0; i < numObstacles; ++i)
        {
            int x = rand() % gridWidth;
            int y = rand() % gridHeight;
            Point obs(x, y);

            if (abs(x - snakeStart.x) > 3 && abs(y - snakeStart.y) > 3)
            {
                positions.push_back(obs);
            }
        }
    }

    bool isObstacle(const Point &p) const
    {
        for (const auto &obs : positions)
        {
            if (obs == p)
                return true;
        }
        return false;
    }

    const vector<Point> &getPositions() const
    {
        return positions;
    }
};

// ============ Food Class ============
class Food
{
private:
    Point position;

public:
    Food() : position(0, 0) {}

    void spawn(int gridWidth, int gridHeight, const deque<Point> &snakeBody, const Obstacle &obstacles)
    {
        bool validPosition;
        do
        {
            validPosition = true;
            position.x = rand() % gridWidth;
            position.y = rand() % gridHeight;

            for (const auto &segment : snakeBody)
            {
                if (position == segment)
                {
                    validPosition = false;
                    break;
                }
            }

            if (validPosition && obstacles.isObstacle(position))
            {
                validPosition = false;
            }
        } while (!validPosition);
    }

    Point getPosition() const
    {
        return position;
    }
};

// ============ Snake Class ============
class Snake
{
private:
    deque<Point> body;
    Point direction;
    Point nextDirection;
    bool growing;

public:
    Snake(int startX, int startY) : direction(1, 0), nextDirection(1, 0), growing(false)
    {
        body.push_back(Point(startX, startY));
        body.push_back(Point(startX - 1, startY));
        body.push_back(Point(startX - 2, startY));
    }

    void setDirection(int dx, int dy)
    {
        if (dx != -direction.x || dy != -direction.y)
        {
            nextDirection.x = dx;
            nextDirection.y = dy;
        }
    }

    void move()
    {
        direction = nextDirection;

        Point newHead(body.front().x + direction.x, body.front().y + direction.y);
        body.push_front(newHead);

        if (!growing)
        {
            body.pop_back();
        }
        else
        {
            growing = false;
        }
    }

    void grow()
    {
        growing = true;
    }

    Point getHead() const
    {
        return body.front();
    }

    const deque<Point> &getBody() const
    {
        return body;
    }

    bool checkSelfCollision() const
    {
        Point head = body.front();
        for (size_t i = 1; i < body.size(); ++i)
        {
            if (head == body[i])
            {
                return true;
            }
        }
        return false;
    }
};

// ============ HighScore Manager ============
class HighScoreManager
{
private:
    const string filename = "snake_highscore.dat";
    int highScore;

public:
    HighScoreManager() : highScore(0)
    {
        loadHighScore();
    }

    void loadHighScore()
    {
        ifstream file(filename);
        if (file.is_open())
        {
            file >> highScore;
            file.close();
        }
        else
        {
            highScore = 0;
        }
    }

    void saveHighScore(int score)
    {
        if (score > highScore)
        {
            highScore = score;
            ofstream file(filename);
            if (file.is_open())
            {
                file << highScore;
                file.close();
            }
        }
    }

    int getHighScore() const
    {
        return highScore;
    }
};

// ============ Game Class ============
class Game
{
private:
    int WIDTH;
    int HEIGHT;
    Snake *snake;
    Food food;
    Obstacle obstacles;
    int score;
    bool gameOver;
    struct termios oldt, newt;
    HighScoreManager highScoreManager;
    vector<string> screenBuffer;
    vector<string> previousBuffer;

    void setupTerminal()
    {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }

    void restoreTerminal()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }

    void getTerminalSize()
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        WIDTH = min(60, (int)w.ws_col - 4);
        HEIGHT = min(30, (int)w.ws_row - 8);

        if (WIDTH < 20)
            WIDTH = 20;
        if (HEIGHT < 15)
            HEIGHT = 15;
    }

    void clearScreen()
    {
        cout << "\033[2J\033[H";
    }

    void moveCursor(int row, int col)
    {
        cout << "\033[" << row << ";" << col << "H";
    }

    void hideCursor()
    {
        cout << "\033[?25l";
    }

    void showCursor()
    {
        cout << "\033[?25h";
    }

    void initializeBuffer()
    {
        screenBuffer.clear();
        previousBuffer.clear();

        // Top border
        string topBorder = "+";
        for (int i = 0; i < WIDTH; ++i)
            topBorder += "-";
        topBorder += "+";
        screenBuffer.push_back(topBorder);

        // Game area
        for (int y = 0; y < HEIGHT; ++y)
        {
            string line = "|";
            for (int x = 0; x < WIDTH; ++x)
            {
                line += " ";
            }
            line += "|";
            screenBuffer.push_back(line);
        }

        // Bottom border
        string bottomBorder = "+";
        for (int i = 0; i < WIDTH; ++i)
            bottomBorder += "-";
        bottomBorder += "+";
        screenBuffer.push_back(bottomBorder);

        // Status lines
        stringstream ss;
        ss << "Score: " << score << " | High Score: " << highScoreManager.getHighScore();
        screenBuffer.push_back(ss.str());
        screenBuffer.push_back("Controls: W/A/S/D or Arrow Keys | Q to quit");

        previousBuffer = screenBuffer;
    }

    void updateBuffer()
    {
        // Reset game area to spaces
        for (int y = 0; y < HEIGHT; ++y)
        {
            for (int x = 0; x < WIDTH; ++x)
            {
                screenBuffer[y + 1][x + 1] = ' ';
            }
        }

        // Draw obstacles
        for (const auto &obs : obstacles.getPositions())
        {
            if (obs.x >= 0 && obs.x < WIDTH && obs.y >= 0 && obs.y < HEIGHT)
            {
                screenBuffer[obs.y + 1][obs.x + 1] = '#';
            }
        }

        // Draw food
        Point foodPos = food.getPosition();
        screenBuffer[foodPos.y + 1][foodPos.x + 1] = '*';

        // Draw snake
        const deque<Point> &body = snake->getBody();
        for (size_t i = 0; i < body.size(); ++i)
        {
            if (body[i].x >= 0 && body[i].x < WIDTH && body[i].y >= 0 && body[i].y < HEIGHT)
            {
                if (i == 0)
                    screenBuffer[body[i].y + 1][body[i].x + 1] = 'O';
                else
                    screenBuffer[body[i].y + 1][body[i].x + 1] = 'o';
            }
        }

        // Update status line
        stringstream ss;
        ss << "Score: " << score << " | High Score: " << highScoreManager.getHighScore();
        screenBuffer[HEIGHT + 2] = ss.str();
    }

    void draw()
    {
        updateBuffer();

        // Only redraw changed lines
        for (size_t i = 0; i < screenBuffer.size(); ++i)
        {
            if (screenBuffer[i] != previousBuffer[i])
            {
                moveCursor(i + 1, 1);
                cout << screenBuffer[i];
                cout.flush();
                previousBuffer[i] = screenBuffer[i];
            }
        }
    }

    void drawFullScreen()
    {
        clearScreen();
        for (const auto &line : screenBuffer)
        {
            cout << line << endl;
        }
        cout.flush();
    }

    char getInput()
    {
        char c = 0;
        read(STDIN_FILENO, &c, 1);

        if (c == 27)
        {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1)
            {
                if (seq[0] == '[')
                {
                    if (read(STDIN_FILENO, &seq[1], 1) == 1)
                    {
                        if (seq[1] == 'A')
                            return 'w';
                        if (seq[1] == 'B')
                            return 's';
                        if (seq[1] == 'C')
                            return 'd';
                        if (seq[1] == 'D')
                            return 'a';
                    }
                }
            }
        }

        return c;
    }

    void processInput(char input)
    {
        switch (input)
        {
        case 'w':
        case 'W':
            snake->setDirection(0, -1);
            break;
        case 's':
        case 'S':
            snake->setDirection(0, 1);
            break;
        case 'a':
        case 'A':
            snake->setDirection(-1, 0);
            break;
        case 'd':
        case 'D':
            snake->setDirection(1, 0);
            break;
        case 'q':
        case 'Q':
            gameOver = true;
            break;
        }
    }

    bool checkCollision()
    {
        Point head = snake->getHead();

        if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT)
        {
            return true;
        }

        if (snake->checkSelfCollision())
        {
            return true;
        }

        if (obstacles.isObstacle(head))
        {
            return true;
        }

        return false;
    }

    void checkFood()
    {
        if (snake->getHead() == food.getPosition())
        {
            snake->grow();
            score++;
            food.spawn(WIDTH, HEIGHT, snake->getBody(), obstacles);
        }
    }

public:
    Game() : WIDTH(40), HEIGHT(25), snake(nullptr), score(0), gameOver(false) {}

    ~Game()
    {
        if (snake)
            delete snake;
    }

    void run()
    {
        srand(time(0));
        setupTerminal();
        hideCursor();

        bool running = true;

        while (running)
        {
            getTerminalSize();

            if (snake)
                delete snake;
            snake = new Snake(WIDTH / 2, HEIGHT / 2);
            score = 0;
            gameOver = false;

            obstacles.generateObstacles(WIDTH, HEIGHT, Point(WIDTH / 2, HEIGHT / 2));
            food.spawn(WIDTH, HEIGHT, snake->getBody(), obstacles);

            // Show instructions
            clearScreen();
            cout << "=== SNAKE GAME ===" << endl;
            cout << "Controls: W/A/S/D or Arrow Keys" << endl;
            cout << "Goal: Eat food (*) and grow!" << endl;
            cout << "Avoid walls (#), obstacles, and yourself!" << endl;
            cout << "Play Area: " << WIDTH << "x" << HEIGHT << endl;
            cout << "Current High Score: " << highScoreManager.getHighScore() << endl;
            cout << endl;
            cout << "Press any key to start..." << endl;

            while (getInput() == 0)
            {
                usleep(10000);
            }

            // Initialize buffers
            initializeBuffer();
            drawFullScreen();

            // Game loop
            while (!gameOver)
            {
                draw();

                char input = getInput();
                if (input != 0)
                {
                    processInput(input);
                }

                snake->move();

                if (checkCollision())
                {
                    gameOver = true;
                }

                checkFood();

                usleep(120000);
            }

            highScoreManager.saveHighScore(score);

            // Game over screen
            clearScreen();
            cout << "=== GAME OVER ===" << endl;
            cout << "Final Score: " << score << endl;
            cout << "High Score: " << highScoreManager.getHighScore() << endl;
            if (score == highScoreManager.getHighScore() && score > 0)
            {
                cout << "🎉 NEW HIGH SCORE! 🎉" << endl;
            }
            cout << endl;
            cout << "Press R to restart or Q to quit: ";
            cout.flush();

            char choice = 0;
            while (choice != 'r' && choice != 'R' && choice != 'q' && choice != 'Q')
            {
                choice = getInput();
                usleep(10000);
            }

            if (choice == 'q' || choice == 'Q')
            {
                running = false;
            }
        }

        showCursor();
        restoreTerminal();
        clearScreen();
        cout << "Thanks for playing!" << endl;
        cout << "Final High Score: " << highScoreManager.getHighScore() << endl;
    }
};

int main()
{
    Game game;
    game.run();
    return 0;
}
