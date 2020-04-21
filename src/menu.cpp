#include "menu.h"

using namespace Ossium;

REGISTER_COMPONENT(Menu);

void Menu::OnCreate()
{

    bg = entity->CreateChild()->AddComponent<Texture>();
    Image* image = GetService<ResourceController>()->Get<Image>("assets/bg.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC);
    bg->SetSource(image);
    bg->SetRenderLayer(0);
    bg->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));

    tutorial = entity->CreateChild()->AddComponent<Button>();
    tutorial->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/menu_tutorial.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    play = entity->CreateChild()->AddComponent<Button>();
    play->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/menu_play.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    quit = entity->CreateChild()->AddComponent<Button>();
    quit->sprite->AddState(
        "default",
        GetService<ResourceController>()->Get<Image>("assets/menu_quit.png", *GetService<Renderer>(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC),
        true,
        3
    );

    gui = entity->AddComponent<InputGUI>();
    GetService<InputController>()->AddContext("gui_menu", gui);

    gui->AddInteractable(SID("tutorial")::str, *tutorial);
    gui->AddInteractable(SID("play")::str, *play);
    gui->AddInteractable(SID("quit")::str, *quit);

    tutorial->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 3));
    play->GetTransform()->SetWorldPosition(Vector2(1280 / 2, (768 / 3) + 100));
    quit->GetTransform()->SetWorldPosition(Vector2(1280 / 2, (768 / 3) + 200));

    tutorial->OnClicked += [&] (const Button& b) { state = MENU_TUT; engine->Exit(); };
    play->OnClicked += [&] (const Button& b) { state = MENU_PLAY; engine->Exit(); };
    quit->OnClicked += [&] (const Button& b) { state = MENU_QUIT; engine->Exit(); };
    play->SetRenderLayer(20);
    quit->SetRenderLayer(20);
    tutorial->SetRenderLayer(20);

    GetService<Renderer>()->SetBackgroundColor(Colors::BLACK);

}

void Menu::OnDestroy()
{
    GetService<InputController>()->Clear();
}
