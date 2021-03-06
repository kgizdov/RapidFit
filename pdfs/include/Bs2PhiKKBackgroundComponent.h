#ifndef Bs2PhiKKBackgroundComponent_H
#define Bs2PhiKKBackgroundComponent_H
// Std
#include <vector>
#include <utility>
#include <memory>
// RapidFit
#include "Bs2PhiKK.h"
#include "LegendreMomentShape.h"
// common
#include "NDHist_Fixed.h"

class Bs2PhiKKBackgroundComponent
{
	public:
		Bs2PhiKKBackgroundComponent() {}
		Bs2PhiKKBackgroundComponent(PDFConfigurator*, std::string, std::string); // config, name, type
		Bs2PhiKKBackgroundComponent(const Bs2PhiKKBackgroundComponent&);
		~Bs2PhiKKBackgroundComponent() {}
		Bs2PhiKKBackgroundComponent& operator=(const Bs2PhiKKBackgroundComponent&);
		void SetPhysicsParameters(ParameterSet* pars);
		std::vector<ObservableRef> GetPhysicsParameters() const;
		double Evaluate(const Bs2PhiKK::datapoint_t&) const; // {KK_M, Phi_angle, cos_theta1, cos_theta2}
	private:
		// Floatable parameters
		Bs2PhiKK::PhysPar fraction; // Unnormalised variable to control the relative contribution of each component. Do not use as the fit fraction!!
		// Shape parameters for the mass-dependent part
		std::string type;
		std::vector<Bs2PhiKK::PhysPar> KKpars;
		// Resonance lineshape function if the mass-dependent part is resonant
		std::unique_ptr<DPMassShape> KKLineShape {};
		std::string lineshape;
		int JKK;
		// Histogram object if the mass-dependent part is a histogram shape
		NDHist_Fixed mKKhist; // This has to be a pointer in order to keep Evaluate() const. Both TH1::FindBin and TH1::Interpolate modify the object. Why ROOT, why?
		// LMS object for the angular part
		LegendreMomentShape angulardistribution;
		// Helper functions
		void Initialise();
};
#endif

