#ifndef INFO_WINDOW_H
#define INFO_WINDOW_H

#include "TRootEmbeddedCanvas.h"

#include <vector>

#define MAXCOLORS 6

class DAQEvent;
class WFAnalyzer;
class TObject;

class InfoWindow: public TRootEmbeddedCanvas
{
public:
    InfoWindow(const TGWindow *p, int w,int h);
    virtual ~InfoWindow();

    void ClearCanvas();
    void UpdateCanvas();


    void DrawEventInfo(DAQEvent *event, WFAnalyzer *ana);
    
    TCanvas* can; 
    vector<TObject*> listOfDrawables;

    ClassDef(InfoWindow, 0)
};

#endif