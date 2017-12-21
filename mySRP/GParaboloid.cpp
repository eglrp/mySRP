//
//  GParaboloid.cpp
//  myRayTracing
//
//  Created by lizhen on 25/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GParaboloid.hpp"

void GParaboloid::computeBoundingBox()
{
    //building the local system
    cs = center_b;
    cz = center_t - center_b;
    cz.normalise();
    
    double p = dotproduct(cz, center_b);
    
    //find arbitrary point on the bottom plane pointing to the x axis
    cx.x = cs.x + 1.0;  // make this different from cs
    if( cz.y != 0.0 )
    {
        cx.z = 0.0;
        cx.y = (p - cx.x*cz.x)/cz.y;
    }
    else if( cz.z != 0.0 )
    {
        cx.y = 0.0;
        cx.z = (p - cx.x*cz.x)/cz.z;
    }
    else // both cz.y and cz.z are 0, thus cz.x can not be 0
    {
        if( p == 0.0 )
        {
            cx.y = 1.0;
            cx.z = 1.0;
        }
        else
        {
            cx.y =0.0;
            cx.z =0.0;
        }
        
        cx.x = p/cz.x;
    }
    
    cx = cx - cs;
    cx.normalise();
    
    cy = crossproduct(cz, cx);
    cy.normalise();
    
    //calculating bounding box
    // the edge points on local coordinate system
    boundingBox.minLimit_l.set(-radius_b,-radius_b,0.0);
    boundingBox.maxLimit_l.set(radius_b, radius_b, height);
    
    GVector box[8];
    GVector pnt;
    boundingBox.generateLocalBox(box);
    
    //convert back the local bounding box limits to the BFS
    //convert the local box to BFS box and update limits to get the AABB in BFS
    // //convert back the local bounding box limits to the BFS
    for( int i = 0 ; i< 8 ; i++ )
    {
        pnt.x = cx.x*box[i].x + cy.x*box[i].y + cz.x*box[i].z;
        pnt.y = cx.y*box[i].x + cy.y*box[i].y + cz.y*box[i].z;
        pnt.z = cx.z*box[i].x + cy.z*box[i].y + cz.z*box[i].z;
        
        pnt += cs;
        boundingBox.updateBoundingBoxLimits(pnt, boundingBox.minLimit, boundingBox.maxLimit);
    }
    
    boundingBox.computeCenter();
    
}


bool GParaboloid::intersection(GRay &ray, gfc::GVector &intersection, gfc::GVector &normal)
{
    bool ret = false;
    
    GVector ray_origin_local, ray_dir_local;
    GVector uvw;
    
    //first, convert the ray into geometry local system
    uvw = ray.start - cs;
    ray_origin_local.x = cx.x*uvw.x + cx.y*uvw.y + cx.z*uvw.z;
    ray_origin_local.y = cy.x*uvw.x + cy.y*uvw.y + cy.z*uvw.z;
    ray_origin_local.z = cz.x*uvw.x + cz.y*uvw.y + cz.z*uvw.z;
    
    uvw = ray.direction;
    ray_dir_local.x = cx.x*uvw.x + cx.y*uvw.y + cx.z*uvw.z;
    ray_dir_local.y = cy.x*uvw.x + cy.y*uvw.y + cy.z*uvw.z;
    ray_dir_local.z = cz.x*uvw.x + cz.y*uvw.y + cz.z*uvw.z;
    ray_dir_local.normalise();
    
    //solve the quadratic equation about parameter t
    // z=-h/r^2(x^2+y^2) +h  &&
    
    double r = radius_b;
    double h = depth;
    
    double A = -( ray_dir_local.x*ray_dir_local.x + ray_dir_local.y*ray_dir_local.y);
    double B = -( 2.0*ray_origin_local.x*ray_dir_local.x + 2.0*ray_origin_local.y*ray_dir_local.y + r*r/h*ray_dir_local.z );
    double C = r*r - ray_origin_local.x*ray_origin_local.x - ray_origin_local.y*ray_origin_local.y - r*r/h*ray_origin_local.z;
    
    double t=0.0,t0 = 0.0, t1 =0.0;
    
    if( fabs(A) < GGeometry::EPS ) // the ray is parallerl to the z axis, only one solution
    {
        t = -C/B;
    }
    else
    {
        double discriminent = B*B - 4.0*A*C;
        if( discriminent <= 0.0 )
        {
            return  false;
        }
        else
        {
            t0 = (-B + sqrt(discriminent))/2.0/A;
            t1 = (-B - sqrt(discriminent))/2.0/A;
            
            //t = t0<=t1?t0:t1;
            if (t0 >= t1) std::swap(t0, t1);
            if( t0 >= 0.0 )
            {
                t = t0;
            }
            else
            {
                if( t1 >= 0.0 )
                {
                    t = t1;
                }
                else
                {
                    return  false;
                }
            }

            
        }
        
    }
    
    if(t < 0.0)
    {
        return false;
    }
    
    if(fabs(t)<GGeometry::EPS)
    {
        return false;
    }
    
    //then check the z coorindate
    GVector candidate_local = ray_origin_local + t*ray_dir_local;
    if( candidate_local.z <=0.0 || candidate_local.z >= height )
    {
        return false;
    }
    
    //calculate the normal in geometry local system
    GVector normal_local(0.0, 0.0,1.0); // this normal is pointing to the outward direction
    double temp = 2.0*h/r/r;
    normal_local.x = candidate_local.x * temp;
    normal_local.y = candidate_local.y * temp;
    normal_local.z = 1.0;
    
    //temp = dotproduct(normal_local, GVector(0,0,1));
    
    if( in_out == true ) // the inward surface of the cone , dotproduct with (0,0,1) should be < 0
    {
        normal_local = -normal_local;
    }
    
    //if the ray hit the opposite of the normal surface, it should reture false
    // sometimes, we need the surface to be both sides
    temp = dotproduct(normal_local, ray_dir_local);
    if(temp >= 0.0 )
    {
        normal_local = -normal_local;
        //return false;
    }
    
    //convert the candidate back to the BFS coordinate
    
    intersection.x = cx.x*candidate_local.x + cy.x*candidate_local.y + cz.x*candidate_local.z;
    intersection.y = cx.y*candidate_local.x + cy.y*candidate_local.y + cz.y*candidate_local.z;
    intersection.z = cx.z*candidate_local.x + cy.z*candidate_local.y + cz.z*candidate_local.z;
    intersection += cs;
    
    //convert the normal_local to the BFS coordinate, note that normal vector is a vector , the transformation is different from a point
    normal.x = cx.x*normal_local.x + cy.x*normal_local.y + cz.x*normal_local.z;
    normal.y = cx.y*normal_local.x + cy.y*normal_local.y + cz.y*normal_local.z;
    normal.z = cx.z*normal_local.x + cy.z*normal_local.y + cz.z*normal_local.z;
    normal.normalise();
    ret = true;
    
    
    return ret;
}
