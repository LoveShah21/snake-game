# Snake Game - Enhanced Edition

A modern, feature-rich terminal-based Snake game written in C++ with powerups, obstacles, multiple food items, and sound effects.

## Features

### Core Gameplay

- Classic snake mechanics with smooth terminal rendering
- Dynamic grid size that adapts to terminal dimensions
- Multiple food items spawning simultaneously
- Randomized obstacles for increased difficulty
- Persistent high score tracking

### PowerUp System

Five unique powerups with visual indicators and timed effects:

- **Speed Boost (S)**: Increases movement speed for faster gameplay
- **Slow Motion (L)**: Reduces speed for easier maneuvering
- **Double Score (D)**: Earn 2x points for limited time
- **Invincibility (I)**: Temporary immunity to collisions
- **Shrink (R)**: Removes tail segment for tight spaces

### Technical Features

- Optimized double-buffered rendering (updates only changed screen areas)
- Non-blocking input handling with arrow key support
- Cross-platform terminal bell sound system
- Automatic terminal size detection
- Smooth 60+ FPS gameplay

## Requirements

- Linux/Unix system with POSIX terminal support
- C++11 or later compiler (g++, clang++)
- Standard C++ libraries
- Terminal with minimum 20x15 character display

## Compilation

```bash
g++ -std=c++11 -o snake_game snake_game.cpp
```

For optimized performance:

```bash
g++ -std=c++11 -O3 -o snake_game snake_game.cpp
```

## Controls

| Input | Action                     |
| ----- | -------------------------- |
| W / ↑ | Move Up                    |
| S / ↓ | Move Down                  |
| A / ← | Move Left                  |
| D / → | Move Right                 |
| Q     | Quit Game                  |
| R     | Restart (Game Over screen) |

## Gameplay Elements

### Symbols

- `O` - Snake head
- `o` - Snake body
- `*` - Food
- `#` - Obstacles
- `S/L/D/I/R` - PowerUps (see Features section)

### Scoring

- Base: 1 point per food
- With Double Score powerup: 2 points per food
- High scores persist between sessions

## Game Rules

### Win Conditions

- Eat food to grow longer and increase score
- Collect powerups for temporary advantages
- Beat your high score

### Lose Conditions

- Colliding with walls
- Colliding with obstacles
- Colliding with your own body
- Note: Invincibility powerup temporarily disables collision detection

## File Structure

```
game.cpp           # Main game source code
snake_highscore.dat      # Persistent high score storage (auto-generated)
README.md                # This file
```

## Code Architecture

### Classes

**SoundManager**: Handles terminal bell audio feedback for game events

**Point**: 2D coordinate structure with equality operator

**PowerUp**: Manages powerup spawning, effects, and durations

**Obstacle**: Generates and tracks static obstacle positions

**FoodManager**: Handles multiple simultaneous food items

**Snake**: Core snake logic (movement, growth, collision detection)

**HighScoreManager**: Persistent score storage and retrieval

**Game**: Main game loop, rendering, input handling, state management

## Performance Optimizations

- Double-buffered rendering with delta updates
- Efficient collision detection using deque structures
- Non-blocking input with minimal latency
- Adaptive refresh rate based on powerup effects

## Troubleshooting

### Terminal Display Issues

Resize your terminal to at least 25 columns × 20 rows for optimal experience.

### Compilation Errors

Ensure you're using C++11 or later:

```bash
g++ --version  # Check compiler version
```

### Sound Not Working

The game uses terminal bell (`\a`). Enable system beep in terminal preferences.

### High Score Not Saving

Verify write permissions in the game directory for `snake_highscore.dat`.

## Future Enhancements

- Difficulty levels with adjustable obstacle density
- Color support using ANSI escape codes
- Leaderboard with multiple score entries
- Level progression system
- Custom terminal themes

## Contributing

Feel free to fork this project and submit pull requests for improvements!

## License

This project is open source and available for educational purposes.

## Credits

Developed as an enhanced terminal-based game using standard C++ libraries with POSIX terminal control.

---

**Enjoy the game! Try to beat your high score! 🐍**
