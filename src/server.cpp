#include "server.h"
#include "simulator.h"

using namespace Ossium;

REGISTER_COMPONENT(Server);

void Server::OnCreate()
{
    // nasty hack because I'm not serialising
    OnLoadFinish();
}

void Server::Simulate(GameSim& sim, GameController& game)
{



}
