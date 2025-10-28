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

// ============ Food Class ============
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
};
int main()
{
    cout << "Hello, Snake Game!" << endl;
    return 0;
}