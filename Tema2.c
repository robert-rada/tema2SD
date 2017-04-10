/*
 ============================================================================
 Name        : Tema2.c
 Author      : Dan Novischi
 Version     :
 Copyright   : Your copyright notice
 Description : Dictionare
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVLTree.h"

/* Lungimea maxima a unui buffer */
#define BUFLEN 1024

/* Lungimea unui element din arbore */
#define ELEMENT_TREE_LENGTH 3

/* Range of models*/
typedef struct Range{
	int *index; // Array of models in the range
	int size; // Number of array elements
	int capacity; // Array capacity
}Range;

void* createStrElement(void* str){
	// TODO: 2
}

void destroyStrElement(void* elem){
	// TODO: 2
}


void* createPriceElement(void* price){
	// TODO: 2
	return NULL;
}

void destroyPriceElement(void* price){
	// TODO: 2
}

void* createIndexInfo(void* index){
	// TODO: 2
	return NULL;
}

void destroyIndexInfo(void* index){
	// TODO: 2
}

int compareStr(void* str1, void* str2){
	// TODO: 2
	return -2;
}

int comparePrice(void* price1, void* price2){
	// TODO: 2
	return -2;
}

void buildTreesFromFile(char* fileName, TTree* modelTree, TTree* priceTree){
	// TODO: 2
}

Range* modelGroupQuery(TTree* tree, char* q){
	// TODO: 3
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
