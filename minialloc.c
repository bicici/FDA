#include <glib.h>
#include "minialloc.h"

#define BLOCKSIZE (1<<24)
static gpointer data;
static gpointer free;
static gsize remaining;

static void newdata(gpointer olddata) {
    data = g_malloc(BLOCKSIZE);
    *((gpointer *)data) = olddata;
    free = data + sizeof(gpointer);
    remaining = BLOCKSIZE - sizeof(gpointer);
}

gpointer minialloc(gsize size) {
  g_assert(size <= BLOCKSIZE);
  if ((data == NULL) || (size > remaining)) {
    newdata(data);
  }
  gpointer rval = free;
  free += size;
  remaining -= size;
  return rval;
}

void minialloc_free_all() {
  int n = 0;
  while (data != NULL) {
    gpointer p = *((gpointer*)data);
    g_free(data);
    data = p;
    n++;
  }
  // g_message("Freed %d blocks of %d bytes", n, BLOCKSIZE);
}
