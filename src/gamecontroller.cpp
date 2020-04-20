#include "gamecontroller.h"
#include "server.h"
#include "gui.h"
#include "simulator.h"
#include "connection.h"
#include "tooltip.h"

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

    tooltip = entity->CreateChild()->AddComponent<Tooltip>();
    tooltip->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 60));

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

    gui->purgeButton->OnClicked += [&] (const Button& caller) {
        if (true)
        {
            SetPaused(true);
            mouseIcon->SetSource(GetService<ResourceController>()->Get<Image>("assets/purge_icon.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
            if (mouseMode != NONE)
            {
                // TODO clear mouse mode
            }
            mouseMode = PURGE_SERVER;
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
                    case PURGE_SERVER:
                    {
                        Server* server = FindServer(Vector2(data.x, data.y));
                        if (server != nullptr)
                        {
                            PurgeServer(server);
                        }
                        else
                        {
                            // flash cancel
                        }
                        mouseMode = NONE;
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

    if (simulator->money < GameSim::server_cost)
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

    simulator->money -= GameSim::server_cost;

}

void GameController::PurgeServer(Server* server)
{
    if (server->status == SERVER_HACKED)
    {
        if (simulator->money < GameSim::purge_cost)
        {
            // Show error
            return;
        }

        simulator->money -= GameSim::purge_cost;

        server->status = SERVER_RUNNING;
        server->daysSinceFault = 10;
        // todo: sound effect etc.
    }
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
            if (simulator->money >= GameSim::repairs_cost)
            {
                simulator->money -= GameSim::repairs_cost;
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

int GameController::ConnectionCost(Server* a, Server* b)
{
    return 10;
}

void GameController::ConnectServers(Server* first, Server* second)
{
    int cost = ConnectionCost(first, second);
    if (simulator->money < cost)
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
        simulator->money -= cost;
    }
    // cancel sound
}

void GameController::InfoMessage(string text)
{
    tooltip->SetText(text);
    tooltip->Show();
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
        InfoMessage(Utilities::Format("Server Cost: {0}", GameSim::server_cost));
    }
    else if (mouseMode != MouseInteraction::NONE)
    {
        mouseIcon->GetTransform()->SetWorldPosition(mousePos);
        if (mouseMode == REPAIR_SERVER)
        {
            InfoMessage(Utilities::Format("Repairs Cost: {0}", GameSim::repairs_cost));
        }
        else if (mouseMode == CONNECT_SERVER_END)
        {
            Server* target = FindServer(mousePos);
            if (target != nullptr)
            {
                InfoMessage(Utilities::Format("Connection Cost: {0}", ConnectionCost(connectee, target)));
            }
            else
            {
                tooltip->Hide();
            }
        }
        else if (mouseMode == PURGE_SERVER)
        {
            InfoMessage(Utilities::Format("Purge Cost: {0}", GameSim::purge_cost));
        }
    }
    else
    {
        tooltip->Hide();
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
