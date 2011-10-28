/**
  @class PDFWithData

  A class for creating/storing a PDF and its associated data set

  @author Benjamin M Wynne bwynne@cern.ch
  @date 2009-10-5
 */

//	RapidFit Headers
#include "PDFWithData.h"
#include "ClassLookUp.h"
//	System Headers
#include <stdlib.h>
#include <iostream>

using namespace std;

//Default constructor
PDFWithData::PDFWithData() : fitPDF(NULL), inputBoundary(), parametersAreSet(false), dataProcessors(), dataSetMakers(), cached_data(), delete_data_decision(true)
{
}

//Constructor with correct aruments
PDFWithData::PDFWithData( IPDF * InputPDF, PhaseSpaceBoundary * InputBoundary, vector< DataSetConfiguration* > DataConfig, vector< IPrecalculator* > InputPrecalculators ) : fitPDF(ClassLookUp::CopyPDF(InputPDF)), inputBoundary(InputBoundary),  parametersAreSet(false), dataProcessors(InputPrecalculators), dataSetMakers(DataConfig), cached_data(), delete_data_decision(true)
{
	if ( DataConfig.size() < 1 )
	{
		cerr << "No data sets configured" << endl;
		exit(1);
	}
}

//Destructor
PDFWithData::~PDFWithData()
{
	if( fitPDF != NULL ) delete fitPDF;
	while( !dataSetMakers.empty() )
	{
		if( dataSetMakers.back() != NULL ) delete dataSetMakers.back();
		dataSetMakers.pop_back();
	}
	//cout << "Hello from PDFWithData destructor" << endl;
}

//Return the PDF
IPDF * PDFWithData::GetPDF()
{
	if (!parametersAreSet)
	{
		cerr << "Warning: PDF parameters have not yet been set" << endl;
	}
	//return ClassLookUp::CopyPDF(fitPDF);
	return fitPDF;
}

void PDFWithData::AddCachedData( vector<IDataSet*> input_cache )
{
	for( unsigned short int element=0; element < input_cache.size(); ++element )
	{
		cached_data.push_back( input_cache[element] );
	}
}

DataSetConfiguration* PDFWithData::GetDataSetConfig()
{
	return dataSetMakers[0];
}

vector<DataSetConfiguration*> PDFWithData::GetAllDataSetConfigs()
{
	return dataSetMakers;
}

void PDFWithData::SetDelete( bool new_decision )
{
	delete_data_decision = new_decision;
}

//Return the data set associated with the PDF
IDataSet * PDFWithData::GetDataSet()
{
	//Combine all data sources
	IDataSet * newDataSet=NULL;
	
	if( cached_data.empty() )
	{
		newDataSet = dataSetMakers[0]->MakeDataSet( inputBoundary, fitPDF );
		for (unsigned int sourceIndex = 1; sourceIndex < dataSetMakers.size(); ++sourceIndex )
		{
			IDataSet * extraData = dataSetMakers[sourceIndex]->MakeDataSet( inputBoundary, fitPDF );
			for (int dataIndex = 0; dataIndex < extraData->GetDataNumber(); ++dataIndex )
			{
				newDataSet->AddDataPoint( extraData->GetDataPoint(dataIndex) );
			}
			delete extraData;
		}
	} else {
		newDataSet = cached_data.back();
		//	Pop back?	Yes?	No?
		//	I can't decide what the correct behaviour should be for 'cached data' I suppose it should still be kept
		//	However, I am passing by pointer and cached data can be deleted by the calling function which makes any
		//	future attempts to fit to this DANGEROUS so I remove any reference to it once I've fitted
		//	This is contrary to the intent I think for RapidFit, but I wish to use this for FC and so it's
		//	MUCH easier to re-use existing objects as much as possible.
		if( delete_data_decision )
		{
			cached_data.pop_back();
		}
	}

	//Precalculation, if required
	for (unsigned int precalculatorIndex = 0; precalculatorIndex < dataProcessors.size(); ++precalculatorIndex )
	{
		IDataSet * oldDataSet = newDataSet;
		newDataSet = dataProcessors[precalculatorIndex]->ProcessDataSet(oldDataSet);
		delete oldDataSet;
	}

	cout << "DataSet contains " << newDataSet->GetDataNumber() << " events\n" << endl;
	return newDataSet;
}

//Set the physics parameters of the PDF
bool PDFWithData::SetPhysicsParameters( vector<ParameterSet*> NewParameters )
{
	//	I am in the process of adding more flexibility to the RapidFit structure
	//	As such this requires that the Paramaters from the XML be passed in vectors
	//	I will update this function in time to stop refering to just the first element it sees
	
	//Set the parameters for the stored PDF and all data set makers
	bool success = fitPDF->SetPhysicsParameters(NewParameters.back());
	for (unsigned int dataIndex = 0; dataIndex < dataSetMakers.size(); ++dataIndex )
	{
		success &= dataSetMakers[dataIndex]->SetPhysicsParameters(NewParameters);
	}

	if (success)
	{
		parametersAreSet = true;
		return true;
	}
	else
	{
		cerr << "Failed to set PDF parameters in initialisation" << endl;
		exit(1);
	}
}
