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

    mouseIcon = entity->CreateChild()->AddComponent<Texture>();

    pausebtn_handle = gui->pauseButton->OnClicked += [&] (const Button& caller) {
        SetPaused(clock.IsPaused());
    };

    gui->buildServerButton->OnClicked += [&] (const Button& caller) {
        if (true)
        {
            SetPaused(true);
            mouseIcon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_icon.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
            mouseIcon->SetColorMod(150, 255, 150);
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
    gui->pauseButton->sprite->ChangeState(paused ? "pause" : "play");
    clock.SetPaused(!paused);
}

void GameController::BuildServer(Vector2 position)
{
}

void GameController::Update()
{

    if (mouseMode == PLACE_SERVER)
    {
        Vector2 mousePos = gui->input->GetHandler<MouseHandler>()->GetMousePosition();
        Vector2 closest = Vector2(-4000, -4000);
        for (Vector2& pos : grid)
        {
            if (pos.DistanceSquared(mousePos) < closest.DistanceSquared(mousePos))
            {
                closest = pos;
            }
        }
        mouseIcon->GetTransform()->SetWorldPosition(closest);
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

}
