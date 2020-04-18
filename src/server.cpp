#include "server.h"

using namespace Ossium;

REGISTER_COMPONENT(Server);

void Server::OnCreate()
{
    // nasty hack because I'm not serialising
    OnLoadFinish();
    SetRenderLayer(5);
}

void Server::Render(Renderer& renderer)
{
    Vector2 t = GetTransform()->GetWorldPosition();
    Rect(t.x, t.y, 20, 20).DrawFilled(renderer, Colors::RED);
}

