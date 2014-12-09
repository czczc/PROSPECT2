{
    TString include = ".include ";
    TString load = ".L ";

    TString prefix = "../Event";
    gROOT->ProcessLine( include + prefix );
    gROOT->ProcessLine( load + prefix + "/DAQEvent.cc+" );
}