#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

int pathHeight(Node* node, bool& isEqual);


bool equalPaths(Node * root)
{
    if(root == nullptr){
        return true;
    }

    bool isEqual = true;
    pathHeight(root, isEqual);
    return isEqual;
}


int pathHeight(Node* node, bool& isEqual){
    // base case: empty node
    if(node == nullptr){
        return 0;
    }

    // base case: leaf node
    if(node->left == nullptr && node->right == nullptr){
        return 1;
    }

    // height of left subtree
    int leftHeight = 0;
    if(node->left != nullptr){
        leftHeight = pathHeight(node->left, isEqual);
    }

    // height of right subtree
    int rightHeight = 0;
    if(node->right != nullptr){
        rightHeight = pathHeight(node->right, isEqual);
    }

    // if a subtree already has unequal heights
    if(!isEqual){
        return -1;
    }

    // if both subtrees are valid, heights must be equal
    if(node->left != nullptr && node->right != nullptr){
        if(leftHeight != rightHeight){
            isEqual = false;
            return -1;
        }
    }

    // return the height of the path
    // if only one child exists, use its height
    // if both exist, heights should be equal
    int maxHeight = leftHeight;
    if(rightHeight > leftHeight){
        maxHeight = rightHeight;
    }
    return 1 + maxHeight;


}

