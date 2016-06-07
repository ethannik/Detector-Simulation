
#ifndef TS01_PrimaryGenerator_h
#define TS01_PrimaryGenerator_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

class TS01_PrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
	TS01_PrimaryGenerator();
	virtual ~TS01_PrimaryGenerator();

	virtual void GeneratePrimaries(G4Event*);

private:
	G4GeneralParticleSource *src;
};
#endif
