// Menu.h
#include "Scene.h"

class Menu : public Scene {
public:
    const int ENEMY_COUNT = 0;
    GLuint m_background_texture = 0;

    ~Menu();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    const int get_number_of_enemies() const override { return ENEMY_COUNT; }
};