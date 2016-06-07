/*
 * ===================================================
 * Large effective photon area detector trade study #1
 * ===================================================
 *
 * Compare photon collection for optical detector composed of
 * WLS fibers versus standard large area PMT, the R7081-02 as
 * used in the IceCube experiment.
 *
 * (C) 2016 Kael HANSON
 */

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "Randomize.hh"

#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "TS01_DetectorConstruction.hh"
#include "TS01_PhysicsList.hh"
#include "TS01_ActionInitialization.hh"
#include "G4SystemOfUnits.hh"

#include <unistd.h>
#include <stdio.h>

void print_help(void)
{
    fprintf(stderr,
            "usage: ts_01 [ options ]\n"
            "  options are ...\n"
            "  -h           this help\n"
            "  -B <file>    batch mode with commands from <file>\n"
            "  -D           DOM mode (default)\n"
            "  -F           Fiber OM mode\n"
            "  -r <radius>  Set detector radius [cm]\n"
            "  -d <dia.>    Set fiber diameter [mm]\n"
            "  -n <n_fiber> Set number of fibers around circle\n");
    exit(1);
}

int main(int argc, char** argv)
{
    
    long seed = 0x7B81AF65;
    G4String macroFile;
    bool     doFiber = false;
    G4double fiber_d = 1.0*mm;
    G4double radius  = 10.0*cm;
    G4int    n_fib   = 120;
    
    int ch;
    while ((ch = getopt(argc, argv, "S:B:r:d:n:DFh")) != -1)
    {
        switch (ch)
        {
            case 'S':
                seed = strtol(optarg, NULL, 0);
                break;
            case 'B':
                macroFile = G4String(optarg);
                break;
            case 'D':
                doFiber = false;
                break;
            case 'F':
                doFiber = true;
                break;
            case 'r':
                radius = strtod(optarg, NULL) * CLHEP::cm;
                break;
            case 'd':
                fiber_d = strtod(optarg, NULL) * CLHEP::mm;
                break;
            case 'n':
                n_fib = strtol(optarg, NULL, 0);
                break;
            case 'h':
                print_help();
        }
    }
        
    G4Random::setTheEngine(new CLHEP::MTwistEngine(seed));
    
    G4RunManager* runManager = new G4RunManager;
    runManager->SetUserInitialization(new TS01_DetectorConstruction(doFiber, n_fib, fiber_d, radius));
    runManager->SetUserInitialization(new TS01_PhysicsList);
    runManager->SetUserInitialization(new TS01_ActionInitialization);
    
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    
    if (macroFile == "")
    {
        G4UIExecutive *ui = new G4UIExecutive(argc, argv);
        G4VisManager* visManager = new G4VisExecutive;
        visManager->Initialize();
        ui->SessionStart();
        delete ui;
        delete visManager;
    }
    else
    {
        G4String cmd = "/control/execute ";
        UImanager->ApplyCommand(cmd+macroFile);
    }
    
	delete runManager;
    
	return 0;
}

	

