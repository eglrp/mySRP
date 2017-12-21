//
//  GObjects.cpp
//  myRayTracing
//
//  Created by lizhen on 23/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GScene.hpp"


/*
 compute the Bounding Box according to the bounding box(bfs) of every geometry
 */
void GScene::computeBoundingBox()
{
    // all the components are in BFS coordinate system
    // because GObjects are in BFS, that means the local bounding box is the same as BFS bounding box
    cs.set(0, 0, 0);
    cx.set(1, 0, 0);
    cy.set(0, 1, 0);
    cz.set(0, 0, 1);
    
    for( int i = 0 ; i< components.size(); i++ )
    {
        GVector& p = components[i]->boundingBox.minLimit;
        
        boundingBox.updateBoundingBoxLimits(p, boundingBox.minLimit_l, boundingBox.maxLimit_l);
        GVector& q = components[i]->boundingBox.maxLimit;
        
        boundingBox.updateBoundingBoxLimits(q, boundingBox.minLimit_l, boundingBox.maxLimit_l);
        
    }
    
    //because all the components in this scene are in BFS
    // there is no need to convert back, get the AABB of this scene in boundingBox;
    GVector box[8];
    boundingBox.generateLocalBox(box);
    
    GVector pnt;
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


void GScene::loadUserfile(std::string filename)
{
    std::ifstream infile(filename);
    if(infile.fail())
    {
        printf("the UCL userfile open failed!\n");
        exit(0);
    }
    
    int charsize = 1024;
    
    
    char temp[10]={};
    
    int record_num = 0;
    int mli_type= 0; // 0 for not covered MLI, 1 for MLI
    int component_group_number = 0;
    long count = 0;
    
    // this should be set according to the userfile, the emissivity(infrared) for MLI is 0.84
    double myemissivity = 0.84;
    
    while(infile)
    {
        
        char line[1024]={};
        char line2[1024]={};
        
        char *tmp = NULL;
        infile.getline(line, charsize);
        
        infile.getline(line2, charsize);
        
        if(infile.eof()) break;
        
        if ((tmp = strstr(line, "\n")))
            *tmp = '\0';
        //去掉回车符
        if ((tmp = strstr(line, "\r")))
            *tmp = '\0';
        
        
        if ((tmp = strstr(line2, "\n")))
            *tmp = '\0';
        //去掉回车符
        if ((tmp = strstr(line2, "\r")))
            *tmp = '\0';
        
        // check number of items scanned from sscanf return value
        sscanf(line2,"%d %s %d %d", &record_num, temp, &mli_type, &component_group_number);
        
        
        if(!strcmp(line, "GENERAL"))
        {
            auto general =  std::make_unique<GPolygon>();
            int num = 0;
            infile >> num;
            general->vertices.resize(num);
            for( int count = 0; count < num; count++)
            {
                infile >> general->vertices[count].x >>general->vertices[count].y >> general->vertices[count].z ;
            }
            
            infile >> general->op.solar_reflectivity;
            infile >> general->op.solar_specularity;
            
            general->op.ir_emissivity = myemissivity;
            general->op.solar_absorptivity = 1.0 - general->op.solar_reflectivity;
            
            //currently, there is only 1 type of MLI considering in the software
            general->op.mli_type = mli_type;
            //general->isMLI = material_type;
            
            general->index = count++;
            
            
            //general->op.solar_specularity = 0.5;
            
            components.push_back( std::move(general)   );
            //components.push_back( general   );
            
        }
        else if(!strcmp(line, "CYL_X"))
        {
            auto cylinder = std::make_unique<GCylinder>();
            infile >> cylinder->radius;
            infile >> cylinder->vertices[0].x >> cylinder->vertices[0].y >> cylinder->vertices[0].z;
            infile >> cylinder->vertices[1].x >> cylinder->vertices[1].y >> cylinder->vertices[1].z;
            infile >> cylinder->op.solar_reflectivity;
            infile >> cylinder->op.solar_specularity;
            
            cylinder->index = count++;
            
            cylinder->op.mli_type = mli_type;
            
            cylinder->op.ir_emissivity = myemissivity;
            cylinder->op.solar_absorptivity = 1.0 - cylinder->op.solar_reflectivity;
            
            //cylinder->op.solar_specularity = 0.5;
            
            components.push_back( std::move(cylinder)   );
            
        }
        else if(!strcmp(line, "CIRCLE"))
        {
            auto cirlce =  std::make_unique<GDisc>() ;
            
            cirlce->vertices.resize(2);
            
            infile >> cirlce->radius;
            infile >> cirlce->center.x >> cirlce->center.y >> cirlce->center.z;
            
            infile >> cirlce->vertices[0].x >> cirlce->vertices[0].y >> cirlce->vertices[0].z;
            infile >> cirlce->vertices[1].x >> cirlce->vertices[1].y >> cirlce->vertices[1].z;
            
            infile >> cirlce->op.solar_reflectivity;
            infile >> cirlce->op.solar_specularity;
            
            cirlce->index = count++;
            cirlce->op.mli_type = mli_type;
            
            cirlce->op.ir_emissivity = myemissivity;
            cirlce->op.solar_absorptivity = 1.0 - cirlce->op.solar_reflectivity;
            
            components.push_back( std::move(cirlce)   );
            
        }
        else if(!strcmp(line, "RING"))
        {
            auto ring =  std::make_unique<GRing>() ;
            
            ring->vertices.resize(2);
            
            infile >> ring->radius_external;
            
            infile >> ring->radius_internal;
            
            infile >> ring->center.x >> ring->center.y >> ring->center.z;
            
            infile >> ring->vertices[0].x >> ring->vertices[0].y >> ring->vertices[0].z;
            infile >> ring->vertices[1].x >> ring->vertices[1].y >> ring->vertices[1].z;
            
            infile >> ring->op.solar_reflectivity;
            infile >> ring->op.solar_specularity;

            ring->index = count++;
            
            ring->op.mli_type = mli_type;
            
            ring->op.ir_emissivity = myemissivity;
            ring->op.solar_absorptivity = 1.0 - ring->op.solar_reflectivity;
            
            components.push_back( std::move(ring)   );
            
        }
        else if(!strcmp(line, "SPHERE"))
        {
            auto sphere =  std::make_unique<GSphere>() ;
            //the second line
            infile >>  sphere->radius;
            infile >> sphere->centre.x >> sphere->centre.y >> sphere->centre.z;
            infile >> sphere->op.solar_reflectivity >> sphere->op.solar_specularity ;
            sphere->radius_squared = sphere->radius*sphere->radius;
            
            sphere->index = count++;
            
            //sphere->isMLI = material_type;
            
            sphere->op.mli_type = mli_type;
            
            sphere->op.ir_emissivity = myemissivity;
            sphere->op.solar_absorptivity = 1.0 - sphere->op.solar_reflectivity;
            
            components.push_back( std::move(sphere)   );
            
        }
        
        else if(!strcmp(line, "CONEI") || !strcmp(line, "CONEO") )
        {
            auto cone =  std::make_unique<GCone>() ;
            
            if(!strcmp(line, "CONEI"))
            {
               cone->in_out = true;
            }
            
            //the second line
            infile >>  cone->depth;
            cone->height = cone->depth;
            
            infile >>  cone->radius_b;
            
            infile >> cone->center_b.x >> cone->center_b.y >> cone->center_b.z ;
            infile >> cone->center_t.x >> cone->center_t.y >> cone->center_t.z ;
            
            infile >> cone->op.solar_reflectivity  >> cone->op.solar_specularity ;
            
            cone->index = count++;
            
            //cone->isMLI = material_type;
            
            cone->op.mli_type = mli_type;
            
            cone->op.ir_emissivity  = myemissivity;
            cone->op.solar_absorptivity = 1.0 - cone->op.solar_reflectivity;
            
            components.push_back( std::move(cone)   );
        }
        else if(!strcmp(line, "TCONEO") || !strcmp(line, "TCONEI"))
        {
            auto cone =  std::make_unique<GCone>() ;
            
            if(!strcmp(line, "TCONEI"))
            {
                cone->in_out = true;
            }
            
            //the second line
            
            infile >>  cone->depth; // the distance from the bottom to the top vertex(which is a virtual point in this case)
            infile >>  cone->radius_b;
            infile >>  cone->height;
            
            //calculate the radius of the top
            cone->radius_t = cone->radius_b*(cone->depth - cone->height)/cone->depth;
            
            infile >> cone->center_b.x >> cone->center_b.y >> cone->center_b.z ;
            infile >> cone->center_t.x >> cone->center_t.y >> cone->center_t.z ;
            
            infile >> cone->op.solar_reflectivity >> cone->op.solar_specularity ;
            
            cone->index = count++;
            
            //cone->isMLI = material_type;
            cone->op.mli_type = mli_type;
            
            cone->op.ir_emissivity = myemissivity;
            cone->op.solar_absorptivity = 1.0 - cone->op.solar_reflectivity;
            
            components.push_back( std::move(cone)   );
            
            
        }
        else if(!strcmp(line, "TCONEI"))
        {
            auto cone =  std::make_unique<GCone>() ;
            cone->in_out = true;
            
            //the second line
            double depth = 0.0;
            infile >>  depth;
            infile >>  cone->radius_b;
            infile >>  cone->height;
            
            //calculate the radius of the top
            cone->radius_t = cone->radius_b*(depth - cone->height)/depth;
            
            infile >> cone->center_b.x >> cone->center_b.y >> cone->center_b.z ;
            infile >> cone->center_t.x >> cone->center_t.y >> cone->center_t.z ;
            
            infile >> cone->op.solar_reflectivity >> cone->op.solar_specularity ;
            
            cone->index = count++;
            
            //cone->isMLI = material_type;
            cone->op.mli_type = mli_type;
            
            cone->op.ir_emissivity = myemissivity;
            cone->op.solar_absorptivity = 1.0 - cone->op.solar_reflectivity;
            
            components.push_back( std::move(cone)   );
        }
        else if(!strcmp(line, "PARABO") || !strcmp(line, "PARABI") )
        {
            auto paraboloid =  std::make_unique<GParaboloid>() ;
            
            if(!strcmp(line, "PARABI"))
            {
                paraboloid->in_out = true;
            }
            
            //the second line
            infile >>  paraboloid->depth;
            infile >>  paraboloid->radius_b;
            paraboloid->height = paraboloid->depth;
            
            //calculate the radius of the top
            paraboloid->radius_t = 0.0;
            
            infile >> paraboloid->center_b.x >> paraboloid->center_b.y >> paraboloid->center_b.z ;
            infile >>paraboloid->center_t.x >> paraboloid->center_t.y >> paraboloid->center_t.z ;
            
            infile >> paraboloid->op.solar_reflectivity >> paraboloid->op.solar_specularity ;
            
            paraboloid->index = count++;
            
            //paraboloid->isMLI = material_type;
            
            paraboloid->op.mli_type = mli_type;
            
            paraboloid->op.ir_emissivity = myemissivity;
            paraboloid->op.solar_absorptivity = 1.0 - paraboloid->op.solar_reflectivity;
            
            components.push_back( std::move(paraboloid)   );
            
        }
        else if(!strcmp(line, "TPARABI") || !strcmp(line, "TPARABO"))
        {
            auto paraboloid =  std::make_unique<GParaboloid>() ;
            
            if( !strcmp(line, "TPARABI") )
            {
                paraboloid->in_out = true;
            }
            //the second line
            infile >>  paraboloid->depth;
            infile >>  paraboloid->radius_b;
            
            infile >> paraboloid->height;
            //calculate the radius of the top
            paraboloid->radius_t = (paraboloid->depth - paraboloid->height)*paraboloid->radius_b*paraboloid->radius_b/paraboloid->depth;
            
            paraboloid->radius_t = sqrt(paraboloid->radius_t);
            
            infile >> paraboloid->center_b.x >> paraboloid->center_b.y >> paraboloid->center_b.z ;
            infile >>paraboloid->center_t.x >> paraboloid->center_t.y >> paraboloid->center_t.z ;
            
            infile >> paraboloid->op.solar_reflectivity >> paraboloid->op.solar_specularity ;
            
            paraboloid->index = count++;
            
            //paraboloid->isMLI = material_type;
            
            paraboloid->op.mli_type = mli_type;
            
            paraboloid->op.ir_emissivity = myemissivity;
            paraboloid->op.solar_absorptivity = 1.0 - paraboloid->op.solar_reflectivity;
            
            components.push_back( std::move(paraboloid)   );
            
        }
        else
        {
            printf("severe warning: unrecognised goemtry: %s !\n",line);
        }
        
        // to avoid reading the blank line
        infile.getline(line, charsize); // read the last line
        
    } // end of while
    
    
    //updating the Axis aligned bounding box of every geometry
    for( int i = 0 ; i< components.size() ; i++ )
    {
        components[i]->computeBoundingBox();	
    }
    
    //update the bounding box of GObjects itself, find the AABB for the whole satellite
    // the AABB for the whole satellite will be used to determine the size of the pixel array
    computeBoundingBox();
    
    boundingBox.computeCenter();
    
    double aabbSize = (boundingBox.maxLimit - boundingBox.minLimit).norm();
    
    double dis = 1.0*(boundingBox.center.norm() + aabbSize);
    
    GPixelArray::d0 = dis;
    
    //compute the thermal effect due to the satellite internal temperature
    // MLI has a very low emissivity, NOT zero
    // this can not be done in the ray tracer, because ray tracer only process the parts that are illuminated by the sun
    // 关键点是如何确定每个几何体的法向量，因为热辐射力沿着法向量的反方向，在射线追踪中可以很容易确定，这里不能调用射线追踪算法，只能从输入的userfile中来确定
    //updating the Axis aligned bounding box of every geometry
//    for( int i = 0 ; i< components.size() ; i++ )
//    {
//        if(components[i]->op.mli_type)
//        {
//            
//        }
//    }
    
}
