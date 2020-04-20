#include "popup.h"
#include "gui.h"
#include "gamecontroller.h"

using namespace std;
using namespace Ossium;

REGISTER_COMPONENT(Popup);

void Popup::OnCreate()
{
    box = Rect((1280 / 2) - (640 / 2), (768 / 2) - (200 / 2), 640, 200);
    layout.SetBounds(Vector2(box.w - 4, box.h - 4));
    layout.SetPointSize(20);
    layout.mainColor = Colors::BLACK;
    layout.SetAlignment(Typographic::CENTERED);
    // Always show popup on top of (nearly) everything
    SetRenderLayer(97);

    GetTransform()->SetWorldPosition(Vector2((1280 / 2), (768 / 2)));
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

        if (blockHighlight)
        {
            if (currentHighlight.w != 0 && currentHighlight.h != 0)
            {
                gui->Highlight(currentHighlight);
            }
            else
            {
                Rect(0, 0, 1280, 768).DrawFilled(renderer, Color(0, 0, 0, 140));
            }
        }

        box.x = GetTransform()->GetWorldPosition().x - (640 / 2);
        box.y = GetTransform()->GetWorldPosition().y - (200 / 2);

        box.DrawFilled(renderer, Alpha(Colors::CYAN, 200));
        box.Draw(renderer, Colors::WHITE);

        layout.SetText(renderer, *font, currentText, true);
        layout.Update(*font);
        layout.Render(renderer, *font, Vector2(box.x + 2, box.y + 2));
    }
}

void Popup::AddMessage(string text, bool blackout, Rect area, bool blockInput)
{
    textMessages.push(text);
    highlightAreas.push(area);
    blockHighlights.push(blackout);
    blockInputs.push(blockInput);
}

void Popup::Show()
{
    shown = true;
    SetActive(true);
}

void Popup::Hide()
{
    shown = false;
    gui->input->SetActive(true);
    SetActive(false);
}

void Popup::Update()
{
    if (messageWait > 0.0f)
    {
        messageWait -= delta.Time();
        if (messageWait <= 0.0f)
        {
            ShowNextMessage();
        }
    }
}

void Popup::ShowNextMessage(float delay)
{
    if (delay > 0.0f)
    {
        messageWait = delay;
    }
    else
    {
        if (!textMessages.empty())
        {
            Font* font = GetService<ResourceController>()->Get<Font>("assets/Orkney Regular.ttf", 36);
            currentText = textMessages.top();
            textMessages.pop();
            currentHighlight = highlightAreas.top();
            highlightAreas.pop();
            blockHighlight = blockHighlights.top();
            blockHighlights.pop();
            if (blockInputs.top())
            {
                gui->input->SetActive(false);
            }
            else
            {
                gui->input->SetActive(true);
            }
            blockInputs.pop();
            Show();
        }
        else
        {
            Hide();
        }
    }
}
