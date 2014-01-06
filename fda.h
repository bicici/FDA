#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <glib.h>
#include <zlib.h>
#include <algorithm> 
#include <vector>
#include "procinfo.h"
#include "foreach.h"
#include "minialloc.h"
#include "token.h"
#include "sentence.h"
#include "ngram.h"

/* Types */
typedef struct feat_s {
  guint train_cnt;
  guint output_cnt;
  double logscore0;
  double logscore1;
} *feat_t;

/* heap is an array of token-logp pairs. */
typedef struct _Hpair {
  guint32 key;
  gfloat val;
} Hpair;

bool cmp(Hpair hp1, Hpair hp2);

/* Functions */

//extern "C" {static GHashTable *init_features(GPtrArray *sent, guint *bgcnt_ptr);}
static GHashTable *init_features(GPtrArray *sent, guint *bgcnt_ptr);
static guint init_train_count(GHashTable *feats, GPtrArray *sent);
static void init_feature_scores(gpointer key, gpointer val, gpointer dat);
static std::vector<Hpair> init_sentence_heap(GHashTable *feat, GPtrArray *sent);
static gfloat sentence_logscore(Sentence s, GHashTable *feat);
static guint next_best_training_instance(std::vector<Hpair> *heap, GPtrArray *sent, GHashTable *feat, gfloat *score_ptr);
static guint update_counts(GHashTable *feat, Sentence s);

/* Macros */

#define msg1 if(verbosity_level>=1)g_message
#define msg2 if(verbosity_level>=2)g_message

/* We often need to iterate through all ngrams (up to ngram_order) of a sentence. */
/* The macro below makes the rest of the code more readable. */
/* Note that the zeroth element of ngram/sentence gives the length. */
#define foreach_ngram(ngram, sentence)\
  for (guint32 *ngram = (sentence), *_last = ngram+((sentence)[0]), _save = ngram[0];\
       ngram <= _last; ngram[0] = _save, ngram++, _save = ngram[0])\
    for (ngram[0] = 1; (ngram[0] <= ngram_order) && (ngram+(ngram[0]) <= _last); ngram[0]++)
