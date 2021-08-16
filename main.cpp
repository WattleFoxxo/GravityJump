#include <string> 
#include <vector>
#include "raylib.h"

const int screenWidth = 1280;
const int screenHeight = 720;

bool isColliding(std::vector<Vector2> spikes,  Vector2 point){
    bool isColliding;
    for(int i = 0; i < spikes.size(); i++){
        if (spikes[i].y == 1){
            isColliding = CheckCollisionPointTriangle(point, {spikes[i].x, screenHeight-100}, {spikes[i].x+50, screenHeight-100}, {spikes[i].x+25, screenHeight-150});
        } else {
            isColliding = CheckCollisionPointTriangle(point, {spikes[i].x, 100}, {spikes[i].x+25, 150}, {spikes[i].x+50, 100});
        }
        if(isColliding){
            return true;
        }
    }
    return false;
}

int main(void) {
    //init
    int gravity = 1;
    int jumpspeed = 60;
    int speed = 5;
    int framesCounter = 0;
    int scoreframesCounter = 0;
    int score = 0;
    int highscore = 0;

    char* title = "Gravity Jump";
    char* playbouttontext = "play";

    bool isGrounded = true;
    bool playing = false;
    bool gameover = false;

    Vector2 mousePoint = { 0.0f, 0.0f };
    	
    std::vector<Vector2> spikes;
    std::vector<Rectangle> spikescollider;

    Rectangle roof = { 0, 0, (float)screenWidth, 100};
    Rectangle ground = { 0, (float)screenHeight-100, (float)screenWidth, 100};
    Rectangle playagainbutton = {screenWidth/2+5, screenHeight/2+100, 200, 70};
    Rectangle menubutton = {screenWidth/2-205, screenHeight/2+100, 200, 70};
    Rectangle player = { (float)screenWidth/2-25, (float)screenHeight/2, 50, 50};

    InitWindow(screenWidth, screenHeight, "Gravity Jump");

    SetTargetFPS(60);
    
    if(!FileExists("score.txt")){
        SaveFileText("score.txt", "0");
    }

    highscore = std::stoi(LoadFileText("score.txt"));

    while (!WindowShouldClose()) {
        //update
        if(playing){
            framesCounter++;
            scoreframesCounter++;

            if(score > 10000){
                speed = 30;
            } else if(score > 1500){
                speed = 25;
            } else if(score > 1000){
                speed = 20;
            } else if(score > 500){
                speed = 15;
            } else {
                speed = 10;
            }

            if(((framesCounter/GetRandomValue(40-speed, 55-speed))%2) == 1){
                spikes.push_back({screenWidth, GetRandomValue(0, 1)+0.0f});
                framesCounter = 0;
            }

            if(((scoreframesCounter/10)%2) == 1){
                score++;
                scoreframesCounter = 0;
            }

            if (IsKeyPressed(KEY_SPACE) && isGrounded) {
                gravity = gravity*-1;
            }

            if(player.y > screenHeight-(150+jumpspeed)){
                player.y = screenHeight-(150+jumpspeed);
                isGrounded = true;
            } else if(player.y < 100+jumpspeed){
                player.y = 100+jumpspeed;
                isGrounded = true;
            } else {
                isGrounded = false;
            }

            player.y += gravity*jumpspeed;

            if( isColliding(spikes, {player.x, player.y}) || isColliding(spikes, {player.x+50, player.y}) || isColliding(spikes, {player.x+25, player.y+25}) || isColliding(spikes, {player.x+50, player.y+50}) || isColliding(spikes, {player.x, player.y+50})){
                if(score > highscore){
                    highscore = score;
                }
                gameover = true;
                playing = false;
            }

            //draw
            BeginDrawing();
            ClearBackground(RAYWHITE);

            for(int i = 0; i < spikes.size(); i++){
                spikes[i].x-=speed;
                if (spikes[i].y == 1){
                    DrawTriangle({spikes[i].x, screenHeight-100}, {spikes[i].x+50, screenHeight-100}, {spikes[i].x+25, screenHeight-150}, RED);
                } else {
                    DrawTriangle({spikes[i].x, 100}, {spikes[i].x+25, 150}, {spikes[i].x+50, 100}, RED);
                }
                
                if(spikes[i].x < -100){
                    spikes.erase(spikes.begin());
                }
            }

            DrawRectangleRec(roof, LIGHTGRAY);
            DrawRectangleRec(ground, LIGHTGRAY);

            DrawRectangleRec(player, BLACK);

            DrawText(TextFormat("score: %i", score), 10, 10, 50, BLACK);
            DrawText("press [space] to jump", 10, screenHeight-30, 20, BLACK);

            EndDrawing();
        } else {
            mousePoint = GetMousePosition();

            if (CheckCollisionPointRec(mousePoint, playagainbutton)) {
                if (IsMouseButtonReleased(0)) {
                    gameover = false;
                    playing = true;
                    framesCounter = 0;
                    scoreframesCounter = 0;
                    score = 0;
                    spikes.clear();
                    gravity = 1;
                    player.y = screenHeight-150;
                }
            }

            if (CheckCollisionPointRec(mousePoint, menubutton)) {
                if (IsMouseButtonReleased(0)) {
                    SaveFileText("score.txt", (char*)std::to_string(highscore).c_str());
                    CloseWindow();
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            if(gameover){
                title = "Gameover";
                playbouttontext = "retry";
                DrawText(TextFormat("score: %i", score), (screenWidth/2)-(MeasureText(TextFormat("score: %i", score), 80)/2), screenHeight/2-80, 80, BLACK);
            } else {
                title = "Gravity Jump";
                playbouttontext = "play";
            }
            
            DrawText(title, (screenWidth/2)-(MeasureText(TextFormat(title, score), 150)/2), 10, 150, BLACK);

            DrawText(TextFormat("high score: %i", highscore), (screenWidth/2)-(MeasureText(TextFormat("high score: %i", highscore), 50)/2), screenHeight/2, 50, BLACK);
                
            DrawRectangleRec(playagainbutton, LIGHTGRAY);
            DrawText(playbouttontext, playagainbutton.x+200/2-MeasureText(playbouttontext, 40)/2, playagainbutton.y+15, 40, BLACK);

            DrawRectangleRec(menubutton, LIGHTGRAY);
            DrawText("exit", menubutton.x+200/2-MeasureText("exit", 40)/2, menubutton.y+15, 40, BLACK);

            EndDrawing();
        }
    }
    SaveFileText("score.txt", (char*)std::to_string(highscore).c_str());
    CloseWindow();

    return 0;
}