#ifndef SERVER_H
#define SERVER_H

#include "Ossium.h"
#include "simulator.h"

using namespace Ossium;

class Connection;

enum ServerState
{
    SERVER_BUILDING = 0,
    SERVER_RUNNING = 1,
    SERVER_REPAIRING = 2,
    SERVER_FAULT = 4,
    SERVER_HACKED = 8
};

struct ServerSchema : public Schema<ServerSchema, 20>
{
    DECLARE_BASE_SCHEMA(ServerSchema, 20);

    M(vector<Connection*>, connections);

    M(float, health) = 1.0f;

    M(ServerState, status) = SERVER_BUILDING;

    M(unsigned int, buildingTimeLeft) = 3;

    M(unsigned int, daysSinceFault) = 0;

    M(unsigned int, daysSinceHack) = 0;

    M(unsigned int, serverId);

};

class Server : public Automaton, public ServerSchema
{
public:
    CONSTRUCT_SCHEMA(Automaton, ServerSchema);
    DECLARE_COMPONENT(Automaton, Server);

    void OnCreate();

    virtual void Simulate(GameSim& sim, GameController& game, int stage);

    void Render(Renderer& renderer);

    void OnDestroy();

    string GetStatusText();

private:
    TextLayout infoText;

    Texture* icon = nullptr;

};

#endif // SERVER_H
