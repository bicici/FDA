#include "foreach.h"
#include "minialloc.h"
#include "ngram.h"
#define MAX_NGRAM_ORDER 1023

Ngram ngram_from_string(char *str) {
  Token ng[1 + MAX_NGRAM_ORDER];
  int ntok = 0;
  foreach_token(word, str) {
    g_assert(ntok < MAX_NGRAM_ORDER);
    ng[++ntok] = token_from_string(word);
  }
  ng[0] = ntok;
  return ngram_dup(ng);
}

Ngram ngram_dup(Ngram ng) {
  gsize size = (sizeof(Token) * (ngram_size(ng)+1));
  Ngram ngcopy = (Ngram) minialloc(size);
  memcpy(ngcopy, ng, size);
  return ngcopy;
}

Ngram ngram_cpy(Ngram ngcopy, Ngram ng) {
  gsize size = (sizeof(Token) * (ngram_size(ng)+1));
  memcpy(ngcopy, ng, size);
  return ngcopy;
}

/** Ngram hash functions */

static guint ngram_hash_rnd[1 + MAX_NGRAM_ORDER];

static void init_ngram_hash_rnd() {
  guint32 r;
  if (*ngram_hash_rnd == 0) {
    for (int i = MAX_NGRAM_ORDER; i >= 0; i--) {
      /* g_random_int(): Return a random guint32 equally distributed
	 over the range [0..2^32-1]. */
      do { 
	r = g_random_int(); 
      } while (r == 0);
      ngram_hash_rnd[i] = r;
    }
  }
}

guint ngram_hash(gconstpointer p) {
  if (*ngram_hash_rnd == 0) init_ngram_hash_rnd();
  const Token *ng = (Token *) p;
  guint hash = 0;
  for (int i = ngram_size(ng); i > 0; i--) {
    hash += ng[i] * ngram_hash_rnd[i];
  }
  return hash;
}

gboolean ngram_equal(gconstpointer pa, gconstpointer pb) {
  const Token *a = (Token *) pa;
  const Token *b = (Token *) pb;
  if (ngram_size(a) != ngram_size(b)) return 0;
  for (int i = ngram_size(a); i > 0; i--) {
    if (a[i] != b[i]) return 0;
  }
  return 1;
}
