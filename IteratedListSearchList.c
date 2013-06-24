
#include "IteratedList.h"      // for linked list
#include <stdio.h>
static char *file_name = "IteratedList";
void *searchList(List_PNTR l, int (*f)(void *element, void *key), void *key){
  void *first, *current;
  first = current = getNextElement(l);
  if(current != NULL){
    do{
      if( f(current, key) == 0)
	return(current);
      current = getNextElement(l);
    } while (current != first);
  }
  return(NULL);  
}
