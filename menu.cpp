#include "raylib.h"
#include <cstdlib> // Thư viện cần thiết để sử dụng hàm system()
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Lớp Menu
class Menu {
private:
    int screenWidth, screenHeight;
    Rectangle playButton, highscoreButton, howToPlayButton, exitButton;

public:
    Menu(int width, int height) : screenWidth(width), screenHeight(height) {
        // Tạo các nút và căn giữa
        playButton = { (float)(screenWidth / 2 - 100), (float)(screenHeight / 2 - 100), 200, 50 };
        highscoreButton = { (float)(screenWidth / 2 - 100), (float)(screenHeight / 2 - 50), 200, 50 };
        howToPlayButton = { (float)(screenWidth / 2 - 100), (float)(screenHeight / 2), 200, 50 };
        exitButton = { (float)(screenWidth / 2 - 100), (float)(screenHeight / 2 + 50), 200, 50 };
    }

    void Draw() {
        // Tiêu đề
        DrawText("SNAKE GAME", screenWidth / 2 - MeasureText("SNAKE GAME", 40) / 2, 100, 40, DARKGREEN);

        // Hiệu ứng đổi màu khi chuột di qua các nút
        Vector2 mousePosition = GetMousePosition();
        Color playColor = CheckCollisionPointRec(mousePosition, playButton) ? DARKGRAY : LIGHTGRAY;
        Color highscoreColor = CheckCollisionPointRec(mousePosition, highscoreButton) ? DARKGRAY : LIGHTGRAY;
        Color howToPlayColor = CheckCollisionPointRec(mousePosition, howToPlayButton) ? DARKGRAY : LIGHTGRAY;
        Color exitColor = CheckCollisionPointRec(mousePosition, exitButton) ? DARKGRAY : LIGHTGRAY;

        // Vẽ các nút
        DrawRectangleRec(playButton, playColor);
        DrawText("PLAY", screenWidth / 2 - MeasureText("PLAY", 20) / 2, playButton.y + 15, 20, BLACK);

        DrawRectangleRec(highscoreButton, highscoreColor);
        DrawText("HIGHSCORE", screenWidth / 2 - MeasureText("HIGHSCORE", 20) / 2, highscoreButton.y + 15, 20, BLACK);

        DrawRectangleRec(howToPlayButton, howToPlayColor);
        DrawText("HOW TO PLAY", screenWidth / 2 - MeasureText("HOW TO PLAY", 20) / 2, howToPlayButton.y + 15, 20, BLACK);

        DrawRectangleRec(exitButton, exitColor);
        DrawText("EXIT", screenWidth / 2 - MeasureText("EXIT", 20) / 2, exitButton.y + 15, 20, BLACK);
    }

    int HandleInput() {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();
            if (CheckCollisionPointRec(mousePosition, playButton)) {
                system("main.exe");
                return 1;
            } else if (CheckCollisionPointRec(mousePosition, highscoreButton)) {
                return 2;
            } else if (CheckCollisionPointRec(mousePosition, howToPlayButton)) {
                return 3;
            } else if (CheckCollisionPointRec(mousePosition, exitButton)) {
                return 4;
            }
        }
        return 0;
    }
};

// Hàm đọc điểm từ file và trả về danh sách top 5 điểm cao nhất
vector<int> ReadHighScores() {
    vector<int> highScores;
    ifstream file("highscore.txt");

    if (file.is_open()) {
        int score;
        while (file >> score) {
            highScores.push_back(score);
        }
        file.close();
    }

    // Sắp xếp danh sách theo thứ tự giảm dần
    sort(highScores.rbegin(), highScores.rend());

    // Giới hạn chỉ lấy 5 điểm cao nhất
    if (highScores.size() > 5) {
        highScores.resize(5);
    }

    return highScores;
}

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Snake Game Menu");
    SetTargetFPS(60);

    Menu menu(screenWidth, screenHeight);
    bool gameRunning = false;
    bool showInstructions = false;
    bool showHighScores = false;
    vector<int> highScores;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameRunning && !showInstructions && !showHighScores) {
            menu.Draw();
            int action = menu.HandleInput();
            if (action == 1) {
                gameRunning = true;
            } else if (action == 2) {
                showHighScores = true;
                highScores = ReadHighScores(); // Lấy danh sách điểm cao
            } else if (action == 3) {
                showInstructions = true;
            } else if (action == 4) {
                break;
            }
        } else if (showInstructions) {
            DrawText("Use the arrow keys to move", 100, 200, 40, DARKGRAY);
            DrawText("Hunt for food to grow in size", 100, 250, 40, DARKGRAY);  
            DrawText("Don't hit the walls or your tail", 100, 300, 40, DARKGRAY);  
            DrawText("Press BACKSPACE to go back", 100, 400, 30, DARKGRAY);

            if (IsKeyPressed(KEY_BACKSPACE)) {
                showInstructions = false;
            }
        } else if (showHighScores) {
            DrawText("Top 5 High Scores", 100, 100, 40, DARKGRAY);
            for (int i = 0; i < highScores.size(); i++) {
                string scoreText = "Rank " + to_string(i + 1) + ": " + to_string(highScores[i]);
                DrawText(scoreText.c_str(), 100, 150 + i * 50, 30, DARKGRAY);
            }
            DrawText("Press BACKSPACE to go back", 100, 400, 30, DARKGRAY);

            if (IsKeyPressed(KEY_BACKSPACE)) {
                showHighScores = false;
            }
        } else if (gameRunning) {
            const char *text = "Press BACKSPACE to go back";
            int screenWidth = GetScreenWidth(); 
            int textWidth = MeasureText(text, 30); 
            int x = (screenWidth - textWidth) / 2; 
            DrawText(text, x, 400, 30, DARKGRAY);

            if (IsKeyPressed(KEY_BACKSPACE)) {
                gameRunning = false;
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}