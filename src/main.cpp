#include "Ossium.h"
#include "gamecontroller.h"

using namespace Ossium;

int main(int argc, char* argv[])
{
    InitialiseOssium();

    InputController input;
    InputContext mainContext;

    input.AddContext("window_context", &mainContext);

    Window* window = mainContext.AddHandler<Window>();
    window->Init("Network Trouble", 1280, 768);

    Renderer renderer(window, 100);
    ResourceController resources;

    ServicesProvider services(&renderer, &resources, &input);

    EngineSystem engine(&services);

    Image bg;
    bg.Load("assets/world_map.png");
    bg.PushGPU(renderer, SDL_TEXTUREACCESS_STATIC);

    Texture* background = engine.GetScene()->CreateEntity()->AddComponent<Texture>();
    background->SetColorMod(50, 50, 255);
    background->SetSource(&bg);
    background->GetTransform()->SetWorldPosition(Vector2(1280 / 2, 768 / 2));

    renderer.SetBackgroundColor(Colors::GREEN);

    engine.GetScene()->CreateEntity()->AddComponent<GameController>();
//    engine.GetScene()->CreateEntity()->AddComponent<Server>()->GetTransform()->SetWorldPosition();

    delta.Init();
    while (engine.Update())
    {
        delta.Update();
    }

    TerminateOssium();

    return 0;
}
