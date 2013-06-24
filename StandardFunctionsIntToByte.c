/*
 * Standard functions impl file
 * 
 * Defines functions for use in Insense
 *
 * @author jonl
 *
 */

#include "StandardFunctions.h"
#include "events.h"
#include <stdint.h>


unsigned char intToByte_proc(void *this, void *handler, int i){
  return (uint8_t) i;
}


