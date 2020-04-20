#include "server.h"
#include "gamecontroller.h"
#include "simulator.h"
#include "gui.h"
#include "connection.h"
#include "popup.h"

using namespace Ossium;

REGISTER_COMPONENT(Server);

void Server::OnCreate()
{
    // nasty hack because I'm not serialising
    OnLoadFinish();
    entity->name = Utilities::Format("Server {0}", (void*)this);
    icon = entity->CreateChild()->AddComponent<Texture>();
    icon->SetRenderLayer(41);
    icon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_building.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
    //icon->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
    icon->GetTransform()->SetLocalPosition(Vector2(10, 10));
    SetRenderLayer(42);
}

void Server::Simulate(GameSim& sim, GameController& game, int stage)
{
    if (stage == 1)
    {
        switch (status)
        {
            case SERVER_BUILDING:
            {
                buildingTimeLeft--;
                if (buildingTimeLeft <= 0)
                {
                    status = SERVER_RUNNING;
                    icon->SetSource(nullptr);
                }
                else
                {
                    icon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_building.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
                }
                break;
            }
            case SERVER_HACKED:
            {
                if (rng.Float() < 0.05f * connections.size())
                {
                    Connection* connection = connections[rng.Int(0, connections.size() - 1)];
                    Server* target = connection->server_a == this ? connection->server_b : connection->server_a;
                    target->status = SERVER_HACKED;
                }
                health -= 0.01f;
            }
            case SERVER_RUNNING:
            {
                daysSinceFault++;
                daysSinceHack++;

                // Chance of a fault is increased as connections increase
                float fault_chance = ((status == SERVER_HACKED ? 0.02f : 0.01f) * (float)connections.size());

                if (!connections.empty() && sim.daysSinceHackers > sim.hackerInterval && daysSinceHack > 80 && rng.Float() < 0.001f * global_game->servers.size())
                {
                    daysSinceHack = 0;
                    sim.daysSinceHackers = sim.hackerInterval / 3;
                    status = SERVER_HACKED;
                    break;
                }

                icon->SetSource(status == SERVER_HACKED ? GetService<ResourceController>()->Get<Image>("assets/server_hacked.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC) : nullptr);

                if (daysSinceFault > 22 && rng.Float() < fault_chance)
                {
                    if (status == SERVER_HACKED)
                    {
                        // todo: popup message that hacker has lost control of the server because it is faulty!
                    }
                    status = SERVER_FAULT;
                    if (game.isTutorial && game.tutorialState == TUTORIAL_AWAIT_FAULT)
                    {
                        game.gui->popup->AddMessage("\n\nTo fix a fault, you must use the <b>repair</b> tool. Click the highlighted button on the left to select it.",
                                                    true,
                                                    Rect(game.gui->repairButton->GetTransform()->GetWorldPosition().x - 40, game.gui->repairButton->GetTransform()->GetWorldPosition().y - 40, 80, 80)
                        );
                        game.gui->popup->AddMessage("\n\nServers have a small chance of developing a fault while connected to the network. The more connections a server has, the more likely it is to have a fault.");
                        game.gui->popup->AddMessage("\n\nOno! One of your servers have developed a <b>fault</b> (as indicated by the yellow warning icon)!",
                                                    true,
                                                    Rect(GetTransform()->GetWorldPosition().x - 12, GetTransform()->GetWorldPosition().y - 12, 24, 24)
                        );
                        game.gui->popup->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 100));
                        game.gui->popup->ShowNextMessage();
                        game.SetPaused(true);
                        game.tutorialState = TUTORIAL_FAULT;
                    }
                    icon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_fault.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
                }
                break;
            }
            case SERVER_FAULT:
            {
                daysSinceFault = 0;
                icon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_fault.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
                health -= 0.1f;
                break;
            }
            case SERVER_REPAIRING:
            {
                icon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_repairing.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
                if (health < 1.0f)
                {
                    health = Utilities::Clamp(health + 0.05f);
                }
                if (health >= 1.0f)
                {
                    status = SERVER_RUNNING;
                }
                break;
            }
        }

        if (health <= 0)
        {
            auto itr = game.servers.find(serverId);
            game.servers.erase(itr);

            entity->GetComponentInChildren<Texture>()->GetEntity()->Destroy();
            entity->Destroy();

            //Log.Info("Total connections = {0}", connections.size());

            // Clean up connections
            for (auto connection : connections)
            {
                if (connection != nullptr)
                {
                    // Destroy the connection
                    //Log.Info("Attempting to destroy a connection: {0}", connection->GetEntity()->name);
                    connection->GetEntity()->Destroy();

                    Server* otherServer = connection->server_a == this ? connection->server_b : connection->server_a;
                    for (auto itr = otherServer->connections.begin(); itr != otherServer->connections.end(); itr++)
                    {
                        if (connection == *itr)
                        {
                            otherServer->connections.erase(itr);
                            break;
                        }
                    }
                }
            }
            connections.clear();
        }
    }
    else if (stage == 0)
    {
        if (status == SERVER_RUNNING)
        {
            for (Connection* connection : connections)
            {

                if (connection->server_a->status & SERVER_RUNNING && connection->server_b->status & SERVER_RUNNING)
                {
                    sim.money += rng.Int(0, 3);
                }


            }

        }
        else if (status == SERVER_HACKED)
        {
            for (Connection* connection : connections)
            {
                sim.money -= 1;
            }
        }
    }

}

void Server::OnDestroy()
{
    Automaton::OnDestroy();
}

void Server::Render(Renderer& renderer)
{
    Vector2 mousePos = global_game->gui->input->GetHandler<MouseHandler>()->GetMousePosition();
    if (entity->GetComponent<Texture>()->GetRect(GetTransform()->GetWorldPosition()).Contains(mousePos))
    {
        Font* font = GetService<ResourceController>()->Get<Font>("assets/Orkney Regular.ttf", 36);
        infoText.SetBounds(Vector2(1000, 1000));
        infoText.SetPointSize(24);
        infoText.SetText(renderer, *font, Utilities::Format("<color=#FFFFFF>Server Status: {0}\nHealth: {1}</color>", GetStatusText(), health), true);
        infoText.Update(*font);

        Vector2 textsize = infoText.GetSize();
        Vector2 pos = GetTransform()->GetWorldPosition() - Vector2(
            (entity->GetComponent<Texture>()->GetRenderWidth() / 2) + (textsize.x / 2),
            textsize.y + 10 + (entity->GetComponent<Texture>()->GetRenderHeight())
        );
        Rect infoBox = Rect(pos.x - 2, pos.y - 2, textsize.x + 4, textsize.y + 4);
        infoBox.DrawFilled(renderer, Colors::BLACK);
        infoBox.Draw(renderer, Colors::WHITE);

        infoText.Render(renderer, *font, pos);
    }

}

string Server::GetStatusText()
{
    switch (status)
    {
    case SERVER_BUILDING:
        return "Building";
    case SERVER_RUNNING:
        return "Running";
    case SERVER_FAULT:
        return "Faulty";
    case SERVER_REPAIRING:
        return "Repairing";
    case SERVER_HACKED:
        return "Hacked!";
    }
    return "Unknown";
}
