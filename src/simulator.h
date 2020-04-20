#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Ossium.h"

using namespace Ossium;

// Forward declarations
class GameController;
class GameSim;

class Automaton : public GraphicComponent
{
public:
    DECLARE_COMPONENT(GraphicComponent, Automaton);

    virtual void Simulate(GameSim& sim, GameController& game, int stage);

    virtual void Render(Renderer& renderer);

};

class GameSim : public Component
{
public:
    DECLARE_COMPONENT(Component, GameSim);

    void UpdateSim(GameController& game);

    unsigned int GetTimeStep();

//
//  GAME LOGIC VARIABLES
//

    int money = 10000;//145;

    inline static const int server_cost = 50;

    inline static const int repairs_cost = 15;

private:
    unsigned int timeStep = 0;

};

#endif // SIMULATOR_H
