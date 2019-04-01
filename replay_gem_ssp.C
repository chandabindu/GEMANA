#define USE_TREESEARCH

#include "TSystem.h"
#include "TList.h"
#include "THaRun.h"
#include "THaEvent.h"
#include "THaAnalyzer.h"
#include "THaApparatus.h"
//#include "SBSGEMStand.h"
//#include "SBSBigBite.h"
//R__ADD_LIBRARY_PATH(../sbs_offline/devel/build)
//R__LOAD_LIBRARY(../sbs_offline/devel/build/libsbs)
R__ADD_LIBRARY_PATH(../SBS-offline/)
R__LOAD_LIBRARY(../SBS-offline/libsbs)
#include "SBSEArm.h"
#include "SBSHCal.h"

#ifdef USE_TREESEARCH
R__LOAD_LIBRARY(../TreeSearch/libTreeSearch-SBS)
R__LOAD_LIBRARY(Minuit)
R__ADD_INCLUDE_PATH(../TreeSearch/)
#if !defined(__CLING__) || defined(__ROOTCLING__)
#include "SBSGEMTracker.h"
#include "SBSSpec.h"
//#include "THaEvData.h"
#endif

#else
#if !defined(__CLING__) || defined(__ROOTCLING__)
#include "SBSGEMStand.h"
#endif

#endif

void replay_gem_ssp(Int_t runnum = 1982, Int_t lastEvent = -1, Int_t segment = -1){

 // gSystem->Load("../SBS-offline/libsbs.so");
#ifndef USE_TREESEARCH
  SBSGEMStand *ssp_gem = new SBSGEMStand("gems","Test GEMs");

  SBSEArm *ssp = new SBSEArm("sbs","Test Arm with GEMs");
  ssp->AddDetector(ssp_gem);
#else
  SBS::GEMTracker *ssp_gem = new SBS::GEMTracker("gems","Test GEMs");
  ssp_gem->SetBuiltinCrateMap(false);
  SBS::SBSSpec *ssp = new SBS::SBSSpec("sbs","Test Arm with GEMs",1,"gems");
  //SBSGEMStand *ssp_gem = new SBSGEMStand("gem","Test GEMs");
#endif


  //
  //  Steering script for Hall A analyzer demo
  //

  // Set up the equipment to be analyzed.

  // add the two spectrometers with the "standard" configuration
  // (VDC planes, S1, and S2)
  // Collect information about a easily modified random set of channels
  // (see DB_DIR/*/db_D.dat)
  /*
     THaApparatus* DECDAT = new THaDecData("D","Misc. Decoder Data");
     gHaApps->Add( DECDAT );
     */


  // Set up the analyzer - we use the standard one,
  // but this could be an experiment-specific one as well.
  // The Analyzer controls the reading of the data, executes
  // tests/cuts, loops over Apparatus's and PhysicsModules,
  // and executes the output routines.
  THaAnalyzer* analyzer = new THaAnalyzer;

  gHaApps->Add(ssp);

  // A simple event class to be output to the resulting tree.
  // Creating your own descendant of THaEvent is one way of
  // defining and controlling the output.
  THaEvent* event = new THaEvent;

  // Define the run data file we want to use. If segment is non-zero
  // then it means use the specified segmented file. Otherwise it won't do
  // anything.
  const char *segPost = (segment<0 ? "" : Form(".%d",segment));
  TString inFile = "test_88.dat";

  analyzer->SetVerbosity(2);
  analyzer->SetMarkInterval(1000);
  analyzer->SetOdefFile("output.def");
  // File to record cuts accounting information
  analyzer->SetSummaryFile(TString::Format("sbs_hcal_test.log%s",segPost)); // optional

  // Define the analysis parameters
  analyzer->SetEvent( event );

  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
  //  THaRun* run = new THaRun( "prod12_4100V_TrigRate25_4.dat" );
  //THaRun* run = new THaRun(TString::Format("data/fadc_%d.dat.0",runnum) );
  THaRun* run = new THaRun(inFile.Data());
  //run->SetFirstEvent(2834);
  //run->SetLastEvent(lastEvent);
  run->SetLastEvent(100);

  run->SetDataRequired(THaRunBase::kDate);
  run->SetDate(TDatime());

#ifdef USE_TREESEARCH
  analyzer->SetOutFile( TString::Format("~/rootfiles/fadc_%d%s.root",
          runnum,segPost));
#else
  analyzer->SetOutFile( TString::Format("~/rootfiles/fadc_gemstand_%d%s.root",
          runnum,segPost));
#endif

  //analyzer->SetCompressionLevel(0); // turn off compression
  analyzer->Process(run);     // start the actual analysis
}
