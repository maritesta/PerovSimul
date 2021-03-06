#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(),
    fScoringVolume(0)
{ 
  DefineMaterials();
}

DetectorConstruction::~DetectorConstruction()
{ }

void DetectorConstruction::DefineMaterials()
{
  G4double a, z, density;
  G4Element*  O = new G4Element("Oxygen",   "O",  z= 8., a= 16.00*g/mole);
  G4Element* Lu = new G4Element("Lutetium", "Lu", z=71., a=174.97*g/mole);
  G4Element* Si = new G4Element("Silicon",  "Si", z=14., a= 28.09*g/mole);
  G4Element* H  = new G4Element("Hydrogen" ,"H" , 1.,  1.01*g/mole);
  G4Element* C  = new G4Element("Carbon"   ,"C" , 6., 12.00*g/mole);

  G4Material* plastic = new G4Material("plastic", 1.0*g/cm3, 2);
  plastic->AddElement(H, 1);
  plastic->AddElement(C, 1);

  G4Material* LYSO = new G4Material("LYSO", density=7.1*g/cm3,3,kStateSolid);
  LYSO->AddElement(Lu, 2);
  LYSO->AddElement(Si, 1);
  LYSO->AddElement(O,  5);
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  
  // -----------------------------------
  // World
  G4double world_sizeXY = 50*cm;
  G4double world_sizeZ  = 50*cm;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
	      0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
  
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name
                                   
  G4VPhysicalVolume* physWorld = 
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking
                     

  // ------------------------------------  
  // Distance source / crystal
  G4double ring  = 2*cm;           

  /*
  // Source 
  G4Material* plastic_mat = nist->FindOrBuildMaterial("plastic");
  G4double source_radius = 10*mm,  source_dZ = 5.*mm;        
  G4Tubs* source = new G4Tubs("source", 0, source_radius, source_dZ/2, 0, 2*M_PI);
  G4LogicalVolume* logicSource = new G4LogicalVolume(source, plastic_mat, "SourceLV");        
               
  G4RotationMatrix rotmD  = G4RotationMatrix();
  rotmD.rotateY(90*deg); 
  rotmD.rotateZ(180*deg);
  G4ThreeVector positionD(0,0,0);
  G4Transform3D transformD = G4Transform3D(rotmD,positionD);
                                    
  new G4PVPlacement(transformD, logicSource, "source", logicWorld, false, 1, checkOverlaps); 
  */

  // Crystal 
  G4Material* cryst_mat = nist->FindOrBuildMaterial("LYSO");
  //G4double cryst_dX = 10*mm, cryst_dY = 10*mm, cryst_dZ = 10*mm; 
  G4double cryst_dX = 100*mm, cryst_dY = 100*mm, cryst_dZ = 10*mm; 
  
  G4Box* solidCryst = new G4Box("crystal", cryst_dX/2, cryst_dY/2, cryst_dZ/2);
  G4LogicalVolume* logicCryst = 
    new G4LogicalVolume(solidCryst,          //its solid
                        cryst_mat,           //its material
			"CrystalLV");        //its name
  
  G4RotationMatrix rotmC  = G4RotationMatrix();
  rotmC.rotateY(90*deg); 
  rotmC.rotateZ(0*deg);
  //G4ThreeVector uzC = G4ThreeVector(std::cos(0*deg), std::sin(0*deg),0.);     
  G4ThreeVector uzC = G4ThreeVector(0., std::sin(0*deg),std::cos(0*deg));     
  G4ThreeVector positionC = (ring+0.5*cryst_dZ)*uzC;
  G4Transform3D transformC = G4Transform3D(rotmC,positionC);
  
  new G4PVPlacement(transformC,             // rotation,position
		    logicCryst,             // its logical volume
		    "crystal",              // its name
		    logicWorld,             // its mother  volume
		    false,                  // no boolean operation
		    0,                      // copy number
		    checkOverlaps);         // checking overlaps 


  // --------------------------------------------------
  // Set crystals scoring volume
  fScoringVolume = logicCryst;      
  
  //always return the physical World
  return physWorld;
}
