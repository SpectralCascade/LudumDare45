#ifndef TOOLTIP_H
#define TOOLTIP_H

#include "Ossium.h"

using namespace Ossium;

class Tooltip : public GraphicComponent
{
public:
    DECLARE_COMPONENT(GraphicComponent, Tooltip);

    void OnCreate();

    void Render(Renderer& renderer);

    void Hide();

    void Show();

    void SetText(string text);

private:
    TextLayout layout;

    bool shown = false;

};

#endif // TOOLTIP_H
