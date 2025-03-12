/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Plot3D.h"
#include "pgutil.h"
#include "templates.h"

#include <cpgplot.h>

#include <algorithm>
#include <math.h>

using namespace std;


void trapezoid ( pgplot::Plot3D& volume, 
		 Cartesian left, Cartesian right, 
		 double height, double inset, double angle = 45)
{

  vector<Cartesian> pts;
  pts.push_back (left);
  pts.push_back (right);
  pts.push_back (right + Cartesian (-inset, 0.0, height));
  pts.push_back (left +  Cartesian (inset, 0.0, height));

  cpgsfs(3);
  cpgshs (angle, 1, 0);
  volume.poly (pts);

  cpgsfs(2);
  volume.poly (pts);
}

int main (int argc, char** argv)
{
  const char* unknown = "?";

  const char* device = unknown;
  if (argc > 1)
    device = argv[1];

  cpgopen (device);
  cpgscf (2);    // roman font

  float aspect_ratio = pgplot::get_viewport_aspect_ratio ();

  float max = 6.0;

  float xfactor = 1.0;
  float yfactor = 1.0;

  if (aspect_ratio > 1)
    yfactor = aspect_ratio;
  else
    xfactor = 1.0/aspect_ratio;
  
  cpgswin (-max*xfactor, max*xfactor,
	   -max*yfactor, max*yfactor);

  pgplot::Plot3D volume;

  float longitude0 = -90;
  float latitude0 = 0;

  float longitude = 65;
  float latitude = 25;

  volume.set_camera (longitude0, latitude0);

  Cartesian origin (0,0,0);
  Cartesian x0 (1,0,0);
  Cartesian y0 (0,1,0);
  Cartesian z0 (0,0,1);

  double dot_ch = 8.0;

  double S0 = 0.8;
  double P = 0.7;

  double norm = sqrt(S0*S0+P*P);
  double inv = sqrt(S0*S0-P*P);
  double height = norm;
  double inset = norm/8;

  // //////////////////////////////////////////////////////////////////
  //
  // Stokes sample
  //
  // //////////////////////////////////////////////////////////////////

  double separation = 0.25;
  double sample_height = 4.5 + separation;

  double sample_start = -2.0 + separation/2;
  double sample_end = 0.0;


// SUBROUTINE PGSAH (FS, ANGLE, BARB)
//       INTEGER  FS
//       REAL ANGLE, BARB
//
// Set the style to be used for arrowheads drawn with routine PGARRO.
//
//  FS     (input)  : FS = 1 => filled; FS = 2 => outline.
//                    Other values are treated as 2. Default 1.
//  ANGLE  (input)  : the acute angle of the arrow point, in degrees;
//                    angles in the range 20.0 to 90.0 give reasonable
//                    results. Default 45.0.
//  BARB   (input)  : the fraction of the triangular arrow-head that
//                    is cut away from the back. 0.0 gives a triangular
//                    wedge arrow-head; 1.0 gives an open >. Values 0.3
//                    to 0.7 give reasonable results. Default 0.3.

  cpgsch (0.5);
  cpgsah (2, 45, 1);

  for (double sample = sample_start; sample <= sample_end; sample += separation)
  {
    Cartesian from (sample, 0, sample_height);
    Cartesian to (sample, 0, sample_height+height/2);
    volume.arrow (from, to);
  }

  cpgsah (1, 45, 0);

  double t_samp_height = sample_height + height/2 + separation;
  Cartesian l0 (sample_start-2*separation, 0, t_samp_height);
  Cartesian l1 (sample_start, 0, t_samp_height);
  volume.arrow (l0, l1);

  Cartesian r0 (sample_start+3*separation, 0, t_samp_height);
  Cartesian r1 (sample_start+separation, 0, t_samp_height);
  volume.arrow (r0, r1);

  double T_int_height = sample_height - separation;
  double T_int_width = 0.3;
  Cartesian L0 (-1-T_int_width, 0, T_int_height);
  Cartesian L1 (-2, 0, T_int_height);
  volume.arrow (L0, L1);

  Cartesian R0 (-1+T_int_width, 0, T_int_height);
  Cartesian R1 (0, 0, T_int_height);
  volume.arrow (R0, R1);


  cpgsch(0.75);
  volume.text (Cartesian(-4.5,0,sample_height+height/6), "Stokes Sample", 0);

  volume.text (Cartesian(sample_start + 3.5*separation, 0, t_samp_height),
	       "\\fit\\fr\\dsamp", 0);

  volume.text (Cartesian(-1, 0, T_int_height-separation/4),
	       "\\fiT\\fr\\dint", 0.5);

  double left0 = -2.0;
  double left1 = left0 + 2.0;

  double delta = 0.1;

  double right0 = left1 + delta;
  double right1 = right0 + 2.0;

  // //////////////////////////////////////////////////////////////////
  //
  // Single mode
  //
  // //////////////////////////////////////////////////////////////////

  double single = 3;
  trapezoid (volume, 
	     Cartesian(left0, 0, single),
	     Cartesian(left1, 0, single),
	     height, inset);

  trapezoid (volume, 
	     Cartesian(right0, 0, single),
	     Cartesian(right1, 0, single),
	     height, inset);

  // //////////////////////////////////////////////////////////////////
  //
  // Disjoint modes
  //
  // //////////////////////////////////////////////////////////////////

  double disjoint = 1.5;
  trapezoid (volume, 
	     Cartesian(left0 ,0,disjoint),
	     Cartesian(left1 ,0,disjoint),
	     height, inset);

  trapezoid (volume, 
	     Cartesian(right0  ,0,disjoint),
	     Cartesian(right1  ,0,disjoint),
	     height, inset, -45);


  // //////////////////////////////////////////////////////////////////
  //
  // Composite modes
  //
  // //////////////////////////////////////////////////////////////////

  double composite = 0;
  double modeA_width = 1.2;

  trapezoid (volume, 
	     Cartesian(left0 ,0,composite),
	     Cartesian(left0 + modeA_width, 0,composite),
	     height, inset);

  trapezoid (volume, 
	     Cartesian(left0 + modeA_width, 0,composite),
	     Cartesian(left1 ,0,composite),
	     height, inset, -45);

  trapezoid (volume, 
	     Cartesian(right0 ,0,composite),
	     Cartesian(right0 + modeA_width ,0,composite),
	     height, inset);

  trapezoid (volume, 
	     Cartesian(right0 + modeA_width ,0,composite),
	     Cartesian(right1  ,0,composite),
	     height, inset, -45);


  // //////////////////////////////////////////////////////////////////
  //
  // Superposed modes
  //
  // //////////////////////////////////////////////////////////////////

  S0 = 2*S0;
  P = 0;
  norm = S0;
  inv = S0;

  double superposed = -1.5;

  trapezoid (volume, 
	     Cartesian(left0   ,0,superposed),
	     Cartesian(left1   ,0,superposed),
	     height, inset);

  trapezoid (volume, 
	     Cartesian(left0   ,0,superposed),
	     Cartesian(left1   ,0,superposed),
	     height, inset, -45);

  trapezoid (volume, 
	     Cartesian(right0  ,0,superposed),
	     Cartesian(right1  ,0,superposed),
	     height, inset);

  trapezoid (volume, 
	     Cartesian(right0  ,0,superposed),
	     Cartesian(right1  ,0,superposed),
	     height, inset, -45);

  volume.set_camera (longitude0, latitude0);
  
  // cpgsch(1.0);
  volume.text (Cartesian(-4.5,0,single+height/2), "Single Mode", 0);
  volume.text (Cartesian(-4.5,0,disjoint+height/2), "Disjoint", 0);
  volume.text (Cartesian(-4.5,0,composite+height/2), "Composite", 0);
  volume.text (Cartesian(-4.5,0,superposed+height/2), "Superposed", 0);

  cpgsls(2);
  volume.move (Cartesian(left1+delta/2, 0, superposed));
  volume.draw (Cartesian(left1+delta/2, 0, single + height));
  cpgend();

  return 0;
}

