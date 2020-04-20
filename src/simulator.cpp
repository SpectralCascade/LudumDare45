#include "simulator.h"
#include "gamecontroller.h"

using namespace Ossium;

REGISTER_COMPONENT(Automaton);

void Automaton::Render(Renderer& renderer)
{
}

void Automaton::Simulate(GameSim& sim, GameController& game, int stage)
{
}

REGISTER_COMPONENT(GameSim);

void GameSim::UpdateSim(GameController& game)
{
    timeStep++;
    vector<Automaton*> automatons = entity->GetComponentsInChildren<Automaton>();
    for (Automaton* automaton : automatons)
    {
        automaton->Simulate(*this, game, 0);
        //Log.Info("SIMULATING {0}", automaton->GetEntity()->name);
    }
    for (Automaton* automaton : automatons)
    {
        automaton->Simulate(*this, game, 1);
    }
}

unsigned int GameSim::GetTimeStep()
{
    return timeStep;
}
