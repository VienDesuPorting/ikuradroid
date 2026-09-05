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

#include "group.h"

Widget *Group::widget_bin=0;

/*! \brief Creates a new Group object
 *	\param Parent Parent group
 *	\int Position Logical position (Group) to set for the group
 *
 *	If a parent object is provided, the group will add itself to the parent
 *	group automatically. This asserts that newly created groups are ordered 
 *  on the intended stack.
 */
Group::Group(Group *Parent,int Position){
	// Set defaults
	parent=Parent;
	widget_focus=widget_top=widget_bottom=0;
	group_top=group_bottom=0;
	NextPtr=PrevPtr=0;
	widgetcount=0;
	groupcount=0;

	if(parent){
		// Automatically assign to parent
		parent->AddGroup(this,Position);
	}
}

Group::~Group(){
	DestroyGroup();
}

/*! \brief Removes all widgets from the stack
 */
void Group::DropGroup(){
	// Drop all children groups
	if(!parent){
		groupcount=0;
		Group *gptr=group_top;
		group_top=group_bottom=0;
		while(gptr){
			Group *tmpptr=gptr->NextPtr;
			gptr=tmpptr;
		}
	}

	// Drop children widgets
	widgetcount=0;
	Widget *wptr=widget_top;
	widget_top=widget_bottom=0;
	while(wptr){
		Widget *tmpptr=wptr->NextPtr;
		wptr=tmpptr;
	}
}

/*! \brief Destroys all assigned objects in the group
 *
 *	If the object is a parent object it will destroy any assigned groups,
 *	otherwise it will just destroy assigned objects.
 */
void Group::DestroyGroup(){
	// Destroy all children groups
	if(!parent){
		groupcount=0;
		Group *gptr=group_top;
		group_top=group_bottom=0;
		while(gptr){
			Group *tmpptr=gptr->NextPtr;
			delete gptr;
			gptr=tmpptr;
		}
	}

	// Destroy children widgets
	widgetcount=0;
	Widget *wptr=widget_top;
	widget_top=widget_bottom=0;
	while(wptr){
		// Backup pointer and schedule object for deletion
		Widget *tmpptr=wptr->NextPtr;
		wptr->NextPtr=widget_bin;
		widget_bin=wptr;
		wptr=tmpptr;
	}
}


/*! \brief Destroys a widget and removes it from the group
 *	\param Widget Widget to destroy and remove
 *
 *  The object is not actually destroyed, but moved to a different stack
 *  were it will continue to exist until Purge() is called.
 */
void Group::DestroyWidget(Widget *WidgetPtr){
	if(WidgetPtr){
		if(DropWidget(WidgetPtr)){
			// Schedule object for deletion
			if(widget_bin){
				WidgetPtr->NextPtr=widget_bin;
				widget_bin=WidgetPtr->NextPtr;
			}
			else{
				WidgetPtr->NextPtr=0;
				widget_bin=WidgetPtr;
			}
		}
	}
}


/*! \brief Deletes previously removed/destroyed items
 */
void Group::Purge(){
	while(widget_bin){
		Widget *tmpptr=widget_bin;
		widget_bin=widget_bin->NextPtr;
		delete tmpptr;
	}
}

/*! \brief Counts assigned groups
 *	\returns Assigned groups
 *
 *	This will only work for parent objects!
 */
int Group::GetGroupCount(){
	return groupcount;
}

/*! \brief Counts assigned widgets
 *	\returns Assigned widgets
 *
 *	This is not recursive, assigned groups will not be checked
 */
int Group::GetWidgetCount(){
	return widgetcount;
}

/*! \brief Fetches an indexed group
 *  \returns Indexed group or NULL if out of range
 */
Group *Group::GetGroup(int Position){
	Group *retval=group_top;
	if(parent){
		retval=parent->group_top;
	}
	for(int i=1;retval && i<=Position;i++){
		retval=retval->NextPtr;
	}
	return retval;
}

bool Group::CheckWidget(Widget *WidgetPtr){
	bool returnval=false;
	if(WidgetPtr){
		// Search own widgets
		Widget *wptr=widget_top;
		while(wptr && wptr!=WidgetPtr){
			wptr=wptr->NextPtr;
		}
		returnval=wptr;

		// Search child group widgets
		Group *gptr=group_top;
		while(!returnval && gptr){
			returnval=gptr->CheckWidget(WidgetPtr);
			gptr=gptr->NextPtr;
		}
	}
	return returnval;
}

/*! \brief Fetches an indexed widget
 *  \param Position The index of the widget you want
 *  \returns Indexed group or NULL if out of range
 */
Widget *Group::GetWidget(int Position){
	Widget *retval=widget_top;
	for(int i=1;retval && i<=Position;i++){
		retval=retval->NextPtr;
	}
	return retval;
}

/*! \brief Fetches first widget at given position
 *  \param X Screen coordinate
 *  \param Y Screen coordinate
 *  \returns Indexed group or NULL if out of range
 */
Widget *Group::GetWidget(int X,int Y){
	Widget *retval=widget_top;
	while(retval){
		if(retval->TestMouse(X,Y)){
			break;
		}
		retval=retval->NextPtr;
	}
	return retval;
}

void Group::AddGroup(Group *GroupPtr,int Position){
	if(GroupPtr){
		if(!group_top){
			// First object!
			group_top=group_bottom=GroupPtr;
			GroupPtr->PrevPtr=0;
			GroupPtr->NextPtr=0;
		}
		else{
			if(!Position){
				// Top position
				GroupPtr->PrevPtr=0;
				GroupPtr->NextPtr=group_top;
				group_top->PrevPtr=GroupPtr;
				group_top=GroupPtr;
			}
			else if(Position>=groupcount){
				// Bottom position
				GroupPtr->PrevPtr=group_bottom;
				GroupPtr->NextPtr=0;
				group_bottom->NextPtr=GroupPtr;
				group_bottom=GroupPtr;
			}
			else{
				// Seek out a position
				Group *tmpptr;
				tmpptr=group_top;
				for(int i=0;tmpptr->NextPtr && i<Position;i++){
					tmpptr=tmpptr->NextPtr;
				}

				// Inject widget
				GroupPtr->NextPtr=tmpptr->NextPtr;
				GroupPtr->PrevPtr=tmpptr->PrevPtr;
				tmpptr->PrevPtr=GroupPtr;
				tmpptr->NextPtr=GroupPtr;
			}
		}
		groupcount++;
	}		
}

/*! \brief Removes a widget from the stack
 *  \returns True if widget was found and removed
 */
bool Group::DropWidget(Widget *WidgetPtr){
	bool retval=false;
	if(!WidgetPtr){
		// This is futile ...
	}
	else if(WidgetPtr==widget_top){
		// Drop top item
		retval=true;
		if(widget_bottom==WidgetPtr){
			widget_top=0;
			widget_bottom=0;
			widgetcount=0;
		}
		else{
			widget_top=widget_top->NextPtr;
			widget_top->PrevPtr=0;
			widgetcount--;
		}
	}
	else if(WidgetPtr==widget_bottom){
		// Drop bottom item
		widget_bottom=widget_bottom->PrevPtr;
		widget_bottom->NextPtr=0;
		widgetcount--;
		retval=true;
	}
	else{
		// Untangle from stack
		Widget *tmpptr=widget_top;
		while(tmpptr){
			if(tmpptr->NextPtr==WidgetPtr){
				// Found item
				widgetcount--;
				tmpptr->NextPtr=WidgetPtr->NextPtr;
				if(tmpptr->NextPtr){
					tmpptr->NextPtr->PrevPtr=tmpptr;
				}
				WidgetPtr->NextPtr=0;
				WidgetPtr->PrevPtr=0;
				retval=true;
				break;
			}
			tmpptr=tmpptr->NextPtr;
		}
	}
	return retval;
}

/*! \brief Inverse the direction of widgets (Top=bottom, bottom=top)
 */
void Group::FlipWidgets(){
	Group *tmpgroup=new Group(0);
	int oldcnt=widgetcount;
	for(int i=0;i<oldcnt;i++){
		Widget *tmpwidget=GetWidget(oldcnt-i);
		DropWidget(tmpwidget);
		tmpgroup->AddWidget(tmpwidget,i);
	}
}

void Group::AddWidget(Widget *WidgetPtr,int Position){
	if(WidgetPtr){
		if(!widget_top){
			// First object!
			widget_top=widget_bottom=WidgetPtr;
			WidgetPtr->PrevPtr=0;
			WidgetPtr->NextPtr=0;
		}
		else{
			if(!Position){
				// Top position
				WidgetPtr->PrevPtr=0;
				WidgetPtr->NextPtr=widget_top;
				widget_top->PrevPtr=WidgetPtr;
				widget_top=WidgetPtr;
			}
			else if(Position>=widgetcount || Position<0){
				// Bottom position
				WidgetPtr->PrevPtr=widget_bottom;
				WidgetPtr->NextPtr=0;
				widget_bottom->NextPtr=WidgetPtr;
				widget_bottom=WidgetPtr;
			}
			else{
				// Seek out a position
				Widget *tmpptr=widget_top;
				for(int i=0;tmpptr->NextPtr && i<Position;i++){
					tmpptr=tmpptr->NextPtr;
				}

				// Inject widget
				WidgetPtr->NextPtr=tmpptr->NextPtr;
				WidgetPtr->PrevPtr=tmpptr->PrevPtr;
				tmpptr->PrevPtr=WidgetPtr;
				tmpptr->NextPtr=WidgetPtr;
			}
		}

		// Count widgets as they are added
		WidgetPtr->Refresh();
		widgetcount++;
	}
}

void Group::MoveWidget(Widget *WidgetPtr,int Position){
	if(WidgetPtr){
		if(DropWidget(WidgetPtr)){
			AddWidget(WidgetPtr,Position);
		}
	}
}

