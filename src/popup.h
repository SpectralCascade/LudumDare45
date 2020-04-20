#ifndef POPUP_H
#define POPUP_H

#include "Ossium.h"

using namespace Ossium;

class GUI;

class Popup : public GraphicComponent, public InputContext
{
public:
    DECLARE_COMPONENT(GraphicComponent, Popup);

    void OnCreate();

    void Init(GUI* gameGUI, InputController* inputController);

    void AddMessage(string text, Rect area = {0, 0, 0, 0});

    void Render(Renderer& renderer);

    void ShowNextMessage();

    void Show();

    void Hide();

private:
    GUI* gui = nullptr;

    bool shown = false;

    stack<string> textMessages;

    stack<Rect> highlightAreas;

    TextLayout layout;

    Rect box;

    Rect currentHighlight = {0, 0, 0, 0};

};

#endif // POPUP_H
