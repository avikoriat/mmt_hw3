#ifndef TREE_H
#define TREE_H

#include "defs.h"



typedef void* pNode;
//the tree data structure:
typedef struct _tree Tree, *pTree;
//the basic element that comprises the tree:
typedef struct _ELEMENT ELEMENT;
typedef struct _ELEMENT* PELEMENT;

/*************************************************************************
Function name	: GetKeyFunction
Description		: returns the user-defined key of the data in the tree
Paramerters		: pNode - a void pointer
Return value	: key - an integer to identidy the specific node
************************************************************************/
typedef int (*GetKeyFunction)(pNode e);

/*************************************************************************
Function name	: CloneFunction
Description		: allocates a new copy of a node and returns it
Paramerters		: pNode - a void pointer
Return value	: pNode - a pointer to the user-defined node
************************************************************************/
typedef pNode (*CloneFunction)(pNode e);

/*************************************************************************
Function name	: PrintFunction
Description		: prints the Node in a user-defined manner
Paramerters		: pNode - a void pointer
Return value	: none
************************************************************************/
typedef void (*PrintFunction)(pNode e);

/*************************************************************************
Function name	: DelFunction
Description		: deletes the user-defined node and frees its memory allocations
Paramerters		: pNode - a void pointer
Return value	: none
************************************************************************/
typedef void (*DelFunction)(pNode e);

/*************************************************************************
Function name	: TreeCreate
Description		: creates an empty tree
Paramerters		: pointer to the different functions to handle the
                 user-defined node
Return value	: pTree - a pointer to the new tree
************************************************************************/
pTree TreeCreate(GetKeyFunction getKeyFunc,
	CloneFunction cloneFunc,
	PrintFunction printFunc,
	DelFunction delFunc,
	int k);

/*************************************************************************
Function name	: TreeDestroy
Description		: frees all memory allocations in the tree
Paramerters		: tree - a pointer to the tree
Return value	: none
************************************************************************/
void TreeDestroy(pTree tree);

/*************************************************************************
Function name	: TreeNodesCount
Description		: returns the amount of Nodes the tree holds
Paramerters		: tree - a pointer to the tree
Return value	: int - the amount of nodes
************************************************************************/
int TreeNodesCount(pTree tree);

/*************************************************************************
Function name	: TreePrint
Description		: prints every node in a tree in a pre-order manner using the 
				user-defined print function on each node
Paramerters		: tree - a pointer to the tree
Return value	: none
************************************************************************/
void TreePrint(pTree tree);

/*************************************************************************
Function name	: TreeAddLeaf
Description		: Adds a new clone of the node specified by newNode to
				 the tree element specified by parentKey. 
Paramerters		: tree - a pointer to the tree,
				  newNode - the node to be added,
				  parentKey - the key of the node we want to add the 
							  newNode to.
				  
Return value	:Result, SUCCESS if all goes well, FAILURE otherwise.
************************************************************************/
Result TreeAddLeaf(pTree tree, int parentKey, pNode newNode);

/*************************************************************************
Function name	: TreeGetRoot
Description		: returns a pointer to the user-defined root node
Paramerters		: tree - a pointer to the tree,
Return value	:
************************************************************************/
pNode TreeGetRoot(pTree tree);


/*************************************************************************
Function name	: TreeGetNode
Description		: clones copy of  a node whose key is specified by 'key' 
				  and returns it. the user has to free this memory allocation!
Paramerters		: tree - a pointer to the tree,
				  key  - the key of the desired node.
Return value	: pNode - a pointer to the user-defined node
************************************************************************/
pNode TreeGetNode(pTree tree, int key);

/*************************************************************************
Function name	: TreeGetChildren
Description		: allocates a new array and fills it with new copies of pointers to the 
					user defined nodes that belong to the children of the 
					node whose key is specified by 'key'. the user has to free all
					memory allocations - pointers to children and the array itself.
Paramerters		: tree - a pointer to the tree,
Return value	: pNode* a pointer to an array of pointers to Node (Node is
					the user-defined struct).
************************************************************************/
pNode* TreeGetChildren(pTree tree, int key);

/*************************************************************************
Function name	: TreeNodeIsActive
Description		: checks if a node has an available space for 1 or more children
				  if the answer is positive - the variable isActive is updated to TRUE,
				  else, to FALSE.
Paramerters		: tree - a pointer to the tree,
				  key - the key of the node to be checked,
				  isActive - a boolian to update with the answer.
Return value	: Result - SUCCESS if of goes well, FAILURE if key is not found
************************************************************************/
Result TreeNodeIsActive(pTree tree, int key, Bool* isActive);

/*************************************************************************
Function name	: TreeNodeIsLeaf
Description		:checks if a node has no children, if the answer is positive -
				  the variable isLeaf is updated to TRUE,
				  else, to FALSE.
Paramerters		: tree - a pointer to the tree,
				  key - the key of the node to be checked,
				  isLeaf - a boolian to update with the answer.
Return value	:
************************************************************************/
Result TreeNodeIsLeaf(pTree tree, int key, Bool* isLeaf);

/*************************************************************************
Function name	: TreeDelLeaf
Description		: deletes the node whose key is specified by 'key' i
					if it has no children.
Paramerters		: tree - a pointer to the tree,
				key - the key of the node to be deleted.
Return value	: Result - SUCCESS if of goes well, FAILURE if key is not 
				found or if the node has children 
************************************************************************/
Result TreeDelLeaf(pTree tree, int key);

#endif
