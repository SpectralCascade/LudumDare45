#include "gui.h"

using namespace Ossium;

REGISTER_COMPONENT(GUI);

void GUI::OnCreate()
{
    Component::OnCreate();
    input = entity->AddComponent<InputGUI>();

    // build server button
    buildServerButton = entity->CreateChild()->AddComponent<Button>();
    buildServerButton->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/build_server_button.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
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
    input->AddInteractable(SID("PausePlay")::str, *pauseButton);

    buildServerButton->GetTransform()->SetWorldPosition(Vector2(50, (768 / 2) + 60));
    pauseButton->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 - 50));

}
