//
//  TS01_PhotoSD.hh
//  ts_01
//
//  Created by Kael Hanson on 4/4/16.
//
//

#ifndef TS01_PhotoSD_h
#define TS01_PhotoSD_h

#include "G4VSensitiveDetector.hh"

class TS01_PhotoSD : public G4VSensitiveDetector
{
public:
    TS01_PhotoSD(const G4String& name, const G4String& hitsCollectionName);
    ~TS01_PhotoSD();
    
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);
    
private:
    G4double unweighted_hits, weighted_hits;

};

#endif /* TS01_PhotoSD_h */
