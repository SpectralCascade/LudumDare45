#ifndef GUI_H
#define GUI_H

#include "Ossium.h"

using namespace Ossium;

class GameController;
class Popup;

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

    Popup* popup;

    void Highlight(Rect area, SDL_Color color = Colors::YELLOW);

    TextLayout moneyText;

    TextLayout timeText;

    GameController* game = nullptr;

};

#endif // GUI_H
