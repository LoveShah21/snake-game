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