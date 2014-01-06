#ifndef __NGRAM_H__
#define __NGRAM_H__
#include <glib.h>
#include "token.h"

/** Ngrams are represented as Token arrays with zeroth element
 * representing count.  The actual tokens are in positions 1..n.
 */

typedef Token *Ngram;

#define ngram_size(p) ((p)[0])

guint ngram_hash(gconstpointer p);
gboolean ngram_equal(gconstpointer pa, gconstpointer pb);
Ngram ngram_from_string(char *str);
Ngram ngram_dup(Ngram ng);
Ngram ngram_cpy(Ngram ngcopy, Ngram ng);
#endif
