// $Id: Bs2JpsiPhi_Signal_v6.h,v 1.1 2009/12/06  Pete Clarke Exp $
/** @class Bs2JpsiPhi_Signal_v6
 *
 *  This version startef the update from _v6_ires by movingdecay time resolution to a contained class.
 *
 *  @author Pete Clarke peter.clarke@ed.ac.uk
 *  @date 2013-06-00
 */

#ifndef Bs2JpsiPhi_Signal_v6_H
#define Bs2JpsiPhi_Signal_v6_H

#include <exception>

#include "BasePDF.h"
#include "PDFConfigurator.h"
#include "SlicedAcceptance.h"
#include "IResolutionModel.h"
#include "IMistagCalib.h"
#include "AngularAcceptance.h"
#include "Mathematics.h"
#include <iostream>
#include <cstdlib>
#include <float.h>
#include <vector>

#include "TH1.h"
#include "TCanvas.h"

#define DOUBLE_TOLERANCE 1E-6



//============================================================================================
class Bs2JpsiPhi_Signal_v6 : public BasePDF
{
	public:
		Bs2JpsiPhi_Signal_v6( PDFConfigurator* );
		~Bs2JpsiPhi_Signal_v6();

		// Mandatory RapidFit Methods
		virtual double EvaluateForNumericIntegral(DataPoint*);
		virtual double Evaluate(DataPoint*);
		virtual double EvaluateTimeOnly(DataPoint*);
		virtual bool SetPhysicsParameters(ParameterSet*);
		virtual vector<string> GetDoNotIntegrateList();

		vector<string> PDFComponents();

		double EvaluateComponent( DataPoint* input, ComponentRef* );

	private:
		void MakePrototypes();
		void prepareCDS();

		//void prepareTimeFac();
		void SetupAngularTerms();

		int timeBinNum;

		DataPoint* _datapoint;

		bool DebugFlag_v6;

	protected:
		//Calculate the PDF normalisation
		virtual double Normalisation(DataPoint*, PhaseSpaceBoundary*);

		void DebugPrintEvaluate( string message, double value ) const;
		void DebugPrintNormalisation( string message, double value ) const;
		void preCalculateTimeIntegralsDebug() const;

		int componentIndex;

		//PELC For debugging purposes
		//TH1D * histOfPdfValues ;
		//TCanvas * c0 ;
		//mutable int histCounter ;
		//~PELC

		// Parameters
		ObservableRef gammaName;		// gamma
		ObservableRef deltaGammaName;	// delta gamma
		ObservableRef deltaMName;		// delta mass
		ObservableRef Azero_sqName;	// amplitude
		ObservableRef Apara_sqName;	// amplitude
		bool _fitDirectlyForApara;
		ObservableRef Aperp_sqName;	// amplitude
		ObservableRef As_sqName;		// amplitude
		ObservableRef CspName;		// amplitude
		ObservableRef delta_zeroName;	// strong phase, set to 0
		ObservableRef delta_paraName;	// strong phase
		ObservableRef delta_perpName;	// strong phase
		ObservableRef delta_sName;		// strong phase for S-wave
		ObservableRef cosdparName;		//PELC-COSDPAR Special for fitting cosdpar separately


		ObservableRef Phi_sName;		// what we want to measure!
		ObservableRef cosphisName;		// fitting cosphis and sinphis independently
		ObservableRef sinphisName;		// fitting cosphis and sinphis independently
		ObservableRef lambdaName;		// magnitude of lambda

		ObservableRef mistagName;		// mistag fraction  - may be used as observable also
		ObservableRef mistagP1Name;		// mistag calib
		ObservableRef mistagP0Name;		// mistag calib
		ObservableRef mistagSetPointName;// mistag calib
		ObservableRef mistagDeltaP1Name;		// mistag calib
		ObservableRef mistagDeltaP0Name;		// mistag calib
		ObservableRef mistagDeltaSetPointName;// mistag calib

		// Observables
		ObservableRef timeName;		// proper time
		ObservableRef tagName;			// B tag
		ObservableRef cosThetaName;	// cos of angle of mu+ wrt z-axis in Jpsi frame
		ObservableRef cosPsiName;		// helicity angle between K+ and -ve Jpsi direction
		ObservableRef phiName;			// azimuthal angle of the mu+ in Jpsi frame
		ObservableRef cthetakName;
		ObservableRef cthetalName;
		ObservableRef phihName;

		ObservableRef BetaName;
		bool _useBetaParameter;

		double A0A0_value;
		double APAP_value;
		double ATAT_value;
		double ASAS_value;
		double ImAPAT_value;
		double ReA0AP_value;
		double ImA0AT_value;
		double ReASAP_value;
		double ImASAT_value;
		double ReASA0_value;

		// Measured Event Observables
		double t;
		int tag;
		// Transversity angles
		double ctheta_tr;
		double phi_tr;
		double ctheta_1;
		// Helicity angles
		double ctheta_k;
		double ctheta_l;
		double phi_h;
		bool _useHelicityBasis;


		// Physics Fit Parameters
		double _gamma;
		double dgam;

		double Aperp_sq;
		double Apara_sq;
		double Azero_sq;
		double As_sq;
		double Csp;
		double CachedA1;
		double CachedA2;
		double CachedA3;
		double CachedA4;
		double CachedA5;
		double CachedA6;
		double CachedA7;
		double CachedA8;
		double CachedA9;
		double CachedA10;
		void CacheAmplitudesAndAngles();

		double delta_para;
		double delta_perp;
		double delta_zero;
		double delta_s;
		double delta1;
		double delta2;
		double cosdpar; //PELC-COSDPAR Special for fitting cosdpar separately

		double delta_ms;
		double phi_s;
		double _cosphis;
		double _sinphis;
		double lambda;
		double _CC, _DD, _SS;

		// This flag is somewhat outdated now that there is a ResolutionModel
		// Kept for now as  aplaceholder as we will need other configuration to choose the model.
		bool _useEventResolution;
		inline bool useEventResolution() const {return _useEventResolution; }

		IResolutionModel * resolutionModel;
		IMistagCalib* _mistagCalibModel;

		double angAccI1;
		double angAccI2;
		double angAccI3;
		double angAccI4;
		double angAccI5;
		double angAccI6;
		double angAccI7;
		double angAccI8;
		double angAccI9;
		double angAccI10;
		AngularAcceptance * angAcc;
		bool _angAccIgnoreNumerator;

		// Other things calculated later on the fly
		double tlo, thi;

		// stored time primitives
		mutable double expL_stored;
		mutable double expH_stored;
		mutable double expSin_stored;
		mutable double expCos_stored;
		mutable double intExpL_stored;
		mutable double intExpH_stored;
		mutable double intExpSin_stored;
		mutable double intExpCos_stored;
		void preCalculateTimeFactors();
		void preCalculateTimeIntegrals();

		//Time acceptance
		bool _useTimeAcceptance;
		inline bool useTimeAcceptance() const { return _useTimeAcceptance; }
		SlicedAcceptance* timeAcc;

		//Configurationparameters
		bool _numericIntegralForce;
		bool _numericIntegralTimeOnly;
		bool _useCosAndSin;
		bool _useCosDpar;
		bool _usePunziSigmat;
		bool _usePunziMistag;
		bool allowNegativeAsSq;
		bool _usePlotComponents;
		bool _usePlotAllComponents;
		bool performingComponentProjection;
		double _offsetToGammaForBetaFactor;

		bool _useDoubleTres, _useTripleTres, _useNewPhisres;

		bool _useNewMistagModel;

		double sin_delta_perp_s;
		double cos_delta_perp_s;
		double sin_delta_zero_s;
		double cos_delta_zero_s;
		double sin_delta_para_s;
		double cos_delta_para_s;

		double sin_delta1;
		double cos_delta1;
		double sin_delta2;
		double cos_delta2;
		double sin_delta_2_1;
		double cos_delta_2_1;

		//....................................
		//Internal helper functions

		double stored_AT;
		inline double AT() const { return stored_AT; }

		double stored_AP;
		inline double AP() const { return stored_AP; }

		double stored_A0;
		inline double A0() const { return stored_A0; }

		double stored_AS;
		inline double AS() const { return stored_AS; }

		double stored_ASint;
		inline double ASint() const { return stored_ASint; }

		//....................
		// Safety for gammas
		double stored_gammal;
		inline double gamma_l() const { return stored_gammal; }

		double stored_gammah;
		inline double gamma_h() const { return stored_gammah; }

		inline double gamma() const { return _gamma ; }

		//.....................
		// C, D, S
		inline double cosphis() const { return _DD ; } //  _cosphis ; }
		inline double sinphis() const { return _SS ; } //  _sinphis ; }
		inline double CC() const { return _CC ; } //  _sinphis ; }


		//......................................................
		// Time primitives

		inline double expL() const { return expL_stored; }
		inline double intExpL( ) const { return intExpL_stored; }

		inline double expH() const { return expH_stored; }
		inline double intExpH( ) const { return intExpH_stored; }

		inline double expSin() const  { return expSin_stored; }
		inline double intExpSin( ) const { return intExpSin_stored;  }

		inline double expCos() const { return expCos_stored; }
		inline double intExpCos( ) const { return intExpCos_stored; }



		//---------------------------------------------------------
		//............. Differential cross sections and normalisations
		double diffXsec();
		double diffXsecTimeOnly();
		double diffXsecNorm1();
		double diffXsecCompositeNorm1(  );


		void DebugPrint( string , double ) const;
		void DebugPrintXsec( string , double ) const;
		void DebugPrintNorm( string , double ) const;


		//------------------------------------------------------------------------------
		// These are the time factors and their analytic integrals for the one angle PDF

		//..................................
		inline double timeFactorEven()  const
		{
			return
				_mistagCalibModel->D1() * (
						( 1.0 + cosphis() ) * expL( )
						+ ( 1.0 - cosphis() ) * expH( )
				       ) +
				_mistagCalibModel->D2() * (
						( 2.0 * sinphis() ) * expSin( )
						+ ( 2.0 * CC()      ) * expCos( )
				       );
		}

		void timeFactorEvenDebug() const
		{
			cout << "D1: " << _mistagCalibModel->D1() << " * ( " << ( 1.0 + cosphis() ) * expL( ) << " + " << ( 1.0 + cosphis() ) * expH( ) << " )" << endl;
			cout << "D2: " << _mistagCalibModel->D2() << " * ( " << ( 2.0 * sinphis() ) * expSin( ) << " + " << + ( 2.0 * CC()      ) * expCos( ) << " )" << endl;
		}

		inline double timeFactorEvenInt()  const
		{
			return
				_mistagCalibModel->D1() * (
						( 1.0 + cosphis() )  * intExpL()
						+ ( 1.0 - cosphis() )  * intExpH()
				       ) +
				_mistagCalibModel->D2() * (
						( 2.0 * sinphis() ) * intExpSin( )
						+  ( 2.0 * CC()      ) * intExpCos( )
				       ) ;
		}

		void timeFactorEvenIntDebug() const
		{
			cout << "D1: " << _mistagCalibModel->D1() << "  intExpL(): " << intExpL() << "  intExpH(): " << intExpH() << endl;
			cout << "D2: " << _mistagCalibModel->D2() << "  intExpSin(): " << intExpSin( ) << "  intExpCos(): " << intExpSin( ) << endl;
			cout << "   " << _mistagCalibModel->D1() *( ( 1.0 + cosphis() )  * intExpL() + ( 1.0 - cosphis() )  * intExpH() ) << endl;
			cout << " + " << _mistagCalibModel->D2() *( ( 2.0 * sinphis() ) * intExpSin( ) + ( 2.0 * CC()      ) * intExpCos( ) )<< endl;
		}


		//..................................
		inline double timeFactorOdd(  )   const
		{
			return
				_mistagCalibModel->D1() * (
						( 1.0 - cosphis() ) * expL( )
						+ ( 1.0 + cosphis() ) * expH( )
				       ) +
				_mistagCalibModel->D2() * (
						-  ( 2.0 * sinphis() ) * expSin( )
						+  ( 2.0 * CC()      ) * expCos( )
				       ) ;
		}

		inline double timeFactorOddInt(  )  const
		{
			return
				_mistagCalibModel->D1() * (
						( 1.0 - cosphis() ) * intExpL()
						+ ( 1.0 + cosphis() ) * intExpH()
				       ) +
				_mistagCalibModel->D2() * (
						-  ( 2.0 * sinphis() ) * intExpSin( )
						+  ( 2.0 * CC()      ) * intExpCos( )
				       ) ;
		}


		//----------------------------------------------------------
		// These are the time factors and their analytic integrals for the three angle PDF

		//...........................
		inline double timeFactorA0A0( )    const { return timeFactorEven( ) ; }
		inline double timeFactorA0A0Int( ) const { return timeFactorEvenInt( ) ; }

		//...........................
		inline double timeFactorAPAP( )    const { return timeFactorEven( ) ; }
		inline double timeFactorAPAPInt( ) const { return timeFactorEvenInt( ) ; }

		//...........................
		inline double timeFactorATAT( )    const { return timeFactorOdd( ) ; }
		inline double timeFactorATATInt( ) const { return timeFactorOddInt( ) ; }

		//...........................
		inline double timeFactorImAPAT( ) const
		{
			return
				_mistagCalibModel->D1() * (
						( expL( ) - expH( ) ) * cos_delta1 * sinphis()
						+ ( expL( ) + expH( ) ) * sin_delta1 * CC()
				       ) +
				_mistagCalibModel->D2() * (
						2.0  * ( sin_delta1*expCos( ) - cos_delta1*cosphis()*expSin( ) )
				       ) ;
		}

		inline double timeFactorImAPATInt( ) const
		{
			return
				_mistagCalibModel->D1() * (
						( intExpL() - intExpH() ) * cos_delta1 * sinphis()
						+ ( intExpL() + intExpH() ) * sin_delta1 * CC()
				       ) +
				_mistagCalibModel->D2() * (
						2.0  * ( sin_delta1*intExpCos() - cos_delta1*cosphis()*intExpSin() )
				       ) ;
		}


		//...........................
		inline double timeFactorReA0AP( )  const
		{
			if( _useCosDpar ) return cosdpar * this->timeFactorEven(  ) ;//PELC-COSDPAR Special for fitting cosdpar separately
			else return cos_delta_2_1 * this->timeFactorEven(  ) ;
		}

		inline double timeFactorReA0APInt( ) const
		{
			if( _useCosDpar ) return cosdpar * this->timeFactorEvenInt( ) ;//PELC-COSDPAR Special for fitting cosdpar separately
			else return cos_delta_2_1 * this->timeFactorEvenInt( ) ;
		}


		//...........................
		inline double timeFactorImA0AT(  ) const
		{
			return
				_mistagCalibModel->D1() * (
						( expL( ) - expH( ) ) * cos_delta2 * sinphis()
						+ ( expL( ) + expH( ) ) * sin_delta2 * CC()
				       ) +
				_mistagCalibModel->D2() * (
						2.0  * ( sin_delta2*expCos( ) - cos_delta2*cosphis()*expSin( ) )
				       ) ;
		}

		inline double timeFactorImA0ATInt( ) const
		{

			return
				_mistagCalibModel->D1() * (
						( intExpL() - intExpH()  ) * cos_delta2 * sinphis()
						+ ( intExpL() + intExpH()  ) * sin_delta2 * CC()
				       ) +
				_mistagCalibModel->D2() * (
						2.0  * ( sin_delta2*intExpCos() - cos_delta2*cosphis()*intExpSin()  )
				       ) ;
		}

		//.... S wave additions.......

		//...........................
		inline double timeFactorASAS( )    const { return timeFactorOdd( ) ; }
		inline double timeFactorASASInt( ) const { return timeFactorOddInt( ) ; }


		//...........................
		inline double timeFactorReASAP( ) const
		{
			return
				_mistagCalibModel->D1() * (
						( expL( ) - expH( ) ) * sin_delta_para_s * sinphis()
						+ ( expL( ) + expH( ) ) * cos_delta_para_s * CC()
				       ) +
				_mistagCalibModel->D2() * (
						2.0  * ( cos_delta_para_s*expCos( ) - sin_delta_para_s*cosphis()*expSin( ) )
				       ) ;
		}

		inline double timeFactorReASAPInt( ) const
		{
			return
				_mistagCalibModel->D1() * (
						( intExpL() - intExpH() ) * sin_delta_para_s * sinphis()
						+ ( intExpL() + intExpH() ) * cos_delta_para_s * CC()
				       ) +
				_mistagCalibModel->D2() * (
						2.0  * ( cos_delta_para_s*intExpCos() - sin_delta_para_s*cosphis()*intExpSin() )
				       ) ;
		}


		//...........................
		inline double timeFactorImASAT( )  const
		{
			return sin_delta_perp_s * this->timeFactorOdd(  ) ;
		}

		inline double timeFactorImASATInt( ) const
		{
			return sin_delta_perp_s * this->timeFactorOddInt( ) ;
		}

		//...........................
		inline double timeFactorReASA0( ) const
		{
			return
				_mistagCalibModel->D1() * (
						( expL( ) - expH( ) ) * sin_delta_zero_s * sinphis()
						+ ( expL( ) + expH( ) ) * cos_delta_zero_s * CC()
				       ) +
				_mistagCalibModel->D2() * (
						2.0  * ( cos_delta_zero_s*expCos( ) - sin_delta_zero_s*cosphis()*expSin( ) )
				       ) ;
		}

		inline double timeFactorReASA0Int( ) const
		{
			return
				_mistagCalibModel->D1() * (
						( intExpL() - intExpH() ) * sin_delta_zero_s * sinphis()
						+ ( intExpL() + intExpH() ) * cos_delta_zero_s * CC()
				       ) +
				_mistagCalibModel->D2() * (
						2.0  * ( cos_delta_zero_s*intExpCos() - sin_delta_zero_s*cosphis()*intExpSin() )
				       ) ;
		}

};

#endif

