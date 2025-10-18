#include <iostream>
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
    //    food execution
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

// ============ Snake Class ============
class Snake
{
    // snake execution
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