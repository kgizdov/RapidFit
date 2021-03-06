/*!
 * @class DataPoint
 *
 * @brief Holds all observables for a given event
 *
 * @author Benjamin M Wynne bwynne@cern.ch
 * @author Robert Currie rcurrie@cern.ch
 */

#pragma once
#ifndef DATA_POINT_H
#define DATA_POINT_H

///	RapidFit Headers
#include "Observable.h"
#include "ObservableRef.h"
///	System Headers
#include <vector>
#include <string>
#include <cstring>
#include <map>

using std::vector;
using std::string;
using std::pair;

class PhaseSpaceBoundary;

class DataPoint
{
	public:
		/*!
		 * @brief Default Constructor Required for Sorting
		 */
		DataPoint();

		/*!
		 * @brief Correct Copy Constructor for DataPoints
		 */
		DataPoint( const DataPoint& );
		DataPoint& operator= ( const DataPoint& );

		/*!
		 * @brief Correct Constructor which contains the name of all of the observables the class knows about
		 *
		 * @param Names   These are the names of the Observables that should exist within this DataPoint
		 */
		DataPoint( vector<string> Names);

		/*!
		 * @brief Destructor for the DataPoint
		 */
		~DataPoint();

		/*!
		 * @brief Get the list names of all of the Observables contained in this DataPoint
		 *
		 * @return Returns a list of the names of all of the Observables in this DataPoint
		 */
		vector<string> GetAllNames() const;


		/*!
		 * @brief This returns the Observable Requested by location
		 *
		 * @param Input   This is the location of the Observable in the list of Obseravbles in this DataPoint
		 *
		 * @return returns the Observable at the requested DataPoint, NULL if out of range
		 */
		Observable* GetObservable( unsigned int Input );

		/*!
		 * @brief This returns the Observable Requested by the Name
		 *
		 * This used to cause the most CPU to be spent in RapidFit before intelligent Caching was coded up
		 *
		 * @param Name   This contains the Name of the Observable being Requested
		 *
		 * @return returns the Observable with the requested Name
		 */
		Observable* GetObservable( const string Name, const bool silence=false );

		/*!
		 * @brief This returns the Observable Requested by the Name and stores the location
		 *
		 * @param NameRef  This object stored the Name of the object and returns it when requested or the object is cast to a string
		 *                 ObservableRef wraps the caching into a transparent object better than using the pair or string methods
		 *
		 * @return returns the Observable with the requested Name, or at the given location if it's already been found once before and it's location stored
		 */
		Observable* GetObservable( const ObservableRef& NameRef, const bool silence=false );

		/*!
		 * @brief Remove the requested Observable
		 *
		 * @param Name   Name of the Observable to remove
		 *
		 * @return Void
		 */
		void RemoveObservable( const string Name );

		/*!
		 * @brief Adds an Observable to This DataPoint
		 *
		 * This will Add or Alter an internal DataPoint according to the given Input
		 *
		 * @param Name    This is the Name of the Observable you want to add
		 *
		 * @param Input   This is a pointer to the Observable you want to add
		 *
		 * @return Void
		 */
		void AddObservable( string Name, Observable* Input );

		/*!
		 * @brief Adds an Observable to This DataPoint
		 *
		 * This will Add or Alter an internal DataPoint according to the given values
		 *
		 * @param Name     Name of the New observable we want to add
		 *
		 * @param Value    The Value we want it to take
		 *
		 * @param Unit     The corresponding Unit
		 *
		 * @param trusted  Is this trusted? i.e. Do we know the parameter to already exist in the DataPoint (possibly as a NULL parameter in a default DataPoint)
		 *
		 * @param position If this is trusted use this position to save performing a lookup of known DataPoints
		 *
		 * @return Void
		 */
		void AddObservable( string Name, double Value, string Unit, bool trusted = false, int position = -1 );

		/*!
		 * @brief Set an internal Observable of the given name to be equal to this Observable
		 *
		 * This WILL NOT add an observable to the DataPoint if one does not exist with the given name
		 *
		 * @param Name     This is the Name of the Observable we want to set
		 *
		 * @oaram Input    This is the Observable we want to replace the internal Observable for
		 *
		 * @return boolean, true is successful, false if not
		 */
		bool SetObservable( string Name, Observable* Input );

		bool SetObservable( ObservableRef& Name, Observable* Input );

		/*!
		 * @brief Set the internal Observable using the given values
		 *
		 * This WILL NOT add an observable to the DataPoint if one does not exist with the given name
		 *
		 * @param Name     Name of the New observable we want to add
		 *
		 * @param Value    The Value we want it to take
		 *
		 * @param Unit     The corresponding Unit
		 *
		 * @param trusted  Is this trusted? i.e. Do we know the parameter to already exist in the DataPoint (possibly as a NULL parameter in a default DataPoint)
		 *
		 * @param position If this is trusted use this position to save performing a lookup of known DataPoints
		 *
		 * @return boolean, true is sucessful, false if not
		 */
		bool SetObservable( string Name, double Value, string Unit, bool trusted =false, int position =-1 );

		/*!
		 * @brief Wanted for sorting DataPoints
		 *
		 * This allows you to sort the DataSet in one Observable if you need to
		 *
		 * @return true/false result of a comparison
		 */
		bool operator() ( pair<DataPoint , ObservableRef >, pair<DataPoint , ObservableRef > );

		/*!
		 * @brief Output some debugging info
		 *
		 * @return Void
		 */
		void Print() const;

		/*!
		 * @brief Get a Pointer to the PhaseSpaceBoundary that the DataPoint exists in
		 *
		 * @return Returns a Pointer to the PhaseSpace this DataPoint exists within
		 */
		PhaseSpaceBoundary* GetPhaseSpaceBoundary() const;

		/*!
		 * @brief Set the Pointer to the PhaseSpaceBoundary
		 *
		 * This allows you to change the PhaseSpace associated with this DataPoint
		 *
		 * @param Input   New PhaseSpace to be associate with this DataPoint
		 *
		 * @return Void
		 */
		void SetPhaseSpaceBoundary( PhaseSpaceBoundary* Input );

		/*!
		 * @brief Return the Discrete Index that has been assigned to this datapoint within a PhaseSpaceBoundary
		 *
		 * @return Returns an integer corresponding to the discrete index of this datapoint (-1 if not set)
		 */
		int GetDiscreteIndex() const;

		/*!
		 * @brief Set the Discrete Index that has been assigned to this datapoint within a PhaseSpaceBoundary
		 *
		 * @param An integer corresponding to the discrete index of this datapoint
		 */
		void SetDiscreteIndex( int index );

		void SetDiscreteIndexID( size_t thisID );

		size_t GetDiscreteIndexID() const;

		void SetDiscreteIndexIDMap( size_t thisID, int index );
		bool FindDiscreteIndexID( size_t thisID );
		int GetDiscreteIndexMap( size_t thisID );
		void ClearDiscreteIndexMap();

		/*!
		 * @brief Sets the even weight to be used for fitting/projecting etc.
		 *
		 * To avoid the possibility of modifying stored Observables in a DataPoint (NOT ADVISED) this allows a runtime object to contain a possibly modified weight
		 *
		 * @param WeightVal This is the value of the weight corresponding to this DataPoint
		 */
		void SetEventWeight( const double WeightVal );

		/*!
		 * @brief Gets the even weight to be used for fitting/projecting etc.
		 *
		 * @return Returns the value of the weight corresponding to this DataPoint
		 */
		double GetEventWeight() const;

		/*!
		 * @breif This stores a double number which corresponds to a per-event NLL value
		 *
		 * @param input This is the initial NLL calculated by some PDF when performing a fit to data
		 */
		void SetInitialNLL( const double input );

		/*!
		 * @brief This Returns the double number which has been stored in this DataPoint corresponding to some initial NLL
		 *
		 * @return return some NLL value
		 */
		double GetInitialNLL() const;

		/*!
		 * @brief This returns a vector of values which have been temporarily stored within this DataPoint
		 *
		 * @return returns the internal vector of some some double values
		 */
		vector<double> GetPerEventData() const;

		/*!
		 * @brief This sets a vector internally in this datapoint to be equal to some input from a users PDF
		 *
		 * @warning This makes NO EFFORT TO CHEK whether the contents of the vector internally are empty are not or of any given size or type
		 */
		void SetPerEventData( const vector<double> input );

		/*!
		 * @brief This clears the vector of PerEvent Data values
		 */
		void ClearPerEventData();

	private:

		vector<double> PerEventData;

		double initialNLL;

		/*!
		 *	This is REAL Data i.e. it was read in from a file
		 */

		/*!
		 * A list of pointers to all of the Observables in this DataPoint
		 */
		vector<Observable> allObservables;

		/*!
		 * A list of the names of all of the Observables in this DataPoint
		 */
		vector<string> allNames;

		/*!
		 * This is a pointer to the PhaseSpaceBoundary that this datapoint has been defined in
		 * DataPoints do NOT have control over this class and don't check that ths pointer is valid
		 */
		PhaseSpaceBoundary* myPhaseSpaceBoundary;

		/*!
		 *
		 */
		int thisDiscreteIndex;
		size_t storedID;

		double WeightValue;

		mutable int nameIndex;

		map< size_t, int > DiscreteIndexMap;
};

#endif

