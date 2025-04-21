#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

static bool allowMove = false;
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for (unsigned int i = 0; i < deque.size(); i++)
    {
        if (Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

bool EventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

int compare(const void* a, const void* b) {
    return *(int*)b - *(int*)a;
}

void SaveHighScore(int highScore) {
    int scores[6] = { highScore }, count = 1;
    FILE* file = fopen("highscore.txt", "r");

    while (file && fscanf(file, "%d", &scores[count]) == 1 && count < 5) count++;
    if (file) fclose(file);

    qsort(scores, count, sizeof(int), compare);

    file = fopen("highscore.txt", "w");
    for (int i = 0; i < (count < 5 ? count : 5); i++) fprintf(file, "%d\n", scores[i]);
    fclose(file);
}

class Drawable
{
public:
    virtual void Draw() = 0;
};

class Snake : public Drawable
{
public:
    deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    void Update()
    {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            body.pop_back();
        }
    }

    void Reset()
    {
        body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        direction = {1, 0};
    }
};

class Food : public Drawable
{
public:
    Vector2 position;

    Food(deque<Vector2> snakeBody)
    {
        position = GenerateRandomPos(snakeBody);
    }

    void Draw()
    {
        Rectangle rect = {offset + position.x * cellSize, offset + position.y * cellSize, (float)cellSize, (float)cellSize};
        DrawRectangleRounded(rect, 0.5, 6, RED);
    }

    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game
{
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    Game() 
    {
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }

    ~Game() 
    {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw()
    {
        food.Draw();
        snake.Draw();
    }

    void Update()
    {
        if (running)
        {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    void CheckCollisionWithFood()
    {
        if (Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void CheckCollisionWithEdges()
    {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1 || snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }

    void GameOver()
    {
        running = false;
        SaveHighScore(score);
        PlaySound(wallSound);
    }
};

int main()
{
    int screenWidth = 2 * offset + cellSize * cellCount;
    int screenHeight = 2 * offset + cellSize * cellCount;

    InitWindow(screenWidth, screenHeight, "SNAKE GAME");
    SetTargetFPS(60);

    Game game = Game();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(green);

        if (!game.running)
        {
            // Hiển thị "Game Over"
            DrawText("GAME OVER", screenWidth / 2 - MeasureText("GAME OVER", 50) / 2, screenHeight / 2 - 100, 50, RED);

            // Hiển thị điểm số
            DrawText(TextFormat("Your Score: %d", game.score), screenWidth / 2 - MeasureText("Your Score: 0", 30) / 2, screenHeight / 2 - 20, 30, BLACK);

            // Vẽ nút Play Again
            Rectangle playAgainButton = {
            (float)(screenWidth / 2 - 75),
            (float)(screenHeight / 2 + 50),
            150.0f,
            50.0f
            };

            Color buttonColor = CheckCollisionPointRec(GetMousePosition(), playAgainButton) ? GREEN : darkGreen;

            DrawRectangleRec(playAgainButton, buttonColor);
            DrawText("Play Again", screenWidth / 2 - MeasureText("Play Again", 20) / 2, screenHeight / 2 + 65, 20, WHITE);

            // Kiểm tra nếu nhấn nút Play Again
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), playAgainButton))
            {
                game.snake.Reset();
                game.food.position = game.food.GenerateRandomPos(game.snake.body);
                game.score = 0;
                game.running = true;
            }
        }
        else
        {
            if (EventTriggered(0.2))
            {
                allowMove = true;
                game.Update();
            }

            if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1 && allowMove)
            {
                game.snake.direction = {0, -1};
                game.running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1 && allowMove)
            {
                game.snake.direction = {0, 1};
                game.running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1 && allowMove)
            {
                game.snake.direction = {-1, 0};
                game.running = true;
                allowMove = false;
            }
            if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1 && allowMove)
            {
                game.snake.direction = {1, 0};
                game.running = true;
                allowMove = false;
            }

            DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
            DrawText("SNAKE GAME", offset - 5, 20, 40, darkGreen);
            DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
            game.Draw();
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}