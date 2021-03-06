/*
 * @class BasePDF
 *
 * Class that provides a general implementation of IPDF.
 * Can inherit from this to make a PDF without worrying about the details.
 *
 * @author Benjamin M Wynne bwynne@cern.ch
 * @author Robert Currie rcurrie@cern.ch
 */
///	ROOT Headers
#include "TRandom3.h"
///	RapidFit Headers
#include "IPDF_Framework.h"
#include "BasePDF_Framework.h"
#include "RapidFitIntegrator.h"
/*#include "StringProcessing.h"
#include "ObservableRef.h"
#include "PhaseSpaceBoundary.h"
#include "RapidFitIntegrator.h"*/
///	System Headers
#include <iostream>
#include <cmath>
#include <sstream>
#include <pthread.h>

using std::vector;
using std::stringstream;
using std::cout;
using std::endl;

//Constructor
BasePDF_Framework::BasePDF_Framework( IPDF* thisPDF ) : IPDF_Framework(), PDFName("Base"), PDFLabel("Base"), copy_object( NULL ), debug_mutex(NULL), can_remove_mutex(true),
	debuggingON(false), CopyConstructorIsSafe(true), thisConfig(NULL), myIntegrator(NULL)
{
	debug_mutex = new pthread_mutex_t();

	myIntegrator = new RapidFitIntegrator( thisPDF );
}

BasePDF_Framework::BasePDF_Framework( const BasePDF_Framework& input ) : IPDF_Framework( input ),
	PDFName( input.PDFName ), PDFLabel( input.PDFLabel ), copy_object( input.copy_object ),
	thisConfig(NULL), debuggingON(input.debuggingON), debug_mutex(input.debug_mutex), can_remove_mutex(false),
	CopyConstructorIsSafe(input.CopyConstructorIsSafe), myIntegrator(NULL)
{
	if( input.thisConfig != NULL ) thisConfig = new PDFConfigurator( *(input.thisConfig) );

	if( input.myIntegrator != NULL ) myIntegrator = new RapidFitIntegrator( *(input.myIntegrator) );
}

void BasePDF_Framework::SetCopyConstructorSafe( bool input )
{
	CopyConstructorIsSafe=input;
}

bool BasePDF_Framework::IsCopyConstructorSafe() const
{
	return CopyConstructorIsSafe;
}

bool BasePDF_Framework::IsDebuggingON()
{
	debuggingON = DebugClass::DebugThisClass( "BasePDF" ) && !DebugClass::GetClassNames().empty();
	return debuggingON;
}

//Destructor
BasePDF_Framework::~BasePDF_Framework()
{
	if( thisConfig != NULL ) delete thisConfig;
	if( debug_mutex != NULL && can_remove_mutex == true ) delete debug_mutex;
	if( myIntegrator != NULL ) delete myIntegrator;
}

void BasePDF_Framework::SetCopyConstructor( CopyPDF_t* input ) const
{
	copy_object = input;
}

CopyPDF_t* BasePDF_Framework::GetCopyConstructor() const
{
	return copy_object;
}

string BasePDF_Framework::GetName() const
{
	return PDFName;
}

void BasePDF_Framework::SetName( string input )
{
	PDFName = input;
}

string BasePDF_Framework::GetLabel() const
{
	return PDFLabel;
}

void BasePDF_Framework::SetLabel( string input )
{
	PDFLabel = input;
}

string BasePDF_Framework::XML() const
{
	stringstream xml;
	xml << "<PDF>" << endl;
	xml << "\t<Name>" << this->GetName() << "</Name>" << endl;
	string config = thisConfig->XML();
	if( !config.empty() ) xml << config << endl;
	xml << "</PDF>" << endl;
	return xml.str();
}

void BasePDF_Framework::SetConfigurator( PDFConfigurator* config )
{
	if( thisConfig != NULL ) delete thisConfig;
	if( DebugClass::DebugThisClass( "BasePDF_Framework" ) ) cout << "BasePDF_Framework:: Removed old Configuration, Adding new" << endl;
	thisConfig = new PDFConfigurator( *config );
}

PDFConfigurator* BasePDF_Framework::GetConfigurator() const
{
	return thisConfig;
}

pthread_mutex_t* BasePDF_Framework::DebugMutex() const
{
	return debug_mutex;
}

void BasePDF_Framework::SetDebugMutex( pthread_mutex_t* Input, bool can_remove )
{
	can_remove_mutex = can_remove;
	if( debug_mutex != NULL && can_remove_mutex ) delete debug_mutex;
	debug_mutex = Input;
}

void BasePDF_Framework::Print() const
{
	cout << "This PDF is: " << this->GetLabel() << endl;
}

RapidFitIntegrator* BasePDF_Framework::GetPDFIntegrator() const
{
        return myIntegrator;
}

void BasePDF_Framework::SetUpIntegrator( const RapidFitIntegratorConfig* input )
{
        myIntegrator->SetUpIntegrator( input );
}

void BasePDF_Framework::ChangePhaseSpace( PhaseSpaceBoundary * InputBoundary )
{
        (void) InputBoundary;
        return;
}

PhaseSpaceBoundary* BasePDF_Framework::GetPhaseSpace() const
{
	return NULL;
}

vector<IPDF*> BasePDF_Framework::GetChildren() const
{
	return vector<IPDF*>();
}

