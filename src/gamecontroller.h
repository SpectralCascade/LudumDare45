#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "Ossium.h"

using namespace Ossium;

/// Forward declarations
class Server;
class GUI;

/// Global delta time
extern Delta delta;

struct GameSchema : public Schema<GameSchema, 20>
{
    DECLARE_BASE_SCHEMA(GameSchema, 20);

    M(vector<Server*>, servers);

    // All possible positions that servers can be placed in
    M(vector<Vector2>, grid) = {
        Vector2(155, 163),
        Vector2(487, 128),
        Vector2(287, 216),
        Vector2(324, 439),
        Vector2(362, 543),
        Vector2(571, 349),
        Vector2(607, 228),
        Vector2(679, 524),
        Vector2(738, 411),
        Vector2(684, 328),
        Vector2(652, 419),
        Vector2(233, 364),
        Vector2(176, 274),
        Vector2(765, 338),
        Vector2(740, 196),
        Vector2(840, 262),
        Vector2(885, 362),
        Vector2(876, 167),
        Vector2(993, 168),
        Vector2(1039, 246),
        Vector2(942, 241),
        Vector2(994, 373),
        Vector2(418, 468),
        Vector2(1033, 433),
        Vector2(1079, 516),
        Vector2(1035, 563),
        Vector2(1132, 571)
    };

    M(int, serversToWin) = 20;

};

class GameController : Component, GameSchema
{
public:
    CONSTRUCT_SCHEMA(Component, GameSchema);
    DECLARE_COMPONENT(Component, GameController);

    void OnCreate();

    void Update();

    void BuildServer(Vector2 position);

    GUI* gui = nullptr;

    Clock clock;

private:
    int pausebtn_handle;

};

#endif // GAMECONTROLLER_H
