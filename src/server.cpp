#include "server.h"
#include "gamecontroller.h"
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
        sim.money += (int)(rng.Float() < 0.2f) * rng.Int(1, 2);
        break;
    case SERVER_FAULT:
        break;
    case SERVER_REPAIRING:
        break;
    }
}
