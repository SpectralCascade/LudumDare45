#include "gamecontroller.h"
#include "server.h"
#include "gui.h"

using namespace Ossium;

Delta delta;

REGISTER_COMPONENT(GameController);

void GameController::OnCreate()
{
    gui = entity->AddComponent<GUI>();
    GetService<InputController>()->AddContext("GUI", gui->input);

    pausebtn_handle = gui->pauseButton->OnClicked += [&] (const Button& caller) {
        bool ispaused = clock.IsPaused();
        gui->pauseButton->sprite->ChangeState(ispaused ? "pause" : "play");
        clock.SetPaused(!ispaused);
    };

    /*Rand rng;

    for (int i = 0; i < serversToWin; i++)
    {
        Server* server = entity->GetScene()->CreateEntity(entity)->AddComponent<Server>();
        server->GetTransform()->SetWorldPosition(Vector2(rng.Int(10, 1280 - 10), rng.Int(10, 768 - 10)));

        servers.push_back(server);
    }*/

}

void GameController::BuildServer(Vector2 position)
{
}

void GameController::Update()
{

    // Win condition
    bool won = true;
    for (auto server : servers)
    {
        /*if (server.connections.size() < 3)
        {
            won = false;
            break;
        }*/
    }

    if (won)
    {
        // TODO win state
    }

}
