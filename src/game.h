#include <stdbool.h>
#include <stdint.h>

typedef enum move
{
    IDLE,
    UP,
    DOWN,
    LEFT,
    RIGHT
} game_move_t;

/**
 * @brief Initialize game.
 *
 * @return void*
 */
void game_init(void);

/**
 * @brief Deinitialize  game.
 *
 */
void game_deinit(void);


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
void game_apply_move( uint8_t move);

/*
 * @brief Check if the game ended.
 *

 * @return true
 * @return false
 */
bool game_is_ended(void);

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