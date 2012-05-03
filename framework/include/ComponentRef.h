/*!
 * @class ComponentRef
 * 
 * @brief This class has been written to store complex lookups performed when requesting a single component from a PDF
 * 
 * The important functions for a PDF developer are: get/setComponentNumber() and getComponentName()
 * 
 * @author Robert Currie rcurrie@cern.ch
 */

#pragma once
#ifndef _RAPIDFIT_COMPONENT_REF
#define _RAPIDFIT_COMPONENT_REF

///	System Headers
#include <string>

using namespace::std;

class ComponentRef
{
	public:
		/*!
		 * @brief Constructor with full name of component
		 *
		 * @param Name   This is the full name of the Component
		 */
		ComponentRef( string Name );

		/*!
		 * @brief Destructor
		 */
		~ComponentRef();

		/*!
		 * @brief Correct Copy Constuctor for this class
		 */
		ComponentRef( const ComponentRef& );

		/*!
		 * @brief  Add a subComponetRef with the given name
		 *         i.e. add a new link in the chain
		 *
		 * @param Input   This is the Name of the new ComponentRef to be created and linked to this one
		 *
		 * @return Void
		 */
		void addSubComponent( string );

		/*!
		 * @brief Get the subComponentRef for this instance
		 *        i.e. get the next link in the chain
		 *
		 * @return Returns the pointer to the CompoentRef internally stored by this object
		 */
		ComponentRef* getSubComponent();

		/*!
		 * @brief set the component number in this ComponentRef
		 *
		 * @oaram Input      This is a value you want to store after performing a lookup for this component in a list
		 *
		 * @return Void
		 */
		void setComponentNumber( int Input );

		/*!
		 * @brief get the component number in this ComponentRef
		 *
		 * @return returns the stored result from a previous lookup
		 */
		int getComponentNumber();

		/*!
		 * @brief get the name of this ComponenRef
		 *
		 * @return returns the name of this component
		 */
		string getComponentName();

	private:
		/*!
		 * Don't Copy the class this way!
		 */
		ComponentRef& operator= ( const ComponentRef& );

		/*!
		 * This is the reference to the sub-ComponentRef stored in this instance
		 */
		ComponentRef* thisSubComponent;

		int thisIndex;		/*!	The index stored in this ComponentRef from an external lookup	*/
		string thisName;	/*!	The name of this particluar ComponentRef			*/
};

#endif
