#include <iostream>
#include <deque>
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

class Food
{
private:
    Point position;

public:
    Food() : position(0, 0) {}

    void spawn(int gridWidth, int gridHeight, const deque<Point> &snakeBody)
    {
        bool validPosition;
        do
        {
            validPosition = true;
            position.x = rand() % gridWidth;
            position.y = rand() % gridHeight;

            // Ensure food doesn't spawn on snake
            for (const auto &segment : snakeBody)
            {
                if (position == segment)
                {
                    validPosition = false;
                    break;
                }
            }
        } while (!validPosition);
    }

    Point getPosition() const
    {
        return position;
    }
};

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
        // Start with 3 segments
        body.push_back(Point(startX, startY));
        body.push_back(Point(startX - 1, startY));
        body.push_back(Point(startX - 2, startY));
    }

    void setDirection(int dx, int dy)
    {
        // Prevent reversing into itself
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

// ============ Game Class ============
class Game
{
    // game execution
    private:
    const int WIDTH = 20;
    const int HEIGHT = 20;
    Snake *snake;
    Food food;
    int score;
    bool gameOver;
    struct termios oldt, newt;

    void setupTerminal()
    {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // Set non-blocking mode
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }

    void restoreTerminal()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        // Restore blocking mode
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }

    void clearScreen()
    {
        cout << "\033[2J\033[H";
    }

    void hideCursor()
    {
        cout << "\033[?25l";
    }

    void showCursor()
    {
        cout << "\033[?25h";
    }

    void draw()
    {
        clearScreen();

        // Draw top border
        cout << "+";
        for (int i = 0; i < WIDTH; ++i)
            cout << "-";
        cout << "+" << endl;

        // Draw game area
        for (int y = 0; y < HEIGHT; ++y)
        {
            cout << "|";
            for (int x = 0; x < WIDTH; ++x)
            {
                Point current(x, y);
                bool isSnake = false;
                bool isHead = false;

                // Check if snake is at this position
                const deque<Point> &body = snake->getBody();
                for (size_t i = 0; i < body.size(); ++i)
                {
                    if (body[i] == current)
                    {
                        isSnake = true;
                        if (i == 0)
                            isHead = true;
                        break;
                    }
                }

                if (isHead)
                {
                    cout << "O";
                }
                else if (isSnake)
                {
                    cout << "o";
                }
                else if (current == food.getPosition())
                {
                    cout << "*";
                }
                else
                {
                    cout << " ";
                }
            }
            cout << "|" << endl;
        }

        // Draw bottom border
        cout << "+";
        for (int i = 0; i < WIDTH; ++i)
            cout << "-";
        cout << "+" << endl;

        // Display score
        cout << "Score: " << score << endl;
        cout << "Controls: W/A/S/D or Arrow Keys | Q to quit" << endl;
    }

    char getInput()
    {
        char c = 0;
        read(STDIN_FILENO, &c, 1);

        // Handle arrow keys
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
                            return 'w'; // Up
                        if (seq[1] == 'B')
                            return 's'; // Down
                        if (seq[1] == 'C')
                            return 'd'; // Right
                        if (seq[1] == 'D')
                            return 'a'; // Left
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

        // Wall collision
        if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT)
        {
            return true;
        }

        // Self collision
        if (snake->checkSelfCollision())
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
            food.spawn(WIDTH, HEIGHT, snake->getBody());
        }
    }

public:
    Game() : snake(nullptr), score(0), gameOver(false) {}

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
            // Initialize game
            if (snake)
                delete snake;
            snake = new Snake(WIDTH / 2, HEIGHT / 2);
            score = 0;
            gameOver = false;

            food.spawn(WIDTH, HEIGHT, snake->getBody());

            // Show instructions
            clearScreen();
            cout << "=== SNAKE GAME ===" << endl;
            cout << "Controls: W/A/S/D or Arrow Keys" << endl;
            cout << "Goal: Eat food (*) and grow!" << endl;
            cout << "Avoid walls and yourself!" << endl;
            cout << endl;
            cout << "Press any key to start..." << endl;

            // Wait for key press
            while (getInput() == 0)
            {
                usleep(10000);
            }

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

                usleep(150000); // 150ms delay - adjusted for slower, more playable speed
            }

            // Game over screen
            clearScreen();
            cout << "=== GAME OVER ===" << endl;
            cout << "Final Score: " << score << endl;
            cout << endl;
            cout << "Press R to restart or Q to quit: ";
            cout.flush();

            // Wait for restart or quit
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
    }
};
int main()
{
    cout << "Hello, Snake Game!" << endl;
    return 0;
}