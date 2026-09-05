/*! \class Group
 *  \brief Groups widgets into logical layers
 *
 *  The group object manages a collection of widgets and/or other groups, but
 *  does not handle events or input focus type of things (See DialogBase for
 *  dialog handling). It merely clusters widgets together (usually) into 
 *  so-called layers.
 *
 *  The Group class is also responsible for safely destroying client widgets
 *  using the Purge() mechanism.
 *
 *	The stack is bidirectional and can be visualized like this:
 *
 *  LAST                               FIRST
 *  0<-TOP->Next     ...     Prev<-BOTTOM->0
 */
#ifndef _GROUP_H_
#define _GROUP_H_

#include "widget.h"

class Group {
	private:
		static Widget *widget_bin;	//!< Objects scheduled for destruction
		Group *group_top;			//!< Top group (for parents only)
		Group *group_bottom;		//!< Bottom group (for parents only)
		Group *parent;				//!< Parent group (Null for parent)
		Widget *widget_top;			//!< Top widget of THIS group
		Widget *widget_bottom;		//!< Bottom widget of THIS group
		Widget *widget_focus;		//!< Holds current focus
		int widgetcount;			//!< Widgets attached to THIS group
		int groupcount;				//!< Groups attached to this group
	public:
		Group(Group *Parent,int Position=0);
		~Group();
		int GetGroupCount();
		int GetWidgetCount();
		Group *GetGroup(int Position);
		Widget *GetWidgetByTag(int Tag);
		Widget *GetWidget(int Position);
		Widget *GetWidget(int X,int Y);
		void AddGroup(Group *GroupPtr,int Position=0);
		void MoveGroup(Group *GroupPtr,int Position);
		void DropGroup(Group *GroupPtr);
		void FlipWidgets();
		void AddWidget(Widget *WidgetPtr,int Position=0);
		void MoveWidget(Widget *WidgetPtr,int Position);
		bool CheckWidget(Widget *WidgetPtr);
		bool DropWidget(Widget *WidgetPtr);

		// Removes objects from their stacks
		void DestroyWidget(Widget *WidgetPtr);
		void DestroyGroup();
		void DropGroup();

		// Deletes removed objects
		static void Purge();

		// Navigation pointers	
		Group *PrevPtr;
		Group *NextPtr;
};

#endif


