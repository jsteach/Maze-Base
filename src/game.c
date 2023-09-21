#include "game.h"
#include <raylib.h>
#include "raymath.h"
#include <float.h>
#include <assert.h>
#include <stdlib.h>

static size_t factor = 60;
static Vector2 player_pos = {0};
static Vector2 end_pos = {.x=4,.y=6};

#define REWARD_CLOSE +10
#define REWARD_FAR -10

void game_init(void){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(factor*16, factor*9, "Maze");

    game_restart();
}
void game_deinit(void){
    CloseAudioDevice();
    CloseWindow();
}
static size_t gridsize = 64; 
#define NUM_GRID_SQUARES 8
void game_draw(void){
    ClearBackground(GetColor(0x000000FF));
    BeginDrawing();
    int w = GetRenderWidth();
    int h = GetRenderHeight();
    gridsize = w/NUM_GRID_SQUARES > h/NUM_GRID_SQUARES ? h/NUM_GRID_SQUARES : w/NUM_GRID_SQUARES;

    for(int x = 0 ; x < gridsize* NUM_GRID_SQUARES;x+= gridsize){
        for(int y = 0 ; y < gridsize* NUM_GRID_SQUARES;y+= gridsize){
            DrawRectangleLines(x,y,gridsize,gridsize,GetColor(0xFF0000FF));
        }
    }
    DrawRectangle(end_pos.x*gridsize,end_pos.y*gridsize,gridsize,gridsize,GetColor(0x008000FF));
    DrawCircle(player_pos.x*gridsize+gridsize*0.5f,player_pos.y*gridsize+gridsize*0.5f,gridsize*0.5f,GetColor(0x0000FFFF));
    
    EndDrawing();
}

/**
 * @brief Restart game.
 *
 */
void game_restart(void);

/**
 * @brief Move snake.
 *
 * @param move Player move: IDLE, UP, DOWN, LEFT, or RIGHT.
 *
 */
void game_apply_move( uint8_t move){
    if(move == IDLE) return;
    switch (move)
    {
    case UP:
        player_pos.y -=1;
        break;
    case DOWN:
        player_pos.y +=1;
        break;
    case RIGHT:
        player_pos.x +=1;
        break;
    case LEFT:
        player_pos.x -=1;
        break;
    default:
        assert(0 && "unreachable");
    }
    
}

/**
 * @brief Check if the game ended (i.e., player on end).
 *
 * @return true
 * @return false
 */
bool game_is_ended(void){
    int check1 =player_pos.x == end_pos.x;
    int check2 =player_pos.y == end_pos.y; 
    int check3 = player_pos.x * gridsize > NUM_GRID_SQUARES * gridsize;
    int check4 = player_pos.y *gridsize > NUM_GRID_SQUARES * gridsize;
    int check5 = player_pos.x *gridsize < 0;
    int check6 = player_pos.y *gridsize < 0;
    return (check1 && check2) || check3 || check4 || check5 || check6; 
}

/**
 * @brief Get 8-bit state representation from game instance.
 *
 * @return uint16_t
 */
uint16_t game_get_state(void);

/**
 * @brief Get reward for making last move.
 *
 * @return int16_t
 */
int16_t game_get_reward(void);