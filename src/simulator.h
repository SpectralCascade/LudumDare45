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

    int daysSinceHackers = 0;

    inline static const int hackerInterval = 100;

    int money = 550;

    inline static const int purge_cost = 500;

    inline static const int server_cost = 200;

    inline static const int repairs_cost = 50;

private:
    unsigned int timeStep = 0;

};

#endif // SIMULATOR_H
