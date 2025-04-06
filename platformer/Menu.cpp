// Menu.cpp
/**
* Author: Kristie Lee
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Menu.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

constexpr char MENU_TEXTURE_PATH[] = "assets/menu_screen_transp.png",
SPRITESHEET_FILEPATH[] = "assets/mario.png",
PLATFORM_FILEPATH[] = "assets/spritesheet_ground.png";

unsigned int MENU_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

Menu::~Menu()
{
    delete    m_game_state.player;
    delete    m_game_state.map;
    Mix_FreeChunk(m_game_state.jump_sfx);
    //Mix_FreeMusic(m_game_state.bgm);
}


void Menu::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/spritesheet_ground.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, MENU_DATA, map_texture_id, 1.0f, 8, 16);

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
        3.0f,                      // jumping power
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

    /**
     BGM and SFX
     */
    m_background_texture = Utility::load_texture(MENU_TEXTURE_PATH);
}

void Menu::update(float delta_time)
{

}


void Menu::render(ShaderProgram* program)
{
    m_game_state.map->render(program);

    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, glm::vec3(5.0f, -3.5f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(15.0f, 10.0f, 1.0f));

    program->set_model_matrix(model_matrix);

    float vertices[] = {
        -0.5f, -0.5f,   0.5f, -0.5f,   0.5f,  0.5f,
        -0.5f, -0.5f,   0.5f,  0.5f,  -0.5f,  0.5f
    };

    float tex_coords[] = {
        0.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f,
        0.0f, 1.0f,   1.0f, 0.0f,   0.0f, 0.0f
    };

    glBindTexture(GL_TEXTURE_2D, m_background_texture);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
