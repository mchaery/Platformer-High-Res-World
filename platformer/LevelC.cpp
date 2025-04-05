#include "LevelC.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/mario.png",
PLATFORM_FILEPATH[] = "assets/spritesheet_ground.png",
ENEMY_FILEPATH[] = "assets/spritesheet_enemies.png";

unsigned int LevelC_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 53, 45, 0, 0, 0, 0, 0, 0, 0, 0, 5,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0,
    0, 53, 45, 0, 0, 0, 0, 0, 0, 0, 53, 76, 45, 0, 
    0, 0, 0, 53, 76, 76, 45, 0, 0, 0, 0, 69, 0, 0,
};

LevelC::~LevelC()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    //Mix_FreeMusic(m_game_state.bgm);
}

void LevelC::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/spritesheet_ground.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LevelC_DATA, map_texture_id, 1.0f, 8, 16);

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
        3.5f,                      // jumping power
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
    m_game_state.player->set_position(glm::vec3(2.5f, 0.0f, 0.0f));

    /* Enemies' stuff */
    GLuint enemy_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    int enemy_flying_animation[4][4] =
    {
        { 43, 59, 43, 59 },   // LEFT
        { 44, 60, 44, 60 },   // RIGHT
        { 43, 59, 43, 59 },   // UP
        { 43, 59, 43, 59 }    // DOWN
    };

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i] = Entity(enemy_texture_id, 1.0f, 0.8f, 0.8f, ENEMY, FLYER, IDLE);
        m_game_state.enemies[i].set_walking(enemy_flying_animation);
        m_game_state.enemies[i].set_animation_rows(16);
        m_game_state.enemies[i].set_animation_cols(8);
        m_game_state.enemies[i].set_animation_frames(4);
        m_game_state.enemies[i].face_left();
    }
    m_game_state.enemies[0].set_position(glm::vec3(1.5f, 2.0f, 0.0f));
    m_game_state.enemies[0].set_position(glm::vec3(9.0f, 3.0f, 0.0f));


    /**
     BGM and SFX
     */
    m_game_state.jump_sfx = Mix_LoadWAV("assets/spinjump.wav");
    Mix_VolumeChunk(m_game_state.jump_sfx, 48);
}

void LevelC::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }
}


void LevelC::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    for (int i = 0; i < ENEMY_COUNT; i++)
        m_game_state.enemies[i].render(g_shader_program);
}
