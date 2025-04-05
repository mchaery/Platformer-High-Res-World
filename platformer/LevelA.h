// LevelA.h
#include "Scene.h"

class LevelA : public Scene {
public:
    const int ENEMY_COUNT = 1;

    ~LevelA();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
    const int get_number_of_enemies() const override { return ENEMY_COUNT; }
};