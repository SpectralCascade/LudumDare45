#ifndef GUI_H
#define GUI_H

#include "Ossium.h"

using namespace Ossium;

class GameController;

class GUI : public GraphicComponent
{
public:
    DECLARE_COMPONENT(Component, GUI);

    void OnCreate();

    void Render(Renderer& renderer);

    InputGUI* input = nullptr;

    Button* buildServerButton = nullptr;

    Button* repairButton = nullptr;

    Button* connectorButton = nullptr;

    Button* cutConnectionButton = nullptr;

    Button* purgeButton = nullptr;

    Button* pauseButton = nullptr;

    TextLayout moneyText;

    TextLayout timeText;

    GameController* game = nullptr;

};

#endif // GUI_H
