//
//  TS01_PhotoSD.cpp
//  ts_01
//
//  Created by Kael Hanson on 4/4/16.
//
//

#include <stdio.h>

#include "TS01_PhotoSD.hh"

// Bialkali PMT QE starts at 300 nm goes to 690 nm in steps of 10 n,
static G4double QE[] = {
    1.773,  6.173,  11.074, 16.593, 20.03,  // 300 nm .. 340 nm
    22.368, 23.366, 24.242, 24.39 , 24.843, // 350 nm .. 390 nm
    24.969, 25.323, 24.879, 23.809, 22.959, // 400 nm .. 440 nm
    22.204, 21.097, 19.572, 18.351, 17.425, // 450 nm .. 490 nm
    16.655, 15.647, 13.518, 10.752,  8.71,  // 500 nm .. 540 nm
    7.473 ,  6.587,  5.736,  4.906,  4.067, // 550 nm .. 590 nm
    3.289 ,  2.553,  1.878,  1.211,  0.835, // 600 nm .. 640 nm
    0.533 ,  0.304,  0.175,  0.093,  0.048  // 650 nm .. 690 nm
};

TS01_PhotoSD::TS01_PhotoSD(const G4String& name, const G4String& hitsCollectionName) :
    G4VSensitiveDetector(name)
{
    
}

TS01_PhotoSD::~TS01_PhotoSD() { }

void TS01_PhotoSD::Initialize(G4HCofThisEvent *hitCollection)
{
    unweighted_hits = 0.0;
    weighted_hits = 0.0;
}

G4bool TS01_PhotoSD::ProcessHits(G4Step *step, G4TouchableHistory *history)
{
    G4double p = step->GetTrack()->GetKineticEnergy();
    G4double wl = 1240.0 / p * CLHEP::eV;
    G4StepPoint* post = step->GetPostStepPoint();
    G4cout << "SD-S " << post->GetGlobalTime() << " " << wl << G4endl;
    unweighted_hits++;
    if (wl >= 300.0 && wl < 700.0)
    {
        int iw = (int) ((wl - 300.0) / 10.0);
        weighted_hits += QE[iw]*0.01;
    }
    return true;
}

void TS01_PhotoSD::EndOfEvent(G4HCofThisEvent *hitCollection)
{
    G4cout << "SD-W " << unweighted_hits << " " << weighted_hits << G4endl;
}