#include "simulator.h"
#include "gamecontroller.h"
#include "server.h"
#include "gui.h"
#include "popup.h"
#include "connection.h"

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
    daysSinceHackers++;
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

    bool didWin = game.servers.empty() ? false : game.servers.size() >= 20;
    for (auto itr : game.servers)
    {
        if (itr.second->connections.size() < 2)
        {
            didWin = false;
            break;
        }
    }

    if (didWin)
    {
        game.gui->popup->AddMessage("\n\nThanks for playing! Don't forget to check out other people's awesome game jam games :)");
        game.gui->popup->AddMessage("<b>YOU WON!</b>\n\nCongratulations, you have successfully built and kept a global server network alive without succumbing to hackers or poor maintenance standards!");
        game.gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));
        // Force pause the game
        game.clock.Scale(0);
        game.gui->popup->ShowNextMessage();
    }
    else
    {
        // Check lose condition
        if ((game.servers.empty() || game.servers.size() < 2) && game.GetEntity()->GetComponentsInChildren<Connection>().empty() && money < server_cost + 10)
        {
            game.gui->popup->AddMessage("Thanks for playing! Feel free to play again, or go check out other people's awesome game jam games :)");
            game.gui->popup->AddMessage("<b>GAME OVER.</b>\n\nYour global network has died, and lies in ruins :(");
            game.gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));
            // Force pause the game
            game.clock.Scale(0);
            game.gui->popup->ShowNextMessage();
        }
    }

}

unsigned int GameSim::GetTimeStep()
{
    return timeStep;
}
