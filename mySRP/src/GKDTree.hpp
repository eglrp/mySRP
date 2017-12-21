//
//  GKDTree.hpp
//  myRayTracing
//
//  Created by lizhen on 26/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#ifndef GKDTree_hpp
#define GKDTree_hpp

#include <stdio.h>
#include "GGeometry.hpp"
#include <vector>
#include <string>

class KDNode
{
    
public:
    
    GBoundingBox aabb; // the aabb for this node
    //int currentDepth;
    //std::vector<GGeometry*> geometryList;
    
    std::vector<std::unique_ptr<GGeometry> > geometryList;
    
    std::unique_ptr<KDNode> left;
    std::unique_ptr<KDNode> right;
    
    double  splitPos;  // the split position, x = splitPos, y=splitPos or z=splitPos
    unsigned int splitAxis; // the split axis,0 for x, 1 for y and 2 for z
    bool leaf; // whether this node is leaf, true is leaf, false isn't
    
    KDNode()
    {
        //geometryList = geo;
        //geometryList.insert( std::move( geo.begin()), std::move(geo.end()));
        
//        for(int i = 0 ; i< geo.size() ; i++ )
//        {
//            geometryList.push_back(std::move(geo[i]));
//        }
        
        leaf = false;
        left = NULL;
        right = NULL;
        splitPos = 0.0;
        splitAxis = 0;
    }
    
    
    void getSplitPlane();
    
    void split( std::vector<std::unique_ptr<GGeometry>>& geo,
                       std::vector<std::unique_ptr<GGeometry>>& myleft,
                       std::vector<std::unique_ptr<GGeometry>>& myright);
    
    
    void insert(std::vector<std::unique_ptr<GGeometry> >& geo,int& depth);
    
};


/* the kd tree class
 
 !!!!!!!                 !!!!!!
 !!!!!!!  very important !!!!!!
    because I am using unique_prt, after building the tree, all the memroy in GScene::objects will be released, GRayTracer:: objects is not accessable any more !!!!!!
 every operation on the objects should through the tree!!!!!!!
 **************************************************************************************************
 
 */
class KDTree
{
    
public:
    
    std::unique_ptr<KDNode> root;
    int currentDepth;  // this is used for the building of the tree
    
    double mindis;
    
    GOpticalProperty op;
    
    GVector intersection;
    GVector normal;
    //bool isMLI;
    bool isChecked;
    
    KDTree()
    {
        root.reset(new KDNode()) ;
        currentDepth = 0;
        
        isChecked = false;
        //isMLI = false;
        mindis =  GBoundingBox::infinite;
    }
    
    //clear the information stroed in this node
    void clear();
    
    
    int getTreeDepth();
    
    
    // 用递归方法求树的高度
    int Depth(std::unique_ptr<KDNode>& T);
    
    //this function just return root Node
    void buildKDTree(std::vector<std::unique_ptr<GGeometry> >& geo);
    
    GVector getSplitPlane(  std::unique_ptr<KDNode>& node);
    
    //split the geo into left and right parts
    void split( std::vector<std::unique_ptr<GGeometry>>& geo, std::unique_ptr<KDNode>& node,
                std::vector<std::unique_ptr<GGeometry>>& myleft,
                std::vector<std::unique_ptr<GGeometry>>& myright);
    
    void LevelTraverse(std::unique_ptr<KDNode>& node);
    void PrintNodeAtLevel(std::unique_ptr<KDNode>& node,int level);
    void showTree2();
    
    void showTree( std::unique_ptr<KDNode>& node, int tag, int& depth);
    
    bool intersect(GRay& ray);
    
    bool tranverse(std::unique_ptr<KDNode>& node, GRay& ray);
    
    
    
    
};




#endif /* GKDTree_hpp */
