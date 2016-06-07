
#include "G4Scintillation.hh"
#include "G4Cerenkov.hh"
#include "G4OpAbsorption.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4PhysicsListHelper.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "TS01_PhysicsList.hh"

TS01_PhysicsList::TS01_PhysicsList() : G4VModularPhysicsList()
{
    SetVerboseLevel(1);
    RegisterPhysics(new G4EmStandardPhysics());
    RegisterPhysics(new G4OpticalPhysics());
}

void TS01_PhysicsList::SetCuts()
{
    G4VModularPhysicsList::SetCuts();
}

