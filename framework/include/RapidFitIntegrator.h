/**
	@class RapidFitIntegrator

	A numerical integrator to be used for projecting the PDF, and as a fallback if the PDF does not provide its own normalisation
	This class uses two integrator classes provided by root: AdaptiveIntegratorMultiDim and GaussLegendreIntegrator.
	Both of these assume the function to be integrated is well behaved.

	@author Benjamin M Wynne bwynne@cern.ch
	@date 2009-10-8
*/

#ifndef RAPIDFIT_INTEGRATOR_H
#define RAPIDFIT_INTEGRATOR_H

#include "IntegratorFunction.h"
#include "Math/AdaptiveIntegratorMultiDim.h"
#include "Math/Integrator.h"

using namespace ROOT::Math;

class RapidFitIntegrator
{
	public:
		RapidFitIntegrator();
		RapidFitIntegrator( IPDF* );
		~RapidFitIntegrator();

		double Integral( DataPoint*, PhaseSpaceBoundary* );
		double ProjectObservable( DataPoint*, PhaseSpaceBoundary*, string );

	private:
		double DoNumericalIntegral( DataPoint*, PhaseSpaceBoundary*, vector<string> );

		IPDF * functionToWrap;
		AdaptiveIntegratorMultiDim * multiDimensionIntegrator;
		IntegratorOneDim * oneDimensionIntegrator;
		bool functionCanIntegrate;
		bool functionCanProject;
		bool haveTestedIntegral;
};

#endif
