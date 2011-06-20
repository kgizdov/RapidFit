/**
        @class FitResult

        Container for all results from a minimisation

        @author Benjamin M Wynne bwynne@cern.ch
	@date 2009-10-02
*/

//	RapidFit Headers
#include "FitResult.h"

//Default constructor
FitResult::FitResult() : minimumValue(-9999), fittedParameters(NULL), covarianceMatrix(), contours(), fitStatus(-9999), fittedBottle(NULL)
{
}

//Constructor with correct arguments
FitResult::FitResult( double MinimumValue, ResultParameterSet * FittedParameters, int FitStatus, PhysicsBottle* FittedBottle ) : minimumValue( MinimumValue ), fittedParameters( FittedParameters ), covarianceMatrix(), contours(), fitStatus( FitStatus ), fittedBottle( FittedBottle )
{
}

//Constructor with correct arguments, including covariance Matrix
FitResult::FitResult( double MinimumValue, ResultParameterSet * FittedParameters, int FitStatus, PhysicsBottle* FittedBottle, vector<double> covMatrix ) : minimumValue( MinimumValue ), fittedParameters( FittedParameters ), covarianceMatrix( covMatrix ), contours(), fitStatus( FitStatus ), fittedBottle( FittedBottle )
{
}

//Constructor with correct arguments, including covariance Matrix and contours
FitResult::FitResult( double MinimumValue, ResultParameterSet * FittedParameters, int FitStatus, PhysicsBottle* FittedBottle, vector<double> covMatrix, vector< FunctionContour* > ContourPlots ) : minimumValue( MinimumValue ), fittedParameters( FittedParameters ), covarianceMatrix( covMatrix ), contours( ContourPlots ), fitStatus( FitStatus ), fittedBottle( FittedBottle )
{
}

FitResult::FitResult( double MinimumValue, ResultParameterSet* FittedParameters, int FitStatus ) : minimumValue( MinimumValue ), fittedParameters( FittedParameters ), covarianceMatrix(), contours(), fitStatus( FitStatus ), fittedBottle(NULL)
{
}

//Destructor
FitResult::~FitResult()
{
	if( fittedBottle != NULL )
		delete fittedBottle;
	if( fittedParameters != NULL )
		delete fittedParameters;
}

vector< FunctionContour* > FitResult::GetContours()
{
	return contours;
}

vector<double> FitResult::GetCovarianceMatrix()
{
	return covarianceMatrix;
}

double FitResult::GetMinimumValue()
{
	return minimumValue;
}

ResultParameterSet * FitResult::GetResultParameterSet()
{
	return fittedParameters;
}

int FitResult::GetFitStatus()
{
	return fitStatus;
}

void FitResult::ForceFitStatus( int input_status )
{
	fitStatus = input_status;
}

PhysicsBottle* FitResult::GetPhysicsBottle()
{
	return fittedBottle;
}
