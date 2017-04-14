#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVLTree.h"

/* Lungimea maxima a unui buffer */
#define BUFLEN 1024

#define STR_ELEM_LEN 3

/* Range of models*/
typedef struct Range
{
	int *index; // Array of models in the range
	int size; // Number of array elements
	int capacity; // Array capacity
} Range;

void *createStrElement(void *str)
{
    void *ret = malloc(STR_ELEM_LEN);
    for (int i = 0; i < STR_ELEM_LEN; i++)
        *(char*)(ret+i) = *(char*)(str+i);
    return ret;
}

void destroyStrElement(void* elem)
{
    free(elem);
}

void *createPriceElement(void *price)
{
    long *ret = malloc(sizeof(long));
    *ret = *(long*)price;
    return ret;
}

void destroyPriceElement(void *price)
{
    free((long*)price);
}

void *createIndexInfo(void *index)
{
    int *ret = malloc(sizeof(int));
    *ret = *(int*)index;
    return ret;
}

void destroyIndexInfo(void *index)
{
    free((int*)index);
}

int compareStr(void *str1, void *str2)
{
    for (int i = 0; i < STR_ELEM_LEN; i++)
    {
        if ( *(char*)(str1+i) == 0 || *(char*)(str2+i) == 0)
            return 0;
        if ( *(char*)(str1+i) < *(char*)(str2+i) )
            return -1;
        if ( *(char*)(str1+i) > *(char*)(str2+i) )
            return 1;
    }

    return 0;
}

int comparePrice(void *price1, void *price2)
{
    if (*(long*)price1 < *(long*)price2)
        return -1;
    if (*(long*)price1 > *(long*)price2)
        return 1;
    return 0;
}

void buildTreesFromFile(char *fileName, TTree *modelTree, TTree *priceTree)
{
    FILE *input_file = fopen("input.csv", "r");

    char input[BUFLEN+1];
    char str[BUFLEN];
    int file_index = 0;
    while (fgets(input, BUFLEN, input_file))
    {
        int i = 0;
        int index = 0;
        while (input[index] != ',')
        {
            str[i++] = input[index++];
        }

        for (int j = i; j < STR_ELEM_LEN; j++)
            str[j] = 0;

        long price = 0;
        for (index = index+1; index < strlen(input) && input[index] >= '0';
                index++)
        {
            price = price * 10 + (long)input[index] - (long)'0';
        }

        insert(modelTree, str, &file_index);
        insert(priceTree, &price, &file_index);
        file_index += strlen(input);
    }

    fclose(input_file);
}

void pushBack(Range *range, void *elem)
{
    if (range->size == range->capacity)
    {
        range->capacity *= 2;
        range->index = realloc(range->index, sizeof(int) * range->capacity);
    }

    range->index[range->size++] = *(int*)elem;
}

TreeNode *findLeftRangeLimit(TTree *tree, char *q)
{
    TreeNode *node = tree->root;
    unsigned char found = 0;

    while (1)
    {
        if (tree->compare(q, node->elem) <= 0)
        {
            if (tree->compare(q, node->elem) == 0)
                found = 1;

            if (node->lt != tree->nil)
                node = node->lt;
            else
                break;
        }
        else
        {
            if (node->rt != tree->nil)
                node= node->rt;
            else
                break;
        }
    }

    if (!found)
        return tree->nil;

    if (tree->compare(q, node->elem) != 0)
        node = node->end->next;

    return node;
}

TreeNode *findRightRangeLimit(TTree *tree, char *q)
{
    TreeNode *node = tree->root;

    while (1)
    {
        if (tree->compare(q, node->elem) < 0)
        {
            if (node->lt != tree->nil)
                node = node->lt;
            else
                break;
        }
        else
        {
            if (node->rt != tree->nil)
                node= node->rt;
            else
                break;
        }
    }

    if (tree->compare(q, node->elem) == 0)
        node = node->end->next;

    return node;
}

Range *modelGroupQuery(TTree *tree, char *q)
{
    Range *range = malloc(sizeof(Range));
    range->size = 0;
    range->capacity = 1;
    range->index = malloc(sizeof(int) * range->capacity);

    TreeNode *left = findLeftRangeLimit(tree, q);
    TreeNode *right = findRightRangeLimit(tree, q);

    if (left == tree->nil)
        return range;

    while (left != right)
    {
        pushBack(range, left->info);
        left = left->next;
    }

    return range;
}
Range* modelRangeQuery(TTree* tree, char* q, char* p){
	// TODO: 4b
	return NULL;
}
Range* priceRangeQuery(TTree* tree, long q, long p){
	// TODO: 4a
	return NULL;
}
Range* modelPriceRangeQuery(char* fileName, TTree* tree, char* m1, char* m2, long p1, long p2){
	// TODO: 5
	return NULL;
}


void printFile(char* fileName);
void inorderModelTreePrint(TTree* tree, TreeNode* node);
void inorderPriceTreePrint(TTree* tree, TreeNode* node);
void printRange(Range* range, char* fileName);


int main(void) {
	
	TTree* modelTree = createTree(createStrElement, destroyStrElement, createIndexInfo, destroyIndexInfo, compareStr);
	TTree* priceTree = createTree(createPriceElement, destroyPriceElement, createIndexInfo, destroyIndexInfo, comparePrice);

	buildTreesFromFile("input.csv", modelTree, priceTree);

	printf("Model Tree In Order:\n");
	inorderModelTreePrint(modelTree, modelTree->root);
	printf("\n\n");

	printf("Price Tree In Order:\n");
	inorderPriceTreePrint(priceTree, priceTree->root);
	printf("\n\n");

	printf("Group Model Search:\n");
	Range *range = modelGroupQuery(modelTree,"MG3");
	printRange(range,"input.csv");
	printf("\n\n");

	printf("Price Range Search:\n");
	Range *range2 = priceRangeQuery(priceTree,100,400);
	printRange(range2,"input.csv");
	printf("\n\n");

	printf("Model Range Search:\n");
	Range *range3 = modelRangeQuery(modelTree,"L2","M");
	printRange(range3,"input.csv");
	printf("\n\n");

	printf("Model Price Range Search:\n");
	Range *range4 = modelPriceRangeQuery("input.csv",modelTree,"L2","M", 300, 600);
	printRange(range4,"input.csv");

	free(range->index);
	free(range);
	free(range2->index);
	free(range2);
	free(range3->index);
	free(range3);
	free(range4->index);
	free(range4);
	destroyTree(priceTree);
	destroyTree(modelTree);
	return 0;
}



void inorderModelTreePrint(TTree* tree, TreeNode* node){
    if (node == minimum(tree, tree->root))
            {
                int stop = 1;
            }
	if(node != tree->nil){
		inorderModelTreePrint(tree, node->lt);
		TreeNode* begin = node;
		TreeNode* end = node->end->next;
		while(begin != end){
			printf("%d:%s  ",*((int*)begin->info),(char*)begin->elem);
			begin = begin->next;
		}
		inorderModelTreePrint(tree, node->rt);
	}
}

void inorderPriceTreePrint(TTree* tree, TreeNode* node){
	if(node != tree->nil){
		inorderPriceTreePrint(tree, node->lt);
		TreeNode* begin = node;
		TreeNode* end = node->end->next;
		while(begin != end){
			printf("%d:%ld  ",*((int*)begin->info),*((long*)begin->elem));
			begin = begin->next;
		}
		inorderPriceTreePrint(tree, node->rt);
	}
}

void printRange(Range* range, char* fileName){
	if(fileName == NULL) return;
	FILE * file = fopen(fileName,"r");
	if (file == NULL) return;

	char *buf = (char*) malloc(BUFLEN+1);

	for(int i = 0; i < range->size;i++){
		fseek(file,range->index[i],SEEK_SET);
		if(fgets(buf,BUFLEN,file) != NULL){
			char* model = strtok(buf,",");
			long price = atol(strtok(NULL,","));
			printf("%s:%ld  ", model, price);
		}
	}
	printf("\n");
	free(buf);
	fclose(file);
}

void printFile(char* fileName){
	printf("---------\n");
	if(fileName == NULL) return;
	FILE * file = fopen(fileName,"r");
	if (file == NULL)
		return;

	char *buf = (char*) malloc(BUFLEN+1);
	while(fgets(buf,BUFLEN,file) != NULL){
		buf[strlen(buf) - 1] = '\0';
		printf("%s",buf);
	}
	printf("\n");
	printf("---------\n");
	free(buf);
	fclose(file);
}
