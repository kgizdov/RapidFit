#include "DPGLassShape.hh"
#include "DPBarrierL0.hh"
#include "DPBarrierL1.hh"
#include "DPBarrierL2.hh"
#include "DPBarrierL3.hh"

#include <iostream>

#define DOUBLE_TOLERANCE 1E-6

DPGLassShape::DPGLassShape(double mRR, double gammaRR, int L,
                    double mm1, double mm2, double RR, double aa, double rr):
   mR(mRR)
  ,gammaR(gammaRR)
  ,LR(L)
  ,m1(mm1)
  ,m2(mm2)
  ,a(aa)
  ,r(rr)
  ,R(RR)
  ,fraction(0.5)
  ,phaseR(0)
  ,phaseB(0)
{
  switch (LR)
  {
    case 0: barrier=new DPBarrierL0(R);
            break;
    default: std::cout<<"WARNING: Do not know which barrier factor to use.  Using L=0 and you should check what are you doing.\n";
             barrier=new DPBarrierL0(R);
             break;
  }

  pR0=daughterMomentum(mR);
}

DPGLassShape::DPGLassShape( const DPGLassShape& other ) : DPMassShape( other ),
	mR( other.mR ), gammaR( other.gammaR ), LR(other.LR),
	m1(other.m1), m2(other.m2), a(other.a), r(other.r), R(other.R),
	fraction(other.fraction), phaseR(other.phaseR), phaseB(other.phaseB),
	barrier(NULL)
{
if ( other.barrier != NULL )
{
  switch (LR)
  {
    case 0: barrier=new DPBarrierL0(R);
            break;
    default:  std::cout<<"WARNING: Do not know which barrier factor to use.  Using L=0 and you should check what are you doing.\n";
              barrier=new DPBarrierL0(R);
              break;
  }
}
pR0=daughterMomentum(mR);
}

DPGLassShape::~DPGLassShape()
{
  if (barrier)
  {
    delete barrier;
  }
}

std::complex<double> DPGLassShape::massShape(double m)
{
// Calculate delta_R
  double tanDeltaR=mR*gamma(m)/(mR*mR-m*m);
  double deltaR=0;
  if ( (mR-m) < DOUBLE_TOLERANCE )
  {
    deltaR=M_PI/2.0;
  }
  else
  {
    deltaR=std::atan(tanDeltaR);
  }

// Calculate delta_B
  double q=daughterMomentum(m);
  double cotDeltaB=1./(a*q)+r*q/2.;
  double deltaB=0;
  if ( q < DOUBLE_TOLERANCE )
  {
    if (a>0)
    {
      deltaB=0;
    }
    else
    {
      deltaB=M_PI;
    }
  }
  else if (cotDeltaB < DOUBLE_TOLERANCE || std::isnan(1./cotDeltaB))
  {
    deltaB=M_PI/2.0;
  }
  else
  {
    deltaB=std::atan(1.0/cotDeltaB);
  }

  std::complex<double> result(0,0);
  result += fraction * sin(deltaR) * std::polar(1.,deltaR+phaseR) * std::polar(1.,2.0*(deltaB+phaseB));
  result += (1-fraction) * sin(deltaB+phaseB) * std::polar(1.,deltaB+phaseB);

  return result;
}

double DPGLassShape::gamma(double m)
{
  double pp=daughterMomentum(m);  // momentum of daughter at the actual mass
  double bb=barrier->barrier(pR0,pp);  // Barrier factor
  double gg=gammaR*mR/m*bb*bb*std::pow(pp/pR0,2*LR+1);

  return gg;
}

double DPGLassShape::daughterMomentum(double m)
{
  double momentum;

  momentum=(m*m-(m1+m2)*(m1+m2))*(m*m-(m1-m2)*(m1-m2));
  momentum=std::sqrt(momentum);
  momentum/=2*m;

  return momentum;
}

void DPGLassShape::setParameters(double* pars)
{
  mR=pars[0];
  gammaR=pars[1];
  a=pars[2];
  r=pars[3];
  fraction=pars[4];
  phaseR=pars[5];
  phaseB=pars[6];
  pR0=daughterMomentum(mR);
}

void DPGLassShape::setResonanceParameters(double a_lass, double r_lass)
{
  a = a_lass;
  r = r_lass;
  pR0=daughterMomentum(mR);
  return;
}
