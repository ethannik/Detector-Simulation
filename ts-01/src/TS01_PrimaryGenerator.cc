/*
 * Geant4 physics generator using GPS.  It should be configured through
 * the UI.
 */
#include "TS01_PrimaryGenerator.hh"

TS01_PrimaryGenerator::TS01_PrimaryGenerator()
{
	src = new G4GeneralParticleSource;
}

TS01_PrimaryGenerator::~TS01_PrimaryGenerator()
{
	delete src;
}

void TS01_PrimaryGenerator::GeneratePrimaries(G4Event* evt)
{
	src->GeneratePrimaryVertex(evt);
    evt->Print();
     // G4cout << "Generated something" << G4endl;
}
