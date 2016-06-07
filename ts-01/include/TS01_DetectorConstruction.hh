
#ifndef TS01_DetectorConstruction_h
#define TS01_DetectorConstruction_h

#include <vector>
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4OpticalSurface.hh"
#include "G4VUserDetectorConstruction.hh"

class TS01_DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    TS01_DetectorConstruction(bool, int, G4double, G4double);
    virtual ~TS01_DetectorConstruction();
	virtual G4VPhysicalVolume* Construct();

private:
    void ConstructMaterials();
    void ConstructDOM(G4LogicalVolume*);
    void ConstructFibers(G4LogicalVolume*);
    
    void add_air_optics(void);
    void add_ice_optics(void);
    void add_glass_optics(void);
    void add_wls_optics(void);
    
    G4bool   doFiber;
    G4int    num_fiber;
	G4double fiber_dia;
	G4double fiber_len;
	G4double det_radius;
    G4double pmt_face_z;
    G4double pmt_body_z;
	
    G4Material *polystyrene, *pmma, *fp;
    G4Material *vacuum, *air, *ice, *glass, *al;
    G4Material *gel;
    
    G4LogicalVolume *fiber_inner_clad_lv;
    G4LogicalVolume *fiber_outer_clad_lv;
    G4LogicalVolume *fiber_core_lv;
    G4LogicalVolume *pmt_face, *pmt_body;
    
    G4LogicalVolume *dom_sphere;
    G4LogicalVolume *dom_interior;
    G4LogicalVolume *dom_pmt_vac, *dom_pmt_pc;
    
    std::vector<G4VPhysicalVolume*> fibers;
    G4VPhysicalVolume *core, *inner;
    
    G4OpticalSurface* photocathode;
};

	
#endif // TS01_DetectorConstruction_h
