/**
  @class XMLConfigReader

  Opens an xml config file and uses it to create RapidFit data objects

  @author Benjamin M Wynne bwynne@cern.ch
  @date 2009-10-02
 */

//	RapidFit Headers
#include "XMLConfigReader.h"
#include "ClassLookUp.h"
#include "SumPDF.h"
#include "NormalisedSumPDF.h"
#include "ProdPDF.h"
#include "StringProcessing.h"
#include "AcceptReject.h"
#include "ObservableContinuousConstraint.h"
#include "ObservableDiscreteConstraint.h"
#include "Blinder.h"
#include "ScanParam.h"
#include "PDFConfigurator.h"
//	System Headers
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <float.h>

//#define DOUBLE_TOLERANCE DBL_MIN
#define DOUBLE_TOLERANCE 1E-6

//Default constructor
XMLConfigReader::XMLConfigReader() : All_XML_Tags(new XMLTag()), children(), isLoaded(false), seed(0), PDF_index(0), ParamSet_index(0)
{
}

//Constructor with file name argument
XMLConfigReader::XMLConfigReader( string FileName, vector<pair<string, string> >* OverrideXML ) : All_XML_Tags(new XMLTag(OverrideXML)), children(), isLoaded(false), seed(0), PDF_index(0), ParamSet_index(0)
{
	//Open the config file
	ifstream configFile( FileName.c_str() );
	if ( !configFile.is_open() )
	{
		cerr << "Failed to open config file \"" << FileName << "\"" << endl;
		exit(1);
	}

	//Read the whole file into a vector
	vector<string> wholeFile;
	while ( configFile.good() )
	{
		string newLine;
		getline( configFile, newLine );
		wholeFile.push_back( newLine );
	}
	StringProcessing::RemoveWhiteSpace(wholeFile);


//	THIS CAUSES MORE HEADACHES THAN IT FIXES WE WOULD LIKE TO MOVE TO USING A PROPER XML PARSER IN THE FUTURE ANYWAY!

	//Check the root tag is correct
//	if ( wholeFile[0] == "<RapidFit>" && wholeFile[ wholeFile.size() - 1 ] == "</RapidFit>" )
//	{
		//Parse the first level tags
//		vector<string> middleOfFile;
//	for ( unsigned int lineIndex = 1; lineIndex < wholeFile.size() -1; ++lineIndex )
//	{
//		middleOfFile.push_back( wholeFile[lineIndex] );
//	}
	vector<string> value;
	vector<XMLTag*> File_Tags = All_XML_Tags->FindTagsInContent( wholeFile, value );
	children = File_Tags[0]->GetChildren();

//		//Anything in "value" is considered debug data
//		for ( unsigned int valueIndex = 0; valueIndex < value.size(); ++valueIndex )
//		{
//			cout << value[valueIndex] << endl;
//		}
//	}
//	else
//	{
//		cerr << "Config file in wrong format: root tag is \"" << wholeFile[0] << "\" not \"<RapidFit>\"" << endl;
//		exit(1);
//	}

	if( ( File_Tags.size() != 1) || ( children.size() == 0 ) )
	{
		cerr << "Error processing XMLFile" << endl;
		exit( -234 );
	}

	isLoaded = true;
}

//Destructor
XMLConfigReader::~XMLConfigReader()
{
	delete All_XML_Tags;
	//cout << "Hello from XMLConfigReader destructor" << endl;
}

//Return whether file is loaded
bool XMLConfigReader::IsLoaded()
{
	return isLoaded;
}

//Return the parameter set
//ParameterSet * XMLConfigReader::GetFitParameters()
//{
//	//Find the ParameterSet tag
//	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
//	{
//		if ( children[childIndex]->GetName() == "ParameterSet" )
//		{
//			return GetParameterSet( children[childIndex] );
//		}
//	}

//	//If no such tag is found, fail
//	cerr << "ParameterSet tag not found in config file" << endl;
//	exit(1);
//}

vector<ParameterSet*> XMLConfigReader::GetFitParameters( vector<string> CommandLineParam )
{
	vector<ParameterSet*> RawParameters = XMLConfigReader::GetFitParameters();

	for( unsigned int i=0; i < CommandLineParam.size() ; ++i )
	{
		vector<string> input_args = StringProcessing::SplitString( CommandLineParam[i], ',' );
		if( input_args.size() != 6 )
		{
			cerr << "Cannot understand Physics Parameter info you passed at runtime" << endl;
			cerr << "They should be defined as:\t\tgamma,value,min,max,stepsize,type" << endl;
			exit(-598);
		}
		bool found_parameter=false;
		for( unsigned int j=0; j < RawParameters.size(); ++j )
		{
			bool local_find = false;
			PhysicsParameter* try_get_param=NULL;
			try{
				try_get_param = RawParameters[j]->GetPhysicsParameter( input_args[0] );
				local_find = true;
			}
			catch( int e )
			{
				local_find = false;
			}

			if( local_find )
			{
				string Unit = try_get_param->GetUnit();

				//		min			-		max			< 	1E-5

				double step = strtod(input_args[4].c_str(),NULL);
				double max = strtod(input_args[3].c_str(),NULL);
				double min = strtod(input_args[2].c_str(),NULL);
				double val = strtod(input_args[1].c_str(),NULL);
				string type = input_args[5];

				if( ( ( fabs( min - max ) < 	1E-6 ) && ( max < 1E-6 ) ) || (type=="Fixed") )
				{
					//		Unbounded				name	value	step	type	unit
					PhysicsParameter* new_param = new PhysicsParameter( input_args[0], val , step, input_args[4], Unit);
					RawParameters[j]->SetPhysicsParameter( input_args[0], new_param );
				}
				else
				{
					//					name	val	min	max	step	type	unit
					RawParameters[j]->SetPhysicsParameter( input_args[0], val, min, max, step, input_args[4], Unit );
				}
				found_parameter = local_find;
			}
		}
		if( !found_parameter )
		{
			cerr << "Couldn't find parameter: " << input_args[0] << " exiting!" << endl;
			exit(-345);
		}
	}

	return RawParameters;
}

vector<ParameterSet*> XMLConfigReader::GetFitParameters()
{
	vector<ParameterSet*> All_Parameters;
	//Find the ParameterSet tag
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "ParameterSet" )
		{
			All_Parameters.push_back( GetParameterSet( children[childIndex] ) );
		}
	}

	if( !All_Parameters.empty() )	return All_Parameters;
	//If no such tag is found, fail
	cerr << "ParameterSet tag not found in config file" << endl;
	exit(1);
}

//Return the minimiser for the fit
MinimiserConfiguration * XMLConfigReader::GetMinimiserConfiguration()
{
	//Find the Minimiser tag
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "Minimiser" )
		{
			return MakeMinimiser( children[childIndex] );
		}
	}

	//If no such tag is found, fail
	cerr << "Minimiser tag not found in config file" << endl;
	exit(1);
}

//Make a minimiser configuration object
MinimiserConfiguration * XMLConfigReader::MakeMinimiser( XMLTag * MinimiserTag )
{
	if ( MinimiserTag->GetName() == "Minimiser" )
	{
		//Examine all minimiser components
		string minimiserName = "Uninitialised";
		vector< XMLTag* > minimiserComponents = MinimiserTag->GetChildren();
		//vector<string> valueLines = MinimiserTag->GetValue();
		int MAXIMUM_MINIMISATION_STEPS = 1000000.0;
		double FINAL_GRADIENT_TOLERANCE = 0.001;
		MinimiserConfiguration* returnableConfig = NULL;
		vector<string> minimiserOptions;
		int Quality = 1;
		bool MultiMini=false;
		if ( minimiserComponents.size() == 0 )
		{
			minimiserName = MinimiserTag->GetValue()[0];
		}
		else
		{
                        //New style - can have weights
                        for ( unsigned int childIndex = 0; childIndex < minimiserComponents.size(); ++childIndex )
                        {
                                if ( minimiserComponents[childIndex]->GetName() == "MinimiserName" )
                                {
					minimiserName = minimiserComponents[childIndex]->GetValue()[0];
				}
				else if( minimiserComponents[childIndex]->GetName() == "MaxSteps" )
				{
					MAXIMUM_MINIMISATION_STEPS = atoi( minimiserComponents[childIndex]->GetValue()[0].c_str() );
				}
				else if( minimiserComponents[childIndex]->GetName() == "GradTolerance" )
				{
					FINAL_GRADIENT_TOLERANCE = strtod( minimiserComponents[childIndex]->GetValue()[0].c_str(), NULL );
				}
				else if( minimiserComponents[childIndex]->GetName() == "ConfigureMinimiser" )
				{
					minimiserOptions.push_back( minimiserComponents[childIndex]->GetValue()[0] );
				}
				else if( minimiserComponents[childIndex]->GetName() == "Quality" )
				{
					Quality = atoi( minimiserComponents[childIndex]->GetValue()[0].c_str() );
				}
				else if( minimiserComponents[childIndex]->GetName() == "MultiMini" )
				{
					if( minimiserComponents[childIndex]->GetValue()[0] == "true" )
					{
						MultiMini = true;
					}
				}
				else
				{
					cerr << "Minimiser not properly configured" << endl;
					exit(9234);
				}
			}
		}
		returnableConfig = new MinimiserConfiguration( minimiserName, GetOutputConfiguration() );
		returnableConfig->SetSteps( MAXIMUM_MINIMISATION_STEPS );
		returnableConfig->SetTolerance( FINAL_GRADIENT_TOLERANCE );
		returnableConfig->SetOptions( minimiserOptions );
		returnableConfig->SetQuality( Quality );
		returnableConfig->SetMultiMini( MultiMini );
		return returnableConfig;
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << MinimiserTag->GetName() << "\" not \"Minimiser\"" << endl;
		exit(1);
	}
}

//Return the output configuration for the fit
OutputConfiguration * XMLConfigReader::GetOutputConfiguration()
{
	//Find the Output tag
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "Output" )
		{
			return MakeOutputConfiguration( children[childIndex] );
		}
	}

	//If no such tag is found, make default
	cout << "Output tag not found in config file - using default" << endl;
	return new OutputConfiguration();
}

//Make an output configuration object
OutputConfiguration * XMLConfigReader::MakeOutputConfiguration( XMLTag * OutputTag )
{
	if ( OutputTag->GetName() == "Output" )
	{
		vector< pair< string, string > > contourPlots;
		vector<string> projections;
//		vector<string> LLscanList;
		string pullType = "None";
		vector<ScanParam*> ScanParameters;
		vector<pair<ScanParam*, ScanParam*> > _2DScanParameters;

		vector< XMLTag* > outputComponents = OutputTag->GetChildren();
		for ( unsigned int childIndex = 0; childIndex < outputComponents.size(); ++childIndex )
		{
			if ( outputComponents[childIndex]->GetName() == "ContourPlot" )
			{
				contourPlots.push_back( MakeContourPlot( outputComponents[childIndex] ) );
			}
			else if ( outputComponents[childIndex]->GetName() == "Projection" )
			{
				projections.push_back( outputComponents[childIndex]->GetValue()[0] );
			}
			else if ( outputComponents[childIndex]->GetName() == "DoPullPlots" )
			{
				pullType = outputComponents[childIndex]->GetValue()[0];
			}
			else if ( outputComponents[childIndex]->GetName() == "Scan" )
			{
				ScanParam* temp_SParam = GetScanParam( outputComponents[childIndex] );
				ScanParameters.push_back( temp_SParam );
			}
			else if ( outputComponents[childIndex]->GetName() == "TwoDScan" )
			{
				pair<ScanParam*, ScanParam*> temp_2DScan = Get2DScanParam( outputComponents[childIndex] );
				_2DScanParameters.push_back( temp_2DScan );
			}
			else if ( outputComponents[childIndex]->GetName() == "2DScan" )
			{
				cerr << "PLEASE MOVE YOUR XML TO THE NEW SYNTAX \'<TwoDScan>\' TO BE STANDARDS COMPLIANT!" << endl;
				pair<ScanParam*, ScanParam*> temp_2DScan = Get2DScanParam( outputComponents[childIndex] );
				_2DScanParameters.push_back( temp_2DScan );
			}
			else
			{
				cerr << "Unrecognised output component: " << outputComponents[childIndex]->GetName() << endl;
				exit(1);
			}
		}

		return new OutputConfiguration( contourPlots, projections, pullType, ScanParameters, _2DScanParameters );
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << OutputTag->GetName() << "\" not \"Output\"" << endl;
		exit(1);
	}
}

//Return the pair of observables to plot the function contours for
pair< string, string > XMLConfigReader::MakeContourPlot( XMLTag * PlotTag )
{
	if ( PlotTag->GetName() == "ContourPlot" )
	{
		vector< XMLTag* > plotComponents = PlotTag->GetChildren();
		if ( plotComponents.size() == 2 )
		{
			//Retrieve the names of the parameters to plot
			string xName, yName;
			bool hasX = false;
			bool hasY = false;
			for ( unsigned int childIndex = 0; childIndex < plotComponents.size(); ++childIndex )
			{
				if ( plotComponents[childIndex]->GetName() == "XParameter" )
				{
					xName = plotComponents[childIndex]->GetValue()[0];
					hasX = true;
				}
				else if ( plotComponents[childIndex]->GetName() == "YParameter" )
				{
					yName = plotComponents[childIndex]->GetValue()[0];
					hasY = true;
				}
				else
				{
					cerr << "Unrecognised ContourPlot component: " << plotComponents[childIndex]->GetName() << endl;
					exit(1);
				}
			}

			//Check both parameters are specified
			if ( hasX && hasY )
			{
				return make_pair( xName, yName );
			}
			else
			{
				cerr << "ContourPlot tag is missing parameter name for ";
				if ( !hasX )
				{
					cerr << "x";
				}
				else
				{
					cerr << "y";
				}
				cerr << " axis" << endl;
				exit(1);
			}
		}
		else
		{
			cerr << "ContourPlot tag should only contain the two parameters to plot contours for" << endl;
			exit(1);
		}
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << PlotTag << "\" not \"ContourPlot\"" << endl;
		exit(1);
	}
}

//Return the number of repeats for the fit
int XMLConfigReader::GetNumberRepeats()
{
	//Find the NumberRepeats tag
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "NumberRepeats" )
		{
			return atoi( children[childIndex]->GetValue()[0].c_str() );
		}
	}

	//If no such tag is found, fail
	cerr << "NumberRepeats tag not found in config file" << endl;
	exit(1);
}

//Return the function to minimise
FitFunctionConfiguration * XMLConfigReader::GetFitFunctionConfiguration()
{
	//Find the FitFunction tag
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "FitFunction" )
		{
			return MakeFitFunction( children[childIndex] );
		}
	}

	//If no such tag is found, fail
	cerr << "FitFunction tag not found in config file" << endl;
	exit(1);
}

//Make FitFunction configuration object
FitFunctionConfiguration * XMLConfigReader::MakeFitFunction( XMLTag * FunctionTag )
{
	if ( FunctionTag->GetName() == "FitFunction" )
	{
		string functionName = "Uninitialised";
		string weightName = "Uninitialised";
		bool hasWeight = false;
		bool want_Trace = false;
		bool change_style = false;
		string Trace_FileName;
		string Strategy;
		vector< XMLTag* > functionInfo = FunctionTag->GetChildren();
		if ( functionInfo.size() == 0 )
		{
			//Old style - just specifies the function name
			functionName = FunctionTag->GetValue()[0];
		}
		else
		{
			//New style - can have weights
			for ( unsigned int childIndex = 0; childIndex < functionInfo.size(); ++childIndex )
			{
				if ( functionInfo[childIndex]->GetName() == "FunctionName" )
				{
					functionName = functionInfo[childIndex]->GetValue()[0];
				}
				else if ( functionInfo[childIndex]->GetName() == "WeightName" )
				{
					hasWeight = true;
					weightName = functionInfo[childIndex]->GetValue()[0];
				}
				else if ( functionInfo[childIndex]->GetName() == "Trace" )
				{
					want_Trace = true;
					Trace_FileName = functionInfo[childIndex]->GetValue()[0];
				}
				else if ( functionInfo[childIndex]->GetName() == "Strategy" )
				{
					change_style = true;
					Strategy = functionInfo[childIndex]->GetValue()[0];
				}
				else
				{
					cerr << "Unrecognised FitFunction component: \"" << functionInfo[childIndex]->GetName() << endl;
					exit(1);
				}
			}
		}

		FitFunctionConfiguration* returnable_function = NULL;

		//Make the function
		if (hasWeight)
		{
			cerr <<"Weighted events have been asked for in XML using" << weightName << endl ;
			returnable_function = new FitFunctionConfiguration( functionName, weightName );
		}
		else
		{
			returnable_function = new FitFunctionConfiguration( functionName );
		}

		if( want_Trace )
		{
			returnable_function->SetupTrace( Trace_FileName );
		}

		if( change_style )
		{
			returnable_function->SetStrategy( Strategy );
		}

		return returnable_function;
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << FunctionTag->GetName() << " not \"FitFunction\"" << endl;
		exit(1);
	}
}

//Organise all the PDFs and DataSets
vector< PDFWithData* > XMLConfigReader::GetPDFsAndData( vector<int> Starting_Value )
{
	//Collect all ToFit elements
	vector< XMLTag* > toFits;
	vector< PDFWithData* > pdfsAndData;
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "ToFit" )
		{
			toFits.push_back( children[childIndex] );
		}
	}

	//Go through the collected ToFit elements
	if ( toFits.size() == 0 )
	{
		cerr << "No ToFit tags found in config file" << endl;
		exit(1);
	}
	else
	{
		//Loop over all ToFits
		for ( unsigned int fitIndex = 0; fitIndex < toFits.size(); ++fitIndex )
		{
			XMLTag * pdfTag=NULL;
			XMLTag * dataTag=NULL;
			bool foundPDF = false;
			bool foundData = false;
			bool foundConstraint = false;

			//Find the PDF and data set configuration
			vector< XMLTag* > fitComponents = toFits[fitIndex]->GetChildren();
			for ( unsigned int componentIndex = 0; componentIndex < fitComponents.size(); ++componentIndex )
			{
				string name = fitComponents[componentIndex]->GetName();
				if ( name == "PDF" || name == "SumPDF" || name == "NormalisedSumPDF" || name == "ProdPDF" )
				{
					pdfTag = fitComponents[componentIndex];
					foundPDF = true;
				}
				else if ( name == "DataSet" )
				{
					dataTag = fitComponents[componentIndex];
					foundData = true;
				}
				else if ( name == "ConstraintFunction" )
				{
					//Just so can check for  bad ToFit tags
					foundConstraint = true;
				}
				else
				{
					cerr << "Unrecognised fit component: " << name << endl;
					exit(1);
				}
			}

			//Make the data set, and populate the physics bottle
			if ( foundData && foundPDF )
			{
				if( !Starting_Value.empty() )
				{
					int s_val_index = (int) pdfsAndData.size(); ++s_val_index;
					pdfsAndData.push_back( GetPDFWithData( dataTag, pdfTag, Starting_Value[s_val_index] ) );
				} else {
					pdfsAndData.push_back( GetPDFWithData( dataTag, pdfTag, -1 ) );
				}
			}
			else if ( !foundConstraint )
			{
				cerr << "A ToFit xml tag is incomplete:";
				if ( !foundData && foundPDF )
				{
					cerr << " Data set configuration missing";
				}
				else if ( !foundPDF && foundData )
				{
					cerr << " PDF configuration missing";
				}
				else
				{
					cerr << " No configuration found";
				}
				cerr << endl;
				exit(1);
			}
		}
	}

	return pdfsAndData;
}

//Organise all the PDFs and DataSets
vector< ConstraintFunction* > XMLConfigReader::GetConstraints()
{
	//Collect all ToFit elements
	vector< XMLTag* > toFits;
	vector< ConstraintFunction* > constraints;
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "ToFit" )
		{
			toFits.push_back( children[childIndex] );
		}
	}

	//Go through the collected ToFit elements
	if ( toFits.size() == 0 )
	{
		cerr << "No ToFit tags found in config file" << endl;
		exit(1);
	}
	else
	{
		//Loop over all ToFits
		for ( unsigned int fitIndex = 0; fitIndex < toFits.size(); ++fitIndex )
		{
			XMLTag* constraintTag=NULL;
			bool foundConstraint = false;

			//Find the PDF and data set configuration
			vector< XMLTag* > fitComponents = toFits[fitIndex]->GetChildren();
			for ( unsigned int componentIndex = 0; componentIndex < fitComponents.size(); ++componentIndex )
			{
				if ( fitComponents[componentIndex]->GetName() == "ConstraintFunction" )
				{
					constraintTag = fitComponents[componentIndex];
					foundConstraint = true;
				}
			}

			if (foundConstraint)
			{
				constraints.push_back( GetConstraintFunction(constraintTag) );
			}
		}
	}

	return constraints;
}

//Create a ConstraintFunction for the appropriate xml tag
ConstraintFunction * XMLConfigReader::GetConstraintFunction( XMLTag * InputTag )
{
	if ( InputTag->GetName() == "ConstraintFunction" )
	{
		vector< ExternalConstraint* > constraints;
		vector< XMLTag* > functionComponents = InputTag->GetChildren();
		for ( unsigned int componentIndex = 0; componentIndex < functionComponents.size(); ++componentIndex )
		{
			if ( functionComponents[componentIndex]->GetName() == "ExternalConstraint" )
			{
				constraints.push_back( GetExternalConstraint( functionComponents[componentIndex] ) );
			}
		}

		return new ConstraintFunction(constraints);
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << InputTag->GetName() << "\" not \"ConstraintFunction\"" << endl;
		exit(1);
	}
}

//Create an ExternalConstraint for the appropriate xml tag
ExternalConstraint * XMLConfigReader::GetExternalConstraint( XMLTag * InputTag )
{
	if ( InputTag->GetName() == "ExternalConstraint" )
	{
		string name;
		double value=0.;
		double error=0.;
		vector< XMLTag* > externalComponents = InputTag->GetChildren();
		for ( unsigned int componentIndex = 0; componentIndex < externalComponents.size(); ++componentIndex )
		{
			if ( externalComponents[componentIndex]->GetName() == "Name" )
			{
				name = externalComponents[componentIndex]->GetValue()[0];
			}
			else if ( externalComponents[componentIndex]->GetName() == "Value" )
			{
				value = strtod( externalComponents[componentIndex]->GetValue()[0].c_str(), NULL );
			}
			else if ( externalComponents[componentIndex]->GetName() == "Error" )
			{
				error = strtod( externalComponents[componentIndex]->GetValue()[0].c_str(), NULL );
			}
			else
			{
				cerr << "Unrecognised constraint component: " << externalComponents[componentIndex]->GetName() << endl;
				exit(1);
			}
		}

		return new ExternalConstraint( name, value, error );
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << InputTag->GetName() << "\" not \"ExternalConstraint\"" << endl;
		exit(1);
	}
}

//Create a ParameterSet from the appropriate xml tag
ParameterSet * XMLConfigReader::GetParameterSet( XMLTag * InputTag )
{
	//Load data from the xml file
	if ( InputTag->GetName() == "ParameterSet" )
	{
		vector< PhysicsParameter* > physicsParameters;
		vector<string> names;
		string name = "";

		//Create each physics parameter
		vector< XMLTag* > parameters = InputTag->GetChildren();
		for ( unsigned int parameterIndex = 0; parameterIndex < parameters.size(); ++parameterIndex )
		{
			PhysicsParameter * newParameter = GetPhysicsParameter( parameters[parameterIndex], name );
			if ( newParameter->GetType() != "Invalid" )
			{
				physicsParameters.push_back( newParameter );
				names.push_back(name);
			}
		}

		//Create the parameter set
		ParameterSet * newParameters = new ParameterSet(names);
		for ( unsigned int nameIndex = 0; nameIndex < names.size(); ++nameIndex )
		{
			newParameters->SetPhysicsParameter( names[nameIndex], physicsParameters[nameIndex] );

		}
		return newParameters;
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << InputTag->GetName() << "\" not \"ParameterSet\"" << endl;
		exit(1);
	}
}

//Make a physics parameter from an appropriate XML tag
PhysicsParameter * XMLConfigReader::GetPhysicsParameter( XMLTag * InputTag, string & ParameterName )
{
	//Check the tag is actually a physics parameter
	if ( InputTag->GetName() == "PhysicsParameter" )
	{
		//Create some default values;
		ParameterName = "Uninitialised";
		string type = "Uninitialised";
		string unit = "Uninitialised";
		string blindString = "Uninitialised";
		double value = 0.0;
		double minimum = 0.0;
		double maximum = 0.0;
		double blindScale = 0.0 ;
		double blindOffset = 0.0 ;
		double stepSize = -1.;
		bool hasValue = false;
		bool hasMaximum = false;
		bool hasMinimum = false;
		bool hasBlindString = false ;
		bool hasBlindScale = false ;

		//Loop over the tag children, which correspond to the parameter elements
		vector< XMLTag* > elements = InputTag->GetChildren();
		for ( unsigned int elementIndex = 0; elementIndex < elements.size(); ++elementIndex )
		{
			string name = elements[elementIndex]->GetName();
			if ( name == "Name" )
			{
				ParameterName = elements[elementIndex]->GetValue()[0];
			}
			else if ( name == "Value" )
			{
				hasValue = true;
				value = strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL );
			}
			else if ( name == "Minimum" )
			{
				hasMinimum = true;
				minimum = strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL );
			}
			else if ( name == "Maximum" )
			{
				hasMaximum = true;
				maximum = strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL );
			}
			else if ( name == "Type" )
			{
				type = elements[elementIndex]->GetValue()[0];
			}
			else if ( name == "Unit" )
			{
				unit = elements[elementIndex]->GetValue()[0];
			}
			else if ( name == "BlindString" )
			{
				hasBlindString = true ;
				blindString = elements[elementIndex]->GetValue()[0];
			}
			else if ( name == "BlindScale" )
			{
				hasBlindScale =  true ;
				blindScale = strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL );
			}
			else if ( name == "StepSize" )
			{
				stepSize = strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL );
			}
			else
			{
				cerr << "Unrecognised physics parameter configuration: " << name << endl;
				exit(1);
			}
		}

		//See of blinding has been specified, and if so construct the blinding offset
		if( hasBlindString && hasBlindScale )
		{
			blindOffset = Blinder::getBlindOffset( blindString.c_str(), blindScale ) ;
		}
		else if( (hasBlindString && ! hasBlindScale) || (! hasBlindString && hasBlindScale) )
		{
			cerr << "Blinding information incomplete for parameter: " << ParameterName << " Ignoring blinding for this parameter" << endl;
		}

		//Now construct the physics parameter
		if (hasValue)
		{
			if ( hasMaximum && hasMinimum )
			{
				if ( ( ( fabs(maximum - 0.0) < DOUBLE_TOLERANCE ) && ( ( fabs(minimum - 0.0) < DOUBLE_TOLERANCE ) ) ) || type == "Unbounded" )
				{
					//Unbounded parameter
					PhysicsParameter * p = new PhysicsParameter( ParameterName, value, stepSize, type, unit );
					if( hasBlindString && hasBlindScale ) p->SetBlindOffset( blindOffset ) ;
					return p ;
				}
				else
				{
					//Bounded parameter
					PhysicsParameter * p =  new PhysicsParameter( ParameterName, value, minimum, maximum, stepSize, type, unit );
					if( hasBlindString && hasBlindScale ) p->SetBlindOffset( blindOffset ) ;
					return p ;
				}
			}
			else
			{
				//Check for ambiguity
				if ( hasMaximum || hasMinimum )
				{
					cerr << "Ambiguous parameter definition: " << ParameterName << " has value and ";
					if ( hasMaximum )
					{
						cerr << "maximum, but not minimum";
					}
					if ( hasMinimum )
					{
						cerr << "minimum, but not maximum";
					}
					cerr << " defined" << endl;
					exit(1);
				}
				else
				{
					//Unbounded parameter
					PhysicsParameter * p =  new PhysicsParameter( ParameterName, value, stepSize, type, unit );
					if( hasBlindString && hasBlindScale ) p->SetBlindOffset( blindOffset ) ;
					return p ;
				}
			}
		}
		else
		{
			cerr << "Ambiguous definition for parameter " << ParameterName << endl;
			exit(1);
		}
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << InputTag->GetName() << "\" not \"PhysicsParameter\"" << endl;
		exit(1);
	}
	exit(-99);
}

//Collect the information needed to make a data set
PDFWithData * XMLConfigReader::GetPDFWithData( XMLTag * DataTag, XMLTag * FitPDFTag, int Starting_Value )
{
	//Check the tag actually is a data set
	if ( DataTag->GetName() == "DataSet" )
	{
		string dataSource = "Uninitialised";
		long numberEvents = 0;
		string cutString = "";
		PhaseSpaceBoundary * dataBoundary=NULL;
		vector<string> dataArguments, argumentNames;
		bool boundaryFound = false;
		vector< IPrecalculator* > dataProcessors;
		vector< DataSetConfiguration* > dataSetMakers;
		bool generatePDFFlag = false;
		IPDF * generatePDF=NULL;

		//Retrieve the data set config
		vector< XMLTag* > dataComponents = DataTag->GetChildren();
		for ( unsigned int dataIndex = 0; dataIndex < dataComponents.size(); ++dataIndex )
		{
			string name = dataComponents[dataIndex]->GetName();
			if ( name == "Source" )
			{
				dataSource = dataComponents[dataIndex]->GetValue()[0];
			}
			else if ( name == "Subset" )
			{
				dataSetMakers.push_back( MakeDataSetConfiguration( dataComponents[dataIndex], dataBoundary ) );
			}
			else if ( name == "CutString" )
			{
				cutString = dataComponents[dataIndex]->GetValue()[0];
			}
			else if ( name == "FileName" || name == "NTuplePath" )
			{
				argumentNames.push_back(name);
				dataArguments.push_back( dataComponents[dataIndex]->GetValue()[0] );
			}
			else if ( name == "NumberEvents" )
			{
				numberEvents = strtol( dataComponents[dataIndex]->GetValue()[0].c_str(), NULL, 10 );
			}
			else if ( name == "PhaseSpaceBoundary" )
			{
				boundaryFound = true;
				dataBoundary = GetPhaseSpaceBoundary( dataComponents[dataIndex] );
			}
			else if ( name == "PDF" || name == "SumPDF" || name == "NormalisedSumPDF" || name == "ProdPDF" )
			{
				generatePDF = GetPDF( dataComponents[dataIndex], dataBoundary );
				generatePDFFlag = true;
			}
			else if ( name == "Precalculator" )
			{
				dataProcessors.push_back( MakePrecalculator( dataComponents[dataIndex], dataBoundary ) );
			}
			else if ( name == "StartingEntry" )
			{
				if( Starting_Value < 0 )
				{
					Starting_Value = atoi( dataComponents[dataIndex]->GetValue()[0].c_str() );
				}
			}
			else
			{
				cerr << "Unrecognised data set component: " << name << endl;
				exit(1);
			}
		}

		//Return the collection of configuration information - data generation will happen later
		if (!boundaryFound)
		{
			cerr << "DataSet defined without PhaseSpaceBoundary" << endl;
			exit(1);
		}

		//If there are no separate data sources, go for backwards compatibility
		if ( dataSetMakers.size() < 1 )
		{
			DataSetConfiguration * oldStyleConfig;

			if (generatePDFFlag)
			{
				oldStyleConfig = new DataSetConfiguration( dataSource, numberEvents, cutString, dataArguments, argumentNames, generatePDF );
			}
			else
			{
				if( Starting_Value > 0 )
				{
					oldStyleConfig = new DataSetConfiguration( dataSource, numberEvents, cutString, dataArguments, argumentNames, Starting_Value );
				}
				else
				{
					oldStyleConfig = new DataSetConfiguration( dataSource, numberEvents, cutString, dataArguments, argumentNames, 0 );
				}
			}

			dataSetMakers.push_back(oldStyleConfig);
		}

		//Make the objects
		IPDF * fitPDF = GetPDF( FitPDFTag, dataBoundary );
		return new PDFWithData( fitPDF, dataBoundary, dataSetMakers, dataProcessors);
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << DataTag->GetName() << "\" not \"DataSet\"" << endl;
		exit(1);
	}
}

//Collect the information needed to make a data set
DataSetConfiguration * XMLConfigReader::MakeDataSetConfiguration( XMLTag * DataTag, PhaseSpaceBoundary * DataBoundary )
{
	//Check the tag actually is a data set
	if ( DataTag->GetName() == "Subset" )
	{
		string dataSource = "Uninitialised";
		string cutString = "";
		long numberEvents = 0;
		vector<string> dataArguments, argumentNames;
		bool generatePDFFlag = false;
		IPDF * generatePDF=NULL;

		//Retrieve the data set config
		vector< XMLTag* > dataComponents = DataTag->GetChildren();
		for ( unsigned int dataIndex = 0; dataIndex < dataComponents.size(); ++dataIndex )
		{
			string name = dataComponents[dataIndex]->GetName();
			if ( name == "Source" )
			{
				dataSource = dataComponents[dataIndex]->GetValue()[0];
			}
			else if ( name == "CutString" )
			{
				cutString = dataComponents[dataIndex]->GetValue()[0];
			}
			else if ( name == "FileName" || name == "NTuplePath" )
			{
				argumentNames.push_back(name);
				dataArguments.push_back( dataComponents[dataIndex]->GetValue()[0] );
			}
			else if ( name == "NumberEvents" )
			{
				numberEvents = strtol( dataComponents[dataIndex]->GetValue()[0].c_str(), NULL, 10 );
			}
			else if ( name == "PDF" || name == "SumPDF" || name == "NormalisedSumPDF" || name == "ProdPDF" )
			{
				generatePDF = GetPDF( dataComponents[dataIndex], DataBoundary );
				generatePDFFlag = true;
			}
			else
			{
				cerr << "Unrecognised data set component: " << name << endl;
				exit(1);
			}
		}

		if (generatePDFFlag)
		{
			return new DataSetConfiguration( dataSource, numberEvents, cutString, dataArguments, argumentNames, generatePDF );
		}
		else
		{
			return new DataSetConfiguration( dataSource, numberEvents, cutString, dataArguments, argumentNames );
		}
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << DataTag->GetName() << "\" not \"Subset\"" << endl;
		exit(1);
	}
}

//  Return the PhaseSpaceBoundaries defined in the XML
//  I don't want to add bloat and store the PhaseSpaceBoudaries as the file is loaded,
//  this just leads to storing lots of not wanted data within this class
//  I try to make use of the structure correctly
vector<PhaseSpaceBoundary*> XMLConfigReader::GetPhaseSpaceBoundaries()
{
	vector< PhaseSpaceBoundary* > PhaseSpaceBoundary_vec;
	//Find the ParameterSets
	vector< XMLTag* > toFits;
	//  Children Defined globally on initialization!
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "ToFit" )
		{
			toFits.push_back( children[childIndex] );
		}
	}
	//  Loop over all of the fits
	for ( unsigned short int fitnum=0; fitnum < toFits.size(); ++fitnum )
	{
		vector<XMLTag*> allChildren = toFits[fitnum]->GetChildren();
		for( unsigned short int Childnum=0; Childnum < allChildren.size(); ++Childnum )
		{
			if( allChildren[Childnum]->GetName() == "DataSet" )
			{
				vector<XMLTag*> allFitDataSets = allChildren[Childnum]->GetChildren();
				for( unsigned short int DataSetNum=0; DataSetNum < allFitDataSets.size(); ++DataSetNum )
				{
					if( allFitDataSets[DataSetNum]->GetName() == "PhaseSpaceBoundary" )
					{
						PhaseSpaceBoundary_vec.push_back( GetPhaseSpaceBoundary( allFitDataSets[DataSetNum] ) );
					}
				}
			}

		}
	}
	return PhaseSpaceBoundary_vec;
}

//Make a PhaseSpaceBoundary from the appropriate xml tag
PhaseSpaceBoundary * XMLConfigReader::GetPhaseSpaceBoundary( XMLTag * InputTag )
{
	//Check the tag is actually a PhaseSpaceBoundary
	if ( InputTag->GetName() == "PhaseSpaceBoundary" )
	{
		vector< IConstraint* > constraints;
		vector<string> names;
		string name;

		//Create each single bound
		vector< XMLTag* > constraintTags = InputTag->GetChildren();
		for ( unsigned int boundIndex = 0; boundIndex < constraintTags.size(); ++boundIndex )
		{
			IConstraint * newConstraint = GetConstraint( constraintTags[boundIndex], name );
			if ( newConstraint->GetUnit() != "Invalid" )
			{
				constraints.push_back(newConstraint);
				names.push_back(name);
			}
		}

		//Create the parameter set
		PhaseSpaceBoundary * newBoundary = new PhaseSpaceBoundary(names);
		for ( unsigned int nameIndex = 0; nameIndex < names.size(); ++nameIndex )
		{
			newBoundary->SetConstraint( names[nameIndex], constraints[nameIndex] );

		}
		return newBoundary;
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << InputTag->GetName() << "\" not \"PhaseSpaceBoundary\"" << endl;
		exit(1);
	}
}

//Make an IConstraint from the appropriate xml tag
IConstraint * XMLConfigReader::GetConstraint( XMLTag * InputTag, string & Name )
{
	//Check the tag is actually a single bound
	if ( InputTag->GetName() == "Observable" )
	{
		//Create some default values;
		Name = "Uninitialised";
		string unit = "Uninitialised";
		double minimum = 0.0;
		double maximum = 0.0;
		vector<double> allValues;

		//Loop over the tag children, which correspond to the parameter elements
		vector< XMLTag* > elements = InputTag->GetChildren();
		for ( unsigned int elementIndex = 0; elementIndex < elements.size(); ++elementIndex )
		{
			if ( elements[elementIndex]->GetName() == "Name" )
			{
				Name = elements[elementIndex]->GetValue()[0];
			}
			else if ( elements[elementIndex]->GetName() == "Minimum" )
			{
				minimum = strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL );
			}
			else if ( elements[elementIndex]->GetName() == "Maximum" )
			{
				maximum = strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL );
			}
			else if ( elements[elementIndex]->GetName() == "Value" )
			{
				allValues.push_back( strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL ) );
			}
			else if ( elements[elementIndex]->GetName() == "Unit" )
			{
				unit = elements[elementIndex]->GetValue()[0];
			}
			else
			{
				cerr << "Unrecognised constraint configuration: " <<  elements[elementIndex]->GetName() << endl;
				exit(1);
			}
		}

		//If there are discrete values, make a discrete constraint
		if ( allValues.size() > 0 )
		{
			return new ObservableDiscreteConstraint( Name, allValues, unit );
		}
		else
		{
			return new ObservableContinuousConstraint( Name, minimum, maximum, unit );
		}
	}
	else
	{
		cerr << "Incorrect xml tag provided: \"" << InputTag->GetName() << "\" not \"Observable\"" << endl;
		exit(1);
	}
}

//Create a PDF from an appropriate xml tag
IPDF * XMLConfigReader::GetNamedPDF( XMLTag * InputTag )
{
	IPDF* returnable_NamedPDF;
	//Check the tag actually is a PDF
	if ( InputTag->GetName() == "PDF" )
	{
		vector< XMLTag* > pdfConfig = InputTag->GetChildren();
		string name;
		vector<string> observableNames, parameterNames;
		PDFConfigurator configurator;

		//Load the PDF configuration
		for ( unsigned int configIndex = 0; configIndex < pdfConfig.size(); ++configIndex )
		{
			if ( pdfConfig[configIndex]->GetName() == "Name" )
			{
				name = pdfConfig[configIndex]->GetValue()[0];
			}
			else if ( pdfConfig[configIndex]->GetName() == "ObservableName" )
			{
				observableNames.push_back( pdfConfig[configIndex]->GetValue()[0] );
			}
			else if ( pdfConfig[configIndex]->GetName() == "ParameterName" )
			{
				parameterNames.push_back( pdfConfig[configIndex]->GetValue()[0] );
			}
			else if ( pdfConfig[configIndex]->GetName() == "ParameterSubstitution" )
			{
				configurator.addParameterSubstitution( pdfConfig[configIndex]->GetValue()[0] );
			}
			else if ( pdfConfig[configIndex]->GetName() == "ConfigurationParameter" )
			{
				configurator.addConfigurationParameter( pdfConfig[configIndex]->GetValue()[0] );
			}
			else
			{
				cerr << "Unrecognised PDF configuration: " << pdfConfig[configIndex]->GetName() << endl;
				exit(1);
			}
		}

		//Check if the name is recognised as a PDF
		returnable_NamedPDF = ClassLookUp::LookUpPDFName( name, observableNames, parameterNames, configurator );
	}
	else
	{
		cerr << "Incorrect tag provided: \"" << InputTag->GetName() << "\" not \"PDF\"" << endl;
		exit(1);
	}

//	returnable_NamedPDF->SetRandomFunction( GetSeed() );
	return returnable_NamedPDF;
}

//Create a SumPDF from an appropriate xml tag
IPDF * XMLConfigReader::GetSumPDF( XMLTag * InputTag, PhaseSpaceBoundary * InputBoundary )
{
	IPDF* returnable_SUMPDF;
	//Check the tag actually is a PDF
	if ( InputTag->GetName() == "SumPDF" )
	{
		vector< XMLTag* > pdfConfig = InputTag->GetChildren();
		string fractionName = "unspecified";
		vector< IPDF* > componentPDFs;

		//Load the PDF configuration
		for ( unsigned int configIndex = 0; configIndex < pdfConfig.size(); ++configIndex )
		{
			if ( pdfConfig[configIndex]->GetName() == "FractionName" )
			{
				fractionName = pdfConfig[configIndex]->GetValue()[0];
			}
			else
			{
				componentPDFs.push_back( GetPDF( pdfConfig[configIndex], InputBoundary ) );
			}
		}

		//Check there are two component PDFs to sum
		if ( componentPDFs.size() == 2 )
		{
			if ( fractionName == "unspecified" )
			{
				returnable_SUMPDF = new SumPDF( componentPDFs[0], componentPDFs[1], InputBoundary );
			}
			else
			{
				returnable_SUMPDF = new SumPDF( componentPDFs[0], componentPDFs[1], InputBoundary, fractionName );
			}
		}
		else
		{
			cerr << "Incorrect number of PDFs to sum: " << componentPDFs.size() << " not 2" << endl;
			exit(1);
		}
	}
	else
	{
		cerr << "Incorrect tag provided: \"" << InputTag->GetName() << "\" not \"SumPDF\"" << endl;
		exit(1);
	}

//	returnable_SUMPDF->SetRandomFunction( GetSeed() );
	return returnable_SUMPDF;
}

//Create a NormalisedSumPDF from an appropriate xml tag
IPDF * XMLConfigReader::GetNormalisedSumPDF( XMLTag * InputTag, PhaseSpaceBoundary * InputBoundary )
{
	IPDF* returnable_NormPDF;
	//Check the tag actually is a PDF
	if ( InputTag->GetName() == "NormalisedSumPDF" )
	{
		vector< XMLTag* > pdfConfig = InputTag->GetChildren();
		string fractionName = "unspecified";
		vector< IPDF* > componentPDFs;

		//Load the PDF configuration
		for ( unsigned int configIndex = 0; configIndex < pdfConfig.size(); ++configIndex )
		{
			if ( pdfConfig[configIndex]->GetName() == "FractionName" )
			{
				fractionName = pdfConfig[configIndex]->GetValue()[0];
			}
			else
			{
				componentPDFs.push_back( GetPDF( pdfConfig[configIndex], InputBoundary ) );
			}
		}

		//Check there are two component PDFs to sum
		if ( componentPDFs.size() == 2 )
		{
			if ( fractionName == "unspecified" )
			{
				returnable_NormPDF = new NormalisedSumPDF( componentPDFs[0], componentPDFs[1], InputBoundary );
			}
			else
			{
				returnable_NormPDF =  new NormalisedSumPDF( componentPDFs[0], componentPDFs[1], InputBoundary, fractionName );
			}
		}
		else
		{
			cerr << "Incorrect number of PDFs to sum: " << componentPDFs.size() << " not 2" << endl;
			exit(1);
		}
	}
	else
	{
		cerr << "Incorrect tag provided: \"" << InputTag->GetName() << "\" not \"NormalisedSumPDF\"" << endl;
		exit(1);
	}

//	returnable_NormPDF->SetRandomFunction( GetSeed() );
	return returnable_NormPDF;
}

//Create a ProdPDF from an appropriate xml tag
IPDF * XMLConfigReader::GetProdPDF( XMLTag * InputTag, PhaseSpaceBoundary * InputBoundary )
{
	IPDF* returnable_ProdPDF;
	//Check the tag actually is a PDF
	if ( InputTag->GetName() == "ProdPDF" )
	{
		vector< XMLTag* > pdfConfig = InputTag->GetChildren();
		vector< IPDF* > componentPDFs;

		//Load the PDF configuration
		for ( unsigned int configIndex = 0; configIndex < pdfConfig.size(); ++configIndex )
		{
			componentPDFs.push_back( GetPDF( pdfConfig[configIndex], InputBoundary ) );
		}

		//Check there are two component PDFs to sum
		if ( componentPDFs.size() == 2 )
		{
			returnable_ProdPDF = new ProdPDF( componentPDFs[0], componentPDFs[1] );
		}
		else
		{
			cerr << "Incorrect number of PDFs to multiply: " << componentPDFs.size() << " not 2" << endl;
			exit(1);
		}
	}
	else
	{
		cerr << "Incorrect tag provided: \"" << InputTag->GetName() << "\" not \"ProdPDF\"" << endl;
		exit(1);
	}

//	returnable_ProdPDF->SetRandomFunction( GetSeed() );
	return returnable_ProdPDF;
}

//Choose one of the PDF instantiation methods
IPDF * XMLConfigReader::GetPDF( XMLTag * InputTag, PhaseSpaceBoundary * InputBoundary )
{
	IPDF* returnable_pdf;
	if ( InputTag->GetName() == "PDF" )
	{
		returnable_pdf = GetNamedPDF(InputTag);
	}
	else if ( InputTag->GetName() == "SumPDF" )
	{
		returnable_pdf = GetSumPDF( InputTag, InputBoundary );
	}
	else if ( InputTag->GetName() == "NormalisedSumPDF" )
	{
		returnable_pdf = GetNormalisedSumPDF( InputTag, InputBoundary );
	}
	else if ( InputTag->GetName() == "ProdPDF" )
	{
		returnable_pdf = GetProdPDF( InputTag, InputBoundary );
	}
	else
	{
		cerr << "Unrecognised PDF configuration: " << InputTag->GetName() << endl;
		exit(1);
	}

	//	Create a unique name for this PDF to use internally
	TString PDF_ID("PDF_");
	PDF_ID+=PDF_index;
	//	Increment the number of discovered PDFs
	++PDF_index;
	returnable_pdf->SetRandomFunction( int(GetSeed()) );
	returnable_pdf->SET_ID( PDF_ID );
	returnable_pdf->SetMCCacheStatus( false );
	return returnable_pdf;
}

//  Return the Precalculators defined in the XML
//  I don't want to add bloat and store the Precalculators as the file is loaded,
//  this just leads to storing lots of not wanted data within this class
//  I try to make use of the structure correctly
vector<vector<IPrecalculator*> > XMLConfigReader::GetPrecalculators()
{
  	vector<vector<IPrecalculator*> > IPrecalculator_vec;
	//Find the ParameterSets
	vector< XMLTag* > toFits;
	//  Children Defined globally on initialization!
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "ToFit" )
		{
			toFits.push_back( children[childIndex] );
		}
	}
	//  Loop over all of the fits
	for ( unsigned short int fitnum=0; fitnum < toFits.size(); ++fitnum )
	{
		vector<XMLTag*> allChildren = toFits[fitnum]->GetChildren();
		for( unsigned short int Childnum=0; Childnum < allChildren.size(); ++Childnum )
		{
			if( allChildren[Childnum]->GetName() == "DataSet" )
			{
				vector<IPrecalculator*> IPrecalculator_local_vec;
				vector<XMLTag*> allFitDataSets = allChildren[Childnum]->GetChildren();
				PhaseSpaceBoundary * dataBoundary=NULL;
				bool boundaryFound=false;
				for( unsigned short int DataSetNum=0; DataSetNum < allFitDataSets.size(); ++DataSetNum )
				{
					if( allFitDataSets[DataSetNum]->GetName() == "PhaseSpaceBoundary" )
					{
						boundaryFound = true;
						dataBoundary = GetPhaseSpaceBoundary( allFitDataSets[DataSetNum] );
					}
					else if( ( allFitDataSets[DataSetNum]->GetName() == "Precalculator" ) && boundaryFound )
					{
						IPrecalculator_local_vec.push_back( MakePrecalculator( allFitDataSets[DataSetNum], dataBoundary ) );
					}
				}
				IPrecalculator_vec.push_back( IPrecalculator_local_vec );
			}

		}
	}
	return IPrecalculator_vec;
}

//Make a precalculator for a data set
IPrecalculator * XMLConfigReader::MakePrecalculator( XMLTag * InputTag, PhaseSpaceBoundary * InputBoundary )
{
	//Check the correct tag has been provided
	if ( InputTag->GetName() == "Precalculator" )
	{
		//Parse the tag components
		vector< IPDF* > componentPDFs;
		string precalculatorName = "Unspecified";
		string weightName = "Unspecified";
		vector< XMLTag* > children = InputTag->GetChildren();
		for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
		{
			string name = children[childIndex]->GetName();
			if ( name == "Name" )
			{
				precalculatorName = children[childIndex]->GetValue()[0];
			}
			else if ( name == "WeightName" )
			{
				weightName = children[childIndex]->GetValue()[0];
			}
			else if ( name == "PDF" || name == "SumPDF" || name == "NormalisedSumPDF" || name == "ProdPDF" )
			{
				componentPDFs.push_back( GetPDF( children[childIndex], InputBoundary ) );
			}
			else
			{
				cerr << "Unrecognised Precalculator component: " << name << endl;
				exit(1);
			}
		}

		//Check a signal and background PDF have been provided
		if ( componentPDFs.size() == 2 )
		{
			return ClassLookUp::LookUpPrecalculator( precalculatorName, componentPDFs[0], componentPDFs[1], GetFitParameters(), weightName );
		}
		else
		{
			cerr << "Precalculator expecting 2 PDFs, not " << componentPDFs.size() << endl;
			exit(1);
		}
	}
	else
	{
		cerr << "Incorrect tag provided: \"" << InputTag->GetName() << "\" not \"Precalculator\"" << endl;
		exit(1);
	}
}


//	Return the Integer Seed used in RapidFit XML tag <Seed>SomeInt</Seed>
unsigned int XMLConfigReader::GetSeed()
{
	if( seed.empty() )
	{
		//Find the NumberRepeats tag
		for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
		{
			if ( children[childIndex]->GetName() == "Seed" )
			{
				seed.push_back ( abs( atoi( children[childIndex]->GetValue( )[0].c_str() ) ) );
				cout << "Using seed: " << seed.back() << " from input file." << endl;
				return unsigned(seed.back());
			}
		}
		seed.push_back( 0 );
		//If no such tag is found, report
		cout << "Seed tag not found in config file, defaulting to TRandom3(0)." << endl;
	} else  return unsigned(seed.back());
	return 0;
}

//	Set a new TRandom seed that is returned by the XMLFile
void XMLConfigReader::SetSeed( unsigned int new_seed )
{
	while( !seed.empty() )  {  seed.pop_back();  }	//  Remove the old seed
	seed.push_back(int(new_seed));			//  Set the new Random Seed
}

pair<ScanParam*, ScanParam*> XMLConfigReader::Get2DScanParam( XMLTag * InputTag )
{
	pair<ScanParam*, ScanParam*> Returnable_Pair;
	ScanParam* Param_1=NULL;
	ScanParam* Param_2=NULL;
	if ( ( InputTag->GetName() == "2DScan" ) || ( InputTag->GetName() == "TwoDScan" ) )
	{
		bool param1 = false;
		bool param2 = false;
		//Loop over the tag children, which correspond to the parameter elements
		vector< XMLTag* > elements = InputTag->GetChildren();
		for ( unsigned int elementIndex = 0; elementIndex < elements.size(); ++elementIndex )
		{
			string name = elements[elementIndex]->GetName();
			if ( name == "X_Param" )  {
				Param_1 = XMLConfigReader::GetScanParam( elements[elementIndex] );
				param1 = true;
			} else if ( name == "Y_Param" )  {
				Param_2 = XMLConfigReader::GetScanParam( elements[elementIndex] );
				param2 = true;
			} else {
				cerr << "Bad Configuration for 2DScan, XMLTag " << name << " is bad." << endl;
				exit(-1);
			}
		}

		if( !param1 || !param2 )
		{
			cerr << "Sorry 2DScan not correctly defined" <<endl;
			exit(-3);
		}

		Returnable_Pair.first = Param_1;
		Returnable_Pair.second = Param_2;
		return Returnable_Pair;
	}
	return Returnable_Pair;
}

//Make a physics parameter from an appropriate XML tag
ScanParam * XMLConfigReader::GetScanParam( XMLTag * InputTag )
{
	ScanParam * ScanParamLocal;
	//Check the tag is actually a physics parameter
	if ( (InputTag->GetName() == "Scan") || (InputTag->GetName() == "X_Param") || (InputTag->GetName() == "Y_Param") )
	{
		//Create some default values;
		vector<string> name_tag, type;
		vector<double> maximum, minimum;
		vector<int> points, sigma;

		//Loop over the tag children, which correspond to the parameter elements
		vector< XMLTag* > elements = InputTag->GetChildren();
		for ( unsigned int elementIndex = 0; elementIndex < elements.size(); ++elementIndex )
		{
			string name = elements[elementIndex]->GetName();
			if ( name == "Name" )
			{
				name_tag.push_back( elements[elementIndex]->GetValue()[0].c_str() );
			}
			else if ( name == "Minimum" )
			{
				minimum.push_back( strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL ) );
			}
			else if ( name == "Maximum" )
			{
				maximum.push_back( strtod( elements[elementIndex]->GetValue()[0].c_str(), NULL ) );
			}
			else if ( name == "Points" )
			{
				points.push_back( atoi( elements[elementIndex]->GetValue()[0].c_str() ) );
			}
			else if ( name == "Sigma" )
			{
				sigma.push_back( atoi( elements[elementIndex]->GetValue()[0].c_str() ) );
			}
			else
			{
				cerr << "Unrecognised Scan configuration: " << name << endl;
				exit(1);
			}
		}

		//Check for ambiguity
		if ( ( maximum.empty() || minimum.empty() ) && ( sigma.empty() ) )
		{
			cerr << "Ambiguous Scan definition: " << name_tag[0] << " has value and ";
			if ( maximum.empty() )
			{
				cerr << " maximum, but not minimum";
			}
			if ( minimum.empty() )
			{
				cerr << " minimum, but not maximum";
			}
			cerr << " defined" << endl;
			exit(1);
		}

		if( ( !maximum.empty() || !minimum.empty() ) && !sigma.empty() )
		{
			cerr << "Ambiguous Scan definition:\t" << name_tag[0] <<"\t" << type[0] << endl;
			cerr << "has both numerical and sigma based limits, defaulting to numerical!" << endl;
			while( !sigma.empty() ) { sigma.pop_back(); }
		}

		ScanParamLocal = new ScanParam( name_tag, maximum, minimum, sigma, points );
		return ScanParamLocal;

	} else	{ cerr << "Unreconised Scan Config: " << InputTag->GetName() << endl; exit(1); }

}


int XMLConfigReader::GetTotalDataSetSize()
{
	int DataSetSize=0;
	//Find the ParameterSets
	vector< XMLTag* > toFits;
	//  Children Defined globally on initialization!
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "ToFit" )
		{
			toFits.push_back( children[childIndex] );
		}
	}
	//  Loop over all of the fits
	for ( unsigned short int fitnum=0; fitnum < toFits.size(); ++fitnum )
	{
		vector<XMLTag*> allChildren = toFits[fitnum]->GetChildren();
		for( unsigned short int Childnum=0; Childnum < allChildren.size(); ++Childnum )
		{
			if( allChildren[Childnum]->GetName() == "DataSet" )
			{
				vector< XMLTag* > dataComponents = allChildren[Childnum]->GetChildren();
				for ( unsigned int dataIndex = 0; dataIndex < dataComponents.size(); ++dataIndex )
				{
					if ( dataComponents[dataIndex]->GetName() == "NumberEvents" )
					{
						DataSetSize += (int) atoi( dataComponents[dataIndex]->GetValue()[0].c_str() );
					}
				}
			}
		}
	}

	return DataSetSize;
}

vector<int> XMLConfigReader::GetAllDataSetSizes()
{
	vector<int> DataSetSizes;
	//Find the ParameterSets
	vector< XMLTag* > toFits;
	//  Children Defined globally on initialization!
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "ToFit" )
		{
			toFits.push_back( children[childIndex] );
		}
	}
	//  Loop over all of the fits
	for ( unsigned short int fitnum=0; fitnum < toFits.size(); ++fitnum )
	{
		vector<XMLTag*> allChildren = toFits[fitnum]->GetChildren();
		for( unsigned short int Childnum=0; Childnum < allChildren.size(); ++Childnum )
		{
			if( allChildren[Childnum]->GetName() == "DataSet" )
			{
				vector< XMLTag* > dataComponents = allChildren[Childnum]->GetChildren();
				for ( unsigned int dataIndex = 0; dataIndex < dataComponents.size(); ++dataIndex )
				{
					if ( dataComponents[dataIndex]->GetName() == "NumberEvents" )
					{
						DataSetSizes.push_back( (int) atoi( dataComponents[dataIndex]->GetValue()[0].c_str() ) );
					}
				}
			}
		}
	}
	return DataSetSizes;
}

vector<int> XMLConfigReader::GetAllStartEntries()
{
	vector<int> StartEntries;
	//Find the ParameterSets
	vector< XMLTag* > toFits;
	//  Children Defined globally on initialization!
	for ( unsigned int childIndex = 0; childIndex < children.size(); ++childIndex )
	{
		if ( children[childIndex]->GetName() == "ToFit" )
		{
			toFits.push_back( children[childIndex] );
		}
	}
	//  Loop over all of the fits
	for ( unsigned short int fitnum=0; fitnum < toFits.size(); ++fitnum )
	{
		vector<XMLTag*> allChildren = toFits[fitnum]->GetChildren();
		for( unsigned short int Childnum=0; Childnum < allChildren.size(); ++Childnum )
		{
			if( allChildren[Childnum]->GetName() == "DataSet" )
			{
				vector< XMLTag* > dataComponents = allChildren[Childnum]->GetChildren();
				bool found_one = false;
				for ( unsigned int dataIndex = 0; dataIndex < dataComponents.size(); ++dataIndex )
				{
					if ( dataComponents[dataIndex]->GetName() == "NumberEvents" )
					{
						found_one = true;
						StartEntries.push_back( (int) atoi( dataComponents[dataIndex]->GetValue()[0].c_str() ) );
					}
				}
				if( !found_one )
				{
					StartEntries.push_back( 0 );
				}
			}
		}
	}
	return StartEntries;
}

