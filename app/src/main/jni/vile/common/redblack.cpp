/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "redblack.h"

RedBlackLeave::RedBlackLeave(){
};

RedBlackLeave::~RedBlackLeave(){
};

RedBlackNode::RedBlackNode(RedBlackLeave *Leave){
	entry=Leave;
};

RedBlackNode::~RedBlackNode(){
	delete entry;
};

/*! \brief Contructor for a new tree object
 *  \param Nil Node which will always stay on top
 *  \param Root Node which will always stay at the bottom
 *
 *  Nil and Root must be leave objects' whose Compare methods returns
 *  the min or max of the objects range. This asserts top and bottom of
 *  the stack without doublechecking pointers.
 */
RedBlackTree::RedBlackTree(RedBlackLeave *Nil,RedBlackLeave *Root)
{
	nil=new RedBlackNode(Nil);
	nil->left=nil->right=nil->parent=nil;
	nil->color=RB_BLACK;
	root=new RedBlackNode(Root);
	root->parent=root->left=root->right=nil;
	root->color=RB_BLACK;
	count=0;
}

/*!\brief Rotates x to the left of its parent
 * \param x Node to rotate
 */
void RedBlackTree::LeftRotate(RedBlackNode* x) {
	RedBlackNode* y;
	y=x->right;
	x->right=y->left;
	if(y->left!=nil){
		y->left->parent=x;
	}
	y->parent=x->parent;   
	if( x == x->parent->left) {
		x->parent->left=y;
	}
	else{
		x->parent->right=y;
	}
	y->left=x;
	x->parent=y;
}

/*!\brief Rotates y to the right of its parent
 * \param y Node to rotate
 */
void RedBlackTree::RightRotate(RedBlackNode* y) {
	RedBlackNode* x;
	x=y->left;
	y->left=x->right;
	if(nil!=x->right){
		x->right->parent=y;
	}
	x->parent=y->parent;
	if(y==y->parent->left){
		y->parent->left=x;
	}
	else{
		y->parent->right=x;
	}
	x->right=y;
	y->parent=x;
}

/*!\brief Helper function to insert a leaf
 * \param z Lead to insert
 * \param Mode Search mode
 */
void RedBlackTree::TreeInsertHelp(RedBlackNode* z,int Mode) {
	/*  This function should only be called by RedBlackTree::Insert */
	RedBlackNode* x;
	RedBlackNode* y;
	z->left=z->right=nil;
	y=root;
	x=root->left;
	while( x != nil) {
		y=x;
		if(x->entry->Compare(z->entry,Mode)>0){ 
			x=x->left;
		}
		else{
			x=x->right;
		}
	}
	z->parent=y;
	if((y==root)||(y->entry->Compare(z->entry,Mode)>0)){ 
		y->left=z;
	}
	else{
		y->right=z;
	}
}

/*!\brief Insert a leaf
 * \param z Lead to insert
 * \param Mode Search mode
 */
RedBlackNode * RedBlackTree::Insert(RedBlackLeave * Leave,int Mode)
{
	RedBlackNode * y;
	RedBlackNode * x;
	RedBlackNode * node;
	count++;
	x = new RedBlackNode(Leave);
	TreeInsertHelp(x,Mode);
	node = x;
	x->color=RB_RED;
	while(x->parent->color){
		if(x->parent==x->parent->parent->left){
			y=x->parent->parent->right;
			if(y->color){
				x->parent->color=RB_BLACK;
				y->color=RB_BLACK;
				x->parent->parent->color=RB_RED;
				x=x->parent->parent;
			}
			else{
				if(x==x->parent->right){
					x=x->parent;
					LeftRotate(x);
				}
				x->parent->color=RB_BLACK;
				x->parent->parent->color=RB_RED;
				RightRotate(x->parent->parent);
			} 
		}
		else{
			y=x->parent->parent->left;
			if(y->color){
				x->parent->color=RB_BLACK;
				y->color=RB_BLACK;
				x->parent->parent->color=RB_RED;
				x=x->parent->parent;
			}
			else{
				if(x==x->parent->left){
					x=x->parent;
					RightRotate(x);
				}
				x->parent->color=RB_BLACK;
				x->parent->parent->color=RB_RED;
				LeftRotate(x->parent->parent);
			} 
		}
	}
	root->left->color=RB_BLACK;
	return node;
}

/*! \brief Gets the succeeding leave
 *  \param x Leave to get the successor of
 *  \return Successor of x or NULL if failed
 */
RedBlackNode * RedBlackTree::GetSuccessorOf(RedBlackNode * x) const
{ 
	RedBlackNode* y;
	if(nil!=(y=x->right)){
		// Get the minium of the right subtree of x
		while(y->left!=nil){
			y=y->left;
		}
		return y;
	}
	else{
		y=x->parent;
		while(x==y->right){
			x=y;
			y=y->parent;
		}
		return y==root?nil:y;
	}
}

/*! \brief Gets the preceeding leave
 *  \param x Leave to get the predecessor of
 *  \return Predecessor of x or NULL if failed
 */
RedBlackNode * RedBlackTree::GetPredecessorOf(RedBlackNode * x) const {
	RedBlackNode* y;
	if(nil!=(y=x->left)){
		// Get maximum of the left subtree of x
		while(y->right!=nil){
			y=y->right;
		}
		return y;
	}
	else{
		y=x->parent;
		while(x==y->left){ 
			if(y==root){
				return nil;
			}
			x=y;
			y=y->parent;
		}
		return y;
	}
}

RedBlackTree::~RedBlackTree() {
	Destroy(root);
	delete nil;
}

/*! \brief Recursively destroy nodes
 */
void RedBlackTree::Destroy(RedBlackNode *Node){
	if(Node && Node!=nil) {
		Destroy(Node->left);
		Destroy(Node->right);
		Node->left=0;
		Node->right=0;
		delete Node;
	}
}

RedBlackLeave *RedBlackTree::Search(RedBlackLeave *x,int Mode){
	RedBlackNode *tmpptr=root;
	while(tmpptr){
		if(tmpptr==nil){
			return 0;
		}
		else{
			int t=tmpptr->entry->Compare(x,Mode);
			if(t>0){
				tmpptr=tmpptr->left;
			}
			else if(t<0){
				tmpptr=tmpptr->right;
			}
			else{
				return tmpptr->entry;
			}
		}
	}
	return 0;
}

void RedBlackTree::Enumerate(RedBlackNode *Node,
		RedBlackLeave **Buffer,int &Count,int &Size){
	if(Node==nil || Count==Size){
		// Stop search
	}
	else{
		// Recursively add nodex
		Buffer[Count++]=Node->entry;
		Enumerate(Node->left,Buffer,Count,Size);
		Enumerate(Node->right,Buffer,Count,Size);
	}
}

int RedBlackTree::Enumerate(RedBlackLeave **Buffer,int Size){
	int cnt=0;
	Enumerate(root->left,Buffer,cnt,Size);
	Enumerate(root->right,Buffer,cnt,Size);
	return cnt;
}

int RedBlackTree::Count(){
	return count;
}

/*! \brief Restores red-black properties after node removal
 *  \param x Child of the spliced out node
 */
void RedBlackTree::DeleteFixUp(RedBlackNode* x) {
	RedBlackNode *w;
	RedBlackNode *rootLeft=root->left;
	while((!x->color) && (rootLeft!=x)){
		if(x==x->parent->left){
			w=x->parent->right;
			if(w->color){
				w->color=RB_BLACK;
				x->parent->color=RB_RED;
				LeftRotate(x->parent);
				w=x->parent->right;
			}
			if((!w->right->color) && (!w->left->color)){ 
				w->color=RB_RED;
				x=x->parent;
			}
			else{
				if(!w->right->color){
					w->left->color=RB_BLACK;
					w->color=RB_RED;
					RightRotate(w);
					w=x->parent->right;
				}
				w->color=x->parent->color;
				x->parent->color=RB_BLACK;
				w->right->color=RB_BLACK;
				LeftRotate(x->parent);
				x=rootLeft;
			}
		}
		else{
			w=x->parent->left;
			if(w->color){
				w->color=RB_BLACK;
				x->parent->color=RB_RED;
				RightRotate(x->parent);
				w=x->parent->left;
			}
			if((!w->right->color) && (!w->left->color)){ 
				w->color=RB_RED;
				x=x->parent;
			}
			else{
				if(!w->left->color){
					w->right->color=RB_BLACK;
					w->color=RB_RED;
					LeftRotate(w);
					w=x->parent->left;
				}
				w->color=x->parent->color;
				x->parent->color=RB_BLACK;
				w->left->color=RB_BLACK;
				RightRotate(x->parent);
				x=rootLeft;
			}
		}
	}
	x->color=RB_BLACK;
}

/*! \brief Deletes a node from the tree
 *  \param z Node to be deleted
 */
RedBlackLeave * RedBlackTree::DeleteNode(RedBlackNode * z){
	RedBlackNode* y;
	RedBlackNode* x;
	RedBlackLeave * retval = z->entry;
	y=((z->left==nil) || (z->right == nil)) ? z : GetSuccessorOf(z);
	x=(y->left==nil) ? y->right : y->left;
	if(root==(x->parent=y->parent)){
		root->left=x;
	}
	else{
		if(y==y->parent->left){
			y->parent->left=x;
		}
		else{
			y->parent->right=x;
		}
	}
	if(y!=z){
		y->left=z->left;
		y->right=z->right;
		y->parent=z->parent;
		z->left->parent=z->right->parent=y;
		if(z==z->parent->left){
			z->parent->left=y;
		}
		else{
			z->parent->right=y;
		}
		if(!(y->color)){
			y->color = z->color;
			DeleteFixUp(x);
		}
		else{
			y->color = z->color; 
		}
		delete z;
  		count--;
	}
	else{
		if(!(y->color)){
			DeleteFixUp(x);
		}
		delete y;
  		count--;
	}
	return retval;
}

