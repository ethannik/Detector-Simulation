#include <math.h>
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Orb.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "TS01_DetectorConstruction.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4VisAttributes.hh"
#include "TS01_PhotoSD.hh"
// New code
#include "G4Material.hh"
#include "G4NistManager.hh" 

TS01_DetectorConstruction::TS01_DetectorConstruction(bool fiber, int n, G4double dia, G4double r) :
    num_fiber(n),
    fiber_dia(dia),
    fiber_len(2.0*CLHEP::m),
    det_radius(r),
    pmt_face_z(1.0*CLHEP::mm),
    pmt_body_z(5.0*CLHEP::mm),
    fibers(n)
{
    doFiber = fiber;
}

TS01_DetectorConstruction::~TS01_DetectorConstruction()
{
    for (auto const &fiber : fibers) delete fiber;
}

void TS01_DetectorConstruction::ConstructMaterials()
{
    // Necessary Elements
    G4Element* H  = new G4Element("Hydrogen" , "H" , 1.0 , 1.01 *g/mole);
    G4Element* C  = new G4Element("Carbon"   , "C" , 6.0 , 12.00*g/mole);
    G4Element* O  = new G4Element("Oxygen"   , "O" , 8.0 , 16.0 *g/mole);
    G4Element* F  = new G4Element("Fluorine" , "F" , 9.0 , 19.0 *g/mole);
    G4Element* Si = new G4Element("Silicon"  , "Si", 14.0, 28.09*g/mole);
    G4Element* Al = new G4Element("Aluminum" , "Al", 13.0, 26.98*g/mole);

    //Vacuum
    vacuum = new G4Material("Vacuum", 1.0, 1.0*g/mole, 1E-08*g/cm3);

    //Ice
    ice = new G4Material("Ice", 0.9167*g/cm3, 2);
    ice->AddElement(H, 2);
    ice->AddElement(O, 1);

    //Gel
    gel = new G4Material("Silicone RTV Gel", 0.98*g/cm3, 4);
    gel->AddElement(H, 5);
    gel->AddElement(C, 6);
    gel->AddElement(O, 1);
    gel->AddElement(Si, 1);

    //Aluminum
    al = new G4Material("Aluminum", 13.0, 26.98*g/mole, 2.7*CLHEP::g/CLHEP::cm3);

    //Fiber (PMMA)
    pmma = new G4Material("PMMA", 1.19*g/cm3, 3);
    pmma->AddElement(H, 8);
    pmma->AddElement(O, 2);
    pmma->AddElement(C, 5);
    
    fp = new G4Material("FP", 1.46*g/cm3, 2);
    fp->AddElement(C, 2);
    fp->AddElement(F, 4);

    // Materials found using NIST Database
    G4NistManager* man = G4NistManager::Instance();

    //Air
    G4Material* Air = man->FindOrBuildMaterial("G4_AIR");

    // Polystyrene
    G4Material* Polystyrene = man->FindOrBuildMaterial("G4_POLYSTYRENE");

    // Glass
    G4Material* Glass = man->FindOrBuildMaterial("G4_Pyrex_Glass");
	
}

void TS01_DetectorConstruction::ConstructFibers(G4LogicalVolume *w)
{
    fiber_outer_clad_lv = new G4LogicalVolume(new G4Tubs("OuterCladding",
                                                         0.0, 0.5*fiber_dia,
                                                         0.5*fiber_len, 0.0, 360.0*deg),
                                              fp, "FiberOuterCladding");
    
    fiber_inner_clad_lv = new G4LogicalVolume(new G4Tubs("InnerCladding",
                                                         0.0, 0.97*0.5*fiber_dia,
                                                         0.5*fiber_len, 0.0, 360.0*deg),
                                              fp, "FiberInnerCladding");
    
    fiber_core_lv = new G4LogicalVolume(new G4Tubs("PlasticFiber",
                                                   0.0, 0.94*0.5*fiber_dia,
                                                   0.5*fiber_len,
                                                   0.0, 360.0*deg),
                                        polystyrene, "PlasticFiberLV");
    
    pmt_face = new G4LogicalVolume(new G4Tubs("PMT",
                                              0.0, 0.6*fiber_dia, 0.5*pmt_face_z,
                                              0.0, 360.0*deg),
                                   ice, "PhotoCathode_LV");
    
    pmt_body = new G4LogicalVolume(new G4Tubs("PMT",
                                              0.0, 0.6*fiber_dia, 0.5*pmt_body_z,
                                              0.0, 360.0*deg),
                                   al, "PMT_LV");
    
    TS01_PhotoSD *photo_sd = new TS01_PhotoSD("TS01/Photomultiplier", "PMTHitsCollection");
    SetSensitiveDetector(pmt_face, photo_sd);
    
    fiber_core_lv->SetVisAttributes(G4VisAttributes(G4Colour(0.1, 0.75, 0.1, 0.5)));
    fiber_outer_clad_lv->SetVisAttributes(G4VisAttributes(false));
    fiber_inner_clad_lv->SetVisAttributes(G4VisAttributes(false));
    
    inner = new G4PVPlacement(NULL, G4ThreeVector(0.0, 0.0, 0.0),
                              fiber_inner_clad_lv, "Cladding",
                              fiber_outer_clad_lv, false, 0);
    core  = new G4PVPlacement(NULL, G4ThreeVector(0.0, 0.0, 0.0),
                              fiber_core_lv, "Core",
                              fiber_inner_clad_lv, false, 0);
    
    const double PI = 2.0 * std::acos(0.0);
    
    for (int i=0; i<num_fiber; i++)
    {
        const double phi = 2.*PI/num_fiber*i;
        double x = det_radius*cos(phi);
        double y = det_radius*sin(phi);
        
        G4VPhysicalVolume* pv_fib =
            new G4PVPlacement(NULL,
                              G4ThreeVector(x, y, 0.0),
                              fiber_outer_clad_lv, "DetectorFiber",
                              w, false, i);
        G4VPhysicalVolume* pv_det =
            new G4PVPlacement(NULL,
                              G4ThreeVector(x, y, 0.5*(fiber_len+pmt_face_z)),
                              pmt_face, "PhotoCathode",
                              w, false, i);
        G4VPhysicalVolume* pv_pmt =
            new G4PVPlacement(NULL,
                              G4ThreeVector(x, y, 0.5*(fiber_len+pmt_body_z)+pmt_face_z),
                              pmt_body, "PMTBody",
                              w, false, i);
        
        new G4LogicalBorderSurface("PMTin", pv_det, pv_pmt, photocathode);
    }
}

void TS01_DetectorConstruction::ConstructDOM(G4LogicalVolume *w)
{
    G4double p_k[] = {2.00*eV, 3.47*eV};
    G4double refl_k[] = { 0.0, 0.0 };
    G4double effi_k[] = { 1.0, 1.0 };
    
    G4MaterialPropertiesTable* pcSurfProp = new G4MaterialPropertiesTable();
    pcSurfProp->AddProperty("REFLECTIVITY", p_k, refl_k, 2);
    pcSurfProp->AddProperty("EFFICIENCY",   p_k, effi_k, 2);
    
    dom_sphere = new G4LogicalVolume(new G4Orb("DOMSphere", 16.51*CLHEP::cm),
                                     glass, "DOMPressureVessel");
    
    dom_sphere->SetVisAttributes(G4VisAttributes(G4Colour(0.4, 0.4, 0.8, 0.4)));
                                 
    dom_interior = new G4LogicalVolume(new G4Orb("DOMInnerVoid", (16.51-1.27)*CLHEP::cm),
                                       gel, "DOMInstrumentVolume");
    
    dom_interior->SetVisAttributes(G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.05)));
    
    dom_pmt_pc = new G4LogicalVolume(new G4Sphere("R7081Sphere",
                                                  134.0*CLHEP::mm,
                                                  136.7* CLHEP::mm,
                                                  0.0,
                                                  360.0*CLHEP::deg,
                                                  127.0*CLHEP::deg,
                                                  180.0*CLHEP::deg),
                                     glass, "PMTEnvelope");
    dom_pmt_vac = new G4LogicalVolume(new G4Sphere("R7081VAC",
                                                   0.0, 134.0*CLHEP::mm,
                                                   0.0, 360.0*CLHEP::deg,
                                                   127.0*CLHEP::deg,
                                                   180.0*CLHEP::deg),
                                      vacuum, "PMTVacuum");
    
    TS01_PhotoSD *photo_sd = new TS01_PhotoSD("TS01/Photomultiplier", "PMTHitsCollection");
    SetSensitiveDetector(dom_pmt_pc, photo_sd);
    
    dom_pmt_pc->SetVisAttributes(G4VisAttributes(G4Colour(0.75, 0.65, 0.1, 0.2)));
    
    G4VPhysicalVolume *pv_dom = new G4PVPlacement(NULL, G4ThreeVector(), dom_sphere,
                                                  "R7081PV", w, false, 0);
    G4VPhysicalVolume *pv_din = new G4PVPlacement(NULL, G4ThreeVector(), dom_interior,
                                                  "IR7081PV", dom_sphere, false, 0);
    G4VPhysicalVolume *pv_pc  = new G4PVPlacement(NULL, G4ThreeVector(0, 0, 0), dom_pmt_pc,
                                                  "PC7081PV", dom_interior, false, 0);
    G4VPhysicalVolume *pv_vac = new G4PVPlacement(NULL, G4ThreeVector(), dom_pmt_vac,
                                                  "V7081PV", dom_interior, false, 0);
    
    new G4LogicalBorderSurface("PMTin", pv_pc, pv_vac, photocathode);
}

G4VPhysicalVolume* TS01_DetectorConstruction::Construct()
{
    ConstructMaterials();

	G4LogicalVolume* world_lv = new G4LogicalVolume(
		new G4Box("WorldBox", 1.25*m, 1.25*m, 1.25*m),
		ice, "IceBox");
    G4VPhysicalVolume* world = new G4PVPlacement(NULL, G4ThreeVector(), world_lv,
                                                 "World", NULL, false, 0, true);
    
    add_ice_optics();
    add_air_optics();
    add_glass_optics();
    add_wls_optics();
    
    // PMT optical surface to interface with the fiber (absorbing metallic plaque)
    photocathode = new G4OpticalSurface("cathode",
                                        glisur,
                                        polished,
                                        dielectric_metal,
                                        0.0);
    
    G4double p_k[] = {2.00*eV, 3.47*eV};
    G4double refl_k[] = { 0.0, 0.0 };
    G4double effi_k[] = { 0.0, 0.0 };
    
    G4MaterialPropertiesTable* pcSurfProp = new G4MaterialPropertiesTable();
    pcSurfProp->AddProperty("REFLECTIVITY", p_k, refl_k, 2);
    pcSurfProp->AddProperty("EFFICIENCY",   p_k, effi_k, 2);
    
    photocathode->SetMaterialPropertiesTable(pcSurfProp);
    
    world_lv->SetVisAttributes((G4VisAttributes(true)));
    world_lv->SetVisAttributes(G4VisAttributes(G4Colour(0.25, 0.75, 0.625, 0.33)));
    
    if (doFiber)
        ConstructFibers(world_lv);
    else
        ConstructDOM(world_lv);

	return world;
}

void TS01_DetectorConstruction::add_air_optics(void)
{
    G4double pp[] = { 1.0*CLHEP::eV, 6.0*CLHEP::eV };
    G4double pn[] = { 1.0, 1.0 };
    G4double pa[] = { 10.0*CLHEP::km, 10.0*CLHEP::km };
    
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
    
    mpt->AddProperty("RINDEX",    pp, pn, 2)->SetSpline(true);
    mpt->AddProperty("ABSLENGTH", pp, pa, 2)->SetSpline(true);
    air->SetMaterialPropertiesTable(mpt);
    
}

void TS01_DetectorConstruction::add_ice_optics(void)
{
    // From SPICE Mie - sort of

    G4double photonEnergy[] = {
        1.90*CLHEP::eV, 2.25*CLHEP::eV,
        2.75*CLHEP::eV, 3.54*CLHEP::eV,
        4.96*CLHEP::eV };
    G4double n[]            = { 1.36, 1.35, 1.34, 1.34, 1.33 };
    G4double abs[]          = { 3.67*CLHEP::m, 19.3*CLHEP::m, 63.4*CLHEP::m , 58.8*CLHEP::m, 41.0*m };
    
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
    
    mpt->AddProperty("RINDEX",    photonEnergy, n  , 5)->SetSpline(true);
    mpt->AddProperty("ABSLENGTH", photonEnergy, abs, 5)->SetSpline(true);
    
    ice->SetMaterialPropertiesTable(mpt);

}

void TS01_DetectorConstruction::add_glass_optics(void)
{
    G4double p[] =      {
        1.771*CLHEP::eV, 2.067*CLHEP::eV,
        2.480*CLHEP::eV, 3.100*CLHEP::eV,
        4.133*CLHEP::eV };
    
    G4double n[] =      { 1.513, 1.516, 1.521, 1.531, 1.553 };
    G4double n_gel[] =  { 1.41, 1.41, 1.41, 1.41, 1.41 };
    G4double atten[] =  {
        6.24*CLHEP::m, 4.52*CLHEP::m,
        4.15*CLHEP::m, 3.11*CLHEP::m,
        8.34*CLHEP::mm };
    
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable;
    mpt->AddProperty("RINDEX",    p, n    , 5)->SetSpline(true);
    mpt->AddProperty("ABSLENGTH", p, atten, 5)->SetSpline(true);
    
    glass->SetMaterialPropertiesTable(mpt);
    
    G4MaterialPropertiesTable* mpt_gel = new G4MaterialPropertiesTable;
    mpt_gel->AddProperty("RINDEX"   , p, n_gel, 5);
    mpt_gel->AddProperty("ABSLENGTH", p, atten, 5);
    gel->SetMaterialPropertiesTable(mpt_gel);
}

void TS01_DetectorConstruction::add_wls_optics(void)
{
    G4double photonEnergy[] =
    {
        2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
        2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
        2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
        2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
        2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
        2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
        2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
        3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
        3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
        3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV
    };
    
    //--------------------------------------------------
    //  PMMA for WLSfibers
    //--------------------------------------------------
    
    G4double refractiveIndexWLSfiber[] = {
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
        1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60
    };
    
    G4double absWLSfiber[] =
    {
        5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
        5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,
        5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,5.40*m,1.10*m,
        1.10*m,1.10*m,1.10*m,1.10*m,1.10*m,1.10*m, 1.*mm, 1.*mm, 1.*mm, 1.*mm,
        1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm, 1.*mm
    };

    
    G4double emissionFib[] = {
        0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
        3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
        12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
        15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
        0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 };

    int nEntries = 50;
    
    // Add entries into properties table
    G4MaterialPropertiesTable* mptWLSfiber = new G4MaterialPropertiesTable();
    mptWLSfiber->AddProperty("RINDEX",photonEnergy,refractiveIndexWLSfiber,nEntries);
    mptWLSfiber->AddProperty("WLSABSLENGTH",photonEnergy,absWLSfiber,nEntries);
    mptWLSfiber->AddProperty("WLSCOMPONENT",photonEnergy,emissionFib,nEntries);
    mptWLSfiber->AddConstProperty("WLSTIMECONSTANT", 0.5*ns);
    // Change WLSTIMECONSTANT to 2.0 ns?
    
    polystyrene->SetMaterialPropertiesTable(mptWLSfiber);
    
    //--------------------------------------------------
    //  Polyethylene
    //--------------------------------------------------
    
    G4double refractiveIndexClad1[] = {
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
        1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49 };
    
    G4double absClad[] = {
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,
        20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m,20.0*m};
    
    // Add entries into properties table
    G4MaterialPropertiesTable* mptClad1 = new G4MaterialPropertiesTable();
    mptClad1->AddProperty("RINDEX",photonEnergy,refractiveIndexClad1,nEntries);
    mptClad1->AddProperty("ABSLENGTH",photonEnergy,absClad,nEntries);
    
    pmma->SetMaterialPropertiesTable(mptClad1);
    
    //--------------------------------------------------
    // Fluorinated Polyethylene
    //--------------------------------------------------
    
    G4double refractiveIndexClad2[] = {
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
        1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42
    };
    
    // Add entries into properties table
    G4MaterialPropertiesTable* mptClad2 = new G4MaterialPropertiesTable();
    mptClad2->AddProperty("RINDEX",photonEnergy,refractiveIndexClad2,nEntries);
    mptClad2->AddProperty("ABSLENGTH",photonEnergy,absClad,nEntries);
    
    fp->SetMaterialPropertiesTable(mptClad2);
    
}


