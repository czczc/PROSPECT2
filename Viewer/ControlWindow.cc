#include "ControlWindow.h"

#include "TGButton.h"
#include "TGNumberEntry.h"

#include <iostream>
using namespace std;

ControlWindow::ControlWindow(const TGWindow *p, int w, int h)
    :TGVerticalFrame(p, w, h)
{    
    // Navigation Frame
    TGHorizontalFrame *navigationFrame = new TGHorizontalFrame(this, w, 100, kFixedWidth);
    AddFrame(navigationFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2));
    {
        fPrevButton = new TGTextButton(navigationFrame, "< Prev");
        navigationFrame->AddFrame(fPrevButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));

        fEventEntry = new TGNumberEntryField(navigationFrame, -1, 0, TGNumberFormat::kNESInteger);
        fEventEntry->SetDefaultSize(80, 20);
        navigationFrame->AddFrame(fEventEntry, new TGLayoutHints(kLHintsTop | kLHintsCenterY, 3, 2, 2, 2));

        fNextButton = new TGTextButton(navigationFrame, "Next >");
        navigationFrame->AddFrame(fNextButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 3, 2, 2, 2));
    }
    fNextCIButton = new TGTextButton(this, "Find Next Coincidence");
    AddFrame(fNextCIButton, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 2, 2, 2)); 
}

ControlWindow::~ControlWindow()
{
}