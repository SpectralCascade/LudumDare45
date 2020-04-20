#include "gamecontroller.h"
#include "server.h"
#include "gui.h"
#include "simulator.h"
#include "connection.h"

using namespace Ossium;

Delta delta;
// TODO: get random seed from current time
Rand rng;
GameController* global_game = nullptr;

REGISTER_COMPONENT(GameController);

void GameController::OnCreate()
{
    /// TODO: REMOVE ME
    clock.Scale(2);

    connections_entity = entity;

    global_game = this;
    simulator = entity->AddComponent<GameSim>();

    gui = entity->AddComponent<GUI>();
    gui->game = this;
    GetService<InputController>()->AddContext("GUI", gui->input);

    mouseIcon = entity->CreateChild()->AddComponent<Texture>();
    mouseIcon->SetRenderLayer(90);

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

    gui->repairButton->OnClicked += [&] (const Button& caller) {
        if (true)
        {
            SetPaused(true);
            mouseIcon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_repairing.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
            if (mouseMode != NONE)
            {
                // TODO clear mouse mode
            }
            mouseMode = REPAIR_SERVER;
        }
        else
        {
        }
    };

    gui->connectorButton->OnClicked += [&] (const Button& caller) {
        if (true)
        {
            SetPaused(true);
            mouseIcon->SetSource(GetService<ResourceController>()->Get<Image>("assets/lay_connector.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
            if (mouseMode != NONE)
            {
                // TODO clear mouse mode
            }
            mouseMode = CONNECT_SERVER_START;
        }
        else
        {
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
                    {
                        mouseIcon->SetSource(nullptr);
                        mouseMode = NONE;
                        BuildServer(closest);
                        break;
                    }
                    case REPAIR_SERVER:
                    {
                        mouseIcon->SetSource(nullptr);
                        mouseMode = NONE;
                        RepairServer(Vector2(data.x, data.y));
                        break;
                    }
                    case CONNECT_SERVER_START:
                    {
                        Server* server = FindServer(Vector2(data.x, data.y));
                        if (server != nullptr)
                        {
                            connectee = server;
                            mouseMode = CONNECT_SERVER_END;
                        }
                        else
                        {
                            // flash cancel
                            mouseMode = NONE;
                            mouseIcon->SetSource(nullptr);
                        }
                        break;
                    }
                    case CONNECT_SERVER_END:
                    {
                        Server* server = FindServer(Vector2(data.x, data.y));
                        if (server != nullptr)
                        {
                            ConnectServers(connectee, server);
                        }
                        else
                        {
                            // flash cancel
                        }
                        mouseMode = NONE;
                        connectee = nullptr;
                        mouseIcon->SetSource(nullptr);
                        break;
                    }
                    case NONE:
                    {
                        break;
                    }
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
    SetPaused(false);

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
    servers[pos]->GetEntity()->GetComponent<Texture>()->SetRenderLayer(40);
    servers[pos]->GetTransform()->SetWorldPosition(grid[pos]);
    servers[pos]->serverId = pos;

    simulator->money -= 50;

}

Server* GameController::FindServer(Vector2 pos)
{
    for (auto itr : servers)
    {
        if (itr.second != nullptr && itr.second->GetEntity()->GetComponent<Texture>()->GetRect(itr.second->GetTransform()->GetWorldPosition()).Contains(pos))
        {
            return itr.second;
        }
    }
    return nullptr;
}

void GameController::RepairServer(Vector2 pos)
{
    SetPaused(false);

    Server* server = FindServer(pos);
    if (server != nullptr)
    {
        if (server->status == SERVER_FAULT)
        {
            if (simulator->money >= 15)
            {
                simulator->money -= 15;
                server->status = ServerState::SERVER_REPAIRING;
            }
            else
            {
                // TODO: not enough money
            }
            return;
        }
    }
    // TODO: cancel noise
}

int ConnectionCost(Server* a, Server* b)
{
    return 10 + (a->GetTransform()->GetWorldPosition().Distance(b->GetTransform()->GetWorldPosition() * 0.2f));
}

void GameController::ConnectServers(Server* first, Server* second)
{
    if (simulator->money < ConnectionCost(first, second))
    {
        // Money not enough
        return;
    }

    if (first != second &&
        Utilities::Pick<Connection*>(
            first->connections,
            [&] (auto connection) {
                return connection->server_a == second || connection->server_b == second;
            }
    ) == nullptr)
    {
        Connection* connection = connections_entity->CreateChild()->AddComponent<Connection>();
        connection->Init(first, second);
        first->connections.push_back(connection);
        second->connections.push_back(connection);
    }
    // cancel sound
}

void GameController::Update()
{
    Vector2 mousePos = gui->input->GetHandler<MouseHandler>()->GetMousePosition();

    if (mouseMode == PLACE_SERVER)
    {
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
    else if (mouseMode == REPAIR_SERVER || mouseMode == CONNECT_SERVER_START || mouseMode == CONNECT_SERVER_END)
    {
        mouseIcon->GetTransform()->SetWorldPosition(mousePos);
    }
    mouseIcon->SetRenderWidth(2);
    mouseIcon->SetRenderHeight(2);

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
