#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "Ossium.h"

using namespace Ossium;

/// Forward declaration
class Server;

/// Global delta time
extern Delta delta;

struct GameSchema : public Schema<GameSchema, 20>
{
    DECLARE_BASE_SCHEMA(GameSchema, 20);

    M(vector<Server*>, servers);

    M(int, serversToWin) = 20;

};

class GameController : Component, GameSchema
{
public:
    CONSTRUCT_SCHEMA(Component, GameSchema);
    DECLARE_COMPONENT(Component, GameController);

    void OnCreate();

    void Update();

};

#endif // GAMECONTROLLER_H
