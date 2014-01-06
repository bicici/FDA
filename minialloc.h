#ifndef __MINIALLOC_H__
#define __MINIALLOC_H__
#include <glib.h>

/* Simple memory allocator efficient for small chunks.  Individual
   items cannot be freed, the whole memory chain can be freed at the
   end. */ 

gpointer minialloc(gsize size);
void minialloc_free_all();

#endif
