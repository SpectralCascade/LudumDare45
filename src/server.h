#ifndef SERVER_H
#define SERVER_H

#include "Ossium.h"
#include "simulator.h"

using namespace Ossium;

class Server;

struct ServerSchema : public Schema<ServerSchema, 20>
{
    DECLARE_BASE_SCHEMA(ServerSchema, 20);

    M(vector<Server*>, connections);

    M(float, health) = 1.0f;

};

class Server : public Automaton, public ServerSchema
{
public:
    CONSTRUCT_SCHEMA(Automaton, ServerSchema);
    DECLARE_COMPONENT(Automaton, Server);

    void OnCreate();

    void Simulate(GameSim& sim, GameController& game);

};

#endif // SERVER_H
