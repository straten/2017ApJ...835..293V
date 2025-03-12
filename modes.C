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

void ellipse ( pgplot::Plot3D& volume, Cartesian centre,
	       Cartesian x0, double a0,
	       Cartesian x1, double a1 )
{
  for (unsigned deg=0; deg <= 360; deg +=2)
  {
    double radians = deg * M_PI/180;
    Cartesian p	= centre + a0*cos(radians)*x0 + a1*sin(radians)*x1;
    if (deg == 0)
      volume.move (p);
    else
      volume.draw (p);
  }
}

void ellipsoid ( pgplot::Plot3D& volume, Cartesian centre,
		 Cartesian axis[3], double length[3] )
{
  // draw the meridians (lines of longitude)
  for (unsigned deg=0; deg<180; deg += 30)
  {
    double radians = deg * M_PI/180;
    Cartesian other
      = length[1]*cos(radians)*axis[1]
      + length[2]*sin(radians)*axis[2];

    ellipse (volume, centre, axis[0], length[0], other, 1.0);
  }

  // draw the lines of latitude
  for (int deg=-60; deg < 90; deg+=30)
  {
    double radians = deg * M_PI/180;
    Cartesian c = centre + length[0]*sin(radians)*axis[0];
    double a1 = length[1]*cos(radians);
    double a2 = length[2]*cos(radians);
    ellipse (volume, c, axis[1], a1, axis[2], a2);
  }
}

void ellipsoid ( pgplot::Plot3D& volume, Cartesian centre,
		 Cartesian x0, double a0,
		 Cartesian x1, double a1,
		 Cartesian x2, double a2 )
{
  Cartesian axis[3] = { x0, x1, x2 };
  double length[3] = { a0, a1, a2 };

  ellipsoid (volume, centre, axis, length);
}

static pgplot::Plot3D volume;

void draw_axis (double height, double length)
{
  cpgsls(2);
  volume.move (Cartesian(-length,0,height));
  volume.draw (Cartesian(length,0,height));
  cpgsls(1);
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

  float max = 5.0;

  float xfactor = 1.0;
  float yfactor = 1.0;

  if (aspect_ratio > 1)
    yfactor = aspect_ratio;
  else
    xfactor = 1.0/aspect_ratio;
  
  cpgswin (-max*xfactor, max*xfactor,
	   -max*yfactor, max*yfactor);

  float longitude0 = 90;
  float latitude0 = 0;

  float longitude = 65;
  float latitude = 25;

  volume.set_camera (longitude0, latitude0);

  Cartesian origin (0,0,0);
  Cartesian x0 (1,0,0);
  Cartesian y0 (0,1,0);
  Cartesian z0 (0,0,1);

  double dot_ch = 8.0;

  double axis = 3.0;
  
  double S0 = 0.8;
  double P = 0.7;

  double norm = sqrt(S0*S0+P*P);
  double inv = sqrt(S0*S0-P*P);

  // //////////////////////////////////////////////////////////////////
  //
  // Single mode
  //
  // //////////////////////////////////////////////////////////////////

  double single = 3;
  ellipsoid (volume, Cartesian(1.5,0,single),
	     x0, norm,
	     y0, inv,
	     z0, inv);

  cpgsch(dot_ch);
  volume.plot (Cartesian(0,0,single), 1);

  draw_axis (single, axis);
  
  // //////////////////////////////////////////////////////////////////
  //
  // Disjoint modes
  //
  // //////////////////////////////////////////////////////////////////

  double disjoint = 1.5;
  ellipsoid (volume, Cartesian(-1.5,0,disjoint),
	     x0, norm,
	     y0, inv,
	     z0, inv);

  ellipsoid (volume, Cartesian(1.5,0,disjoint),
	     x0, norm,
	     y0, inv,
	     z0, inv);

  cpgsch(dot_ch);
  volume.plot (Cartesian(0,0,disjoint), 1);

  draw_axis (disjoint, axis);

  // //////////////////////////////////////////////////////////////////
  //
  // Composite modes
  //
  // //////////////////////////////////////////////////////////////////

  double composite = 0;
  ellipsoid (volume, Cartesian(0,0,composite),
	     x0, norm,
	     y0, inv,
	     z0, inv);

  cpgsch(dot_ch);
  volume.plot (Cartesian(0,0,composite), 1);

  draw_axis (composite, axis);

  // //////////////////////////////////////////////////////////////////
  //
  // Superposed modes
  //
  // //////////////////////////////////////////////////////////////////

  S0 = 2*S0;
  P = 0;
  norm = S0;
  inv = S0;

  double superposed = -2.5;
  ellipsoid (volume, Cartesian(0,0,superposed),
	     x0, norm,
	     y0, inv,
	     z0, inv);

  cpgsch(dot_ch);
  volume.plot (Cartesian(0,0,superposed), 1);

  draw_axis (superposed, axis);
    
  cpgsch(1.0);
  volume.set_camera (longitude0, latitude0);

  double offset = 0.25;
  volume.text (Cartesian(-4.5,0,single+offset), "Single Mode", 1);
  volume.text (Cartesian(-4.5,0,disjoint+offset), "Disjoint", 1);
  volume.text (Cartesian(-4.5,0,composite+offset), "Composite", 1);
  volume.text (Cartesian(-4.5,0,superposed+offset), "Superposed", 1);


  cpgend();

  return 0;
}

