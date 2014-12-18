{
    TString include = ".include ";
    TString load = ".L ";

    TString prefix = "../Event";
    gROOT->ProcessLine( include + prefix );
    gROOT->ProcessLine( load + prefix + "/DAQEvent.cc+" );
    gROOT->ProcessLine( load + prefix + "/WFAnalyzer.cc+" );
    gROOT->ProcessLine( load + prefix + "/RecEvent.cc+" );

    prefix = "../Viewer";
    gROOT->ProcessLine( include + prefix );
    gROOT->ProcessLine( load + prefix + "/ViewWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/InfoWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/ControlWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/MainWindow.cc+" );
    gROOT->ProcessLine( load + prefix + "/GuiController.cc+" );
}