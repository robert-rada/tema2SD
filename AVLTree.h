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
    // 1. Create new node
    TreeNode *new_node = createTreeNode(tree, elem, info);
    // 2. Iterate to down-wards to nil 
    TreeNode *node = tree->root;

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

    // 4. Update linked list

    if (duplicate)
    {
        new_node->next = node->end->next;
        node->end->next = new_node;
        new_node->prev = node->end;
        new_node->next->prev = new_node;
        node->end = new_node;
    }
    else
    {
        for (node = minimum(tree, tree->root); node != tree->nil; node = node->next)
        {
            if (tree->compare(new_node->elem, node->elem) < 0)
            {
                new_node->next = node;
                new_node->prev = node->prev;
                node->prev->next = new_node;
                node->prev = new_node;
                break;
            }
        }

        if (node == tree->nil)
        {
            node = maximum(tree, tree->root);
            if (node != new_node)
            {
                node->next = new_node;
                new_node->prev = node;
            }
        }
    }

    if (isEmpty(tree))
        tree->root = new_node;

    // 5. Update size of tree and call fix-up

    tree->size++;
    avlFixUp(tree, new_node);
}

void delete(TTree* tree, void* elem){



	/* 
	 * TODO: 
	 * 1. Search for the node
     * 2. If duplicate delete from the list and exit
     * 3. Otherwise perform tree deletion and update linked list at the end
     * 4. Update size and call fix-up
     * 
     * Hints:
     * 1. If you MUST delete a node from the tree than it has no duplicates!
     *
     * 2. Changing the links with the splice out node is the way to GO,
     *    exchaning node fields does not work in this case,
          you might have duplicates for the splice-out node!!!! 
	 */
}

void destroyTree(TTree* tree){
	// TODO:

	// Hint: Can you use the list?
}


#endif /* AVLTREE_H_ */
