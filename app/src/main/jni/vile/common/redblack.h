#ifndef E_REDBLACK_TREE
#define E_REDBLACK_TREE

#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <strings.h>
#include <string>

#define RB_RED				true
#define RB_BLACK			false

/*! \class RedBlackLeave
 *  \brief Base class for searchable items
 */
class RedBlackLeave {
	public:
		RedBlackLeave();
		virtual ~RedBlackLeave();
		virtual int Compare(RedBlackLeave *Leave,int Mode)=0;
};

/*! \class RedBlackNode
 *  \brief Internal class for managing items
 */
class RedBlackNode {
	private:
		friend class RedBlackTree;
		RedBlackLeave *entry;
		RedBlackNode *left;
		RedBlackNode *right;
		RedBlackNode *parent;
		bool color;
	public:
		RedBlackNode(RedBlackLeave *Leave);
		~RedBlackNode();
};

class RedBlackTree {
	private:
		// Helper funcions
		void LeftRotate(RedBlackNode *Node);
		void RightRotate(RedBlackNode *Node);
		void TreeInsertHelp(RedBlackNode *Node,int Mode);
		void FixUpMaxHigh(RedBlackNode *Node);
		void DeleteFixUp(RedBlackNode *Nodw);
		RedBlackNode *GetPredecessorOf(RedBlackNode *Node) const;
		RedBlackNode *GetSuccessorOf(RedBlackNode *Node) const;

		// Recursive iterators
		void Destroy(RedBlackNode *Node);
		void Enumerate(RedBlackNode *Node,RedBlackLeave **B,int &C,int &L);

		// Object data
		RedBlackNode *root;
		RedBlackNode *nil;
		int count;
	public:
		RedBlackTree(RedBlackLeave *Nil,RedBlackLeave *Root);
		~RedBlackTree();
		RedBlackLeave *DeleteNode(RedBlackNode *Node);
		RedBlackNode *Insert(RedBlackLeave *Leave,int Mode);
		RedBlackLeave *Search(RedBlackLeave *Leave,int Mode);
		int Enumerate(RedBlackLeave **Buffer,int Size);
		int Count();
};


#endif
