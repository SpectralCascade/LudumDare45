#ifndef MENU_H
#define MENU_H

#include "Ossium.h"

using namespace Ossium;

enum MenuState
{
    MENU_TUT = 0,
    MENU_PLAY,
    MENU_QUIT
};

class Menu : public Component
{
public:
    DECLARE_COMPONENT(Component, Menu);

    void OnCreate();

    void OnDestroy();

    MenuState state = MENU_QUIT;

    EngineSystem* engine = nullptr;

private:
    InputGUI* gui;

    Button* tutorial = nullptr;
    Button* play = nullptr;
    Button* quit = nullptr;

    Texture* bg = nullptr;

};

#endif // MENU_H
