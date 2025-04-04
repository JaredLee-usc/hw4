#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here

    // rotation methods
    void rotateRight(AVLNode<Key, Value>* node);
    void rotateLeft(AVLNode<Key, Value>* node);

    // helper for remove
    void removeFix(AVLNode<Key, Value>* node, int diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    // if tree is empty, create ne node as the root
    if(this->root_ == NULL){
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    // find point to insert
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;

    while(curr != NULL){
        if(new_item.first == curr->getKey()){
            // update value if key exists
            curr->setValue(new_item.second);
            return;
        }

        parent = curr;

        if(new_item.first < curr->getKey()){
            curr = curr->getLeft();
        }
        else{
            curr = curr->getRight();
        }
    }

    // create new node with its parent
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    // set as left or right child or parent
    if(new_item.first < parent->getKey()){
        parent->setLeft(newNode);
    }
    else{
        parent->setRight(newNode);
    }

    // update balances and rebalance
    AVLNode<Key,Value>* current = parent;


    // traverse up tree and adjust balances
    while(current != NULL){
        int8_t balanceChange;
        // determine if we went left or right
        if(current->getLeft() == newNode){
            // going left decreases balance
            balanceChange = -1;
        }
        else{
            // going right increase balance
            balanceChange = 1;
        }

        // update the balance factor
        current->updateBalance(balanceChange);

        // check if the tree is unbalanced at this node
        if (current->getBalance() == 0) {
            break;
        } 
        else if(current->getBalance() == -2){
            // left heavy
            if(current->getLeft()->getBalance() == -1){
                // left-left case: single right rotation
                rotateRight(current);
                
                // fix balances
                current->setBalance(0);
                current->getParent()->setBalance(0);
            } 
            else{
                // left-right case: left rotation then right rotation
                rotateLeft(current->getLeft());
                rotateRight(current);
                
                // fix balances - depends on the balance of what is now current->parent
                AVLNode<Key, Value>* newRoot = current->getParent();
                
                if(newRoot->getBalance() == 0){
                    current->setBalance(0);
                    newRoot->getLeft()->setBalance(0);
                } 
                else if(newRoot->getBalance() == -1){
                    current->setBalance(1);
                    newRoot->getLeft()->setBalance(0);
                } 
                else{  // newRoot->getBalance() == 1
                    current->setBalance(0);
                    newRoot->getLeft()->setBalance(-1);
                }
                newRoot->setBalance(0);
            }
            break;
        } 
        else if (current->getBalance() == 2) {
            // Right heavy
            if (current->getRight()->getBalance() == 1) {
                // Right-right case: single left rotation
                rotateLeft(current);
                
                // Fix balances
                current->setBalance(0);
                current->getParent()->setBalance(0);
            } 
            else {
                // Right-left case: right rotation then left rotation
                rotateRight(current->getRight());
                rotateLeft(current);
                
                // Fix balances - depends on the balance of what is now current->parent
                AVLNode<Key, Value>* newRoot = current->getParent();
                
                if (newRoot->getBalance() == 0) {
                    current->setBalance(0);
                    newRoot->getRight()->setBalance(0);
                } 
                else if (newRoot->getBalance() == 1) {
                    current->setBalance(-1);
                    newRoot->getRight()->setBalance(0);
                } 
                else {  // newRoot->getBalance() == -1
                    current->setBalance(0);
                    newRoot->getRight()->setBalance(1);
                }
                
                newRoot->setBalance(0);
            }
            break;
        } 
        else if (current->getBalance() == -1 || current->getBalance() == 1) {
            // The tree is still balanced but may need propagation
            newNode = current;
            current = current->getParent();
        }

    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    // find node to remove
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    
    // if key not in tree
    if (nodeToRemove == NULL) {
        return;
    }
    
    // save parent for rebalancing before any swaps
    AVLNode<Key, Value>* parent = nodeToRemove->getParent();
    int diff = 0;
    if (parent != NULL) {
        diff = (parent->getLeft() == nodeToRemove) ? 1 : -1;
    }
    
    // node has two children
    if (nodeToRemove->getLeft() != NULL && nodeToRemove->getRight() != NULL) {
        // find predecessor
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        
        // store the pred's parent and relationship before swap
        AVLNode<Key, Value>* predParent = pred->getParent();
        
        // if pred is direct child of nodeToRemove, predParent will be changed after swap
        bool predIsLeftChildOfNodeToRemove = (nodeToRemove->getLeft() == pred);
        
        // if pred is not direct child
        bool predIsLeftChild = false;
        if(predParent != nodeToRemove){
            predIsLeftChild = (predParent->getLeft() == pred);
        }
        
        // swap the nodes
        nodeSwap(nodeToRemove, pred);
        
        // determine new parent for rebalancing
        if(predParent == nodeToRemove){
            parent = pred;
            diff = predIsLeftChildOfNodeToRemove ? 1 : -1;
        }
        else{
            parent = predParent;  // Start rebalancing from pred's original parent
            diff = predIsLeftChild ? 1 : -1;
        }
    }
    
    // Now nodeToRemove has at most one child
    
    // Get the child that will replace nodeToRemove
    AVLNode<Key, Value>* child = NULL;
    if (nodeToRemove->getLeft() != NULL) {
        child = nodeToRemove->getLeft();
    } else {
        child = nodeToRemove->getRight();
    }
    
    // Update parent pointers
    if (parent == NULL) {
        // nodeToRemove is root
        this->root_ = child;
        if (child != NULL) {
            child->setParent(NULL);
        }
    } else {
        // Update parent to point to child
        if (parent->getLeft() == nodeToRemove) {
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
        
        // Update child to point to parent
        if (child != NULL) {
            child->setParent(parent);
        }
    }
    
    // Delete the node
    delete nodeToRemove;
    
    // Start rebalancing from parent
    removeFix(parent, diff);
}

// Helper function for rebalancing after removal
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff) {
    // Base case: if node is null, return
    if (n == NULL) {
        return;
    }
    
    // Compute next recursive call's arguments before altering the tree
    AVLNode<Key, Value>* p = n->getParent();
    int ndiff = 0;
    if (p != NULL) {
        ndiff = (p->getLeft() == n) ? 1 : -1;
    }
    
    // Apply balance update based on the diff
    int oldBalance = n->getBalance();
    int newBalance = oldBalance + diff;
    
    // Case 3: Balance becomes 0
    if (newBalance == 0) {
        n->setBalance(0);
        removeFix(p, ndiff);
        return;
    }
    
    // Case 2: Balance becomes +1 or -1
    if (newBalance == 1 || newBalance == -1) {
        n->setBalance(newBalance);
        return; // Tree is still balanced, we're done
    }
    
    // Case 1: Balance becomes +2 or -2, need to rebalance
    if (newBalance == -2) {
        // Left subtree is taller
        AVLNode<Key, Value>* c = n->getLeft();
        
        if (c->getBalance() == -1) {
            // Case 1a: zig-zig case (left-left)
            rotateRight(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, ndiff);
        } 
        else if (c->getBalance() == 0) {
            // Case 1b: zig-zig case (left-balanced)
            rotateRight(n);
            n->setBalance(-1);
            c->setBalance(1);
            // Done, no height change
        }
        else if (c->getBalance() == 1) {
            // Case 1c: zig-zag case (left-right)
            AVLNode<Key, Value>* g = c->getRight();
            int g_balance = g->getBalance();
            
            rotateLeft(c);
            rotateRight(n);
            
            // Update balances based on g's original balance
            if (g_balance == 1) {
                n->setBalance(0);
                c->setBalance(-1);
                g->setBalance(0);
            }
            else if (g_balance == 0) {
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            }
            else { // g_balance == -1
                n->setBalance(1);
                c->setBalance(0);
                g->setBalance(0);
            }
            
            removeFix(p, ndiff);
        }
    }
    else if (newBalance == 2) {
        // Right subtree is taller
        AVLNode<Key, Value>* c = n->getRight();
        
        if (c->getBalance() == 1) {
            // Mirror of Case 1a: zig-zig case (right-right)
            rotateLeft(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, ndiff);
        }
        else if (c->getBalance() == 0) {
            // Mirror of Case 1b: zig-zig case (right-balanced)
            rotateLeft(n);
            n->setBalance(1);
            c->setBalance(-1);
            // Done, no height change
        }
        else if (c->getBalance() == -1) {
            // Mirror of Case 1c: zig-zag case (right-left)
            AVLNode<Key, Value>* g = c->getLeft();
            int g_balance = g->getBalance();
            
            rotateRight(c);
            rotateLeft(n);
            
            // Update balances based on g's original balance
            if (g_balance == -1) {
                n->setBalance(0);
                c->setBalance(1);
                g->setBalance(0);
            }
            else if (g_balance == 0) {
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            }
            else { // g_balance == 1
                n->setBalance(-1);
                c->setBalance(0);
                g->setBalance(0);
            }
            
            removeFix(p, ndiff);
        }
    }
}



// rotate right method
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    // store the left child that will become the new root
    AVLNode<Key, Value>* leftChild = node->getLeft();

    // set node's left pointer to left child's right subtree
    node->setLeft(leftChild->getRight());

    // if left child's right subtree exists, update parent pointer
    if(leftChild->getRight() != NULL){
        leftChild->getRight()->setParent(node);
    }

    // set left child's parent pointer
    leftChild->setParent(node->getParent());

    // set parent's child pointer
    if(node->getParent() == NULL){
        // node was the root
        this->root_ = leftChild;
    }
    else if(node == node->getParent()->getLeft()){
        // node was a left child
        node->getParent()->setLeft(leftChild);
    }
    else{
        // node was a right child
        node->getParent()->setRight(leftChild);
    }

    // set node as right child or left child
    leftChild->setRight(node);

    node->setParent(leftChild);
}


// rotate left method
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    // store the right child that will become the new root
    AVLNode<Key, Value>* rightChild = node->getRight();

    // set node's right pointer to right child's left subtree
    node->setRight(rightChild->getLeft());

    // if right child's left subtree exists, update parent pointer
    if(rightChild->getLeft() != NULL){
        rightChild->getLeft()->setParent(node);
    }

    // set right child's parent pointer
    rightChild->setParent(node->getParent());

    // set parent's child pointer
    if(node->getParent() == NULL){
        // node was the root
        this->root_ = rightChild;
    }
    else if(node == node->getParent()->getLeft()){
        // node was a left child
        node->getParent()->setLeft(rightChild);
    }
    else{
        // node was a right child
        node->getParent()->setRight(rightChild);
    }

    // set node as right child or left child
    rightChild->setLeft(node);

    node->setParent(rightChild);
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
