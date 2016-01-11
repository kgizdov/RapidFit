// THIS .cpp FILE IS GENERATED BY A SCRIPT, SO CHANGES WILL BE LOST
#include "Bs2PhiKKAcceptance.h"
#include <gsl/gsl_sf_legendre.h>
#include <cmath>
#include <iostream>
using std::cout;
using std::endl;
Bs2PhiKKAcceptance::Bs2PhiKKAcceptance()
{
  createcoefficients();
}
Bs2PhiKKAcceptance::Bs2PhiKKAcceptance(const Bs2PhiKKAcceptance& copy)
{
  createcoefficients();
}
Bs2PhiKKAcceptance::~Bs2PhiKKAcceptance()
{
  for ( int l = 0; l < l_max; l++ )
  {
    for ( int i = 0; i < i_max; i++ )
    {
      for ( int k = 0; k < k_max; k++ )
      {
        delete c[l][i][k];
      }
      delete c[l][i];
    }
    delete c[l];
  }
  delete c;
}
double Bs2PhiKKAcceptance::Evaluate(double mKK, double phi, double ctheta_1, double ctheta_2)
{
  double acceptance = 0;
  double mKK_mapped = (mKK - mKK_min) / (mKK_max - mKK_min)*2 - 1;
  double Q_l = 0;
  double P_i = 0;
  double Y_jk = 0;
  for ( int l = 0; l < l_max; l++ )
  {
    for ( int i = 0; i < i_max; i++ )
    {
      for ( int k = 0; k < k_max; k++ )
      {
        for ( int j = 0; j < j_max; j++ )
        {
          if (j < k) continue; // must have l >= k
          Q_l  = gsl_sf_legendre_Pl   (l,  mKK_mapped);
          P_i  = gsl_sf_legendre_Pl   (i,  ctheta_2);
          // only consider case where k >= 0
          // these are the real valued spherical harmonics
          if ( k == 0 ) Y_jk =       gsl_sf_legendre_sphPlm (j, k, ctheta_1);
          else      Y_jk = sqrt(2) * gsl_sf_legendre_sphPlm (j, k, ctheta_1) * cos(k*phi);
          acceptance += c[l][i][k][j]*(Q_l * P_i * Y_jk);
        }
      }
    }
  }
  return acceptance;
}
//BEGIN CONSTANTS---------------------------------------------------------------
double Bs2PhiKKAcceptance::mKK_min = 988;
double Bs2PhiKKAcceptance::mKK_max = 4379;
int Bs2PhiKKAcceptance::l_max = 7;
int Bs2PhiKKAcceptance::i_max = 7;
int Bs2PhiKKAcceptance::k_max = 3;
int Bs2PhiKKAcceptance::j_max = 3;
//END CONSTANTS-----------------------------------------------------------------
void Bs2PhiKKAcceptance::createcoefficients()
{
//BEGIN CODE--------------------------------------------------------------------
//double**** c;
c = new double***[l_max];
for ( int l = 0; l < l_max; l++ )
{
  c[l] = new double**[i_max];
  for ( int i = 0; i < i_max; i++ )
  {
    c[l][i] = new double*[k_max];
    for ( int k = 0; k < k_max; k++ )
    {
      c[l][i][k] = new double[j_max];
      for ( int j = 0; j < j_max; j++ )
      {
        c[l][i][k][j] = 0;
      }
    }
  }
}
c[0][0][0][0] = 0.039543;// +- 0.000116
c[0][0][2][2] = 0.001709;// +- 0.000244
c[0][1][1][2] = 0.028771;// +- 0.000382
c[0][2][0][0] = -0.013098;// +- 0.000524
c[0][2][2][2] = -0.012249;// +- 0.000530
c[0][3][1][2] = -0.009074;// +- 0.000631
c[1][0][0][0] = -0.015890;// +- 0.000510
c[1][1][1][2] = -0.018535;// +- 0.000871
c[1][2][2][2] = 0.007741;// +- 0.001132
c[2][0][0][0] = -0.008893;// +- 0.000701
c[2][1][1][2] = -0.009025;// +- 0.001164
c[2][2][0][0] = 0.017517;// +- 0.001493
c[2][3][1][2] = 0.012133;// +- 0.001801
c[3][0][0][0] = -0.016349;// +- 0.000852
//END CODE----------------------------------------------------------------------
}
