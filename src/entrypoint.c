#include "entrypoint.h"

#include "game.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/**
 * @brief Q-learning algorithm settings.
 *
 */
#define QLEARN_N_STATES 16 // 4 bits i.e. 2^4, can be up to 8 bits i.e. 2^8
#define QLEARN_N_ACTIONS 5
#define QLEARN_LEARNING_RATE 0.1f
#define QLEARN_DISCOUNT_FACTOR 0.9f
#define QLEARN_EPSILON_RATE 0.9999f


/* Public typedefs ---------------------------------------------------------- */

/**
 * @brief Q-value data type.
 *
 */
typedef float qvalue_t;

/**
 * @brief Reward data type.
 *
 */
typedef int16_t reward_t;

/**
 * @brief State data type.
 *
 */
typedef uint16_t state_t;

/**
 * @brief Action data type.
 *
 */
typedef uint8_t action_t;

/**
 * @brief Q-learning opaque data type.
 *
 */
typedef struct qlearn qlearn_t;

/**
 * @brief Q-table configuration data structure.
 *
 */
typedef struct qtable_conf
{
    const uint32_t n_states;
    const uint8_t n_actions;
    const float learning_rate;
    const float discount_factor;
    const float epsilon_rate;
} qtable_conf_t;

/**
 * @brief Q-learning configuration data structure.
 *
 */
typedef struct qlearn_conf
{
    void *game;
    void (*restart)(void *);
    bool (*is_ended)(void *);
    void (*apply_action)(void *, uint8_t);
    state_t (*get_state)(void *);
    reward_t (*get_reward)(void *);
    qtable_conf_t *params;
} qlearn_conf_t;

/* Private typedefs --------------------------------------------------------- */

/**
 * @brief Q-learning algorithm data structure.
 *
 */
typedef struct qlearn
{
    float **table;
    void (*restart)();
    bool (*is_ended)();
    void (*apply_action)( action_t);
    state_t (*get_state)();
    reward_t (*get_reward)();
    uint32_t n_states;
    uint8_t n_actions;
    float learning_rate;
    float discount_factor;
    float epsilon;
    float epsilon_rate;
} qlearn_t;

/* Public functions --------------------------------------------------------- */

/**
 * @brief Save Q-table to file in CSV format.
 *
 * @param qlearn Q-learning instance.
 * @param filename File path where to save Q-table values.
 */
void qlearn_save_table(qlearn_t *qlearn, const char *filename)
{
    FILE *fptr = fopen(filename, "w");

    for (uint32_t i = 0; i < qlearn->n_states; i++)
    {
        for (uint8_t j = 0; j < qlearn->n_actions; j++)
        {
            fprintf(fptr, "%f,", qlearn->table[i][j]);
        }
        fprintf(fptr, "\n");
    }

    fclose(fptr);
}

/**
 * @brief Load Q-table values inside Q-learning instance.
 *
 * @param qlearn Q-learning instance.
 * @param filename File path where to read Q-table values.
 */
void qlearn_load_table(qlearn_t *qlearn, const char *filename)
{
    FILE *fptr = fopen(filename, "r");

    char line[100];
    for (uint32_t i = 0; i < qlearn->n_states; i++)
    {
        fgets(line, 100, fptr);
        char *token = strtok(line, ",");
        for (uint8_t j = 0; j < qlearn->n_actions; j++)
        {
            qlearn->table[i][j] = (float)atof(token);
            token = strtok(NULL, ",");
        }
    }

    qlearn->epsilon = 0.0;

    fclose(fptr);
}

/**
 * @brief Initialize Q-learning algorithm.
 *
 * @param conf Q-learning configurations.
 * @return qlearn_t*
 */
qlearn_t *qlearn_init(qlearn_conf_t *conf)
{
    qvalue_t **table = calloc(conf->params->n_states, sizeof(qvalue_t *));
    for (uint32_t i = 0; i < conf->params->n_states; i++)
    {
        table[i] = calloc(conf->params->n_actions, sizeof(qvalue_t));
    }

    qlearn_t *qlearn = malloc(sizeof(qlearn_t));
    // Game APIs
    qlearn->restart = conf->restart;
    qlearn->is_ended = conf->is_ended;
    qlearn->apply_action = conf->apply_action;
    qlearn->get_state = conf->get_state;
    qlearn->get_reward = conf->get_reward;
    // Q-table
    qlearn->table = table;
    // Q-learning parameters
    qlearn->n_states = conf->params->n_states;
    qlearn->n_actions = conf->params->n_actions;
    qlearn->learning_rate = conf->params->learning_rate;
    qlearn->discount_factor = conf->params->discount_factor;
    qlearn->epsilon = 1.0f;
    qlearn->epsilon_rate = conf->params->epsilon_rate;

    return qlearn;
}

/**
 * @brief Deinitialize Q-learning algorithm.
 *
 * @param qlearn Q-learning instance.
 */
void qlearn_deinit(qlearn_t *qlearn)
{
    for (uint16_t i = 0; i < qlearn->n_states; i++)
    {
        free(qlearn->table[i]);
    }
    free(qlearn->table);
    free(qlearn);
}

/**
 * @brief Restart training episode.
 *
 * @param qlearn Q-learning instance.
 */
void qlearn_restart(qlearn_t *qlearn)
{
    qlearn->restart();
}

/**
 * @brief Check if game has ended.
 *
 * @param qlearn Q-learning instance.
 * @return true
 * @return false
 */
bool qlearn_is_ended(qlearn_t *qlearn)
{
    qlearn->epsilon *= qlearn->epsilon_rate;
    return qlearn->is_ended();
}
/**
 * @brief Get reward for taking action a on state S.
 *
 * @param qlearn Q-learning instance.
 * @return reward_t
 */
reward_t qlearn_get_reward(qlearn_t *qlearn)
{
    return qlearn->get_reward();
}


/**
 * @brief Get state representation S.
 *
 * @param qlearn Q-learning instance.
 * @return state_t
 */
state_t qlearn_get_state(qlearn_t *qlearn)
{
    state_t state = qlearn->get_state();

    // Debug log message
    // printf("Epsilon: %f\tReward: %d\tState: %c %c %c %c \n",
    //        qlearn->epsilon,
    //        qlearn_get_reward(qlearn),
    //        (state & 0x1) ? '1' : '0',
    //        (state & 0x2) ? '1' : '0',
    //        (state & 0x4) ? '1' : '0',
    //        (state & 0x8) ? '1' : '0'
    //     );
    return state;
}

/**
 * @brief Apply action a on current state S.
 *
 * @param qlearn Q-learning instance.
 * @param a Action.
 * @return state_t
 */
state_t qlearn_apply_action(qlearn_t *qlearn, action_t a)
{
    qlearn->apply_action(a);
    return qlearn_get_state(qlearn);
}

/**
 * @brief Get maximum Q-value at state S entry in Q-table.
 *
 * @param qlearn Q-learning instance.
 * @param S State.
 * @return qvalue_t
 */
qvalue_t qlearn_get_max_qvalue(qlearn_t *qlearn, state_t S);

/**
 * @brief Get action a associated with highest Q-value for state S.
 *
 * NB: Epsilon greedy is used to balance exploration and exploitation during
 * training. ε starts from 1.0f and slowly decrease to 0.0f with more training
 * episodes.
 *
 * @param qlearn Q-learning instance.
 * @param S State.
 * @return action_t
 */
action_t qlearn_get_action(qlearn_t *qlearn, state_t S);

/**
 * @brief Update value Q(S, a) in Q-table using Bellman equation.
 *
 * @param qlearn Q-learning instance.
 * @param S State.
 * @param a Action.
 * @param R Reward.
 * @param Q_max Maximum future Q-value.
 */
void qlearn_update_qvalue(qlearn_t *qlearn, state_t S, action_t a, reward_t R, qvalue_t Q_max);

void raylib_start(void){
    
    srand((unsigned int)time(NULL));
    // Create game instance
	game_init();

	/* Q-learning initialization -------------------------------------------- */

	qtable_conf_t qtable_conf = {.n_states = QLEARN_N_STATES,
								 .n_actions = QLEARN_N_ACTIONS,
								 .learning_rate = QLEARN_LEARNING_RATE,
								 .discount_factor = QLEARN_DISCOUNT_FACTOR,
								 .epsilon_rate = QLEARN_EPSILON_RATE};

	qlearn_conf_t qlearn_conf = {.restart = game_restart,
								 .is_ended = game_is_ended,
								 .apply_action = game_apply_move,
								 .get_state = game_get_state,
								 .get_reward = game_get_reward,
								 .params = &qtable_conf};

	qlearn_t *qlearn = qlearn_init(&qlearn_conf);
    
    while (!WindowShouldClose()) {
        //Train
        if(qlearn->epsilon > 0.00000001f){
            state_t S = qlearn_get_state(qlearn);

            while (!qlearn_is_ended(qlearn))
            {
                action_t a = qlearn_get_action(qlearn, S);
                qvalue_t S_new = qlearn_apply_action(qlearn, a);
                reward_t reward = qlearn->get_reward();
                qvalue_t Q_max = qlearn_get_max_qvalue(qlearn, S_new);
                qlearn_update_qvalue(qlearn, S, a, reward, Q_max);

                S = S_new;

                /* Draw --------------------------------------------------------- */

                game_draw();
            }
            qlearn_restart(qlearn);
        }
        else {
            qlearn_restart(qlearn);
            state_t S = qlearn_get_state(qlearn);

            while (!qlearn_is_ended(qlearn))
            {
                action_t a = qlearn_get_action(qlearn, S);
                qvalue_t S_new = qlearn_apply_action(qlearn, a);
                reward_t reward = qlearn->get_reward();
                qvalue_t Q_max = qlearn_get_max_qvalue(qlearn, S_new);
                qlearn_update_qvalue(qlearn, S, a, reward, Q_max);

                S = S_new;

                /* Draw --------------------------------------------------------- */

                game_draw();
                WaitTime(1);
            }
            qlearn_restart(qlearn);
            break;
        }
        
    }

    qlearn_deinit(qlearn);
    game_deinit();
}