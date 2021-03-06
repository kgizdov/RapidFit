#include "Bs2PhiKK.h"
#include "DPHelpers.hh"

Bs2PhiKK::Bs2PhiKK(PDFConfigurator* config)
{
	// Dependent variable names
	int i(0);
	for(const auto name: {"mKK", "phi", "ctheta_1", "ctheta_2"})
	{
		ObservableNames[i] = config->getName(name);
		i++;
	}
}
Bs2PhiKK::Bs2PhiKK(const Bs2PhiKK& copy)
	// Dependent variable names
	: ObservableNames(copy.ObservableNames)
{
}
void Bs2PhiKK::PhysPar::Update(const ParameterSet* pars)
{
	value = pars->GetPhysicsParameter(name)->GetValue();
	if(std::isnan(value))
		std::cerr << name.Name() << " has been given a nan value!" << std::endl;
}
void Bs2PhiKK::MakePrototypeDataPoint(std::vector<std::string>& allObservables)
{
	// The ordering here matters. It has to be the same as the XML file, apparently.
	for(auto name: ObservableNames)
		allObservables.push_back(name.Name());
}
Bs2PhiKK::datapoint_t Bs2PhiKK::ReadDataPoint(DataPoint* measurement) const
{
	// Get values from the datapoint
	datapoint_t dp = {0,0,0,0};
	int i(0);
	for(const auto& name: ObservableNames)
	{
		dp[i] = measurement->GetObservable(name)->GetValue();
		i++;
	}
	dp[1]+=M_PI;
	return dp;
}

bool Bs2PhiKK::IsPhysicalDataPoint(const Bs2PhiKK::datapoint_t& datapoint)
{
	return datapoint[0] > 2*Bs2PhiKK::mK && std::abs(datapoint[2]) <= 1 && std::abs(datapoint[3]) <= 1;
}

std::vector<std::string> Bs2PhiKK::LineShapeParameterNames(std::string name, std::string lineshape)
{
	// Breit Wigner
	if(lineshape=="BW")
	{
		return {name+"_mass", name+"_width", "KKBFradius"};
	}
	// Flatte
	else if(lineshape=="FT")
	{
		return {name+"_mass", name+"_gpipi", name+"_Rg"};
	}
	// Assume it's non-resonant
	else
	{
		return {};
	}
}
void Bs2PhiKK::UpdateLineshape(const std::string& lineshape, DPMassShape& KKLineShape, const std::vector<PhysPar>& KKpars)
{
	// Update the resonance line shape
	vector<double> respars;
	if(lineshape == "BW")
	{
		respars.push_back(KKpars[0].value); // mass
		respars.push_back(KKpars[1].value); // width
		respars.push_back(KKpars[2].value); // barrier factor radius
	}
	else if(lineshape == "FT")
	{
		respars.push_back(KKpars[0].value); // mass
		respars.push_back(KKpars[1].value); // gpipi
		respars.push_back(KKpars[1].value*KKpars[2].value); // gKK = gpipi*Rg
	}
	KKLineShape.setParameters(respars);
}

