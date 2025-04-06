/**
* Author: Kristie Lee
* Assignment: Rise of the AI
* Date due: 2025-04-05, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Menu.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"


// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

constexpr float BG_RED = 208 / 255.0f,
BG_BLUE = 244 / 255.0f,
BG_GREEN = 247 / 255.0f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr char WIN_MESSAGE_FILEPATH[] = "assets/win_screen.png";
constexpr char LOSE_MESSAGE_FILEPATH[] = "assets/lose_screen.png";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene* g_current_scene;
Menu* g_menu_screen;
LevelA* g_level_a;
LevelB* g_level_b;
LevelC* g_level_c;

int g_lives = 3;
bool g_player_win = false;
bool g_player_lose = false;

GLuint g_lives_texture[4];
GLuint g_win_texture;
GLuint g_lose_texture;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix, g_gameover_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

// ----- AUDIO STUFF ------ //
Mix_Music* g_bgm;
Mix_Chunk* g_sfx_fail;
Mix_Chunk* g_sfx_success;
constexpr char BGM_FILEPATH[] = "assets/Adventures.mp3";
constexpr char SFX_FAIL_FILEPATH[] = "assets/game_over.wav";
constexpr char SFX_SUCCESS_FILEPATH[] = "assets/bling.wav";

void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Welcome to High-Res World!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);


    // ————— SCENES SETUP ————— //
    g_menu_screen = new Menu();
    switch_to_scene(g_menu_screen);

    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();

    // ----- AUDIO SETUP ----- //
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    g_bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(g_bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    g_sfx_fail = Mix_LoadWAV(SFX_FAIL_FILEPATH);
    g_sfx_success = Mix_LoadWAV(SFX_SUCCESS_FILEPATH);
    Mix_VolumeChunk(g_sfx_fail, 96);
    Mix_VolumeChunk(g_sfx_success, 96);

    // ----- LIVES UI ----- //
    g_lives_texture[0] = Utility::load_texture("assets/lives0.png");
    g_lives_texture[1] = Utility::load_texture("assets/lives1.png");
    g_lives_texture[2] = Utility::load_texture("assets/lives2.png");
    g_lives_texture[3] = Utility::load_texture("assets/lives3.png");

    // ––––– MESSAGES ––––– //
    g_gameover_matrix = glm::mat4(1.0f);
    g_gameover_matrix = glm::translate(g_gameover_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
    g_gameover_matrix = glm::scale(g_gameover_matrix, glm::vec3(15.0f));
    g_win_texture = Utility::load_texture(WIN_MESSAGE_FILEPATH);
    g_lose_texture = Utility::load_texture(LOSE_MESSAGE_FILEPATH);


    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    if (g_current_scene->get_state().player != nullptr) {

        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // ————— KEYSTROKES ————— //
            switch (event.type) {
                // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_q:
                    // Quit the game with a keystroke
                    g_app_status = TERMINATED;
                    break;

                case SDLK_RETURN:
                    if (g_current_scene == g_menu_screen) {
                        switch_to_scene(g_level_a);
                    }
                    break;

                case SDLK_SPACE:
                    // ————— JUMPING ————— //
                    if (g_current_scene->get_state().player->get_collided_bottom())
                    {
                        g_current_scene->get_state().player->jump();
                        Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                    }
                    break;

                default:
                    break;
                }

            default:
                break;
            }
        }


        // ————— KEY HOLD ————— //
        const Uint8* key_state = SDL_GetKeyboardState(NULL);

        if (key_state[SDL_SCANCODE_LEFT] && !g_player_lose && !g_player_win)        g_current_scene->get_state().player->move_left();
        else if (key_state[SDL_SCANCODE_RIGHT] && !g_player_lose && !g_player_win)  g_current_scene->get_state().player->move_right();

        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }

}

void update()
{
    if (g_player_win || g_player_lose) return;

    Entity* player = g_current_scene->get_state().player;

    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    // ----- GAME FLOW ----- //
    if (player != nullptr) {
        // enemy
        for (int i = 0; i < g_current_scene->get_number_of_enemies(); ++i) {
            Entity& enemy = g_current_scene->get_state().enemies[i];

            if (player->check_collision(&enemy)) {
                g_lives--;
                Mix_PlayChannel(-1, g_sfx_fail, 0);

                if (g_lives > 0) {
                    g_current_scene->initialise();  // restart the level
                }
                else {
                    g_player_lose = true;
                }
                return;
            }
        }

        // fall
        if (player->get_position().y < -10.0f)
        {
            g_lives--;
            Mix_PlayChannel(-1, g_sfx_fail, 0);

            if (g_lives > 0) {
                g_current_scene->initialise();  // restart the level
            }
            else {
                g_player_lose = true;
            }
            return;
        }

        // reach the goal
        if (player->get_position().x > 12.97f) {
            Mix_PlayChannel(-1, g_sfx_success, 0);

            if (g_current_scene == g_level_a) {
                switch_to_scene(g_level_b);
            }
            else if (g_current_scene == g_level_b) {
                switch_to_scene(g_level_c);
            }
            else if (g_current_scene == g_level_c) {
                g_player_win = true;
            }
        }
    }

    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);

    if (player != nullptr) {
        if (player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-player->get_position().x, 3.75f, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5.0f, 3.75f, 0));
        }
    }
}

void draw_gameover_message(GLuint texture, Scene* g_current_scene) {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    float player_xpos = g_current_scene->get_player()->get_position().x;
    float message_xpos = (player_xpos > 5.0f) ? player_xpos : 5.0f;
    model_matrix = glm::translate(model_matrix, glm::vec3(message_xpos, -3.5f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(8.0f, 8.0f, 0.0f));
    
    g_shader_program.set_model_matrix(model_matrix);
    Utility::draw_textured_quad(texture, model_matrix, g_shader_program);
}

void draw_lives_ui() {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    float player_xpos = g_current_scene->get_player()->get_position().x;
    float lives_xpos = (player_xpos > 5.0f) ? player_xpos + 3.75f : 8.75f;
    model_matrix = glm::translate(model_matrix, glm::vec3(lives_xpos, -0.5f, 0.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(2.1f, 0.7f, 0.0f));

    g_shader_program.set_model_matrix(model_matrix);
    Utility::draw_textured_quad(g_lives_texture[g_lives], model_matrix, g_shader_program);
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);

    // --- LIVES UI --- //
    draw_lives_ui();

    // --- MESSAGE --- //
    if (g_player_win) {
        draw_gameover_message(g_win_texture, g_current_scene);
    }
    else if (g_player_lose) {
        draw_gameover_message(g_lose_texture, g_current_scene);
    }

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_menu_screen;
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}