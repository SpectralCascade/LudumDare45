#include "simulator.h"
#include "gamecontroller.h"

using namespace Ossium;

REGISTER_COMPONENT(Automaton);

void Automaton::Render(Renderer& renderer)
{
}

void Automaton::Simulate(GameSim& sim, GameController& game)
{
}

REGISTER_COMPONENT(GameSim);

void GameSim::UpdateSim(GameController& game)
{
    timeStep++;
    vector<Automaton*> automatons = entity->GetComponentsInChildren<Automaton>();
    for (Automaton* automaton : automatons)
    {
        automaton->Simulate(*this, game);
    }
}

unsigned int GameSim::GetTimeStep()
{
    return timeStep;
}
