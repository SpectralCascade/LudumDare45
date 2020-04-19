#include "server.h"
#include "gamecontroller.h"
#include "simulator.h"
#include "gui.h"

using namespace Ossium;

REGISTER_COMPONENT(Server);

void Server::OnCreate()
{
    // nasty hack because I'm not serialising
    OnLoadFinish();
    icon = entity->CreateChild()->AddComponent<Texture>();
    icon->SetRenderLayer(41);
    icon->SetSource(GetService<ResourceController>()->Get<Image>("assets/server_building.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC));
    //icon->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
    icon->GetTransform()->SetLocalPosition(Vector2(10, 10));
    SetRenderLayer(42);
}

void Server::Simulate(GameSim& sim, GameController& game)
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

            sim.money += (int)(rng.Float() < 0.4f) * connections.size();

            icon->SetSource(nullptr);

            // Chance of a fault is increased as connections increase
            float fault_chance = (0.005f * (float)connections.size());

            if (daysSinceFault > 10 && rng.Float() < fault_chance)
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

void Server::OnDestroy()
{
    // Clean up connections
    for (auto server : connections)
    {
        for (auto itr = server->connections.begin(); itr < server->connections.end(); itr++)
        {
            if (*itr == this)
            {
                server->connections.erase(itr);
                break;
            }
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
