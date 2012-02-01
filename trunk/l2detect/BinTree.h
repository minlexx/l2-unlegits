#ifndef BINTREE_H_
#define BINTREE_H_

class BinTreeNode
{
public:
	BinTreeNode();
	BinTreeNode( unsigned int keyInit );
	virtual ~BinTreeNode();
public:
	BinTreeNode *parent;
	BinTreeNode *left;
	BinTreeNode *right;
	unsigned int key;
};

class BinTree
{
public:
	BinTree() { root = NULL; }
	virtual ~BinTree();
public:
	bool         addNode( BinTreeNode *node );
	BinTreeNode *findNode( unsigned int key );
	bool         delNode( BinTreeNode *node );
	bool         delNode( unsigned int key );

	void         deleteAll();

	BinTreeNode *getRoot() const { return this->root; }

#ifdef _DEBUG
public:
	bool         addNodeDebug( BinTreeNode *node, FILE *f );
	BinTreeNode *findNodeDebug( unsigned int key, FILE *f );
	bool         delNodeDebug( unsigned int key, FILE *f );
	bool         delNodeDebug( BinTreeNode *node, FILE *f );
	void         printTree( FILE *f );
#endif

protected:
	void del_recursive( BinTreeNode *node );
protected:
	BinTreeNode *root;
};

#endif /* BINTREE_H_ */
