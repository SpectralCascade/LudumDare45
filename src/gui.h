#ifndef GUI_H
#define GUI_H

#include "Ossium.h"

using namespace Ossium;

class GUI : public Component
{
public:
    DECLARE_COMPONENT(Component, GUI);

    void OnCreate();

    InputGUI* input = nullptr;

    Button* buildServerButton = nullptr;

    Button* pauseButton = nullptr;

};

#endif // GUI_H
