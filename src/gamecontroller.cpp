#include "gamecontroller.h"
#include "server.h"
#include "gui.h"
#include "simulator.h"

using namespace Ossium;

Delta delta;
// TODO: get random seed from current time
Rand rng;
GameController* global_game = nullptr;

REGISTER_COMPONENT(GameController);

void GameController::OnCreate()
{
    global_game = this;
    simulator = entity->AddComponent<GameSim>();

    gui = entity->AddComponent<GUI>();
    gui->game = this;
    GetService<InputController>()->AddContext("GUI", gui->input);

    mouseIcon = entity->CreateChild()->AddComponent<Texture>();

    pausebtn_handle = gui->pauseButton->OnClicked += [&] (const Button& caller) {
        SetPaused(!clock.IsPaused());
    };

    gui->buildServerButton->OnClicked += [&] (const Button& caller) {
        if (true)
        {
            SetPaused(true);
            mouseIcon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_icon.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
            if (mouseMode != NONE)
            {
                // TODO clear mouse mode
            }
            mouseMode = PLACE_SERVER;
        }
    };

    MouseHandler* mouse = gui->input->GetHandler<MouseHandler>();
    mouse->AddAction(
        "GUI_click",
        [&] (const MouseInput& data) {
            if (data.state)
            {
                switch (mouseMode)
                {
                case PLACE_SERVER:
                    mouseIcon->SetSource(nullptr);
                    mouseMode = NONE;
                    BuildServer(closest);
                    break;
                case NONE:
                    break;
                }
            }
            return ActionOutcome::Ignore;
        },
        MouseEvent::MOUSE_BUTTON_LEFT
    );

    /*Rand rng;

    for (int i = 0; i < serversToWin; i++)
    {
        Server* server = entity->GetScene()->CreateEntity(entity)->AddComponent<Server>();
        server->GetTransform()->SetWorldPosition(Vector2(rng.Int(10, 1280 - 10), rng.Int(10, 768 - 10)));

        servers.push_back(server);
    }*/

}

void GameController::SetPaused(bool paused)
{
    gui->pauseButton->sprite->ChangeState(paused ? "play" : "pause");
    clock.SetPaused(paused);
}

void GameController::BuildServer(unsigned int pos)
{
    if (simulator->money < 50)
    {
        // Show error
        return;
    }

    auto itr = servers.find(pos);
    if (itr != servers.end() && itr->second != nullptr)
    {
        // already placed server here
        return;
    }
    servers[pos] = entity->CreateChild()->AddComponent<Server>();
    servers[pos]->GetEntity()->AddComponent<Texture>()->SetSource(
        GetService<ResourceController>()->Get<Image>("assets/server_icon.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC)
    );
    servers[pos]->GetTransform()->SetWorldPosition(grid[pos]);

    simulator->money -= 50;

    SetPaused(false);
}

void GameController::Update()
{

    if (mouseMode == PLACE_SERVER)
    {
        Vector2 mousePos = gui->input->GetHandler<MouseHandler>()->GetMousePosition();
        Vector2 closestVec = Vector2(-4000, -4000);
        for (unsigned int i = 0, counti = grid.size(); i < counti; i++)
        {
            Vector2 pos = grid[i];
            if (pos.DistanceSquared(mousePos) < closestVec.DistanceSquared(mousePos))
            {
                closestVec = pos;
                closest = i;
            }
        }
        mouseIcon->GetTransform()->SetWorldPosition(closestVec);
    }

    // Update the simulation
    if (clock.GetTime() - lastStepTime >= 1000)
    {
        simulator->UpdateSim(*this);
        lastStepTime = clock.GetTime();
    }

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

    clock.Update(delta.Time());

}
