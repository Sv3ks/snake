#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <time.h>
#include <charconv>
#include <iostream> // for logging

typedef std::pair<int,int> pair;

enum DIRECTION {
    IDLE,
    UP,
    DOWN,
    RIGHT,
    LEFT,
};

const int width = 20, height = 15;
const int screenWidth = 800, screenHeight = 600;
const int tileWidth = screenWidth/width; //screenWidth / width;
const int offsetX = 0, offsetY = 0;

int FPS = 60, UPS = 3, FPU = FPS/UPS; // Frames Per Second, Updates Per Second, Frames Per Update

bool drawOutlines = false;
bool allowOverlap = false;
int fruitAmount = 2;


bool gameOver = false;

std::vector<pair> player = {};
DIRECTION dir = IDLE;
DIRECTION lastDir = IDLE;

std::vector<pair> fruit = {};
void generate_fruit() {
    while(fruit.size()<fruitAmount) {
        bool generate; pair temp;

        do {
            generate = false;

            // generate new position
            temp = {rand() % width, rand() % height};
            
            // check if square is taken
            for(pair i:fruit) if(i==temp) generate = true;
            for(pair i:player) if(i==temp) generate = true;
        
        } while (generate);

        fruit.push_back(temp);
    }
}

int frameCounter = 0;
void Update() {
    frameCounter++;

    DIRECTION tmp = dir; lastDir = dir;
    if(IsKeyPressed(KEY_UP)&&tmp!=DOWN) dir = UP;
    if(IsKeyPressed(KEY_DOWN)&&tmp!=UP) dir = DOWN;
    if(IsKeyPressed(KEY_RIGHT)&&tmp!=LEFT) dir = RIGHT;
    if(IsKeyPressed(KEY_LEFT)&&tmp!=RIGHT) dir = LEFT; 

    if(frameCounter != FPU) return;

    frameCounter = 0;

    if(gameOver)return;
    if(dir==IDLE) return;
    
    // MOVE
    pair front = player.at(player.size()-1);
    switch(dir) {
        case UP:
            front.second--;
            break;
        case DOWN:
            front.second++;
            break;
        case LEFT:
            front.first--;
            break;
        case RIGHT:
            front.first++;
            break;
    }

    // DEATH CHECK
    if(!allowOverlap) {
        for(int i=0;i<player.size()-1;i++) { // playersize-1 cuz the end of the vector is the front 
            if(player[i]==front) {
                gameOver = true;
                return;
            }
        }
    }

    if(front.first<0 || front.first+1>width || front.second<0 || front.second+1>height) {
        gameOver = true;
        return;
    }

    player.push_back(front);

    // FRUIT
    bool ate = false;

    for(int i=0;i<fruit.size();i++) {
        if(front == fruit[i]) {
            ate = true;
            fruit.erase(fruit.begin()+i);
            generate_fruit();

            // increase speed
            if(player.size()%(2*fruitAmount)==0&&UPS<15) {
                FPU = FPS/++UPS;
                std::cout<<UPS<<std::endl;
            }
        }
    }

    if(!ate) player.erase(player.begin()); // pop back of snake
}

void Draw() {
    BeginDrawing();

    // BACKGROUND
    ClearBackground(BLACK);

    // DRAW OUTLINES
    if(drawOutlines) for(int x=0;x<width;x++) for(int y=0;y<height;y++) DrawRectangleLines(offsetX+x*tileWidth,offsetY+y*tileWidth,tileWidth,tileWidth,GRAY);

    // DRAW OBJECTS
    for(pair i:player) DrawRectangle(offsetX+i.first*tileWidth,offsetY+i.second*tileWidth,tileWidth,tileWidth,GREEN);
    for(pair i:fruit) DrawRectangle(offsetX+i.first*tileWidth,offsetY+i.second*tileWidth,tileWidth,tileWidth,RED);

    // TEXT
    if(gameOver) {
        DrawText("GAME OVER", screenWidth/2-MeasureText("GAME OVER",screenHeight/6)/2, screenHeight/2-screenHeight/6/2-screenHeight/10, screenHeight/6, WHITE);

        DrawText("SCORE:       ",screenWidth/2-MeasureText("SCORE:       ",screenHeight/10)/2, screenHeight/2-screenHeight/10/2+screenHeight/10, screenHeight/10, WHITE);
        DrawText(std::to_string(player.size()).c_str(),screenWidth/2+MeasureText("SCORE:       ",screenHeight/10)/6, screenHeight/2-screenHeight/10/2+screenHeight/10, screenHeight/10, WHITE);
    } else {
        DrawText("SCORE:", 40, 20, 40, WHITE);
        DrawText(std::to_string(player.size()).c_str(), 200, 20, 40, WHITE);
    }

    EndDrawing();
}

void Start() {    
    // Set random seed
    srand(time(NULL));

    // starting posistion
    player.push_back({width/2,height/2});
    generate_fruit();
}

int main() {
    // Window Setup
    InitWindow(screenWidth, screenHeight, "Snake");
    SetTargetFPS(60);    
    
    Start();

    while(!WindowShouldClose()) {
        Draw();
        Update();
    }

    CloseWindow();
}