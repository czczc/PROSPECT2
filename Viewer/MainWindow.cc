#include "MainWindow.h"
#include "ViewWindow.h"
#include "ControlWindow.h"

#include "TGMenu.h"

#include "TString.h"

#include <iostream>
using namespace std;


MainWindow::MainWindow(const TGWindow *p, int w,int h)
    : TGMainFrame(p, w, h)
{
    InitMenu();
    controlWindowWidth = 240;

    TGHorizontalFrame *vcFrame = new TGHorizontalFrame(this, w, h, kFixedWidth); // view and control frame
    AddFrame(vcFrame, new TGLayoutHints(kLHintsExpandX  | kLHintsExpandY, 2, 2, 2, 2));
    {
        fViewWindow = new ViewWindow(vcFrame, w - controlWindowWidth - 10, h);
        vcFrame->AddFrame(fViewWindow, new TGLayoutHints(
            kLHintsTop | kLHintsExpandX  | kLHintsExpandY, 2, 2, 2, 2));

        fControlWindow = new ControlWindow(vcFrame, controlWindowWidth, h);
        vcFrame->AddFrame(fControlWindow, new TGLayoutHints(
            kLHintsTop  | kFixedWidth, 2, 2, 2, 2));
    }
    
    // set main window stats
    SetWindowName("PROSPECT 2 Event Display");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

//-----------------------------------------------------------------
MainWindow::~MainWindow()
{
}


//-----------------------------------------------------------------
void MainWindow::InitMenu()
{
    TGLayoutHints *menuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

    TGMenuBar *menuBar = new TGMenuBar(this, 10, 100);
    AddFrame(menuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
    {
        // File Menu
        fFileMenu = new TGPopupMenu(gClient->GetRoot());
        fFileMenu->AddEntry("&Open...", M_FILE_OPEN);
        fFileMenu->AddSeparator();
        fFileMenu->AddEntry(new TGHotString("E&xit"), M_FILE_EXIT);
        menuBar->AddPopup(new TGHotString("&File"), fFileMenu, menuBarItemLayout);
    }
}
