#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "Ossium.h"
#include "tooltip.h"

using namespace Ossium;

/// Forward declarations
class Server;
class GUI;
class GameSim;
class GameController;

/// Globals
extern Delta delta;
extern Rand rng;
extern GameController* global_game;

enum MouseInteraction
{
    NONE = 0,
    PLACE_SERVER,
    REPAIR_SERVER,
    CONNECT_SERVER_START,
    CONNECT_SERVER_END,
    PURGE_SERVER,
    CUT_CONNECTOR_START,
    CUT_CONNECTOR_END
};

struct GameSchema : public Schema<GameSchema, 20>
{
    DECLARE_BASE_SCHEMA(GameSchema, 20);

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

enum TutorialState
{
    TUTORIAL_START = 0,
    TUTORIAL_PLACE_SERVER_A,
    TUTORIAL_PLACE_SERVER_B,
    TUTORIAL_CONNECT_SERVER_A,
    TUTORIAL_CONNECT_SERVER_B,
    TUTORIAL_AWAIT_FAULT,
    TUTORIAL_FAULT,
    TUTORIAL_AWAIT_HACKERS,
    TUTORIAL_CUT_A,
    TUTORIAL_CUT_B,
    TUTORIAL_PURGE_INDICATE,
    TUTORIAL_PURGE,
    TUTORIAL_END
};

class GameController : public Component, public GameSchema
{
public:
    CONSTRUCT_SCHEMA(Component, GameSchema);
    DECLARE_COMPONENT(Component, GameController);

    void OnCreate();

    void Update();

    void BuildServer(unsigned int pos);

    void RepairServer(Vector2 pos);

    void ConnectServers(Server* first, Server* second);

    void CutServers(Server* first, Server* second);

    void SetPaused(bool paused);

    // Returns the cost of a connection to another server
    int ConnectionCost(Server* a, Server* b);

    Server* FindServer(Vector2 pos);

    void InfoMessage(string text);

    void PurgeServer(Server* server);

    void ActivateTutorial();

    GUI* gui = nullptr;

    Clock clock;

    unordered_map<unsigned int, Server*> servers;

    GameSim* simulator = nullptr;

    bool restart = false;

    bool isTutorial = false;

    int tutorialState = TutorialState::TUTORIAL_START;

private:
    unsigned int closest = 0;

    Texture* mouseIcon = nullptr;

    int pausebtn_handle;

    Uint32 lastStepTime = 0;

    MouseInteraction mouseMode = NONE;

    Server* connectee = nullptr;

    Entity* connections_entity = nullptr;

    Tooltip* tooltip = nullptr;

    int pauseRequestStack = 0;

};

#endif // GAMECONTROLLER_H
