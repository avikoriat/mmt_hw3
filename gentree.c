#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "defs.h"
#include "gentree.h"


/* definition of the element in the tree */

typedef struct _ELEMENT{
  pNode obj;
  PELEMENT* children;
  PELEMENT parent;
  int childrenCount;
} ELEMENT, *PELEMENT;

/* definition of the tree structure */    
typedef struct _tree{
  PELEMENT head;
  int k;// number of children in the tree
  int nodeCount;
  GetKeyFunction getKeyFunc;
  CloneFunction cloneFunc;
  PrintFunction printFunc;
  DelFunction delFunc;
} Tree, *pTree;

/* *** complete the interface functions implementation *** */

pTree TreeCreate(GetKeyFunction getKeyFunc,
	CloneFunction cloneFunc,
	PrintFunction printFunc,
	DelFunction delFunc,
	int k) {
	pTree newTree;
	if( ( newTree = (pTree)malloc(sizeof(Tree)) ) == NULL) return NULL;
	newTree->head = NULL;
	newTree->cloneFunc = cloneFunc;
	newTree->getKeyFunc = getKeyFunc;
	newTree->printFunc = printFunc;
	newTree->delFunc = delFunc;
	newTree->k = k;
	newTree->nodeCount = 0;
	return newTree;
}
///////////////////  internal static functions ///////////////////////////

/*************************************************************************
Function name	: RecurTreeDestroy
Description		: 
Paramerters		: 
Return value	: 
************************************************************************/
static void RecurTreeDestroy(pTree tree, PELEMENT head);

/*************************************************************************
Function name	: RecurElementPrint
Description		:
Paramerters		:
Return value	:
************************************************************************/
static void RecurElementPrint(pTree tree, PELEMENT head);

/*************************************************************************
Function name	: CreateElement
Description		:
Paramerters		:
Return value	:
************************************************************************/
static PELEMENT CreateElement(pTree tree, pNode newNode, PELEMENT parentNode);

/*************************************************************************
Function name	: RecurTreeGetElem
Description		:
Paramerters		:
Return value	:
************************************************************************/
static PELEMENT RecurTreeGetElem(pTree tree, int key, PELEMENT treeElem);

/////////////////////////////////////////////////////////////////////////

// destroys recursively the head and all its children
static void RecurTreeDestroy(pTree tree, PELEMENT head) {
	if (head == NULL) return;//input check
	if (head->childrenCount > 0) {
		for (int i = 0; i < tree->k; i++) {
			RecurTreeDestroy(tree, head->children[i] );
		}
	}
	tree->delFunc(head->obj);
	free(head->children);
	free(head);
}
//destroys tree
void TreeDestroy(pTree tree) {
	if (tree == NULL) return;
	if(tree->head != NULL){
		RecurTreeDestroy(tree, tree->head);
	}
	free(tree);
}

int TreeNodesCount(pTree tree) {
	if (tree == NULL) return -1;
	return tree->nodeCount;
}

static void RecurElementPrint(pTree tree, PELEMENT head) {
	if (head == NULL) return;
	tree->printFunc(head->obj);
	if (head->childrenCount > 0) {
		for (int i = 0;i < tree->k; i++) {
			if (head->children + i != NULL) {
				RecurElementPrint(tree, head->children[i]);
			}	
		}
	}
}
void TreePrint(pTree tree) {
	if (tree == NULL) return;
	if (tree->head == NULL) return; 
	RecurElementPrint(tree, tree->head);
}


static PELEMENT CreateElement(pTree tree, pNode newNode, PELEMENT parentNode) {

	PELEMENT newElement = (PELEMENT)malloc(sizeof(ELEMENT)); // Create a new element
	if (newElement == NULL) return NULL;
	newElement->obj = tree->cloneFunc(newNode);
	newElement->parent = parentNode;
	newElement->childrenCount = 0;
	newElement->children = (PELEMENT*)malloc((tree->k) * sizeof(PELEMENT));
	if (newElement->children == NULL) {
		tree->delFunc(newElement->obj);
		free(newElement);
		return NULL;
	}
	for (int i = 0; i < tree->k; i++) {
		newElement->children[i] = NULL;
	}

	return newElement;
}

Result TreeAddLeaf(pTree tree, int parentKey, pNode newNode) {
	if (tree == NULL || newNode == NULL) return FAILURE;//input check
	if (tree->head == NULL) {
		// Make the new leaf a root:
		PELEMENT newElement = CreateElement(tree, newNode, NULL); // Create a new element
		if (newElement == NULL) return FAILURE;
		tree->head = newElement;
		tree->nodeCount++;
	}
	else {
		// find the tree element with desired key value:
		PELEMENT parentElem = RecurTreeGetElem(tree, parentKey,tree->head); 
		if (parentElem == NULL || parentElem->childrenCount == tree->k) return FAILURE;
		PELEMENT newElement = CreateElement(tree, newNode, parentElem); // Create a new element
		if (newElement == NULL) return FAILURE;
		for (int i = 0; i < tree->k; i++) { // Add the new element to the parent node's children, in the first free place
			if (parentElem->children[i] == NULL) {
				parentElem->children[i] = newElement;
				break;
			}
		}
		parentElem->childrenCount++;
		tree->nodeCount++;
	}
	return SUCCESS;

}
pNode TreeGetRoot(pTree tree) {
	if (tree == NULL) return NULL;
	return tree->cloneFunc(tree->head->obj);
}

static PELEMENT RecurTreeGetElem(pTree tree, int key, PELEMENT treeElem) {
	if (treeElem == NULL || tree == NULL) return NULL;//input check
	if (tree->getKeyFunc(treeElem->obj) == key) return treeElem;
	else {
		if (treeElem->childrenCount > 0) {
			PELEMENT tmpResult = NULL;
			for (int i = 0; i < tree->k; i++) {
				tmpResult = RecurTreeGetElem(tree, key, *(treeElem->children + i));
				if (tmpResult != NULL) return tmpResult;
			}
		}
		return NULL;
	}
}

pNode TreeGetNode(pTree tree, int key) {
	if (tree == NULL) return NULL;
	if (tree->head == NULL) return NULL;
	return tree->cloneFunc(RecurTreeGetElem(tree, key, tree->head)->obj);
}

pNode* TreeGetChildren(pTree tree, int key) {
	if (tree == NULL) return NULL;//input check
	if (tree->head == NULL) return NULL;//check if tree is empty
	PELEMENT parentElem = RecurTreeGetElem(tree, key,tree->head);
	if (parentElem == NULL) return NULL;
	pNode* childArr = (pNode*)malloc((tree->k) * sizeof(pNode));
	if (childArr == NULL) return NULL;
	for (int i = 0; i < tree->k; i++) {
		if (parentElem->children[i] != NULL) {
			childArr[i] = tree->cloneFunc(parentElem->children[i]->obj);
		}
		else {
			childArr[i] =  NULL;
		}
	}
	return childArr;
}

Result TreeNodeIsActive(pTree tree, int key, Bool* isActive) {
	if (tree == NULL) return FAILURE;//input check
	if (tree->head == NULL) return FAILURE;//check if tree is empty
	PELEMENT pElem = RecurTreeGetElem(tree, key, tree->head);
	if (pElem == NULL) return FAILURE;
	if (pElem->childrenCount < tree->k) {
		*isActive = TRUE;
	}
	else {
		*isActive = FALSE;
	}
	return SUCCESS;
}

Result TreeNodeIsLeaf(pTree tree, int key, Bool* isLeaf) {
	if (tree == NULL) return FAILURE;//input check
	if (tree->head == NULL) return FAILURE;//check if tree is empty
	PELEMENT pElem = RecurTreeGetElem(tree, key, tree->head);
	if (pElem == NULL) return FAILURE;
	if (pElem->childrenCount == 0) {
		*isLeaf = TRUE;
	}
	else {
		*isLeaf = FALSE;
	}
	return SUCCESS;
}

Result TreeDelLeaf(pTree tree, int key) {
	if (tree == NULL) return FAILURE;//input check
	if (tree->head == NULL) return FAILURE;//check if tree is empty
	PELEMENT pElem = RecurTreeGetElem(tree, key, tree->head);
	if (pElem == NULL) return FAILURE;

	if (pElem->childrenCount == 0) {
		if (pElem->parent != NULL) {//if not root element
			// Updating parent's data:
			pElem->parent->childrenCount--;
			PELEMENT tmpChild = NULL;
			for (int i = 0; i < tree->k; i++) {
				tmpChild = pElem->parent->children[i];
				if (tree->getKeyFunc(tmpChild->obj) == tree->getKeyFunc(pElem->obj)) {
					pElem->parent->children[i] = NULL;
					break;
				}
			}
		}
		tree->delFunc(pElem->obj);
		free(pElem);
		tree->nodeCount--;
		return SUCCESS;
	}
	else {
		return FAILURE;
	}
}
