#include <stdio.h>
#include <stdlib.h>
#include "partition.h"
#include "gentree.h"

#define NUM_CHILDREN 4
#define X_LEFT_INIT 0.0
#define X_RIGHT_INIT 1.0
#define Y_BOT_INIT 0.0
#define Y_TOP_INIT 1.0
#define ROOT_KEY 0


typedef double BOUNDARY;
typedef double COORDINATE;

//global pointer to tree:
static pTree pPartTree = NULL;
typedef struct _partition_node {
	BOUNDARY x_left;
	BOUNDARY x_right;
	BOUNDARY y_bot;
	BOUNDARY y_top;
	int key;
}partNode, *ppartNode;

///////////////////// internal static functions //////////////////////
/*************************************************************************
Function name	: CreatePartNode
Description     : creates the Node of the square partition and returns a pointer
		to it
Paramerters     :x_left, x_right, y_left, y_right boundaries of the
		 partition to initialize the node
Return value	: ppartNode - a pointer to the node
************************************************************************/
static ppartNode CreatePartNode(BOUNDARY x_left,
	BOUNDARY x_right,
	BOUNDARY y_bot,
	BOUNDARY y_top,
	int key);
	
/*************************************************************************
Function name	: getNewSquareBoudaries
Description     : updates the values of the boundaries according to the 
		boundaries of the current node pointed by pNode and the 
		coordinates x y
Paramerters     :x_left, x_right, y_left, y_right boundaries to be updated,
		coordinates of the new partition
		a pointer to the node of the current partition
Return value	: none
************************************************************************/
static void getNewSquareBoudaries(BOUNDARY* x_left,
	BOUNDARY* x_right,
	BOUNDARY* y_bot,
	BOUNDARY* y_top,
	COORDINATE x,
	COORDINATE y,
	ppartNode pNode);

/*************************************************************************
Function name	: IsContained
Description     : checks if the coordinates x and y are contained inside 
		the boundaries of the node pointed by pNode
Paramerters     :pNode- the partition node to check
		x, y - the coordinates to check
Return value	: Bool true if contained else false
************************************************************************/
static Bool IsContained(ppartNode pNode,
	COORDINATE x,
	COORDINATE y);

/*************************************************************************
Function name	: RecurRefineCell
Description     : helper to Refine cell - recursively searches the correct 
		node to insert the new partition node under
Paramerters     :x, y coordinates of the new partition
		curNode - the current node to search
Return value	: none
************************************************************************/
static void RecurRefineCell(COORDINATE x, COORDINATE y, ppartNode curNode);

/*************************************************************************
Function name	: PartitionAddNode
Description     : adds under the current partition the new partition for
		x,y 
Paramerters     :pparentNode - a pointer to the current partition node
		x,y coordinates of the partition.
Return value	: none
************************************************************************/
static void PartitionAddNode(COORDINATE x,
	COORDINATE y,
	ppartNode pparentNode);

/*************************************************************************
Function name	: DelChildArr
Description     : deletes the array that the function TreeGetChildren allocates
Paramerters     :ChildpArr - a pointer to the array
Return value	: none
************************************************************************/
static void DelChildArr(ppartNode* ChildpArr);
//////////////////////////////////////////////////////////////////////


//////////////// functions to be used by the tree ADT ///////////////
pNode partitionClone(pNode pNode);
void partitionPrint(pNode pNode);
void partitionDel(pNode pNode);
int partitionGetKey(pNode pNode);
////////////////////////////////////////////////////////////////////

pNode partitionClone(pNode pNode) {
	if (pNode == NULL) return NULL;
	ppartNode pnewNode = (ppartNode)malloc(sizeof(partNode));
	if (pnewNode == NULL) return NULL;
	pnewNode->x_left = ((ppartNode)pNode)->x_left;
	pnewNode->x_right = ((ppartNode)pNode)->x_right;
	pnewNode->y_bot = ((ppartNode)pNode)->y_bot;
	pnewNode->y_top = ((ppartNode)pNode)->y_top;
	pnewNode->key = ((ppartNode)pNode)->key;
	return pnewNode;
}

void partitionPrint(pNode pNode) {
	if (pNode == NULL) return;
	printf("([%f, %f], [%f, %f])", ((ppartNode)pNode)->x_left,
		((ppartNode)pNode)->x_right,
		((ppartNode)pNode)->y_bot,
		((ppartNode)pNode)->y_top);
	ppartNode* ChildpArr = (ppartNode *)TreeGetChildren(pPartTree, ((ppartNode)pNode)->key);
	for (int i = 0; i < NUM_CHILDREN; i++) {
		if (ChildpArr[i] != NULL) {
			putchar('\\');
			printf("([%f, %f], [%f, %f])", ChildpArr[i]->x_left,
				ChildpArr[i]->x_right,
				ChildpArr[i]->y_bot,
				ChildpArr[i]->y_top);
		}
	}
	putchar('\n');
	DelChildArr(ChildpArr);
}


void partitionDel(pNode pNode) {
	free((ppartNode)pNode);
}

int partitionGetKey(pNode pNode) {
	return ((ppartNode)pNode)->key;
}
//////////////////////////////////////////////////////////////////////



static ppartNode CreatePartNode(BOUNDARY x_left,
	BOUNDARY x_right,
	BOUNDARY y_bot,
	BOUNDARY y_top,
	int key) {
	ppartNode pnewNode = (ppartNode)malloc(sizeof(partNode));
	if (pnewNode == NULL) return NULL;
	pnewNode->x_left = x_left;
	pnewNode->x_right = x_right;
	pnewNode->y_bot = y_bot;
	pnewNode->y_top = y_top;
	pnewNode->key = key;
	return pnewNode;
}



static void getNewSquareBoudaries(BOUNDARY* x_left,
	BOUNDARY* x_right,
	BOUNDARY* y_bot,
	BOUNDARY* y_top,
	COORDINATE x,
	COORDINATE y,
	ppartNode pNode) {

	BOUNDARY x_width = (pNode->x_right - pNode->x_left) ;
	BOUNDARY y_width = (pNode->y_top - pNode->y_bot);
	BOUNDARY x_mid = pNode->x_left + x_width / 2;
	BOUNDARY y_mid = pNode->y_bot + y_width / 2;

	if (x < x_mid) {
		*x_left = pNode->x_left;
		*x_right = x_mid;
	}
	else {
		*x_left = x_mid;
		*x_right = pNode->x_right;
	}
	if (y < y_mid) {
		*y_bot = pNode->y_bot;
		*y_top = y_mid;
	}
	else {
		*y_bot = y_mid;
		*y_top = pNode->y_top;
	}
}

static int GenerateKey() {
	static int key = ROOT_KEY;
	key++;
	return key; 
}

static Bool IsContained(ppartNode pNode,
	COORDINATE x,
	COORDINATE y) {
	return (x >= pNode->x_left) &&
		(x < pNode->x_right) &&
		(y >= pNode->y_bot) &&
		(y < pNode->y_top);
}

static void DelChildArr(ppartNode* ChildpArr) {
	for (int i = 0; i < NUM_CHILDREN; i++) {
		if (ChildpArr[i] != NULL) {
			partitionDel(ChildpArr[i]);
		}
	}
	free(ChildpArr);
}

static void PartitionAddNode(COORDINATE x,
	COORDINATE y,
	ppartNode pparentNode) {
	BOUNDARY x_left;
	BOUNDARY x_right;
	BOUNDARY y_bot;
	BOUNDARY y_top;
	getNewSquareBoudaries(&x_left, &x_right, &y_bot, &y_top, x, y, pparentNode);
	ppartNode pChildNode = CreatePartNode(x_left, x_right, y_bot, y_top, GenerateKey());
	if (pChildNode == NULL) return;
	//insert new node:
	TreeAddLeaf(pPartTree, partitionGetKey(pparentNode), pChildNode);
	partitionDel(pChildNode);//the Tree now has a clone of it, so we can delete it
}

static void RecurRefineCell(COORDINATE x, COORDINATE y, ppartNode curNode){
	int currKey = partitionGetKey(curNode);
	Bool isLeaf = FALSE;
	TreeNodeIsLeaf(pPartTree, currKey, &isLeaf);
	/*go over children to look for a match to current key.
		  if found, recurse into this child, else create
		  new node and insert it in the appropriate place */
	ppartNode* ChildpArr = (ppartNode*)TreeGetChildren(pPartTree, currKey);
	if (!isLeaf) {
		if (ChildpArr == NULL) return;
		for (int i = 0; i < NUM_CHILDREN; i++) {
			if (ChildpArr[i] != NULL) {
				if (IsContained(ChildpArr[i], x, y)) {
					RecurRefineCell(x, y, ChildpArr[i]);
					DelChildArr(ChildpArr);
					return;
				}
			}
		}
		
	}
	DelChildArr(ChildpArr);
	PartitionAddNode(x, y, curNode);
}

/* Refinement function */
void RefineCell(COORDINATE x, COORDINATE y) {
	if (x < 0 || x>1 || y < 0 || y>1) return;//boundary check
	ppartNode prootNode = TreeGetRoot(pPartTree);
	RecurRefineCell(x,y, prootNode);
	partitionDel(prootNode);
}

/* Initialization function */
void InitPartition() {
	if (pPartTree != NULL) {//if not first initialization
		DeletePartition(pPartTree);
	}
	pPartTree = TreeCreate(partitionGetKey,
		partitionClone,
		partitionPrint,
		partitionDel,
		NUM_CHILDREN);
	ppartNode pnewNode = CreatePartNode(X_LEFT_INIT, X_RIGHT_INIT, Y_BOT_INIT, Y_TOP_INIT, ROOT_KEY);
	if (pnewNode == NULL) return;
	TreeAddLeaf(pPartTree, -1, pnewNode);//the value -1 is arbitrary and ignored on first addition
	partitionDel(pnewNode);
}

/* Printing function */
void PrintPartition() {
	TreePrint(pPartTree);
}

/* Destory function */
void DeletePartition() {
	TreeDestroy(pPartTree);
}
