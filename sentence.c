#include <stdio.h>
#include <glib.h>
#include <zlib.h>
#include "foreach.h"
#include "minialloc.h"
#include "sentence.h"
#define SMAX (1<<16) /* maximum words in a sentence */

Sentence read_sentence(Sentence st, char *str, guint nmax) {
  guint32 ntok = 0;
  foreach_token(word, str) {
    Token wtok = token_from_string(word);
    g_assert(ntok < nmax);
    st[++ntok] = wtok;
  }
  st[0] = ntok;
  return st;
}

Sentence copy_sentence(Sentence s) {
  int n = sentence_size(s);
  Sentence s2 = (Sentence) minialloc((n+1)*sizeof(Token));
  for (int i = 0; i <= n; i++) {
    s2[i] = s[i];
  }
  return s2;
}

void print_sentence(Sentence s) {
  for (int i = 1; i <= sentence_size(s); i++) {
    if (i > 1) putchar(' ');
    fputs(token_to_string(s[i]), stdout);
  }
}

void fprint_sentence(Sentence s, FILE *out) {
  for (int i = 1; i <= sentence_size(s); i++) {
    if (i > 1) fputc(' ', out);
    fputs(token_to_string(s[i]), out);
  }
}

GPtrArray *read_sentences(char *file) {
  GPtrArray *sents = g_ptr_array_new();
  Token s[SMAX];
  if (!strcmp(file, "-")) file = NULL;
  foreach_line(str, file) {
    g_ptr_array_add(sents, copy_sentence(read_sentence(s, str, SMAX-1)));
  }
  return sents;
}
