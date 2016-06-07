
#ifndef TS01_PhysicsList_h
#define TS01_PhysicsList_h

#include "G4VModularPhysicsList.hh"

class TS01_PhysicsList : public G4VModularPhysicsList
{
public:
    TS01_PhysicsList();
    virtual ~TS01_PhysicsList() { }
    virtual void SetCuts();
   
};
#endif // TS01_PhysicsList_h
