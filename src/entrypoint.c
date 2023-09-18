#include "entrypoint.h"
#include <raylib.h>
#include <stdlib.h>

#define NUM_GRID_SQUARES 8
void raylib_start(void){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    size_t factor = 60;
    InitWindow(factor*16, factor*9, "Maze");
    Vector2 player_pos = {0};
    Vector2 end_pos = {.x=4,.y=6};
    while (!WindowShouldClose()) {
        BeginDrawing();
        int w = GetRenderWidth();
        int h = GetRenderHeight();
        size_t gridsize = w/NUM_GRID_SQUARES > h/NUM_GRID_SQUARES ? h/NUM_GRID_SQUARES : w/NUM_GRID_SQUARES;

        for(int x = 0 ; x < gridsize* NUM_GRID_SQUARES;x+= gridsize){
            for(int y = 0 ; y < gridsize* NUM_GRID_SQUARES;y+= gridsize){
                DrawRectangleLines(x,y,gridsize,gridsize,GetColor(0xFF0000FF));
            }
        }
        DrawRectangle(end_pos.x*gridsize,end_pos.y*gridsize,gridsize,gridsize,GetColor(0x008000FF));
        DrawCircle(player_pos.x*gridsize+gridsize*0.5f,player_pos.y*gridsize+gridsize*0.5f,gridsize*0.5f,GetColor(0x0000FFFF));
        
        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
}