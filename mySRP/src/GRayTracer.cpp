//
//  GRayTracing.cpp
//  myRayTracing
//
//  Created by lizhen on 24/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GRayTracer.hpp"

// the maximum reflection number
int GRayTracer::MAX_DEPTH = 1;
double GRayTracer::CLIGHT = 299792458;
double GRayTracer::SOLAR_CONST = 1368.0;
double GRayTracer::sigma = 5.670367E-8; // W/m^2/K^4
double GRayTracer::T4 = 298.0*298.0*298.0*298.0;  // Kelvin to the power 4
double GRayTracer::T0 = 2.7*2.7*2.7*2.7;  // the temperature of the outer space is 2.7 Kelvin
double GRayTracer::emissivity_eff = 0.02;  // the infrared effective emissivity, should be read from configure file

//this value is specific for TRR calculation, and it is a constant need to be set in the configure file
double GRayTracer::bus_inside_energy = GRayTracer::emissivity_eff*GRayTracer::sigma*GRayTracer::T4/GRayTracer::SOLAR_CONST;

double GRayTracer::MLI_emissive_heat = GRayTracer::sigma*GRayTracer::emissivity_eff*(GRayTracer::T4-GRayTracer::T0);


//after calling this function, the objects.component is invalide
void GRayTracer::makeTree()
{
    kdtree.buildKDTree(objects.components);
}


//calculate the uncertainty of the srp froce in ray tracing
// the method is described briefly here
// linearise the formular of the srp calculation
// apply the error propagation law
// for every ray, the covariance matrix is generated
void GRayTracer::SRP_cov(GRay& ray, GVector& normal , GVector& reflectionDirection,
                            double specularity, double reflectivity,
                            double specularity_cov, double reflectivity_cov,
                            double cc[9])
{
    double dUV = 0.0; // the co-variance betweeen reflectiviy v and specularity u

    GVector dfdv = -reflectionDirection*specularity - 2.0/3.0*(1.0-specularity)*normal;

    GVector dfdu = -reflectionDirection*reflectivity + 2.0/3.0*reflectivity*normal;

    double tmp[6]={0.0}; // 3 by 2

    tmp[0] = dfdv.x * reflectivity_cov + dfdu.x * dUV;
    tmp[1] = dfdv.x * dUV + dfdu.x * specularity_cov;

    tmp[2] = dfdv.y*reflectivity_cov + dfdu.y*dUV;
    tmp[3] = dfdv.y*dUV + dfdu.y*specularity_cov;

    tmp[4] = dfdv.z*reflectivity_cov + dfdu.z*dUV;
    tmp[5] = dfdv.z*dUV + dfdu.z*specularity_cov;

    cc[0] = tmp[0]*dfdv.x + tmp[1]*dfdu.x;
    cc[1] = tmp[0]*dfdv.y + tmp[1]*dfdu.y;
    cc[2] = tmp[0]*dfdv.z + tmp[1]*dfdu.z;

    cc[3] = tmp[2]*dfdv.x + tmp[3]*dfdu.x;
    cc[4] = tmp[2]*dfdv.y + tmp[3]*dfdu.y;
    cc[5] = tmp[2]*dfdv.z + tmp[3]*dfdu.z;

    cc[6] = tmp[4]*dfdv.x + tmp[5]*dfdu.x;
    cc[7] = tmp[4]*dfdv.y + tmp[5]*dfdu.y;
    cc[8] = tmp[4]*dfdv.z + tmp[5]*dfdu.z;

}

//this funciton only deal with solar radiation pressure
GVector GRayTracer::SRP(GRay& ray, GVector& normal , GVector& reflectionDirection,
                     double specularity, double reflectivity)
{
    GVector force(0.0, 0.0, 0.0);

    //**********************************************************************************************************
    //the cos_theta has been considered in this method itself, so don't need it, but it sounds a bit confusing!
    //**********************************************************************************************************
    // this still need under debate

    //double cos_theta = -dotproduct(ray.direction, normal)/normal.norm()/ray.direction.norm();


    //assuming the solar flux and area are both 1.0;
    //double factor = ray.energy*cos_theta;

    double factor = ray.energy;


    // the direct radiation force
    GVector f1 = factor*ray.direction;

    //the specular reflected radiation force
    GVector f2 = -factor*specularity*reflectivity*reflectionDirection;

    // the diffuse reflected radiation force
    GVector f3 = -2.0/3.0*factor*reflectivity*(1.0-specularity)*normal;

    force = f1 + f2 + f3;

    return force;
}

/*
 the processor for every flection
 in this case, this is the fucntion to calculate the radiation force

 !!! the default solar flux is 1.0 here !!!
 the final result need to be factorized with AW/c
 */
GVector GRayTracer::processor(GRay& ray, GVector& normal , GVector& reflectionDirection,GOpticalProperty& op, double cc[9])
                             // double specularity, double reflectivity, double emissivity, double absorptivity, int isMLI)
{
    GVector srp(0,0,0), trr(0,0,0), force(0,0,0);

    srp = SRP(ray, normal, reflectionDirection, op.solar_specularity, op.solar_reflectivity);

    //SRP_cov(ray, normal, reflectionDirection, op.solar_specularity, op.solar_reflectivity, op.solar_specularity_cov, op.solar_reflectivity_cov, cc);

    if(op.mli_type) // thermal response for the MLI material, only radiation, no conduction and convection
    {
        TRR_MLI( ray, normal, op.ir_emissivity, op.solar_absorptivity,trr);
    }
    else // thermal response for the other kind of material, including radiation and conduction, no convection in space
    {

    }

    force = srp + trr;

    return force;
}

/*
 the other MLI assumption,
 assuming that the absorpted energy will be emited in the form of heat immediantly
 this is suitable for the material that with very low heat capacity and completely prevent heat transfer toward
 the satellite interior,like MLI
 this assumption is different from the one used by UCL
 */
void GRayTracer::TRR_MLI3()
{

}

/*
 This TRR response function only account for the effect from the solar radiaiton, ignore the heat radiated from the internal satellite bus,
 because this effect has been account for when building the spacecraft geometry without ray tracing

 */
void GRayTracer::TRR_MLI2(GRay& ray, GVector& normal,double emissivity, double abosrbtivity,GVector& force)
{
    double factor = ray.energy;
    double cos_theta = -dotproduct(ray.direction, normal)/normal.norm()/ray.direction.norm();
    double mag = 0.0;

    //when cos_theta is near 0, which means ray is perpendicular to the surface normal, the force should be zero
    if(  cos_theta >= GGeometry::EPS )
    {

        mag = 2.0/3.0*emissivity*
        (factor*abosrbtivity + GRayTracer::bus_inside_energy/cos_theta)/(emissivity + GRayTracer::emissivity_eff);


        //for temperature test
        //double t4 = 1368.0*(abosrbtivity + GRayTracer::bus_inside_energy)/(emissivity + GRayTracer::emissivity_eff)/GRayTracer::sigma;
        //double mag2 = 2.0/3.0*GRayTracer::sigma*emissivity*t4/299792458.0*GPixelArray::pixelArea;

    }

    //for MLI, the direction of the TRR should be in opposite normal direction
    force = -mag*normal;

}


/*
 the function to calculate the thermal re-radiation pressure for the satellite bus MLI
 */
void GRayTracer::TRR_MLI( GRay& ray, GVector& normal,double emissivity, double abosrbtivity,GVector& force)
{
    double factor = ray.energy;
    double cos_theta = -dotproduct(ray.direction, normal)/normal.norm()/ray.direction.norm();
    double mag = 0.0;

    //when cos_theta is near 0, which means ray is perpendicular to the surface normal, the force should be zero
    if(  cos_theta >= GGeometry::EPS )
    {

        mag = 2.0/3.0*emissivity*
        (factor*abosrbtivity + GRayTracer::bus_inside_energy/cos_theta)/(emissivity + GRayTracer::emissivity_eff);


        //for temperature test
        //double t4 = 1368.0*(abosrbtivity + GRayTracer::bus_inside_energy)/(emissivity + GRayTracer::emissivity_eff)/GRayTracer::sigma;
        //double mag2 = 2.0/3.0*GRayTracer::sigma*emissivity*t4/299792458.0*GPixelArray::pixelArea;

    }

    //for MLI, the direction of the TRR should be in opposite normal direction
    force = -mag*normal;
}



/*
 find out the intersection of ray for all the components in objects
 the return value is index of component in objects
 if there is no intersection, return -1;
 intersection and normal are return value

 this function needs to be optimized with the KD tree algorithm

 */
int GRayTracer::intersect( GRay& ray,GVector& intersection, GVector& normal, GOpticalProperty& op )
{
    int objectIndex = -1;
    bool  ret = false;
    GVector myintersect, mynormal;
    //double myspecularity =0.0, myreflectivity = 0.0;
    double mindis = GBoundingBox::infinite;
    bool isChecked = false;
    GOpticalProperty myop;

    //double min_volume = 9999.9999;
    //int min_volumn_index = 0;
    // loop all the components, with KD tree, this can be much faster
    for( int i = 0 ; i< objects.components.size(); i++ )
    {
        /*
        double volumn = (objects.components[i]->boundingBox.maxLimit.x - objects.components[i]->boundingBox.minLimit.x)
                        *(objects.components[i]->boundingBox.maxLimit.y - objects.components[i]->boundingBox.minLimit.y)
                        *(objects.components[i]->boundingBox.maxLimit.z - objects.components[i]->boundingBox.minLimit.z);

        if(volumn > 0.00001)
        {
            if(volumn < min_volume) {min_volume = volumn; min_volumn_index = i;}
        }
        */

        ret = objects.components[i]->intersection(ray, myintersect, mynormal);
        //myreflectivity = objects.components[i]->reflectivity;
        //myspecularity = objects.components[i]->specularity;

        myop = objects.components[i]->op;

        if(ret == true) // if intersects, need to find the closed intersection point to ray source
        {
            double dis = (myintersect - ray.start).norm();
            if(dis <= mindis)
            {
                //不同物体的相同交点上，可能具有完全不同的法向量,这里取平均值
               if(myintersect == intersection && mynormal != normal
                  && isChecked == true
                  )
               {
                   //mynormal = 0.5*(normal + mynormal);
                   //myspecularity = 0.5*(specularity + myspecularity);
                   //myreflectivity = 0.5*(reflectivity + myreflectivity);
                   mynormal = -ray.direction;
               }

               normal = mynormal;
               intersection = myintersect;
               op = myop;
               //specularity = myspecularity;
               //reflectivity = myreflectivity;

               mindis = dis;
               isChecked = true;

               objectIndex = i;

            }
        }
    }

    return objectIndex;
}





/*

 recursive ray tracing process
 here ray has to be a reference or a pointer

 force is the final force for this ray after several reflection
 force need to be factorized with AW/c, because solar flux and area are both 1.0

 the covariance matrix of the force f is a 3 by 3 matrix, it is the output
 */
void GRayTracer::raytracing(GRay& ray, GVector& force, double cov[9])
{

    int objectIndex = -1;
    GVector intersection, normal, reflectDirection;
    GOpticalProperty op;
    // the covariance matrix
    double cc[9] = {0.0};

    //it can be controlled by either the depth or the energy of the ray
    if( ray.reflectionNum >= GRayTracer::MAX_DEPTH )
    {
        return ;
    }
    else
    {

#ifdef USE_KDTREE
        //using kdtree
        //kdtree.clear();
        if( !kdtree.intersect(ray) )
        {
            return;
        }
#else
        objectIndex = intersect(ray,intersection,normal, op);
        if( objectIndex == -1) //no intersection for this ray
        {
            return ;
        }

#endif

        else  // intersection point found
        {

#ifdef USE_KDTREE
        ///using kdtree
            op = kdtree.op;
            intersection = kdtree.intersection;
            normal      = kdtree.normal;

#else
            op = objects.components[objectIndex]->op;
#endif

            //ray.routine.push_back(intersection);
            //intersectionPoints.push_back(intersection);

            //run the processing code
            // reflectDirection is a unit vector, do not have to be normalised, make it faster
            reflectDirection= -2.0*dotproduct(ray.direction, normal)*normal + ray.direction;

            force += processor(ray, normal , reflectDirection,op,cc);

            for(int i = 0 ; i< 9;i++)
            {
                cov[i] += cc[i];
            }


            ray.start = intersection;
            //set the direction of this ray to the reflected direction
            ray.direction = reflectDirection;
            ray.inv_direction.set(1.0/reflectDirection.x, 1.0/reflectDirection.y, 1.0/reflectDirection.z);


            ray.reflectionNum++;

//            if(ray.reflectionNum >= 3)
//            {
//                int testc = 0;
//            }

            // the enegy reduced after reflection, this is only the energy of specular refelction
            ray.energy *= (op.solar_reflectivity*op.solar_specularity);

            rayIntersected++;
            area_hit += GPixelArray::pixelArea;

            //recursive call
            raytracing(ray,force,cov);

        }

    }


}


/*

 starting running the ray tracer
 */
GVector GRayTracer::run()
{

    GVector f;
    GVector force;
    GRay myray;

    // these are the first, second and third row of the covariance matrix of the final force
    double c[9] = {0.0};

    double coefficient = (GPixelArray::pixelArea*GRayTracer::SOLAR_CONST/GRayTracer::CLIGHT);
    int count = 0;
    //pixelarray.i_index = 0;
    //pixelarray.j_index = 0;

   // FILE* pf = fopen("pixelArray.js","w+");
   // FILE* intersection_file = fopen("intersections.js", "w+");

   // fprintf(pf, "pixels=[\n");
   // fprintf(intersection_file, "intersections=[\n");

    while (pixelarray.getRay(myray))
    {

       // myray.routine.clear();
       // myray.routine.push_back(myray.start);

       // fprintf(pf, "{x:%.3f,y:%.3f,z:%.3f},\n",myray.start.x,myray.start.y,myray.start.z);

        // has to be set to 0.0
        f.set(0.0, 0.0, 0.0);
        memset(c,0,sizeof(double)*9);

        raytracing(myray, f, c);

        force += f;


//        covariance[0] += c[0];covariance[1] += c[1];covariance[2] += c[2];
//        covariance[3] += c[3];covariance[4] += c[4];covariance[5] += c[5];
//        covariance[6] += c[6];covariance[7] += c[7];covariance[8] += c[8];
//
        pixelarray.totalRay++;

        //std::cout<<"hello"<<std::endl;

        //printf("%d\n", pixelarray.totalRay);


//        if( pixelarray.totalRay == 1807)
//        {
//            int testc =0;
//        }


        /*
        if(myray.routine.size() > 2)
        {
            printf("[");
            //std::cout<<"["<<std::flush;
            for( int i = 0 ; i< myray.routine.size(); i++ )
            {
                if(i == myray.routine.size()-1)
                {
                   printf("{x:%8.6f,y:%8.6f,z:%8.6f}",myray.routine[i].x,myray.routine[i].y,myray.routine[i].z);
                }
                else
                {
                    printf("{x:%8.6f,y:%8.6f,z:%8.6f},",myray.routine[i].x,myray.routine[i].y,myray.routine[i].z);
                }

            }
            printf("],\n");
        }
        */

        //printf("%5d\n",myray.routine.size());
        //for(int i = 0 ; i< myray.routine.size(); i++)
        //printf("%ld %.16f %.16f %.16f %.16f \n", myray.routine.size(),f.x , f.y, f.z,f.norm());



    }

   // fprintf(pf, "];\n");
   // fclose(pf);

    //debugging for all the intersection points
//    int testc = 0;
//
//    for(int i = 0 ; i< intersectionPoints.size(); i++ )
//    {
//        fprintf(intersection_file,"{x:%8.6f,y:%8.6f,z:%8.6f},\n", intersectionPoints[i].x, intersectionPoints[i].y, intersectionPoints[i].z);
//    }
//    fprintf(intersection_file, "];\n");
//    fclose(intersection_file);

//    // may be we can a way to optimize the number of rays to be looped
//    // consider using multi-thread or muli-core cpu or GPU to to this
//    for( int i = 0 ; i< pixelarray.rays.size(); i++ ) // loop all the rays
//    {
//        // has to be set to 0.0
//        f.set(0.0, 0.0, 0.0);
//        raytracing(pixelarray.rays[i], f);
//
////        if(i == 1229)
////        {
////            int testc =0;
////        }
////        printf("%d %.16f %.16f %.16f \n",i,f.x , f.y, f.z);
//
//        force += f;
//    }
//




    //after processing all the rays
    // do the factorization
    force *= coefficient;

    for(int i = 0 ; i< 9; i++)
    {
        covariance[i] *= coefficient;
    }

    //after the ray tracing, add in the thermal effect of the MLI that are not illuminated by the sun



    return force;
}
