// LevelB.h
#include "Scene.h"

class LevelB : public Scene {
public:
    const int ENEMY_COUNT = 2;

    ~LevelB();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    const int get_number_of_enemies() const override { return ENEMY_COUNT; }
};