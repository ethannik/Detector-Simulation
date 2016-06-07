//
//  TS01_ActionInitialization.hh
//  ts_01
//
//  Created by Kael Hanson on 4/3/16.
//
//

#ifndef TS01_ActionInitialization_h
#define TS01_ActionInitialization_h

#include "G4VUserActionInitialization.hh"
#include "TS01_PrimaryGenerator.hh"

class TS01_ActionInitialization : public G4VUserActionInitialization
{
public:
    virtual ~TS01_ActionInitialization() { }
    virtual void Build() const
    {
        SetUserAction(new TS01_PrimaryGenerator);
    }
};

#endif /* TS01_ActionInitialization_h */
