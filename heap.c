#include "heap.h"

/* With a one based binary heap parent is at i/2 and children are at
   2i, 2i+1. */

void heap_insert_min(Heap heap, Token key, gfloat value) {
  guint n = ++heap_size(heap);
  while (n > 1) {
    guint p = n >> 1;
    if (heap[p].val <= value) break; /*  */
    heap[n] = heap[p];
    n = p;
  }
  heap[n].key = key;
  heap[n].val = value;
}

void heap_insert_max(Heap heap, Token key, gfloat value) {
  guint n = ++heap_size(heap);
  while (n > 1) {
    guint p = n >> 1;
    if (heap[p].val >= value) break; /*  */
    heap[n] = heap[p];
    n = p;
  }
  heap[n].key = key;
  heap[n].val = value;
}

bool cmp(Hpair hp1, Hpair hp2) {
  return (hp1.val < hp2.val);
}

Hpair heap_delete_min(Heap heap) {
  g_assert(heap_size(heap) > 0);
  Hpair top = heap[1];
  Hpair bot = heap[heap_size(heap)];
  guint end = --heap_size(heap);
  guint p = 1;
  guint n;
  while ((n = p << 1) <= end) {
    if ((n < end) && (heap[n+1].val <= heap[n].val)) n++; /*  */
    if (bot.val <= heap[n].val) break;		    /*  */
    heap[p] = heap[n];
    p = n;
  }
  heap[p] = bot;
  return top;
}

Hpair heap_delete_max(Heap heap) {
  g_assert(heap_size(heap) > 0);
  Hpair top = heap[1];
  Hpair bot = heap[heap_size(heap)];
  guint end = --heap_size(heap);
  guint p = 1;
  guint n;
  while ((n = p << 1) <= end) {
    if ((n < end) && (heap[n+1].val >= heap[n].val)) n++; /*  */
    if (bot.val >= heap[n].val) break;		    /*  */
    heap[p] = heap[n];
    p = n;
  }
  heap[p] = bot;
  return top;
}

void heap_sort_max(Heap heap) {
  guint size = heap_size(heap);
  for (guint i = size; i > 1; i--) {
    heap[i] = heap_delete_min(heap);
  }
  heap_size(heap) = size;
}

void heap_sort_min(Heap heap) {
  guint size = heap_size(heap);
  for (guint i = size; i > 1; i--) {
    heap[i] = heap_delete_max(heap);
  }
  heap_size(heap) = size;
}
