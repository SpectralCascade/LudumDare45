#include "tooltip.h"

using namespace Ossium;

REGISTER_COMPONENT(Tooltip);

void Tooltip::OnCreate()
{
    layout.SetBounds(Vector2(1000, 1000));
    layout.SetPointSize(20);
    layout.mainColor = Colors::WHITE;
    SetRenderLayer(25);
}

void Tooltip::Render(Renderer& renderer)
{
    if (shown)
    {
        Font* font = GetService<ResourceController>()->Get<Font>("assets/Orkney Regular.ttf", 36);

        Vector2 textsize = layout.GetSize();
        Vector2 pos = GetTransform()->GetWorldPosition() - Vector2(textsize.x / 2, textsize.y / 2);
        Rect infoBox = Rect(pos.x - 2, pos.y - 2, textsize.x + 4, textsize.y + 4);
        infoBox.DrawFilled(renderer, Colors::BLACK);
        infoBox.Draw(renderer, Colors::WHITE);

        layout.Render(renderer, *font, pos);
    }
}

void Tooltip::SetText(string text)
{
    Font* font = GetService<ResourceController>()->Get<Font>("assets/Orkney Regular.ttf", 36);
    layout.SetText(*GetService<Renderer>(), *font, text, true);
    layout.Update(*font);
}

void Tooltip::Show()
{
    shown = true;
}

void Tooltip::Hide()
{
    shown = false;
}
