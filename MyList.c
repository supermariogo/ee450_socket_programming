/*========================================================================
#     FileName: Mylist.c
#         Desc: basic list structure
#       Author: Mark
#   LastChange: 2013-09-04 17:57:55
========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "MyList.h"

int  MyListLength(MyList* List)
{
	return List->num_members;
}
int  MyListEmpty(MyList* List)
{
	if(List->num_members==0)
		return TRUE;
	else
		return FALSE;
}

int  MyListAppend(MyList* List, void* obj)
{
	/*
	 *Add obj after Last(). 
	 *This function returns TRUE if the operation is performed successfully 
	 *and returns FALSE otherwise.	
	 */
	MyListElem* Elem=(MyListElem*)malloc(sizeof(MyListElem));
	if(!Elem)
		return FALSE;
	else{
		Elem->obj = obj;
		Elem->next = &(List->anchor);
		Elem->prev =List->anchor.prev;
		(List->anchor.prev)->next =Elem;
		List->anchor.prev = Elem;
		
		List->num_members++;
		return TRUE;
		}
}
int  MyListPrepend(MyList* List, void* obj){
	MyListElem* Elem=(MyListElem*)malloc(sizeof(MyListElem));
	if(!Elem)
		return FALSE;
	else{
		Elem->obj = obj;
		Elem->next = List->anchor.next;
		Elem->prev = &(List->anchor);
		(List->anchor.next)->prev =Elem;
		List->anchor.next = Elem;
		
		List->num_members++;
		return TRUE;
		}
	}
void MyListUnlink(MyList* List, MyListElem* Elem){
	/*
	 *Unlink and delete elem from the list. 
	 *Please do not delete the object pointed to by elem and
	 *do not check if elem is on the list.
	 */
	Elem->prev->next = Elem->next;
	Elem->next->prev = Elem->prev;
	free(Elem);
	List->num_members--;
	}
void MyListUnlinkAll(MyList* List){
	MyListElem* pElem=List->anchor.next;
	while(pElem != &(List->anchor)){
		pElem = pElem->next;
		free(pElem->prev);
		}
	MyListInit(List);/*make num_members=0; make anthor.next=prev=anthor*/ 
	}
int  MyListInsertAfter(MyList* List, void* obj, MyListElem* Elem){
	/*
	 *Insert obj between elem and elem->next. 
	 *If elem is NULL, then this is the same as Append().
	 *This function returns TRUE if the operation is performed successfully 
	 *and returns FALSE otherwise. Please do not check if elem is on the list.	
	 */
	if(Elem == NULL)
		return MyListAppend(List,obj);
	else{
		MyListElem* NewElem=(MyListElem*)malloc(sizeof(MyListElem));
		if(!NewElem)
			return FALSE;
		else{
			NewElem->obj = obj;
			NewElem->next = Elem->next;
			NewElem->prev = Elem;
			Elem->next->prev = NewElem;
			Elem->next = NewElem;
			List->num_members++;
			return TRUE;
			}
		}
	}
int  MyListInsertBefore(MyList* List, void* obj, MyListElem* Elem){
	if(Elem == NULL)
		return MyListPrepend(List,obj);
	else{
		MyListElem* NewElem=(MyListElem*)malloc(sizeof(MyListElem));
		if(!NewElem)
			return FALSE;
		else{
			NewElem->obj = obj;
			NewElem->next = Elem;
			NewElem->prev = Elem->prev;
			Elem->prev->next = NewElem;
			Elem->prev = NewElem;
			List->num_members++;
			return TRUE;
			}
		}

	}

MyListElem *MyListFirst(MyList* List){
	if(List->num_members!=0)
		return List->anchor.next;
	else
		return NULL;
	}
MyListElem *MyListLast(MyList* List){
	if(List->num_members!=0)
		return List->anchor.prev;
	else
		return NULL;
	}
MyListElem *MyListNext(MyList* List, MyListElem* Elem){
	/*
	 *Returns elem->next or NULL if elem is the last item on the list. 
	 *Please do not check if elem is on the list.	
	 */
	if(Elem==List->anchor.prev)
		return NULL;
	else
		return Elem->next;
	}
MyListElem *MyListPrev(MyList* List, MyListElem* Elem){
    /*
     *Returns elem->prev or NULL if elem is the first item on the list.
     *Please do not check if elem is on the list	
     */
	if(Elem==List->anchor.next)
		return NULL;
	else
		return Elem->prev;
	}

MyListElem *MyListFind(MyList* List, void* obj)
{
    /*
     *Returns the list element elem such that elem->obj == obj. 
     *Returns NULL if no such element can be found
     */
	MyListElem* pElem=NULL;

	if(List->num_members==0)
		return NULL;
	else{
		pElem=List->anchor.next;
		
		while(pElem != &(List->anchor))
			{
			if(pElem->obj == obj)
				return pElem;
			else
				pElem=pElem->next;
			}
		return NULL;
		}
}

int MyListInit(MyList* List)
{
/*
 *num_members of the list should be 0, 
 *anchor's next should point to the anchor itself
 *and anchor's prev should also point to the anchor itself.
 */
	if(List)
	{
		List->num_members = 0;
		List->anchor.obj = NULL;
		List->anchor.next = List->anchor.prev = &(List->anchor);	
		return 1;//Init success;
	}
	else
		return 0;
}
