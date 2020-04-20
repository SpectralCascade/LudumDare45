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

    void AddMessage(string text, bool blackout = true, Rect area = {0, 0, 0, 0}, bool blockInput = true);

    void Render(Renderer& renderer);

    void Update();

    void ShowNextMessage(float delay = 0);

    void Show();

    void Hide();

private:
    GUI* gui = nullptr;

    bool shown = false;

    stack<string> textMessages;

    stack<Rect> highlightAreas;

    stack<bool> blockHighlights;

    stack<bool> blockInputs;

    bool blockHighlight = true;

    TextLayout layout;

    Rect box;

    string currentText;

    Rect currentHighlight = {0, 0, 0, 0};

    float messageWait = 0;

};

#endif // POPUP_H
