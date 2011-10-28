/**
  @class FitResultVector

  The result of a toy study.

  @author Benjamin M Wynne bwynne@cern.ch
  @date 2009-10-02
 */

//	RapidFit Headers
#include "FitResultVector.h"
//#include "LLscanResult.h"
//	System Headers
#include <iostream>

//Default constructor
FitResultVector::FitResultVector() : allResults(), allNames(), allValues(), allErrors(), allPulls(), allGenValues(), allRealTimes(), allCPUTimes(), clock()
{
}

//  Constructor to Return a single array from multiple arrays
FitResultVector::FitResultVector( vector<FitResultVector*> Result_Array ) : allResults(), allNames(), allValues(), allErrors(), allPulls(), allGenValues(), allRealTimes(), allCPUTimes(), clock()
{
	if( !Result_Array.empty() )
	{
		clock = new TStopwatch();
		allNames = Result_Array[0]->GetAllNames();
		//Construct the result data structure
		for (unsigned short int nameIndex = 0; nameIndex < allNames.size(); ++nameIndex )
		{
			allValues.push_back( vector<double>() );
			allErrors.push_back( vector<double>() );
			allPulls.push_back( vector<double>() );
			allGenValues.push_back( vector<double>() );
		}
		for(unsigned int i=0; i < Result_Array.size(); ++i )
		{
			for( short int j=0; j < Result_Array[i]->NumberResults(); ++j )
			{
				AddFitResult(  Result_Array[i]->GetFitResult( j ), false );
			}

			vector<double> input_real_times = Result_Array[i]->GetAllRealTimes();
			vector<double> input_cpu_times = Result_Array[i]->GetAllCPUTimes();
			for(unsigned int j2=0; j2 < input_real_times.size(); ++j2)
			{
				allRealTimes.push_back( input_real_times[j2] );
				allCPUTimes.push_back( input_cpu_times[j2] );
			}

		}
	}
}

//Constructor with correct argument
FitResultVector::FitResultVector( vector<string> AllParameterNames ) : allResults(), allNames(AllParameterNames), allValues(), allErrors(), allPulls(), allGenValues(), allRealTimes(), allCPUTimes(), clock()
{
	//Construct the result data structure
	for (unsigned short int nameIndex = 0; nameIndex < allNames.size(); ++nameIndex )
	{
		allValues.push_back( vector<double>() );
		allErrors.push_back( vector<double>() );
		allPulls.push_back( vector<double>() );
		allGenValues.push_back( vector<double>() );
	}

	clock = new TStopwatch();
}

//Destructor
FitResultVector::~FitResultVector()
{
}

//Note the time the study starts
void FitResultVector::StartStopwatch()
{
	clock->Start();
}

//Add a new fit result
bool FitResultVector::AddFitResult( FitResult * NewResult, bool with_clock )
{
	vector<double> newParameterValues, newParameterErrors, newParameterPulls, newParameterGenValues;
	vector<string>::iterator nameIterator;
	ResultParameterSet * newSet = NewResult->GetResultParameterSet();

	//Check all expected parameters are found
	for ( nameIterator = allNames.begin(); nameIterator != allNames.end(); ++nameIterator )
	{
		ResultParameter * newResult = newSet->GetResultParameter( *nameIterator );
		if ( newResult->GetUnit() == "NameNotFoundError" )
		{
			//If any parameter is not found, fail
			cerr << "Expected fitted parameter \"" << *nameIterator << "\" not found" << endl;
			return false;
		}
		else
		{
			//Retrieve the parameter information
			newParameterValues.push_back( newResult->GetValue() );
			newParameterErrors.push_back( newResult->GetError() );
			newParameterPulls.push_back( newResult->GetPull() );
			newParameterGenValues.push_back( newResult->GetOriginalValue() );
		}
	}

	//If you've got this far, all the parameters have been found, so add them to the record
	allResults.push_back(NewResult);
	for (unsigned int nameIndex = 0; nameIndex < allNames.size(); ++nameIndex )
	{
		allValues[nameIndex].push_back( newParameterValues[nameIndex] );
		allErrors[nameIndex].push_back( newParameterErrors[nameIndex] );
		allPulls[nameIndex].push_back( newParameterPulls[nameIndex] );
		allGenValues[nameIndex].push_back( newParameterGenValues[nameIndex] );
	}

	if( with_clock )
	{
		//Store the duration
		clock->Stop();
		allRealTimes.push_back( clock->RealTime() );
		allCPUTimes.push_back( clock->CpuTime() );
	}

	return true;
}


//  Return an array of the MLL values of the fits
vector<double> FitResultVector::GetAllMLL()
{
	vector<double> output_MLL;
	for (unsigned short int i=0; i < allResults.size(); ++i )
	{
		if( allResults[i]->GetFitStatus() >0 ) 
		{
			output_MLL.push_back( allResults[i]->GetMinimumValue() );
		}
		else{
			output_MLL.push_back( LLSCAN_FIT_FAILURE_VALUE );
		}
	}
	return output_MLL;
}

//Return vectors of values, errors and pulls for a particular parameter name
vector<double> FitResultVector::GetParameterValues( string ParameterName )
{
	for (unsigned int nameIndex = 0; nameIndex < allNames.size(); ++nameIndex )
	{
		//If you find the parameter name, return the vector of values
		if ( ParameterName == allNames[nameIndex] )
		{
			return allValues[nameIndex];
		}
	}

	//If you get this far, the parameter name was not found
	cerr << "Result parameter name \"" << ParameterName << "\" not found" << endl;
	return vector<double>();
}
vector<double> FitResultVector::GetParameterErrors( string ParameterName )
{
	for (unsigned int nameIndex = 0; nameIndex < allNames.size(); ++nameIndex )
	{
		//If you find the parameter name, return the vector of errors
		if ( ParameterName == allNames[nameIndex] )
		{
			return allErrors[nameIndex];
		}
	}

	//If you get this far, the parameter name was not found
	cerr << "Result parameter name \"" << ParameterName << "\" not found" << endl;
	return vector<double>();
}
vector<double> FitResultVector::GetParameterPulls( string ParameterName )
{
	for (unsigned int nameIndex = 0; nameIndex < allNames.size(); ++nameIndex )
	{
		//If you find the parameter name, return the vector of pulls
		if ( ParameterName == allNames[nameIndex] )
		{
			return allPulls[nameIndex];
		}
	}

	//If you get this far, the parameter name was not found
	cerr << "Result parameter name \"" << ParameterName << "\" not found" << endl;
	return vector<double>();
}

//Allow access to the vector of results
int FitResultVector::NumberResults()
{
	return int(allResults.size());
}
FitResult * FitResultVector::GetFitResult( int Index )
{
	if ( Index < int(allResults.size()) )
	{
		return allResults[unsigned(Index)];
	}
	else
	{
		cerr << "Index (" << Index << ") out of range" << endl;
		return new FitResult();
	}
}
double FitResultVector::GetRealTime( int Index )
{
	if( Index < int(allRealTimes.size()) ) return allRealTimes[ unsigned(Index) ];
	else return -1;
}
void FitResultVector::AddRealTimes( vector<double> input_times )
{
	for( unsigned int i=0; i< input_times.size(); ++i)
	{
		allRealTimes.push_back( input_times[i] );
	}
}
void FitResultVector::AddRealTime( double input_time )
{
	allRealTimes.push_back( input_time );
}
void FitResultVector::SetRealTime( int Index, double input_time )
{
	if( int(allRealTimes.size()) < Index ) allRealTimes.resize( unsigned(Index) );
	allRealTimes[unsigned(Index)] = input_time;
}
double FitResultVector::GetCPUTime( int Index )
{
	if( Index < int(allCPUTimes.size()) ) return allCPUTimes[ unsigned(Index) ];
	else return -1;
}
void FitResultVector::AddCPUTimes( vector<double> input_times )
{
	for( unsigned int i=0; i< input_times.size(); ++i)
	{
		allCPUTimes.push_back( input_times[i] );
	}
}
void FitResultVector::AddCPUTime( double input_time )
{
	allCPUTimes.push_back( input_time );
}
void FitResultVector::SetCPUTime( int Index, double input_time )
{
	if( Index < int(allCPUTimes.size()) ) allCPUTimes.resize( unsigned(Index) );
	allCPUTimes[unsigned(Index)] = input_time;  
}

//Return names of all variables
vector<string> FitResultVector::GetAllNames()
{
	return allNames;
}

//Return the time taken for each fit
vector<double> FitResultVector::GetAllRealTimes()
{
	return allRealTimes;
}
vector<double> FitResultVector::GetAllCPUTimes()
{
	return allCPUTimes;
}


vector<double> FitResultVector::GetFlatResult( int Index )
{
	vector<double> Flatresult;
	for(unsigned int i = 0; i<allNames.size(); ++i)
	{
		Flatresult.push_back( (allValues[i][unsigned(Index)]) );
		Flatresult.push_back( (allErrors[i][unsigned(Index)]) );
		Flatresult.push_back( (allPulls[i][unsigned(Index)]) );
		Flatresult.push_back( (allResults[(unsigned)Index]->GetResultParameterSet()->GetResultParameter(allNames[i])->GetMinimum() ) );
		Flatresult.push_back( (allResults[(unsigned)Index]->GetResultParameterSet()->GetResultParameter(allNames[i])->GetMaximum() ) );
		Flatresult.push_back( (allResults[(unsigned)Index]->GetResultParameterSet()->GetResultParameter(allNames[i])->GetStepSize() ) );
		Flatresult.push_back( (allGenValues[i][unsigned(Index)]) );
	}

	Flatresult.push_back(allRealTimes[unsigned(Index)]);
	Flatresult.push_back(allCPUTimes[unsigned(Index)]);
	Flatresult.push_back(allResults[unsigned(Index)]->GetFitStatus());
	Flatresult.push_back(allResults[unsigned(Index)]->GetMinimumValue());
	return Flatresult;
}

TString FitResultVector::GetFlatResultHeader()
{
	TString header = "";
	for(unsigned short int i = 0; i<allNames.size(); ++i)
	{
		TString name = allNames[i];
		header += name + "_value:";
		header += name + "_error:";
		header += name + "_pull:";
		header += name + "_min:";
		header += name + "_max:";
		header += name + "_step:";
		header += name + "_gen:";
	}
	header += "Fit_RealTime:Fit_CPUTime:Fit_Status:NLL";
	return header;
}

