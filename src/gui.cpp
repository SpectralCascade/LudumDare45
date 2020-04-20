#include "gui.h"
#include "gamecontroller.h"
#include "simulator.h"
#include "popup.h"

using namespace Ossium;

REGISTER_COMPONENT(GUI);

void GUI::OnCreate()
{
    GraphicComponent::OnCreate();
    input = entity->AddComponent<InputGUI>();

    // build server button
    buildServerButton = entity->CreateChild()->AddComponent<Button>();
    buildServerButton->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/build_server_button.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    // Repair button
    repairButton = entity->CreateChild()->AddComponent<Button>();
    repairButton->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/repair_button.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    // Purge button
    purgeButton = entity->CreateChild()->AddComponent<Button>();
    purgeButton->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/purge_button.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    // Connector button
    connectorButton = entity->CreateChild()->AddComponent<Button>();
    connectorButton->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/connector_button.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    // Cut connection button
    cutConnectionButton = entity->CreateChild()->AddComponent<Button>();
    cutConnectionButton->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/cut_button.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    menuButton = entity->CreateChild()->AddComponent<Button>();
    menuButton->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/restart.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    // pause button
    pauseButton = entity->CreateChild()->AddComponent<Button>();
    pauseButton->sprite->AddState(
        "pause",
        GetService<ResourceController>()->Get<Image>("assets/pause.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );
    pauseButton->sprite->AddState(
        "play",
        GetService<ResourceController>()->Get<Image>("assets/play.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    input->AddInteractable(SID("BuildServer")::str, *buildServerButton);
    input->AddInteractable(SID("RepairServer")::str, *repairButton);
    input->AddInteractable(SID("PurgeServer")::str, *purgeButton);
    input->AddInteractable(SID("ConnectServers")::str, *connectorButton);
    input->AddInteractable(SID("CutServers")::str, *cutConnectionButton);
    input->AddInteractable(SID("PausePlay")::str, *pauseButton);
    input->AddInteractable(SID("GoMenu")::str, *menuButton);

    connectorButton->GetTransform()->SetWorldPosition(Vector2(50, (768 / 2) - 140));
    buildServerButton->GetTransform()->SetWorldPosition(Vector2(50, (768 / 2) - 40));
    cutConnectionButton->GetTransform()->SetWorldPosition(Vector2(50, (768 / 2) + 60));
    repairButton->GetTransform()->SetWorldPosition(Vector2(50, (768 / 2) + 160));
    purgeButton->GetTransform()->SetWorldPosition(Vector2(50, (768 / 2) + 260));
    pauseButton->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 - 50));

    menuButton->GetTransform()->SetWorldPosition(Vector2(50, 50));

    connectorButton->SetRenderLayer(80);
    buildServerButton->SetRenderLayer(80);
    cutConnectionButton->SetRenderLayer(80);
    repairButton->SetRenderLayer(80);
    purgeButton->SetRenderLayer(80);
    pauseButton->SetRenderLayer(80);
    menuButton->SetRenderLayer(80);

    SetRenderLayer(80);

    popup = entity->CreateChild()->AddComponent<Popup>();

}

void GUI::Highlight(Rect area, SDL_Color color)
{
    Rect top, bottom, left, right;
    top = Rect(0, 0, 1280, area.y);
    bottom = Rect(0, area.y + area.h, 1280, 768 - (area.y + area.h));
    left = Rect(0, top.h, area.x, bottom.y - top.h);
    right = Rect(area.x + area.w, top.h, 1280 - (area.x + area.w), bottom.y - top.h);

    Renderer& renderer = *GetService<Renderer>();

    SDL_Color blockColor = Color(0, 0, 0, 140);

    top.DrawFilled(renderer, blockColor);
    bottom.DrawFilled(renderer, blockColor);
    left.DrawFilled(renderer, blockColor);
    right.DrawFilled(renderer, blockColor);

    area.Draw(renderer, color);
}

void GUI::Render(Renderer& renderer)
{
    Font* font = GetService<ResourceController>()->Get<Font>("assets/Orkney Regular.ttf", 36);
    moneyText.SetBounds(Vector2(1000, 1000));
    moneyText.SetPointSize(36);
    moneyText.SetText(renderer, *font, Utilities::Format("<b>Money: ${0}</b>", game->simulator->money), true);
    moneyText.Update(*font);
    moneyText.Render(renderer, *font, Vector2(10, 768 - 60));

    timeText.SetBounds(Vector2(1000, 1000));
    timeText.SetPointSize(36);
    timeText.SetText(renderer, *font, Utilities::Format("<b>Day {0}</b>", game->simulator->GetTimeStep()), true);
    timeText.Update(*font);
    timeText.Render(renderer, *font, Vector2(1280 - 200, 768 - 60));

    if (global_game->clock.IsPaused())
    {
        pausedText.SetBounds(Vector2(1000, 1000));
        pausedText.SetPointSize(36);
        pausedText.mainColor = Colors::RED;
        pausedText.SetText(renderer, *font, Utilities::Format("<b>GAME PAUSED</b>", game->simulator->GetTimeStep()), true);
        pausedText.Update(*font);
        pausedText.Render(renderer, *font, Vector2(1280 - pausedText.GetSize().x, 0));
    }

}
