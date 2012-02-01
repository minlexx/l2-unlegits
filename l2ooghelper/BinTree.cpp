#include "stdafx.h"
#include "BinTree.h"

BinTreeNode::BinTreeNode()
{
	parent = left = right = NULL;
	key = 0;
}

BinTreeNode::BinTreeNode( unsigned int keyInit )
{
	parent =  left = right = NULL;
	key = keyInit;
	//printf( "BinTreeNode() constructed with key %u\n", key );
}

BinTreeNode::~BinTreeNode()
{
	//printf( "~BinTreeNode: destoyed %u\n", key );
}

//=============================================================
//=============================================================
//=============================================================

BinTree::~BinTree()
{
	deleteAll();
}

void BinTree::deleteAll()
{
	if( root )
	{
		del_recursive( root );
		delete root;
		root = NULL;
	}
}

void BinTree::del_recursive( BinTreeNode *node )
{
	if( !node ) return;
	if( node->left )
	{
		//printf( "cur %u; delete left\n", node->key );
		del_recursive( node->left );
		delete node->left;
	}
	if( node->right )
	{
		//printf( "cur %u; delete right\n", node->key );
		del_recursive( node->right );
		delete node->right;
	}
	/*node->left = NULL;
	node->right = NULL;
	node->key = 0;*/
}

bool BinTree::addNode( BinTreeNode *node )
{
	if( !node ) return false;
	if( !root )
	{
		root = node;
		root->parent = NULL;
		return true;
	}
	BinTreeNode *cur = root;
	//BinTreeNode *prev = root;
	while( true )
	{
		//prev = cur;
		if( node->key > cur->key )
		{
			if( cur->right == NULL )
			{
				//add to right
				node->parent = cur;
				cur->right = node;
				return true;
			}
			else
			{
				cur = cur->right;
			}
		}
		else if( node->key < cur->key )
		{
			if( cur->left == NULL )
			{
				// add to left
				node->parent = cur;
				cur->left = node;
				return true;
			}
			else
			{
				cur = cur->left;
			}
		}
		else if( node->key == cur->key )
		{
			return false;
		}
	}
	// never executed!
	return true;
}

#ifdef _DEBUG
bool BinTree::addNodeDebug( BinTreeNode *node, FILE *f )
{
	if( !node )
	{
		fprintf( f, "BinTree::addNode(): node == NULL!\n" );
		return false;
	}
	if( !root )
	{
		root = node;
		root->parent = NULL;
		fprintf( f, "BinTree::addNodeDebug(): added node key %u to root\n", node->key );
		return true;
	}
	BinTreeNode *cur = root;
	//BinTreeNode *prev = root;
	while( true )
	{
		//prev = cur;
		if( node->key > cur->key )
		{
			if( cur->right == NULL )
			{
				//add to right
				node->parent = cur;
				cur->right = node;
				fprintf( f, "BinTree::addNodeDebug(): added node key %u to right of key %u\n",
					node->key, cur->key );
				return true;
			}
			else
			{
				cur = cur->right;
			}
		}
		else if( node->key < cur->key )
		{
			if( cur->left == NULL )
			{
				// add to left
				node->parent = cur;
				cur->left = node;
				fprintf( f, "BinTree::addNodeDebug(): added node key %u to left of key %u\n",
					node->key, cur->key );
				return true;
			}
			else
			{
				cur = cur->left;
			}
		}
		else if( node->key == cur->key )
		{
			fprintf( f, "BinTree::addNodeDebug(): key %u already exists\n", node->key );
			return false;
		}
	}
	printf( "BinTree::addNode(): never executed!\n" );
	return true;
}
#endif

BinTreeNode *BinTree::findNode( unsigned int key )
{
	if( !root ) return NULL;
	BinTreeNode *cur = root;
	while( cur )
	{
		if( cur->key == key ) return cur;
		if( key > cur->key ) cur = cur->right;
		else if( key < cur->key ) cur = cur->left;
	}
	return cur;
}

bool BinTree::delNode( BinTreeNode *node )
{
	if( !node ) return false;
	if( (node->left == NULL) && (node->right == NULL) )
	{
		if( node->parent )
		{
			if( node->parent->left == node ) node->parent->left = NULL;
			if( node->parent->right == node ) node->parent->right = NULL;
		}
		else root = NULL;
		delete node;
		return true;
	}
	// у удаляемого узла имеется левое поддерево
	if( node->left && (node->right == NULL) )
	{
		if( node->parent ) // у удаляемого узла есть родитель
		{
			if( node->parent->left == node ) // узел - левая ссылка родителя
			{
				// теперь левая ссылка родителя указывает на левое поддерево текущего узла
				node->parent->left = node->left;
			}
			if( node->parent->right == node ) // узел - правая ссылка родителя
			{
				// теперь правая ссылка родителя указывает на левое поддерево текущего узла
				node->parent->right = node->left;
			}
			// обновим родителя левого поддерева - это наш родитель
			node->left->parent = node->parent;
		}
		else // родителя нет - мы удаляем корень
		{
			root = node->left;
			// теперь левое поддерево - корень, у него нет родителя
			node->left->parent = NULL;
		}
		delete node;
		return true;
	}
	// у удаляемого узла имеется правое поддерево
	if( node->right && (node->left == NULL) )
	{
		if( node->parent ) // у удаляемого узла есть родитель
		{
			if( node->parent->left == node ) // узел - левая ссылка родителя
			{
				// теперь левая ссылка родителя указывает на правое поддерево текущего узла
				node->parent->left = node->right;
			}
			if( node->parent->right == node ) // узел - правая ссылка родителя
			{
				// теперь правая ссылка родителя указывает на правое поддерево текущего узла
				node->parent->right = node->right;
			}
			// обновим родителя правого поддерева - это наш родитель
			node->right->parent = node->parent;
		}
		else // родителя нет - мы удаляем корень
		{
			root = node->right;
			// теперь правое поддерево - корень, у него нет родителя
			node->right->parent = NULL;
		}
		delete node;
		return true;
	}
	// у удаляемого узла имеются левое и правое поддерево
	if( node->left && node->right )
	{
		// левое поддерево мы воткнем на наше место...
		if( node->parent ) // у узла есть родитель
		{
			node->left->parent = node->parent;
			if( node->parent->left == node ) node->parent->left = node->left;
			if( node->parent->right == node ) node->parent->right = node->left;
		}
		else
		{
			root = node->left;
			node->left->parent = NULL;
		}
		// а правое поддерево добавим в получившееся дерево
		addNode( node->right );
		delete node;
	}
	return true;
}

bool BinTree::delNode( unsigned int key )
{
	if( !root ) return false;
	BinTreeNode *node = findNode( key );
	if( node ) return delNode( node );
	return false;
}

#ifdef _DEBUG
BinTreeNode *BinTree::findNodeDebug( unsigned int key, FILE *f )
{
	if( !f ) return NULL;
	if( !root ) return NULL;
	fprintf( f, "BinTree::findNodeDebug( int key = %u )\n", key );
	BinTreeNode *cur = root;
	while( cur )
	{
		fprintf( f, "cur = 0x%p (key %u)\n", cur, cur->key );
		if( cur->key == key ) { fprintf( f, "found key %u\n", cur->key ); return cur; }
		if( key > cur->key ) { fprintf( f, "... > %u; right\n", cur->key ); cur = cur->right; continue; }
		if( key < cur->key ) { fprintf( f, "... < %u; left\n", cur->key ); cur = cur->left; continue; }
	}
	if( cur ) fprintf( f, "returning 0x%p key %u\n", cur, cur->key );
	else fprintf( f, "key %u not found\n", key );
	return cur;
}

bool BinTree::delNodeDebug( unsigned int key, FILE *f )
{
	if( !f || !root ) return false;
	BinTreeNode *del = findNodeDebug( key, f );
	if( del ) return delNodeDebug( del, f );
	return false;
}

bool BinTree::delNodeDebug( BinTreeNode *node, FILE *f )
{
	if( !node || !f ) return false;
	fprintf( f, "delNodeDebug: del node key %u, parent = 0x%p\n", node->key, node->parent );
	if( (node->left == NULL) && (node->right == NULL) )
	{
		fprintf( f, "case 0: no children\n" );
		if( node->parent )
		{
			if( node->parent->left == node ) node->parent->left = NULL;
			if( node->parent->right == node ) node->parent->right = NULL;
		}
		else root = NULL;
		delete node;
		return true;
	}
	// у удаляемого узла имеется левое поддерево
	if( node->left && (node->right == NULL) )
	{
		fprintf( f, "case 1: left exists\n" );
		if( node->parent ) // у удаляемого узла есть родитель
		{
			if( node->parent->left == node ) // узел - левая ссылка родителя
			{
				// теперь левая ссылка родителя указывает на левое поддерево текущего узла
				node->parent->left = node->left;
			}
			if( node->parent->right == node ) // узел - правая ссылка родителя
			{
				// теперь правая ссылка родителя указывает на левое поддерево текущего узла
				node->parent->right = node->left;
			}
			// обновим родителя левого поддерева - это наш родитель
			node->left->parent = node->parent;
		}
		else // родителя нет - мы удаляем корень
		{
			root = node->left;
			// теперь левое поддерево - корень, у него нет родителя
			node->left->parent = NULL;
		}
		delete node;
		return true;
	}
	// у удаляемого узла имеется правое поддерево
	if( node->right && (node->left == NULL) )
	{
		fprintf( f, "case 2: right exists\n" );
		if( node->parent ) // у удаляемого узла есть родитель
		{
			if( node->parent->left == node ) // узел - левая ссылка родителя
			{
				// теперь левая ссылка родителя указывает на правое поддерево текущего узла
				node->parent->left = node->right;
			}
			if( node->parent->right == node ) // узел - правая ссылка родителя
			{
				// теперь правая ссылка родителя указывает на правое поддерево текущего узла
				node->parent->right = node->right;
			}
			// обновим родителя правого поддерева - это наш родитель
			node->right->parent = node->parent;
		}
		else // родителя нет - мы удаляем корень
		{
			root = node->right;
			// теперь правое поддерево - корень, у него нет родителя
			node->right->parent = NULL;
		}
		delete node;
		return true;
	}
	// у удаляемого узла имеются левое и правое поддерево
	if( node->left && node->right )
	{
		fprintf( f, "case 3: left and right exist\n" );
		// левое поддерево мы воткнем на наше место...
		if( node->parent ) // у узла есть родитель
		{
			node->left->parent = node->parent;
			if( node->parent->left == node ) node->parent->left = node->left;
			if( node->parent->right == node ) node->parent->right = node->left;
		}
		else
		{
			root = node->left;
			node->left->parent = NULL;
		}
		// а правое поддерево добавим в получившееся дерево
		addNodeDebug( node->right, f );
		delete node;
	}
	return true;
}

void printTreeNode( FILE *f, int recur_count, BinTreeNode *node )
{
	if( !node ) return;
	int i;
	//fprintf( f, "\n" );
	if( node->parent )
	{
		if( node->parent->left == node ) fprintf( f, "LEFT  " );
		else if( node->parent->right == node ) fprintf( f, "RIGHT " );
		else fprintf( f, "Parent unknown??? " );
	}
	else fprintf( f, "ROOT  " );
	for( i=0; i<recur_count; i++ ) fprintf( f, "." );
	fprintf( f, "%d (0x%p)\n", node->key, node );
	//fprintf( f, "\n" );
	if( node->left ) printTreeNode( f, recur_count+1, node->left );
	if( node->right ) printTreeNode( f, recur_count+1, node->right );
}

void BinTree::printTree( FILE *f )
{
	if( !f ) return;
	if( !root ) fprintf( f, "BinTree is empty!\n" );
	printTreeNode( f, 1, root );
}

#endif
