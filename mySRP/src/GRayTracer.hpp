//
//  GRayTracing.hpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GRayTracing_hpp
#define GRayTracing_hpp

#include <stdio.h>

#include "GKDTree.hpp"
#include "GPixelArray.hpp"
#include "GScene.hpp"

#include <memory>

//#define USE_KDTREE

/*
 some tips and tutorial can be found at

 https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/parametric-and-implicit-surfaces

*/

// this is the class for finish the ray tracing process
class GRayTracer {
 public:
  static int MAX_DEPTH;
  static double CLIGHT;
  static double SOLAR_CONST;
  static double sigma;  // stepan-bolzman constan
  static double T4;     // // Kelvin, the 4th power of temperature inside the
                        // satellite bus, should be read from configure file
  static double T0;     // the temperature of the outer space
  static double emissivity_eff;  // the effective emissivity of the MLI, because
                                 // MLI is multi-layer
  static double bus_inside_energy;  // the satellite bus inside inner energy
  static double
      MLI_emissive_heat;  // the heat in W/m^2 that radiated from the internal
                          // satellite bus to the outer surface

  GPixelArray pixelarray;
  GScene objects;
  KDTree kdtree;  // the tree is used to reorganise the objects in GScene for
                  // fast ray tracing
  // this is only for debugging
  long rayIntersected;
  double area_hit;

  std::vector<GVector> intersectionPoints;

  // the covariance matrix
  double covariance[9];

  GRayTracer() {
    rayIntersected = 0;
    area_hit = 0.0;
    intersectionPoints.reserve(20000);
    memset(covariance, 0, sizeof(double) * 9);
  }

  void makeTree();

  int intersect(GRay& ray, GVector& intersection, GVector& normal,
                GOpticalProperty& op);

  void raytracing(GRay& ray, GVector& force, double cov[9]);

  // the solar radiaiton pressure calculation
  GVector processor(GRay& ray, GVector& normal, GVector& reflectionDirection,
                    GOpticalProperty& op, double cc[9]);
  // double specularity, double reflectivity, double emissivity, double
  // absorptivity, int isMLI);

  // the function to deal with only solar radiation pressure
  void SRP(GVector& force, GRay& ray, GVector& normal,
           GVector& reflectionDirection, double specularity,
           double reflectivity);

  GVector SRP_t(GRay& ray, GVector& normal, GVector& reflectionDirection,
                double specularity, double reflectivity);

  void SRP_cov(GRay& ray, GVector& normal, GVector& reflectionDirection,
               double specularity, double reflectivity, double specularity_cov,
               double reflectivity_cov, double cc[9]);

  // the thermal reradiaiton pressure calculation
  void TRR_MLI(GRay& ray, GVector& normal, double emissivity,
               double abosrbtivity, GVector& force);

  // the other version of MLI radiation force
  void TRR_MLI2(GRay& ray, GVector& normal, double emissivity,
                double abosrbtivity, GVector& force);

  void TRR_regular(GVector& force, GRay& ray, GVector& normal,
                   GVector& reflectionDirection, double specularity,
                   double reflectivity);
  /*
   run the ray tracing process
   */
  GVector run();
};

#endif /* GRayTracing_hpp */
