                                                                                                                         
R__ADD_INCLUDE_PATH(../TreeSearch/)                                                                                      
                                                                                                                         
void rootlogon()                                                                                                         
{                                                                                                                        
  gSystem->AddIncludePath(" -I/home/chad/software/SBS-offline");                                            
  gROOT->ProcessLine(".include /home/chad/SBS-offline");                                           
                                                                                                                         
} 
