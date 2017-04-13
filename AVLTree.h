#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <stdlib.h>

#define MAX(a, b) (((a) >= (b))?(a):(b))

typedef struct node
{
    void *elem;
    void *info;
    struct node *pt;
    struct node *lt;
    struct node *rt;
    struct node *next;
    struct node *prev;
    struct node *end;
    long height;
} TreeNode;

typedef struct TTree
{
    TreeNode *root;
    TreeNode *nil;
    void* (*createElement)(void*);
    void (*destroyElement)(void*);
    void* (*createInfo)(void*);
    void (*destroyInfo)(void*);
    int (*compare)(void*, void*);
    long size;
} TTree;

TTree* createTree(void* (*createElement)(void*), void (*destroyElement)(void*),
		void* (*createInfo)(void*), void (*destroyInfo)(void*),
		int compare(void*, void*))
{
    // 1. Allocate tree
    TTree *tree = malloc(sizeof(TTree));

    // 2. Allocate sentinel
    tree->nil = malloc(sizeof(TreeNode));

    // 3. Set sentinel
    tree->nil->elem = tree->nil->info = NULL;
    tree->nil->height = 0;
    tree->nil->pt = tree->nil;
    tree->nil->lt = tree->nil;
    tree->nil->rt = tree->nil;
    tree->nil->next = tree->nil;
    tree->nil->prev = tree->nil;
    tree->nil->end = tree->nil;

    // 4. Set all other fields
    tree->root = tree->nil;
    tree->size = 0;
    tree->createElement = createElement;
    tree->destroyElement = destroyElement;
    tree->createInfo = createInfo;
    tree->destroyInfo = destroyInfo;
    tree->compare = compare;

    return tree;
}

TreeNode* createTreeNode(TTree *tree, void* value, void* info)
{
    // 1. Allocate new node
    TreeNode *new_node = malloc(sizeof(TreeNode));

    // 2. Set it's fields using appropriate functions
    new_node->elem = tree->createElement(value);
    new_node->info = tree->createInfo(info);

    // 3. Set all links
    new_node->pt = tree->nil;
    new_node->lt = tree->nil;
    new_node->rt = tree->nil;
    new_node->next = tree->nil;
    new_node->prev = tree->nil;
    new_node->end = new_node;
    // 4. Set height for the new node
    new_node->height = 1;

    return new_node;
}

void destroyTreeNode(TTree *tree, TreeNode* node)
{
    // 1. Destroy fields
    tree->destroyInfo(node->info);
    tree->destroyElement(node->elem);
    // 2. De-allocate node
    free(node);
}

int isEmpty(TTree* tree)
{
    return tree->size == 0;
}

TreeNode* search(TTree* tree, TreeNode* node, void* elem)
{
    while (node != tree->nil)
    {
        int compare_result = tree->compare(elem, node->elem);

        if (compare_result == 0)
            return node;
        else if (compare_result < 0)
            node = node->lt;
        else
            node = node->rt;
    }

    return tree->nil;
}

TreeNode* minimum(TTree *tree, TreeNode *node)
{
    while (node->lt != tree->nil)
        node = node->lt;
    
    return node;
}

TreeNode* maximum(TTree *tree, TreeNode *node)
{
    while (node->rt != tree->nil)
        node = node->rt;

    return node;
}

TreeNode* successor(TTree *tree, TreeNode *node)
{
    if (node->rt != tree->nil)
        return minimum(tree, node->rt);

    while (node->pt != tree->nil)
    {
        if (tree->compare(node->elem, node->pt->elem) < 0)
            return node->pt;

        node = node->pt;
    }

    return tree->nil;
}

TreeNode* predecessor(TTree *tree, TreeNode *node)
{
    if (node->lt != tree->nil)
        return maximum(tree, node->lt);

    while (node->pt != tree->nil)
    {
        if (tree->compare(node->elem, node->pt->elem) > 0)
            return node->pt;

        node = node->pt;
    }

    return tree->nil;
}

void avlRotateLeft(TTree* tree, TreeNode* x){
	TreeNode *y = x->rt;
	x->rt = y->lt;

	if(y->lt != tree->nil)
		y->lt->pt = x; // the parent of the y left subtree is x
	y->pt = x->pt; // update parent for y

	if(x->pt == tree->nil)
		tree->root = y;
	else if(x->pt->lt == x)
		x->pt->lt = y;
	else
		x->pt->rt = y;

	y->lt = x; // x is to the left of y
	x->pt = y; // x parent is y

	// Update heights
	y->height = MAX(y->lt->height, y->rt->height) + 1;
	x->height = MAX(x->lt->height, x->rt->height) + 1;

}

void avlRotateRight(TTree* tree, TreeNode* y){
	TreeNode *x = y->lt;

	y->lt = x->rt;
	if(x->rt != tree->nil)
		x->rt->pt = y;

	x->pt = y->pt;

	if(y->pt == tree->nil)
		tree->root = x;
	else if(y->pt->lt == y)
		y->pt->lt = x;
	else
		y->pt->rt = x;

	x->rt = y;
	y->pt = x;

	// Update heights
	y->height = MAX(y->lt->height, y->rt->height) + 1;
	x->height = MAX(x->lt->height, x->rt->height) + 1;
}

int avlGetBalance(TTree *tree, TreeNode *node)
{
    long left_height = 0;
    if (node->lt != tree->nil)
        left_height = node->lt->height;
    
    long right_height = 0;
    if (node->rt != tree->nil)
        right_height = node->rt->height;

    return left_height - right_height;
}

void avlFixUp(TTree *tree, TreeNode *node)
{
    while (node != tree->nil)
    {
        int balance = avlGetBalance(tree, node);

        // right subtree has bigger height
        if (balance < -1)
        {
            if (avlGetBalance(tree, node->rt) > 0)
                avlRotateRight(tree, node->rt);

            avlRotateLeft(tree, node);
        }
        //left subtree has bigger height
        else if (balance > 1)
        {
            if (avlGetBalance(tree, node->lt) < 0)
                avlRotateLeft(tree, node->lt);

            avlRotateRight(tree, node);
        }

        node->height = MAX(node->lt->height, node->rt->height) + 1;
        node = node->pt;
    }
}

void insert(TTree *tree, void *elem, void *info) 
{
    TreeNode *new_node = createTreeNode(tree, elem, info);
    TreeNode *node = tree->root;

    if (isEmpty(tree))
        tree->root = new_node;

    unsigned char duplicate = 0;
    while (node != tree->nil)
    {
        int compare_result = tree->compare(new_node->elem, node->elem);

        if (compare_result == 0)
        {
            duplicate = 1;
            break;
        }
        
        if (compare_result < 0)
        {
            if (node->lt != tree->nil)
                node = node->lt;
            else
            {
                node->lt = new_node;
                new_node->pt = node;
                break;
            }
        }
        else
        {
            if (node->rt != tree->nil)
                node = node->rt;
            else
            {
                node->rt = new_node;
                new_node->pt = node;
                break;
            }
        }
    }

    // Update linked list

    if (duplicate)
    {
        new_node->next = node->end->next;
        node->end->next = new_node;
        new_node->prev = node->end;
        if (new_node->next != tree->nil)
            new_node->next->prev = new_node;
        node->end = new_node;
    }
    else
    {
        node = predecessor(tree, new_node);
        if (node != tree->nil)
        {
            new_node->next = node->next;
            if (new_node->next != tree->nil)
                new_node->next->prev = new_node;
            node->next = new_node;
            new_node->prev = node;
        }
        else
        {
            node = successor(tree, new_node);
            new_node->next = node;
            if (node != tree->nil)
                node->prev = new_node;
        }
    }

    tree->size++;
    avlFixUp(tree, new_node);
}

void deleteTreeNode(TTree *tree, TreeNode *node)
{
    TreeNode *substitute = tree->nil;
    TreeNode *sub_child;

    // less than 2 children case
    if (node->lt == tree->nil || node->rt == tree->nil)
    {
        if (node->lt != tree->nil)
            substitute = node->lt;
        else
            substitute = node->rt;
        sub_child = substitute;

        if (node->pt != tree->nil)
        {
            if (node->pt->lt == node)
                node->pt->lt = substitute;
            else
                node->pt->rt = substitute;
        }
        if (substitute != tree->nil)
            substitute->pt = node->pt;
        
        if (sub_child == tree->nil)
            sub_child = node->pt;
    }
    else
    {
        substitute = minimum(tree, node->rt);
        sub_child = substitute->rt;

        // link sub_child with parent of sub
        if (substitute != node->rt)
        {
            if (sub_child != tree->nil)
                sub_child->pt = substitute->pt;
            substitute->pt->lt = sub_child;
        }

        // link sub with left child of node
        substitute->lt = node->lt;
        node->lt->pt = substitute;

        // link sub with right child of node
        if (node->rt != substitute)
            substitute->rt = node->rt;
        if (substitute->rt != tree->nil)
            substitute->rt->pt = substitute;

        // link sub with parent of node
        substitute->pt = node->pt;
        if (node->pt != tree->nil)
        {
            if (node->pt->lt == node)
                node->pt->lt = substitute;
            else
                node->pt->rt = substitute;
        }
    }

    if (sub_child == tree->nil)
        sub_child = substitute;

    if (tree->root == node)
        tree->root = substitute;

    avlFixUp(tree, sub_child);
    destroyTreeNode(tree, node);
}

void delete(TTree *tree, void *elem)
{
    TreeNode *tree_node = search(tree, tree->root, elem);
    TreeNode *list_node = tree_node->end;

    if (tree_node == tree->nil)
        return;

    tree->size--;

    // update list
    if (list_node->prev != tree->nil)
        list_node->prev->next = list_node->next;
    if (list_node->next != tree->nil)
        list_node->next->prev = list_node->prev;

    // delete duplicate from list
    if (tree_node != list_node)
    {
        tree_node->end = list_node->prev;
        destroyTreeNode(tree, list_node);
    }
    else
    {
        // delete node from tree
        deleteTreeNode(tree, tree_node);
    }
}

void destroyTree(TTree* tree)
{
    TreeNode *node = minimum(tree, tree->root);

    while (node != tree->nil)
    {
        TreeNode *t = node;
        node = node->next;
        destroyTreeNode(tree, t);
    }

    destroyTreeNode(tree, tree->nil);
    free(tree);
}


#endif /* AVLTREE_H_ */
