//This program is written for analyzing quartz data
//Chandan Ghosh
//Dec 8, 2018
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "TCanvas.h"
#include <TChain.h>
#include <string>
#include <sstream>
Double_t langaufun(Double_t *x, Double_t *par);
using namespace std;

void Quartzdata()
{
	gDirectory->DeleteAll();
	int ij;
	double charge;
	double ped;
	double mean;
	double sigma = -10;
	double ampli = -100;
	double area = -100;
	double fr[2]={-10};
	double npe;//number of pe
	int runN=-10;
	int channel=-10;

	double R = 50.; //in ohm
	double q = 1.602e-19;
	double gain = 10e6;//PMT gain

	TH1D *h_raw = new TH1D("h_raw","Raw adc",250,0,2500);
	TH1D *h_charge = new TH1D("h_charge","Raw charge",250,0,2500*25);
	TH1D *h_pe = new TH1D("h_pe","pe",400,0,100);
	TChain *T = new TChain("T");
	cout<<"Enter run number "<<endl;
	cin>>runN;
	//cout<<"Enter channel number "<<endl;
	//cin>>channel;
	channel = 2;
	T->Add(Form("~/rootfiles/run_%d.root",runN));
	T->SetBranchAddress(Form("sbs.sbuscint.hadc%d",channel),&charge);
	long entries = T->GetEntries();
	
	for(ij=0;ij<entries;ij++)
	{
		T->GetEntry(ij);
		//cout<<"Event "<<ij<<"  charge  "<<charge<<endl;
		h_raw->Fill(charge);

		h_charge->Fill(charge*25);
		double dumycharge = charge*25*1e-15;//in C 
		npe = dumycharge/(gain*q);
		h_pe->Fill(npe);
		//cout<<"Event "<<ij<<"  pe  "<<npe<<endl;
	}

	TCanvas *plot1 = new TCanvas("CPlot","Plot raw adc",800,600);
	h_raw->GetXaxis()->SetTitle("ADC");
	h_raw->GetYaxis()->SetTitle("Counts");
	h_raw->Draw();

	TCanvas *plot2 = new TCanvas("CPlot1","Plot raw charge",800,600);
	h_charge->GetXaxis()->SetTitle("ADC");
	h_charge->GetYaxis()->SetTitle("Counts");
	h_charge->Draw(); 

	TF1 *langaus = new TF1("langaus",langaufun,27,37,4);
        langaus->SetParNames("lwidth","mpv","integral","gsigma");

        //mean=h_charge->GetMean();
        mean = 32;
        area=h_pe->Integral(28,35);
        //sigma=h_charge->GetRMS();
        sigma = 2.0;
        fr[0]=0.5*mean;
        fr[1]=2*mean;

        cout<<"mean "<<mean<<"  area  "<<area<<"  sigma  "<<sigma<<endl;

        langaus->SetParameters(2.,mean,area,30);
	
	h_pe->Fit("langaus","R0","",28,35);
        gStyle->SetOptStat(1111);
        gStyle->SetOptFit(111);
        TCanvas *plot4 = new TCanvas("plot4","plot4",800,600);
	h_pe->GetXaxis()->SetTitle("PE");
	h_pe->GetYaxis()->SetTitle("Counts");
        h_pe->Draw();
        langaus->Draw("same");
}

Double_t langaufun(Double_t *x, Double_t *par)
{

	//par[0]=width of Landau density
	//par[1]=most probable parameter of Landau density
	//par[2]=Total area
	//par[3]=width of convoluted gaussian function 
	Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
	Double_t mpshift  = -0.22278298;       // Landau maximum location
  	Double_t np = 100.0;      // number of convolution steps
  	Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas	  
	Double_t xx;
	Double_t mpc;
        Double_t fland;
        Double_t sum = 0.0;
        Double_t xlow,xupp;
        Double_t step;
        Double_t i;
	//MP shift correction
	mpc = par[1] - mpshift*par[0];

	//Range of convolution interal
	xlow = x[0] -sc*par[3];
	xupp = x[0] +sc*par[3];

	step = (xupp - xlow)/np;

	//Convolution integral of Landau and Gaussian by sum
	for(i=1.;i<=np/2.;i++)
	{
		xx = xlow + (i-0.5)*step;
		fland = TMath::Landau(xx,mpc,par[0])/par[0];
		sum +=fland* TMath::Gaus(x[0],xx,par[3]);
		xx = xupp - (i-0.5)*step;
		fland = TMath::Landau(xx,mpc,par[0])/par[0];
		sum +=fland* TMath::Gaus(x[0],xx,par[3]);
	}
	return (par[2]*step*sum*invsq2pi/par[3]);
}




