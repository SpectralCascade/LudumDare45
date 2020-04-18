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
    entity->GetScene()->WalkComponents<Automaton>(
        [&] (Automaton* automaton) {
            automaton->Simulate(*this, game);
        }
    );
}

unsigned int GameSim::GetTimeStep()
{
    return timeStep;
}
