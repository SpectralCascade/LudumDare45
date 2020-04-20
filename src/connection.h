#ifndef CONNECTION_H
#define CONNECTION_H

#include "Ossium.h"
#include "simulator.h"
#include "server.h"

using namespace Ossium;

struct ConnectionSchema : public Schema<ConnectionSchema, 20>
{
public:
    DECLARE_BASE_SCHEMA(ConnectionSchema, 20);

    M(Server*, server_a) = nullptr;
    M(Server*, server_b) = nullptr;

};

class Connection : public Automaton, public ConnectionSchema
{
public:
    DECLARE_COMPONENT(Automaton, Connection);
    CONSTRUCT_SCHEMA(Automaton, ConnectionSchema)

    void OnCreate();

    void Init(Server* a, Server* b);

    void Render(Renderer& renderer);

    void Simulate(GameSim& sim, GameController& game, int stage);

};


#endif // CONNECTION_H
