{
    gROOT->Reset();
    gROOT->ProcessLine(".x loadClasses.C" );

    // x = new DAQEvent("~/Projects/PROSPECT/PROSPECT2/PROSPECT2/DAQ/test.root");
    // x.GetEntry(0);
    // x.PrintInfo(1);
    // x.GetEntry(x.nEvents-1);
    // x.PrintInfo(1);

    GuiController *gc = new GuiController(gClient->GetRoot(), 900, 900);

}