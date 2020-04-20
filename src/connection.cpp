#include "connection.h"
#include "gamecontroller.h"
#include "simulator.h"

using namespace Ossium;

REGISTER_COMPONENT(Connection);

void Connection::OnCreate()
{
    SetRenderLayer(25);
    entity->name = Utilities::Format("Connection {0}", (void*)this);
}

void Connection::Init(Server* a, Server* b)
{
    server_a = a;
    server_b = b;
}

void Connection::Simulate(GameSim& sim, GameController& game, int stage)
{
}

void Connection::Render(Renderer& renderer)
{
    Line line = Line(server_a->GetTransform()->GetWorldPosition(), server_b->GetTransform()->GetWorldPosition());
    line.Draw(renderer, Colors::MAGENTA);
}

void Connection::OnDestroy()
{
    //Log.Info("Destroyed connection {0}", entity->name);
    Automaton::OnDestroy();

}
