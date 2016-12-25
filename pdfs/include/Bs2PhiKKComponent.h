#ifndef __BS2PHIKKCOMPONENT_H__
#define __BS2PHIKKCOMPONENT_H__
// Std
#include <complex>
#include <string>
#include <vector>
#include <memory>
// RapidFit
#include "PDFConfigurator.h"
#include "ParameterSet.h"
#include "DPMassShape.hh"
#include "DPBarrierFactor.hh"
#include "DPWignerFunctionGeneral.hh"

// Simplify the case where a value and a name correspond 1:1
struct PhysPar
{
  // Construct this however you want
  PhysPar() {}
  PhysPar(ObservableRef _name) : name(_name), value(0) {}
  PhysPar(ObservableRef _name, double _value) : name(_name), value(_value) {}
  PhysPar(PDFConfigurator* config, std::string _name) : name(config->getName(_name)), value(0) {}
  PhysPar(PDFConfigurator* config, std::string _name, double _value) : name(config->getName(_name)), value(_value) {}
  PhysPar(const PhysPar& other) : value(other.value), name(other.name) {}
  void Update(ParameterSet& pars) { value = pars.GetPhysicsParameter(name)->GetValue(); }
  void Update(ParameterSet* pars) { value = pars->GetPhysicsParameter(name)->GetValue(); }
  double value;
  ObservableRef name;
};
class Bs2PhiKKComponent
{
  public:
    Bs2PhiKKComponent(PDFConfigurator*, std::string, std::string, int, std::string); // config, phi name, resonance name, spin
    Bs2PhiKKComponent(const Bs2PhiKKComponent&);
    ~Bs2PhiKKComponent();
    std::string GetName() {return KKname;}
    void SetPhysicsParameters(ParameterSet* pars); // Update all the parameters
    std::vector<ObservableRef> GetPhysicsParameters();
    std::complex<double> Amplitude(double, double, double, double); // KK_M, Phi_angle, cos_theta1, cos_theta2
    std::complex<double> Amplitude(double, double, double, double, std::string);
    static double mBs;
    static double mK;
    static double mpi;
  protected:
    // Floatable parameters
    PhysPar fraction; // Unnormalised variable to control the relative contribution of each resonance. Do not use at the fit fraction!!
    std::vector<std::complex<double>> Ahel;  // Helicity amplitude(s)
    std::vector<int> helicities; // Store the possible values of helicity to enable looping over A(helicities[i])
    // Polarisation amplitude components (perp, zero, para)
    std::vector<PhysPar> magsqs; // Square of magnitudes: para will be calculated from the other two
    std::vector<PhysPar> phases; // Phases
    // Resonance parameters
    std::vector<PhysPar> KKpars; // Mass and width of Breit Wigner, or mass, g_pipi and R=(g_KK/g_pipi) of Flatte. Empty for non-resonant
    double mphi; // For orbital/barrier factor calculations. Assume the PDF already has this (for p1*p3 calculation)
    // Fixed parameters
    int Jphi; // Spin of the phi (P-wave, 1)
    int JKK; // Spin of the KK resonance (0, 1 or 2)
    double RBs; // Bs barrier factor radius
    double RKK; // KK barrier factor radius
    std::string lineshape; // Choose the resonance shape: "BW", "FT" or "NR"
    std::string phiname; // The name decides which set of PhysicsParameters it will look for in the RapidFit XML
    std::string KKname;
    // Resonance lineshape function for the mass-dependent part
    std::unique_ptr<DPMassShape> KKLineShape {};
  private:
    void Initialise();
    void UpdateParameters();
    std::complex<double> A(int); // Polarisation amplitude coefficients
    std::complex<double> F(int, double, double, double); // Angular distribution: helicity, phi, costheta1, costheta2
    double OFBF(double); // Product of orbital and barrier factors
    // Wigner d-functions for the angular-dependent part
    std::unique_ptr<DPWignerFunction> wignerKK {};
    std::unique_ptr<DPWignerFunction> wignerPhi {};
    // Blatt-Weisskopf barrier penetration factors
    std::unique_ptr<DPBarrierFactor> Bsbarrier {};
    std::unique_ptr<DPBarrierFactor> KKbarrier {};
};
#endif

