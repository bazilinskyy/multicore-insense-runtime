
#include "IteratedList.h"      // for linked list
#include <stdio.h>
static char *file_name = "IteratedList";

// Returns whether the list is empty
bool isEmpty(List_PNTR l){
  if(l==NULL)
    return true;
  return(l->first == NULL);
}
