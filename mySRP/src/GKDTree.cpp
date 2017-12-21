//
//  GKDTree.cpp
//  myRayTracing
//
//  Created by lizhen on 26/02/2017.
//  Copyright © 2017 lizhen. All rights reserved.
//

#include "GKDTree.hpp"

void KDNode::getSplitPlane()
{
    // here 3 is the dimension of the data, for geometry, it is 3
    //交替的选择划分的平面
    //splitAxis = depth % 3;
    
    //选择最长的轴作为划分平面
    double axisLength[3] ={ aabb.maxLimit.x - aabb.minLimit.x,
                            aabb.maxLimit.y - aabb.minLimit.y,
                            aabb.maxLimit.z - aabb.minLimit.z
                          };
    
    double len = axisLength[0];
    splitAxis = 0;
    for( int i = 0 ; i< 3; i++ )
    {
        if(axisLength[i] >= len )
        {
            len = axisLength[i];
            splitAxis = i;
        }
    }
    
//    GVector mycenter;
//    //get the center of all the ceneters of AABB
//    for(int i = 0 ; i< geo.size(); i++ )
//    {
//       mycenter += geo[i]->boundingBox.center;
//    }
//    mycenter = mycenter/geo.size();
    
    //x axis
    if( splitAxis == 0 )
    {
        splitPos = 0.5*( aabb.minLimit.x + aabb.maxLimit.x );
        //splitPos = 0.5*mycenter.x;
    }
    else if(splitAxis == 1)  // y axis
    {
        splitPos = 0.5*( aabb.minLimit.y + aabb.maxLimit.y );
        //splitPos = 0.5*mycenter.y;
    }
    else if(splitAxis == 2)  // z axis
    {
        splitPos = 0.5*( aabb.minLimit.z + aabb.maxLimit.z );
        //splitPos = 0.5*mycenter.z;
    }

}

void KDNode::split( std::vector<std::unique_ptr<GGeometry>>& geo,
           std::vector<std::unique_ptr<GGeometry>>& myleft,
           std::vector<std::unique_ptr<GGeometry>>& myright)
{
    GVector center;
    for(int i = 0 ; i< geo.size() ; i++ )
    {
        center = geo[i]->boundingBox.center;
        
        //push_back will call copy constrcutor, which is deleted for unique_ptr
        // have to call std::move, the original object will be deleted aftering calling move
        
        //for every object, it has only two choice, left or right
        switch ( splitAxis )
        {
            case 0:
                splitPos > center.x ? myright.push_back(std::move(geo[i])) : myleft.push_back(std::move(geo[i]));
                break;
            case 1:
               splitPos  > center.y ? myright.push_back(std::move(geo[i])) : myleft.push_back(std::move(geo[i]));
                break;
            case 2:
                splitPos > center.z ? myright.push_back(std::move(geo[i])) : myleft.push_back(std::move(geo[i]));
                break;
                
            default:
                break;
        }
    }
}


void KDNode::insert(std::vector<std::unique_ptr<GGeometry> >& geo, int& depth)
{
    
    //left = NULL;
    //right = NULL;
    
    if( geo.size() == 0 )
    {
        left = NULL;
        right = NULL;
        splitAxis = -1;
        splitPos = 0.0;
        leaf = true;
        return ;
    }
        //only one geometry in the scene
    if( geo.size() == 1 )
    {
        aabb = geo[0]->boundingBox;
        geometryList.push_back(std::move(geo[0]));
        
        left = NULL;
        right = NULL;
        splitAxis = -1;
        splitPos = 0.0;
        leaf = true;
        //left.reset(new KDNode()) ;
        //right.reset(new KDNode());
        return ;
    }

    //terminate condition
    //bool stop = terminate();
    //if( stop == true )
    //{
    //return new leaf node
    //}
    
    //get a bounding box for the current node
    for( int i = 0 ; i< geo.size(); i++ )
    {
        //update the minimum
        aabb.updateBoundingBoxLimits(geo[i]->boundingBox.minLimit, aabb.minLimit, aabb.maxLimit);
        // update the maximum
        aabb.updateBoundingBoxLimits(geo[i]->boundingBox.maxLimit, aabb.minLimit, aabb.maxLimit);
    }
    
    aabb.computeCenter();
    
     // find the split plane for this node
    getSplitPlane();
    
    //get the left geo
    std::vector<std::unique_ptr<GGeometry> > myleft, myright;
    split(geo, myleft, myright);
    
    // the split plane is not perfect, which means one child may be zero
    // this is one the terminate condition
    
    if(myleft.size() == 0 && myright.size() > 0 )
    {
        //left.reset(new KDNode());
        //right.reset(new KDNode());
        left = NULL;
        right = NULL;
        
        leaf = true;
        
        for( int i = 0  ;i< myright.size() ; i++ )
        {
            geometryList.push_back(std::move(myright[i]));
        }
        return;
    }
    
    if(myright.size() == 0 && myleft.size() > 0  )
    {
        //left.reset(new KDNode());
        //right.reset(new KDNode());
        left = NULL;
        right = NULL;
        
        leaf = true;
        for(int i = 0  ;i< myleft.size() ; i++ )
        {
            geometryList.push_back(std::move(myleft[i]));
        }
        return;
    }
    
    //starting building the next level
    
    //printf("depth: %d,total: %d ,left: %d, right: %d\n", depth,geo.size(), myleft.size(), myright.size());
    
    depth++;
    //if(myleft.size() > 0)
    {
        if(left)
        {
            left->insert(myleft,depth);
        }
        else
        {
            left.reset(new KDNode());
            left->insert(myleft,depth);
        }
    }
    
   // if(myright.size() > 0)
    {
        if(right)
        {
            right->insert(myright,depth);
        }
        else
        {
            right.reset(new KDNode());
            right->insert(myright,depth);
        }
    }
    
    
    
    
}


/*
 the tranverse of binary searching tree
 the node parameter has to be the root of the tree initially
 */
bool KDTree::tranverse(std::unique_ptr<KDNode>& node, GRay &ray )
{
    bool ret = false;
    double dis = 0.0;
    GVector myintersection, mynormal;
    // check if the ray intersects the bounding box of the given node
    if( node->aabb.intersect(ray,dis) )  // hits the AABB of the whole spacecraft
    {
        dis = 0.0;
        if(node->leaf == true)  // we have reached a leaf
        {
            
            for( int i = 0 ; i< node->geometryList.size() ; i++ )
            {
                bool test = node->geometryList[i]->intersection(ray, myintersection, mynormal);
                
                GOpticalProperty myop = node->geometryList[i]->op;
                
                //double myspecularity = node->geometryList[i]->specularity;
                //double myreflectivity = node->geometryList[i]->reflectivity;
                //double myabsorptivity = node->geometryList[i]->absorptivity;
                //double myemissivity = node->geometryList[i]->emmisivity;
                
                //int mli_type = node->geometryList[i]->op.mli_type;
                
                if(test == true)
                {
                    dis = (myintersection-ray.start).norm();
                    if( dis <= mindis)
                    {
                        
                        //不同物体的相同交点上，可能具有完全不同的法向量,这时对光学特性取平均值处理
                        if(myintersection == intersection && mynormal != normal
                           && isChecked == true
                           )
                        {
                            
                            //mynormal = 0.5*(normal + mynormal);
                            //myspecularity = 0.5*(specularity + myspecularity);
                            //myreflectivity = 0.5*(reflectivity+myreflectivity);
                            //myabsorptivity = 0.5*(absorptivity+myabsorptivity);
                            //myemissivity  = 0.5*(emissivity+myemissivity);
                            
                            mynormal = -ray.direction;
                            //myop =
                            
                        }
                        
                        //update data
                        isChecked = true;
                        normal = mynormal;
                        intersection = myintersection;
                        op = myop;
                        
                        //printf("intersect: %.6f %.6f %.6f\n", intersection.x,intersection.y, intersection.z);
                        
                        //specularity = myspecularity;
                        //reflectivity = myreflectivity;
                        //absorptivity = myabsorptivity;
                        //emissivity   = myemissivity;
                        
                        //isMLI = mli;
                        
                        mindis = dis;
                        
                        ret = true;
                    }
                }
            }
    
        }
        else if( node->leaf == false ) //tranverse until the leaf node
        {
            dis = 0.0;
            //这里需要先进行AABB的相交判断，有可能跟既跟left相交，又跟right相交，这时就需要选择正确的交点!!!
            //必须要确保只能跟一个子树相交
            //mindis = GBoundingBox::infinite;
            
            bool l = tranverse(node->left, ray);
            bool r = tranverse(node->right, ray);
            
//            if(l==true && r == true)
//            {
//                int test = 0;
//            }
            
            return l||r;
            
        }
    }
    
    dis = 0;
    
    return ret;
}

void KDTree::clear()
{
    mindis = GBoundingBox::infinite;
    
    GOpticalProperty op;
    
    intersection.x = 0.0;
    intersection.y = 0.0;
    intersection.z = 0.0;
    normal.x = 0.0;
    normal.y = 0.0;
    normal.z = 0.0;
    
    op.mli_type = 0;
    //isMLI = false;
    isChecked = false;
    
}

//if it is true, the specularity, reflectivty and intersection information would be updated
bool KDTree::intersect( GRay &ray )
{
    mindis = GBoundingBox::infinite;
    intersection.set(0, 0, 0);
    normal.set(0, 0, 0);
    
    //specularity = 0.0;
    //reflectivity = 0.0;
    //absorptivity = 0.0;
    //emissivity = 0.0;
    
    //isMLI = false;
    op.mli_type = 0;
    isChecked = false;
    
    return tranverse(root, ray );
}


// build KDTree and return the root node
void KDTree::buildKDTree(std::vector<std::unique_ptr<GGeometry> >& geo )
{
    //int depth = 0;
    //call build function recursively
    root->insert(geo,currentDepth);
    
}

void KDTree::showTree( std::unique_ptr<KDNode>& node, int tag, int& depth)
{
    
    if(node != NULL)
    {
        
        std::string s ;
        for( int i = 0 ; i< depth+1 ; i++ )
        {
            s.append("--");
        }
        if(tag == 0)
        {
            printf("%slevel_%03d\n",s.c_str(),depth);
        }
        else if(tag == 1)
        {
            printf("%slevel_left_%03d\n",s.c_str(),depth);
        }
        else if(tag == 2)
        {
            printf("%slevel_right_%03d\n",s.c_str(),depth);
        }
        
        
        if( node->leaf == true ) //叶子结点，打印所有几何对象
        {
            //printf("%s",s.c_str());
            for( int i = 0 ; i < node->geometryList.size() ; i++ )
            {
                printf("%s%ld\n",s.c_str(),node->geometryList[i]->index);
                // printf("%ld\n",node->geometryList[i]->index);
            }
            
            printf("\n");
        }
        else
        {
            depth++;
            showTree(node->left, 1,depth);
            showTree(node->right, 2,depth);
            
        }
        
    }
    
}


// 用递归方法求树的高度
int KDTree::Depth(std::unique_ptr<KDNode>& T)
{
    if (NULL == T)
        return 0;
    
    int leftDepth = Depth(T->left);
    int rightDepth = Depth(T->right);
    
    return 1+ std::max(leftDepth, rightDepth);
}


void KDTree::PrintNodeAtLevel(std::unique_ptr<KDNode>& node,int level)
{
    // 空树或层级不合理
    if (NULL == node || level < 1 )
        return;
    
    if (1 == level)
    {
        //cout << T->data << "  ";
        return;
    }
    
    // 左子树的 level - 1 级
    PrintNodeAtLevel(node->left,  level - 1);
    
    // 右子树的 level - 1 级
    PrintNodeAtLevel(node->right, level - 1);
}

void KDTree::LevelTraverse(std::unique_ptr<KDNode>& node)
{
    if (NULL == node)
        return;
    
    int depth = Depth(node);
    
    for (int i = 1; i <= depth; i++)
    {
        PrintNodeAtLevel(node, i);
        //cout << endl;
    }
    
}

int KDTree::getTreeDepth()
{
    return Depth(root);
}





