#include "gamecontroller.h"
#include "server.h"
#include "gui.h"
#include "simulator.h"
#include "connection.h"
#include "tooltip.h"
#include "popup.h"

using namespace Ossium;

Delta delta;
// TODO: get random seed from current time
Rand rng;
GameController* global_game = nullptr;

REGISTER_COMPONENT(GameController);

void GameController::OnCreate()
{
    /// TODO: REMOVE ME
    clock.Scale(1.5f);

    connections_entity = entity;

    global_game = this;
    simulator = entity->AddComponent<GameSim>();

    tooltip = entity->CreateChild()->AddComponent<Tooltip>();
    tooltip->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 60));

    gui = entity->AddComponent<GUI>();
    gui->game = this;
    GetService<InputController>()->AddContext("GUI", gui->input);
    gui->popup->Init(gui, global_game->GetService<InputController>());

    mouseIcon = entity->CreateChild()->AddComponent<Texture>();
    mouseIcon->SetRenderLayer(90);

    pausebtn_handle = gui->pauseButton->OnClicked += [&] (const Button& caller) {
        SetPaused(!clock.IsPaused());
    };

    gui->menuButton->OnClicked += [&] (const Button& caller) {

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

            if (isTutorial)
            {
                if (tutorialState == TUTORIAL_PLACE_SERVER_A)
                {
                    gui->popup->AddMessage("\n\nGreat, now you've clicked on the build server button, you can place a server by clicking in an empty spot on the world map (this will automatically snap the server to a pre-defined position).", false, {0,0,0,0}, false);
                    gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 100));
                    gui->popup->ShowNextMessage();
                }
                else if (tutorialState == TUTORIAL_PLACE_SERVER_B)
                {
                    gui->popup->AddMessage("\n\nPlace the second server somewhere nearby to the server that we have already placed.", false, {0,0,0,0}, false);
                    gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 100));
                    gui->popup->ShowNextMessage();
                }
            }

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
            if (isTutorial)
            {
                if (tutorialState == TUTORIAL_FAULT)
                {
                    gui->popup->AddMessage("\n\nGood, now click on the faulty server to start repairing it.", false, {0,0,0,0}, false);
                    gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 100));
                    gui->popup->ShowNextMessage();
                    simulator->money += 60;
                }
            }
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

            if (isTutorial)
            {
                if (tutorialState == TUTORIAL_CONNECT_SERVER_A)
                {
                    gui->popup->AddMessage("\n\nClick on one of the servers in the world map to select it as a candidate for forming a new network connection.", false, {0,0,0,0}, false);
                    gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 100));
                    gui->popup->ShowNextMessage();
                }
            }
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

    gui->cutConnectionButton->OnClicked += [&] (const Button& caller) {
        if (true)
        {
            SetPaused(true);
            mouseIcon->SetSource(GetService<ResourceController>()->Get<Image>("assets/cut_icon.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
            if (mouseMode != NONE)
            {
                // TODO clear mouse mode
            }
            mouseMode = CUT_CONNECTOR_START;

            if (isTutorial && tutorialState == TUTORIAL_CUT_A)
            {
                gui->popup->AddMessage("\n\nClick on the server infected with hackers to begin a connection cutting operation.", false, {0,0,0,0}, false);
                gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 100));
                gui->popup->ShowNextMessage();
            }
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
                        if (closest < grid.size())
                        {
                            BuildServer(closest);
                        }
                        SetPaused(false);
                        break;
                    }
                    case REPAIR_SERVER:
                    {
                        mouseIcon->SetSource(nullptr);
                        mouseMode = NONE;
                        RepairServer(Vector2(data.x, data.y));
                        SetPaused(false);
                        break;
                    }
                    case CONNECT_SERVER_START:
                    {
                        Server* server = FindServer(Vector2(data.x, data.y));
                        if (server != nullptr)
                        {
                            connectee = server;
                            mouseMode = CONNECT_SERVER_END;
                            if (isTutorial && tutorialState == TUTORIAL_CONNECT_SERVER_A)
                            {
                                Server* otherServer = connectee;
                                for (auto itrServer : servers)
                                {
                                    if (connectee != itrServer.second)
                                    {
                                        otherServer = itrServer.second;
                                    }
                                }
                                gui->popup->AddMessage("\n\nGreat, now click on the other server (highlighted) to connect them both together.",
                                                       false,
                                                       Rect(otherServer->GetTransform()->GetWorldPosition().x - 22, otherServer->GetTransform()->GetWorldPosition().y - 22, 44, 44),
                                                       false
                                );
                                gui->popup->ShowNextMessage();
                                gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 100));
                                tutorialState = TUTORIAL_CONNECT_SERVER_B;
                                if (simulator->money < ConnectionCost(connectee, otherServer) + 50)
                                {
                                    // give player money if they ain't got enough for the connection
                                    simulator->money = ConnectionCost(connectee, otherServer) + 50;
                                }
                            }
                        }
                        else
                        {
                            // flash cancel
                            mouseMode = NONE;
                            mouseIcon->SetSource(nullptr);
                            SetPaused(false);
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
                        SetPaused(false);
                        break;
                    }
                    case CUT_CONNECTOR_START:
                    {
                        Server* server = FindServer(Vector2(data.x, data.y));
                        if (server != nullptr)
                        {
                            connectee = server;
                            mouseMode = CUT_CONNECTOR_END;
                            if (isTutorial && tutorialState == TUTORIAL_CUT_A)
                            {
                                Server* otherServer = connectee;
                                for (auto itrServer : connectee->connections)
                                {
                                    otherServer = itrServer->server_a != connectee ? itrServer->server_a : itrServer->server_b;
                                    break;
                                }
                                gui->popup->AddMessage("\n\nGreat, now click on the other server (highlighted) to cut it's connection.",
                                                       false,
                                                       Rect(otherServer->GetTransform()->GetWorldPosition().x - 22, otherServer->GetTransform()->GetWorldPosition().y - 22, 44, 44),
                                                       false
                                );
                                gui->popup->ShowNextMessage();
                                gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 100));
                                tutorialState = TUTORIAL_CUT_B;
                            }
                        }
                        else
                        {
                            // flash cancel
                            mouseMode = NONE;
                            mouseIcon->SetSource(nullptr);
                            SetPaused(false);
                        }
                        break;
                    }
                    case CUT_CONNECTOR_END:
                    {
                        Server* server = FindServer(Vector2(data.x, data.y));
                        if (server != nullptr)
                        {
                            CutServers(connectee, server);
                        }
                        else
                        {
                            // flash cancel
                        }
                        mouseMode = NONE;
                        connectee = nullptr;
                        mouseIcon->SetSource(nullptr);
                        SetPaused(false);
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
                        SetPaused(false);
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

}

void GameController::SetPaused(bool paused)
{
    if (paused)
    {
        pauseRequestStack++;
    }
    else
    {
        pauseRequestStack = pauseRequestStack > 0 ? pauseRequestStack - 1 : 0;
    }
    if (pauseRequestStack == 0)
    {
        gui->pauseButton->sprite->ChangeState("pause");
        clock.SetPaused(false);
    }
    else
    {
        gui->pauseButton->sprite->ChangeState("play");
        clock.SetPaused(true);
    }
}

void GameController::BuildServer(unsigned int pos)
{
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

    if (isTutorial)
    {
        if (tutorialState == TUTORIAL_PLACE_SERVER_A)
        {
            gui->popup->AddMessage("\n\nWe seem to have plenty of money, so why not go ahead and build another server somewhere?");
            gui->popup->AddMessage("\n\nWhenever you build a server, it'll cost you some money. You can see how much money you have in the bottom left corner.", true, Rect(0, 768 - 80, 250, 80));
            gui->popup->AddMessage("\n\nGood job! You've successfully created a server, which will be up and running soon.");
            gui->popup->ShowNextMessage();
            gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));
            tutorialState = TUTORIAL_PLACE_SERVER_B;
        }
        else if (tutorialState == TUTORIAL_PLACE_SERVER_B)
        {
            gui->popup->AddMessage("\n\nAwesome! Now we have more than one server on the world map, we can <b>form a network</b> by clicking on the highlighted <b>connector</b> button.",
                                   true,
                                   Rect(gui->connectorButton->GetTransform()->GetWorldPosition().x - 40, gui->connectorButton->GetTransform()->GetWorldPosition().y - 40, 80, 80),
                                   false
            );
            gui->popup->ShowNextMessage();
            gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));
            tutorialState = TUTORIAL_CONNECT_SERVER_A;
        }
    }

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
    Server* server = FindServer(pos);
    if (server != nullptr)
    {
        if (server->status == SERVER_FAULT)
        {
            if (simulator->money >= GameSim::repairs_cost)
            {
                if (isTutorial && tutorialState == TUTORIAL_FAULT)
                {
                    gui->popup->AddMessage("\n\nNote that repairing a server does cost some money, so be careful to have some spare money in case a server goes bust again! Now that's sorted, go ahead and build up your server network some more.");
                    gui->popup->AddMessage("\n\nWonderful. Now the server will start to be repaired. It will take some time for the server to be operational again, and while it is not in the <b>running</b> state it's connections do not generate any money!");
                    gui->popup->ShowNextMessage();
                    tutorialState = TUTORIAL_AWAIT_HACKERS;
                }
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
    float distance = a->GetTransform()->GetWorldPosition().Distance(b->GetTransform()->GetWorldPosition());
    return max(10, (int)distance - 60);
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

        if (isTutorial && tutorialState == TUTORIAL_CONNECT_SERVER_B)
        {
            gui->popup->AddMessage("\n\nTo continue, you must <b>unpause</b> the game and wait for some time. Once enough money has been generated by your connections, you can build another server and add it to the network!",
                                   true,
                                   Rect(gui->pauseButton->GetTransform()->GetWorldPosition().x - 40, gui->pauseButton->GetTransform()->GetWorldPosition().y - 40, 80, 80)
            );
            gui->popup->AddMessage("\n\nTo win the game, you must have at least 20 servers placed in the world map, all of which must have at least 2 connections.");
            gui->popup->AddMessage("\n\nFantastic, you've formed a server network! Every connection in a server network generates a random amount of money each day as people pay you for using your network.");
            gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));
            gui->popup->ShowNextMessage();
            tutorialState = TUTORIAL_AWAIT_FAULT;
        }
    }

    if (isTutorial && tutorialState == TUTORIAL_CONNECT_SERVER_B)
    {
        tutorialState = TUTORIAL_CONNECT_SERVER_A;
    }
    // cancel sound
}

void GameController::CutServers(Server* first, Server* second)
{
    if (first != second)
    {


        Connection* connection = nullptr;
        for (auto connector : first->connections)
        {
            if (connector->server_a == second || connector->server_b == second)
            {
                connection = connector;
                break;
            }
        }

        if (connection != nullptr)
        {
            // Destroy the connection and remove it from both servers
            for (auto itr = first->connections.begin(); itr < first->connections.end(); itr++)
            {
                if (*itr == connection)
                {
                    first->connections.erase(itr);
                    break;
                }
            }
            for (auto itr = second->connections.begin(); itr < second->connections.end(); itr++)
            {
                if (*itr == connection)
                {
                    second->connections.erase(itr);
                    break;
                }
            }

            connection->GetEntity()->Destroy();

            if (isTutorial && tutorialState == TUTORIAL_CUT_B)
            {
                gui->popup->AddMessage("And that's that! Go ahead and see if you can win. Remember, you must have at least 20 servers which each have at least 2 connections to win. Good luck!");
                gui->popup->AddMessage("You can use the purge tool in exactly the same way as you would use the repair tool, but it costs considerably more to use.");
                gui->popup->AddMessage("\n\nFinally, you can use the purge tool (see highlighted button on the left) to purge a server of a hacker attack. The only other ways for a server to become uninfected is for a server fault to occur or the server is destroyed by the hackers.",
                                   true,
                                   Rect(gui->purgeButton->GetTransform()->GetWorldPosition().x - 40, gui->purgeButton->GetTransform()->GetWorldPosition().y - 40, 80, 80)
                );
                gui->popup->AddMessage("\n\nYou should cut off all other servers connected to an infected server to prevent the spread of hackers.");
                gui->popup->AddMessage("\n\nAwesome! Now the hackers cannot directly spread to the server and take direct control of it.");
                gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));
                gui->popup->ShowNextMessage();
                tutorialState = TUTORIAL_END;
            }

        }
        else
        {
            if (isTutorial && tutorialState == TUTORIAL_CUT_B)
            {
                tutorialState = TUTORIAL_CUT_A;
            }
            // error sound etc.
        }
    }
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
            if (FindServer(pos) == nullptr)
            {
                if (pos.DistanceSquared(mousePos) < closestVec.DistanceSquared(mousePos))
                {
                    closestVec = pos;
                    closest = i;
                }
            }
        }

        if (mousePos.Distance(closestVec) < 90)
        {
            mouseIcon->GetTransform()->SetWorldPosition(closestVec);
            InfoMessage(Utilities::Format("Server Cost: {0}", GameSim::server_cost));
        }
        else
        {
            mouseIcon->GetTransform()->SetWorldPosition(mousePos);
            // Don't allow placement
            closest = grid.size();
        }
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

    clock.Update(delta.Time());

}

void GameController::ActivateTutorial()
{
    isTutorial = true;
    gui->popup->AddMessage("\n\nWe'll have to bring our network to life from scratch, so let's build a server somewhere. You can do this by clicking on the highlighted button.", true, Rect(gui->buildServerButton->GetTransform()->GetWorldPosition().x - 40, gui->buildServerButton->GetTransform()->GetWorldPosition().y - 40, 80, 80));
    gui->popup->AddMessage("<b>Welcome to SPOF: Single Point Of Failure!</b>\n\nIn this game, you are responsible for creating and keeping a world-wide network alive. But watch out! Server faults and malicious hackers seek to destroy your network. You must do whatever it takes to spread your servers all over the world without the network collapsing.\n(Click anywhere to continue)");
    gui->popup->ShowNextMessage();
    tutorialState = TUTORIAL_PLACE_SERVER_A;
    SetPaused(true);
}
