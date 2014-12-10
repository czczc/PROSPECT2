#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "TGFrame.h"

class ViewWindow;
class ControlWindow;
class TGPopupMenu;

enum EMenuIds {
   M_FILE_OPEN,
   M_FILE_EXIT,
};

class MainWindow: public TGMainFrame
{
public:
    MainWindow(const TGWindow *p, int w,int h);
    virtual ~MainWindow();

    int controlWindowWidth; 
    
    ViewWindow        *fViewWindow;
    ControlWindow     *fControlWindow;
    
    TGPopupMenu       *fFileMenu;
    
    void InitMenu();

    ClassDef(MainWindow, 0)
};

#endif