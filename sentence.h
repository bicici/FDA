#ifndef __SENTENCE_H__
#define __SENTENCE_H__
#include <glib.h>
#include "token.h"

/** Sentences are represented as Token arrays.  The zeroth element is
 * token count.
 */
typedef Token *Sentence;

#define sentence_size(s) ((guint32)(*s))
Sentence read_sentence(Sentence st, char *str, guint nmax);
Sentence copy_sentence(Sentence s);
void print_sentence(Sentence s);
void fprint_sentence(Sentence s, FILE *out);
GPtrArray *read_sentences(char *file);

#endif
