#ifndef GUI_CONTROLLER_H
#define GUI_CONTROLLER_H

#include "TGFrame.h"
#include "RQ_OBJECT.h"
#include "TString.h"
#include "TTimeStamp.h"

class MainWindow;
class ViewWindow;
class ControlWindow;
class DAQEvent;
class TCanvas;
class TGraph;
class TLatex;

class GuiController
{
    RQ_OBJECT("EVDGuiController")
public:
    GuiController(const TGWindow *p, int w, int h);
    virtual ~GuiController();

    void InitEvent(const char* filename);
    void Reload();
    void InitConnections();
    void Modified();

    // slots
    void HandleFileMenu(int id);
    void MenuOpen();
    void Prev();
    void Next();
    void Jump();
    void FindNextCoincidence();

    void DrawWF(int i);

    TGraph *gWF[3]; // previous, current, next waveform
    TTimeStamp triggerTS[3];  // previous, current, next waveform
    TLatex *text_dt[3];

    // member variables
    MainWindow    *mw;
    ViewWindow    *vw;
    ControlWindow *cw;
    DAQEvent       *event;

    TCanvas *can;
    TString baseDir;
    int currentEventEntry;


};

#endif
