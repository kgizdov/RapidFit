/**
        @class ResultParameter

        A physics parameter after it has been fitted

        @author Benjamin M Wynne bwynne@cern.ch
	@date 2009-10-02
*/

#include "ResultParameter.h"
#include <iostream>

//Default constructor
ResultParameter::ResultParameter() : value(0.0), originalValue(0.0), error(0.0), minimum(0.0), maximum(0.0), type("Uninitialised"), unit("Uninitialised")
{
}

//Constructor with correct argument
ResultParameter::ResultParameter( string Name, double NewValue, double NewOriginalValue, double NewError, double NewMinimum, double NewMaximum,
		string NewType, string NewUnit ) : value(NewValue), originalValue(NewOriginalValue), error(NewError), minimum(NewMinimum),
		maximum(NewMaximum), type(NewType), unit(NewUnit)
{
	if (maximum < minimum)
	{
		cerr << "Result parameter \"" << Name << "\" has maximum less than minimum: values swapped" << endl;
		minimum = NewMaximum;
		maximum = NewMinimum;
	}

	if (value < minimum)
	{
		cerr << "Result parameter \"" << Name << "\" has value less than minimum" << endl;
	}

	if (value > maximum)
	{
		cerr << "Result parameter \"" << Name << "\" has value greater than maximum" << endl;
	}

	if (unit == "")
	{
		cerr << "Result parameter \"" << Name << "\" has no unit! What kind of physicist are you?" << endl;
	}
}

//Destructor
ResultParameter::~ResultParameter()
{
}

//Get the value
double ResultParameter::GetValue()
{
	return value;
}

//Get the original value
double ResultParameter::GetOriginalValue()
{
	return originalValue;
}

//Get the error
double ResultParameter::GetError()
{
	return error;
}

//Get the pull
double ResultParameter::GetPull()
{
	return (value - originalValue) / error;
}

//Get the minimum
double ResultParameter::GetMinimum()
{       
	return minimum;
}

//Get the maximum
double ResultParameter::GetMaximum()
{       
	return maximum;
}

//Get the type
string ResultParameter::GetType()
{
	return type;
}

//Get the unit
string ResultParameter::GetUnit()
{
	return unit;
}
