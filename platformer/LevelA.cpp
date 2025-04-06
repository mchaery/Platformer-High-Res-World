// levelA.cpp
/**
* Author: Kristie Lee
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "LevelA.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/mario.png",
PLATFORM_FILEPATH[] = "assets/spritesheet_ground.png",
ENEMY_FILEPATH[] = "assets/spritesheet_enemies.png";

unsigned int LEVELA_DATA[] =
{
    35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    35, 56, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8,
    35, 8, 8, 8, 8, 8, 8, 8, 8, 8, 1, 1, 1, 1,
    35, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

LevelA::~LevelA()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    //Mix_FreeMusic(m_game_state.bgm);
}

void LevelA::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/spritesheet_ground.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 8, 16);

    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    int player_walking_animation[4][4] =
    {
    { 4, 5, 6, 7 },  // to move to the left,
    { 8, 9, 10, 11}, // to move to the right,
    { 0, 0, 0, 0 },  // to move upwards,
    { 0, 0, 0, 0 }   // to move downwards
    };

    glm::vec3 acceleration = glm::vec3(0.0f, -4.905f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        3.2f,                      // jumping power
        player_walking_animation,  // animation index sets
        0.0f,                      // animation time
        4,                         // animation frame amount
        0,                         // current animation index
        4,                         // animation column amount
        4,                         // animation row amount
        1.0f,                      // width
        1.0f,                       // height
        PLAYER
    );
    m_game_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));

    /* Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[1];

    int enemy_walking_animation[4][4] =
    {
        { 16, 32, 16, 32 },   // LEFT
        { 23, 39, 23, 39 },   // RIGHT
        { 0, 0, 0, 0 },   // UP
        { 0, 0, 0, 0 }    // DOWN
    };

    m_game_state.enemies[0] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, GUARD, IDLE);
    m_game_state.enemies[0].set_walking(enemy_walking_animation);
    m_game_state.enemies[0].set_animation_rows(16);
    m_game_state.enemies[0].set_animation_cols(8);
    m_game_state.enemies[0].set_animation_frames(4);
    m_game_state.enemies[0].face_left();
    m_game_state.enemies[0].set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.0f, 0.0f));

    /**
     BGM and SFX
     */
    m_game_state.jump_sfx = Mix_LoadWAV("assets/spinjump.wav");
    Mix_VolumeChunk(m_game_state.jump_sfx, 48);
}

void LevelA::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
}


void LevelA::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++)
        m_game_state.enemies[i].render(g_shader_program);
}
