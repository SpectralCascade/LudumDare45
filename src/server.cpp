#include "server.h"
#include "gamecontroller.h"
#include "simulator.h"
#include "gui.h"
#include"connection.h"

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
    if (stage == 0)
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
            case SERVER_RUNNING:
            {

                daysSinceFault++;

                icon->SetSource(nullptr);

                // Chance of a fault is increased as connections increase
                float fault_chance = (0.001f * (float)connections.size());

                sim.money--;

                if (daysSinceFault > 22 && rng.Float() < fault_chance)
                {
                    status = SERVER_FAULT;
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
        }
    }

}

void Server::OnDestroy()
{
    // Clean up connections
    for (auto connection : connections)
    {
        if (connection != nullptr)
        {
            Server* otherServer = connection->server_a == this ? connection->server_b : connection->server_a;
            for (auto itr = otherServer->connections.begin(); itr != otherServer->connections.end(); itr++)
            {
                if (connection == *itr)
                {
                    otherServer->connections.erase(itr);
                    break;
                }
            }
            // Destroy the connection immediately
            connection->GetEntity()->Destroy(true);
            break;
        }
    }

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
    }
    return "Unknown";
}
