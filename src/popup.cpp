#include "popup.h"
#include "gui.h"

using namespace std;
using namespace Ossium;

REGISTER_COMPONENT(Popup);

void Popup::OnCreate()
{
    box = Rect((1280 / 2) - (640 / 2), (768 / 2) - (240 / 2), 640, 240);
    layout.SetBounds(Vector2(box.w - 4, box.h - 4));
    layout.SetPointSize(20);
    layout.mainColor = Colors::BLACK;
    layout.SetAlignment(Typographic::CENTERED);
    // Always show popup on top of (nearly) everything
    SetRenderLayer(97);
}

void Popup::Init(GUI* gameGUI, InputController* inputController)
{
    gui = gameGUI;
    inputController->AddContext("PopupGUI", this);

    MouseHandler* mouse = GetHandler<MouseHandler>();

    mouse->AddAction("NextPopup", [&] (const MouseInput& data) { if (data.state) { gui->popup->ShowNextMessage(); } return ActionOutcome::ClaimContext; }, MOUSE_BUTTON_LEFT);
}

void Popup::Render(Renderer& renderer)
{
    if (shown)
    {
        Font* font = GetService<ResourceController>()->Get<Font>("assets/Orkney Regular.ttf", 36);

        if (currentHighlight.w != 0 && currentHighlight.h != 0)
        {
            gui->Highlight(currentHighlight);
        }

        Rect(0, 0, 1280, 768).DrawFilled(renderer, Color(0, 0, 0, 80));

        box.DrawFilled(renderer, Colors::CYAN);
        box.Draw(renderer, Colors::WHITE);

        layout.Render(renderer, *font, Vector2(box.x + 2, box.y + 2));
    }
}

void Popup::AddMessage(string text, Rect area)
{
    textMessages.push(text);
    highlightAreas.push(area);
}

void Popup::Show()
{
    shown = true;
    gui->input->SetActive(false);
    SetActive(true);
}

void Popup::Hide()
{
    shown = false;
    gui->input->SetActive(true);
    SetActive(false);
}

void Popup::ShowNextMessage()
{
    if (!textMessages.empty())
    {
        Font* font = GetService<ResourceController>()->Get<Font>("assets/Orkney Regular.ttf", 36);
        layout.SetText(*GetService<Renderer>(), *font, textMessages.top(), true);
        layout.Update(*font);
        textMessages.pop();
        currentHighlight = highlightAreas.top();
        highlightAreas.pop();
        Show();
    }
    else
    {
        Hide();
    }
}
