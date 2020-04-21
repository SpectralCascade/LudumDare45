#include "Ossium.h"
#include "gamecontroller.h"
#include "menu.h"

using namespace Ossium;

enum GameState
{
    MENU = 0,
    RUNGAME
};

int main(int argc, char* argv[])
{
    InitialiseOssium();

    InputContext mainContext;

    Window* window = mainContext.AddHandler<Window>();
    window->Init("SPOF: Single Point Of Failure", 1280, 768);
    Renderer renderer(window, 100);
    int state = RUNGAME;
    bool run_tutorial = true;

    bool quit = false;
    while (!quit)
    {
        InputController input;
        input.AddContext("window_context", &mainContext);

        ResourceController resources;

        ServicesProvider services(&renderer, &resources, &input);

        renderer.UnregisterAll();
        resources.FreeAll();
        EngineSystem engine(&services);

        if (state == MENU)
        {
            Menu* menu = engine.GetScene()->CreateEntity()->AddComponent<Menu>();
            menu->engine = &engine;

            int menuNextState = 0;
            while (engine.Update())
            {
                menuNextState = menu->state;
            }

            switch (menuNextState)
            {
            case MENU_TUT:
                run_tutorial = true;
                break;
            case MENU_PLAY:
                run_tutorial = false;
                break;
            case MENU_QUIT:
                quit = true;
                break;
            }

            state = RUNGAME;

        }
        else if (state == RUNGAME)
        {
            EngineSystem engine(&services);

            Image bg;
            bg.Load("assets/world_map.png");
            bg.PushGPU(renderer, SDL_TEXTUREACCESS_STATIC);

            Texture* background = engine.GetScene()->CreateEntity()->AddComponent<Texture>();
            background->SetColorMod(50, 50, 255);
            background->SetSource(&bg);
            background->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));

            renderer.SetBackgroundColor(Color(0, 200, 0));

            GameController* game = engine.GetScene()->CreateEntity()->AddComponent<GameController>();

            if (run_tutorial)
            {
                game->ActivateTutorial();
            }

            delta.Init();
            while (engine.Update())
            {
                delta.Update();
                state = game->restart ? RUNGAME : MENU;
            }

            break;

        }

    }

    TerminateOssium();

    return 0;
}
