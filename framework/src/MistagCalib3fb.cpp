
#include "ParameterSet.h"
#include "DataPoint.h"
#include "PDFConfigurator.h"
#include "DataPoint.h"
#include "ParameterSet.h"
#include "IMistagCalib.h"
#include "MistagCalib3fb.h"
#include "SimpleMistagCalib.h"
#include "ObservableRef.h"
#include "DebugClass.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>

using namespace::std;

MistagCalib3fb::MistagCalib3fb( PDFConfigurator* configurator ) : IMistagCalib(),
	_tagOS(), _tagSS(), _mistagOS(), _mistagSS(),
	_mistagP0_OS(), _mistagP1_OS(), _mistagSetPoint_OS(), _mistagDeltaP1_OS(), _mistagDeltaP0_OS(), _mistagDeltaSetPoint_OS(),
	_mistagP0_SS(), _mistagP1_SS(), _mistagSetPoint_SS(), _mistagDeltaP1_SS(), _mistagDeltaP0_SS(), _mistagDeltaSetPoint_SS(),
	_OSTagged(false), _SSTagged(false), _storedD1(), _storedD2(),
	//	Observable Names
	tagOSName( configurator->getName("tagdecision_os") ),
	tagSSName( configurator->getName("tagdecision_ss") ),
	tagCombName( configurator->getName("tagdecision") ),
	mistagOSName( configurator->getName("tagomega_os") ),
	mistagSSName( configurator->getName("tagomega_ss") ),
	//	Physics Parameter Names
	mistagP1Name_OS( configurator->getName("mistagP1_OS") ),
	mistagP0Name_OS( configurator->getName("mistagP0_OS") ),
	mistagSetPointName_OS( configurator->getName("mistagSetPoint_OS") ),
	mistagDeltaP1Name_OS( configurator->getName("mistagDeltaP1_OS") ),
	mistagDeltaP0Name_OS( configurator->getName("mistagDeltaP0_OS") ),
	mistagDeltaSetPointName_OS( configurator->getName("mistagDeltaSetPoint_OS") ),
	mistagP1Name_SS( configurator->getName("mistagP1_SS") ),
	mistagP0Name_SS( configurator->getName("mistagP0_SS") ),
	mistagSetPointName_SS( configurator->getName("mistagSetPoint_SS") ),
	mistagDeltaP1Name_SS( configurator->getName("mistagDeltaP1_SS") ),
	mistagDeltaP0Name_SS( configurator->getName("mistagDeltaP0_SS") ),
	mistagDeltaSetPointName_SS( configurator->getName("mistagDeltaSetPoint_SS") ),
	_debugMistag(false), _onTuple(false), _floatCalib(false), _untagged(false)
{
	_debugMistag = configurator->isTrue( "DebugMistagModel" );
	_onTuple = ! configurator->isTrue( "Mistag3fbModel" );
	_floatCalib = configurator->isTrue( "FloatCombinedCalib" );

}

MistagCalib3fb::~MistagCalib3fb()
{
}

void MistagCalib3fb::addParameters( vector<string>& parameterNames ) const
{
	parameterNames.push_back( mistagP1Name_OS );
	parameterNames.push_back( mistagP0Name_OS );
	parameterNames.push_back( mistagSetPointName_OS );
	parameterNames.push_back( mistagDeltaP1Name_OS );
	parameterNames.push_back( mistagDeltaP0Name_OS );
	parameterNames.push_back( mistagDeltaSetPointName_OS );

	parameterNames.push_back( mistagP1Name_SS );
	parameterNames.push_back( mistagP0Name_SS );
	parameterNames.push_back( mistagSetPointName_SS );
	parameterNames.push_back( mistagDeltaP1Name_SS );
	parameterNames.push_back( mistagDeltaP0Name_SS );
	parameterNames.push_back( mistagDeltaSetPointName_SS );
}

void MistagCalib3fb::setParameters( const ParameterSet& parameters )
{
	_mistagP1_OS = parameters.GetPhysicsParameter( mistagP1Name_OS )->GetValue();
	_mistagP0_OS = parameters.GetPhysicsParameter( mistagP0Name_OS )->GetValue();
	_mistagSetPoint_OS = parameters.GetPhysicsParameter( mistagSetPointName_OS )->GetValue();
	_mistagDeltaP1_OS = parameters.GetPhysicsParameter( mistagDeltaP1Name_OS )->GetValue();
	_mistagDeltaP0_OS = parameters.GetPhysicsParameter( mistagDeltaP0Name_OS )->GetValue();
	_mistagDeltaSetPoint_OS = parameters.GetPhysicsParameter( mistagDeltaSetPointName_OS )->GetValue();

	_mistagP1_SS = parameters.GetPhysicsParameter( mistagP1Name_SS )->GetValue();
	_mistagP0_SS = parameters.GetPhysicsParameter( mistagP0Name_SS )->GetValue();
	_mistagSetPoint_SS = parameters.GetPhysicsParameter( mistagSetPointName_SS )->GetValue();
	_mistagDeltaP1_SS = parameters.GetPhysicsParameter( mistagDeltaP1Name_SS )->GetValue();
	_mistagDeltaP0_SS = parameters.GetPhysicsParameter( mistagDeltaP0Name_SS )->GetValue();
	_mistagDeltaSetPoint_SS = parameters.GetPhysicsParameter( mistagDeltaSetPointName_SS )->GetValue();
}


void MistagCalib3fb::addObservables( vector<string>& observableNames ) const
{
	observableNames.push_back( tagOSName );
	observableNames.push_back( mistagOSName );
	observableNames.push_back( tagSSName );
	observableNames.push_back( mistagSSName );
}

void MistagCalib3fb::setObservables( const DataPoint* measurement )
{
	double readTagOS = measurement->GetObservable( tagOSName )->GetValue();
	_tagOS = (readTagOS>=0.)?(int)ceil(readTagOS):(int)floor(readTagOS);
	_mistagOS = measurement->GetObservable( mistagOSName )->GetValue();
	double readTagSS = (int) measurement->GetObservable( tagSSName )->GetValue();
	_tagSS = (readTagSS>=0.)?(int)ceil(readTagSS):(int)floor(readTagSS);
	_mistagSS = measurement->GetObservable( mistagSSName )->GetValue();

	if( _tagOS != 0 && _tagSS == 0 ) _OSTagged = true;
	else _OSTagged = false;

	if( _tagOS == 0 && _tagSS != 0 ) _SSTagged = true;
	else _SSTagged = false;

	if( (_tagOS == _tagSS) && (_tagOS == 0) ) _untagged = true;
	else _untagged = false;

	_storedD1 = this->RealD1();
	_storedD2 = this->RealD2();
}

bool MistagCalib3fb::OSTagged() const
{
	return _OSTagged;
}

bool MistagCalib3fb::SSTagged() const
{
	return _SSTagged;
}

double MistagCalib3fb::mistagBbar() const
{
	double returnValue=0.;

	if( this->OSTagged() )
	{
		returnValue = this->mistagOSBbar();
	}
	else if( this->SSTagged() )
	{
		returnValue = this->mistagSSBbar();
	}
	else
	{
		returnValue = 0.5;
	}

	if( returnValue > 0.5 )
	{
		returnValue = 0.5;
	}
	else if( returnValue < 0. )
	{
		returnValue = 0.;
	}

	return returnValue;
}

double MistagCalib3fb::mistagOSBbar() const
{
	//if( fabs(this->q()) < 0.5 ) return 0.5;

	double returnValue = 0.;
	if( _mistagOS > 0.5 ) returnValue = 0.5;
	else if( _mistagOS < 0. ) returnValue = 0.;
	else returnValue = _mistagP0_OS-(_mistagDeltaP0_OS*0.5) + (_mistagP1_OS-(_mistagDeltaP1_OS*0.5))*( _mistagOS - (_mistagSetPoint_OS-(_mistagDeltaSetPoint_OS*0.5)) );

	if( returnValue > 0.5 ) returnValue = 0.5;
	else if( returnValue < 0. ) returnValue = 0.;
	return returnValue;
}

double MistagCalib3fb::mistagSSBbar() const
{
	//if( fabs(this->q()) < 0.5 ) return 0.5;

	double returnValue = 0.;
	if( _mistagSS > 0.5 ) returnValue = 0.5;
	else if( _mistagSS < 0. ) returnValue = 0.;
	else returnValue = _mistagP0_SS-(_mistagDeltaP0_SS*0.5) + (_mistagP1_SS-(_mistagDeltaP1_SS*0.5))*( _mistagSS - (_mistagSetPoint_SS-(_mistagDeltaSetPoint_SS*0.5)) );

	if( returnValue > 0.5 ) returnValue = 0.5;
	else if( returnValue < 0. ) returnValue = 0.;
	return returnValue;
}

double MistagCalib3fb::mistagOSB() const
{
	//if( fabs(this->q()) < 0.5 ) return 0.5;

	double returnValue = 0.;
	if( _mistagOS > 0.5 ) returnValue = 0.5;
	else if( _mistagOS < 0. ) returnValue = 0.;
	else returnValue = _mistagP0_OS+(_mistagDeltaP0_OS*0.5) + (_mistagP1_OS+(_mistagDeltaP1_OS*0.5))*( _mistagOS - (_mistagSetPoint_OS+(_mistagDeltaSetPoint_OS*0.5)) );

	if( returnValue > 0.5 ) returnValue = 0.5;
	else if( returnValue < 0. ) returnValue = 0.;
	return returnValue;
}

double MistagCalib3fb::mistagSSB() const
{
	//if( fabs(this->q()) < 0.5 ) return 0.5;

	double returnValue = 0.;
	if( _mistagSS > 0.5 ) returnValue = 0.5;
	else if( _mistagSS < 0. ) returnValue = 0.;
	else returnValue = _mistagP0_SS+(_mistagDeltaP0_SS*0.5) + (_mistagP1_SS+(_mistagDeltaP1_SS*0.5))*( _mistagSS - (_mistagSetPoint_SS+(_mistagDeltaSetPoint_SS*0.5)) );

	if( returnValue > 0.5 ) returnValue = 0.5;
	else if( returnValue < 0. ) returnValue = 0.;
	return returnValue;
}

double MistagCalib3fb::mistagB() const
{
	double returnValue=0.;

	if( this->OSTagged() )
	{
		returnValue = this->mistagOSB();
	}
	else if( this->SSTagged() )
	{
		returnValue = this->mistagSSB();
	}
	else
	{
		returnValue = 0.5;
	}

	if( returnValue > 0.5 )
	{
		returnValue = 0.5;
	}
	else if( returnValue < 0. )
	{
		returnValue = 0.;
	}

	return returnValue;
}

double MistagCalib3fb::RealD1() const
{
	if( !_untagged )
	{
		return ( (1.+ _tagOS*(1.-2.*this->mistagOSB()) ) * (1.+ _tagSS*(1.-2.*this->mistagSSB())) + (1.- _tagOS*(1.-2.*this->mistagOSBbar()) ) * (1.- _tagSS*(1.-2.*this->mistagSSBbar())) );
	}
	else
	{
		return 1.;
	}
}

double MistagCalib3fb::RealD2() const
{
	if( !_untagged )
	{
		return ( (1.+ _tagOS*(1.-2.*this->mistagOSB()) ) * (1.+ _tagSS*(1.-2.*this->mistagSSB())) - (1.- _tagOS*(1.-2.*this->mistagOSBbar()) ) * (1.- _tagSS*(1.-2.*this->mistagSSBbar())) );
	}
	else
	{
		return 0.;
	}
}

double MistagCalib3fb::D1() const
{
	return _storedD1;
}

double MistagCalib3fb::D2() const
{
	return _storedD2;
}

void MistagCalib3fb::Print() const
{
	cout << endl;
	cout << "_mistagP0_OS              " << _mistagP0_OS << endl;
	cout << "_mistagP0_SS              " << _mistagP0_SS << endl;
	cout << "_mistagDeltaP0_OS         " << _mistagDeltaP0_OS << endl;
	cout << "_mistagDeltaP0_SS         " << _mistagDeltaP0_SS << endl;
	cout << "_mistagP1_OS              " << _mistagP1_OS << endl;
	cout << "_mistagP1_SS              " << _mistagP1_SS << endl;
	cout << "_mistagDeltaP1_OS         " << _mistagDeltaP1_OS << endl;
	cout << "_mistagDeltaP1_SS         " << _mistagDeltaP1_SS << endl;
	cout << "_mistagOS                 " << _mistagOS << endl;
	cout << "_mistagSS                 " << _mistagSS << endl;
	cout << "_mistagSetPoint_OS        " << _mistagSetPoint_OS << endl;
	cout << "_mistagDeltaSetPoint_OS   " << _mistagDeltaSetPoint_OS << endl;
	cout << "_mistagDeltaSetPoint_SS   " << _mistagDeltaSetPoint_SS << endl;
	cout << "this->mistagB()           " << this->mistagB() << endl;
	cout << "this->mistagBbar()        " << this->mistagBbar() << endl;
}

bool MistagCalib3fb::eventIsTagged() const
{
	return !_untagged;
}

double MistagCalib3fb::q() const
{
	return 0.;
}

