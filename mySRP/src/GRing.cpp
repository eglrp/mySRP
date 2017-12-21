//
//  GRing.cpp
//  myRayTracing
//
//  Created by lizhen on 25/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GRing.hpp"

// computing the AABB bounding box of the ring
void GRing::computeBoundingBox()
{
    //building the local coordinate system
    cs = center;
    cx = vertices[0] - center;
    cx.normalise();
    GVector a1 = vertices[1] - center;
    cz = crossproduct(cx, a1);
    cz.normalise();
    cy = crossproduct(cz, cx);
    cy.normalise();
    
    //calculating bounding box
    // the edge points on local coordinate system
    
    boundingBox.minLimit_l.set(-radius_external, -radius_external, 0.0);
    boundingBox.maxLimit_l.set( radius_external,  radius_external, 0.0);
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

bool GRing::intersection(GRay &ray, gfc::GVector &intersection, gfc::GVector &normal)
{
    bool ret = false;
    GVector x0 = center;
    GVector p = ray.start - x0;
    double ptn = p.x * cz.x + p.y * cz.y + p.z * cz.z;
    double Ntn = ray.direction.x * cz.x + ray.direction.y * cz.y + ray.direction.z * cz.z ;
    
    //assuming the normal of the vector is in cz direction
    // the ray is from the back side, according to marek's phd thesis, this ray should be rejected
//    if(Ntn > 0.0)
//    {
//        return false;
//    }
    
    
    if( fabs(Ntn) < GGeometry::EPS ) // ray is parallel to the plane
    {
        return false;
    }
    
    if( fabs(ptn)< GGeometry::EPS)  // ray starts from the plane
    {
        return false;
    }
    
    double t = - ptn / Ntn;
    
    if(t<=0.0)
    {
        return false;
    }
    
    if(fabs(t)<GGeometry::EPS)
    {
        return false;
    }
    
    // the possible intersection
    GVector candidate = ray.start + t*ray.direction;
    
    //test if candidate is inside the ring
    double dis = (candidate - center).norm();
    if(   dis <= radius_external
       && dis >= radius_internal)
    {
        intersection = candidate;
        
        normal = cz;
        // the ray is from the back side // the ray is from the back side, according to marek's phd thesis, this ray should be rejected
        if(Ntn >= 0.0 )
        {
            normal = -cz;
        }
        else  // ray is from the front side
        {
            normal = cz;
        }
        
        return true;

    }
    
    return ret;
}
