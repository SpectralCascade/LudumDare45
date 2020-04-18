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
    SetRenderLayer(40);
}

void Server::Simulate(GameSim& sim, GameController& game)
{
    switch (status)
    {
    case SERVER_BUILDING:
        buildingTimeLeft--;
        if (buildingTimeLeft <= 0)
        {
            status = SERVER_RUNNING;
        }
        break;
    case SERVER_RUNNING:
        sim.money += (int)(rng.Float() < 0.6f) * rng.Int(1, 2);
        if (rng.Float() < 0.01f)
        {
            status = SERVER_FAULT;
        }
        break;
    case SERVER_FAULT:
        health -= 0.1f;
        break;
    case SERVER_REPAIRING:
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

    if (health <= 0)
    {
        entity->Destroy();
    }

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
            (entity->GetComponent<Texture>()->GetRenderHeight() / 2) + (textsize.x / 2),
            textsize.y + 5 + (entity->GetComponent<Texture>()->GetRenderHeight() / 2)
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
