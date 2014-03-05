#include "fda.h"
const char *usage = "Usage: fda [opts] train1 test1 [train2] [test2]\n"
  "train1: first (mandatory) arg gives source language train file\n"
  "test1 : second (mandatory) arg gives source language test file\n"
  "train2: third (optional) arg gives target language train file\n"
  "        this is used to output target sentences as a second column\n"
  "test2 : fourth (optional) arg gives target language test file\n"
  "        this is used to calculate metrics like bigram coverage\n"
  "If any of these arguments are \"-\", the data is read from stdin.\n"
  "Gzip compressed files are automatically recognized and handled.\n"
  "Other options (and defaults) are:\n"
  "-v (1): verbosity level, -v0 no messages, -v2 more detail\n"
  "-n (3): maximum ngram order for features\n"
  "-t (0): number of training words output, -t0 means no limit\n"
  "-o (null): output file, stdout is used if not specified\n"
  "The rest of the options are used to calculate feature and sentence scores:\n"
  "-i (1.0): initial feature score idf exponent\n"
  "-l (1.0): initial feature score ngram length exponent\n"
  "-d (0.5): final feature score decay factor\n"
  "-c (0.0): final feature score decay exponent\n"
  "-s (1.0): sentence score length exponent\n"
  "Formulas:\n"
  "initial feature score: fscore0 = idf^i * ngram^l\n"
  "final feature score  : fscore1 = fscore0 * d^cnt * cnt^(-c)\n"
  "sentence score       : sscore  = sum_fscore1 * slen^(-s)\n"
;

/* Default options */
guint verbosity_level = 1;
guint ngram_order = 3;
guint max_output_words = 0;
double idf_exponent = 1.0;
double ngram_length_exponent = 1.0;
double decay_factor = 0.5;
double decay_exponent = 0.0;
double sentence_length_exponent = 1.0;
char *train_file1 = NULL;
char *test_file1 = NULL;
char *train_file2 = NULL;
char *test_file2 = NULL;
char *output_file = NULL;

int main(int argc, char **argv) {
  g_message_init();
  int opt;
  while ((opt = getopt(argc, argv, "v:t:n:s:i:l:d:c:o:")) != -1) {
    switch (opt) {
    case 'v': verbosity_level = atoi(optarg); break;
    case 't': max_output_words = atoi(optarg); break;
    case 'n': ngram_order = atoi(optarg); break;
    case 's': sentence_length_exponent = atof(optarg); break;
    case 'i': idf_exponent = atof(optarg); break;
    case 'l': ngram_length_exponent = atof(optarg); break;
    case 'd': decay_factor = atof(optarg); break;
    case 'c': decay_exponent = atof(optarg); break;
    case 'o': output_file = optarg; break;
    default: g_error("ERROR: Bad option -%c\n%s", opt, usage); break;
    }
  }

  GPtrArray *test1 = NULL;
  GPtrArray *train1 = NULL;
  GPtrArray *test2 = NULL;
  GPtrArray *train2 = NULL;

  if (decay_factor > 1 || decay_factor <= 0) {
    printf("%f is not in the range (0,1]",decay_factor);
    exit(0);
  }
  if (decay_exponent < 0) {
    printf("%f is not in the range [0,)",decay_exponent);
    exit(0);
  }

  // optind is the first nonoption arg
  // Arguments order: First the options, then the nonoptions. 
  
  if (optind < argc) {
    train_file1 = argv[optind++];
    msg2("Reading train1 from %s...", train_file1);
    train1 = read_sentences(train_file1);
  } else {
    g_error("%s", usage);
  }

  if (optind < argc) {
    test_file1 = argv[optind++];
    msg2("Reading test1 from %s...", test_file1);
    test1 = read_sentences(test_file1);
  } else {
    g_error("%s", usage);
  }

  if (optind < argc) {
    train_file2 = argv[optind++];
    msg2("Reading train2 from %s...", train_file2);
    train2 = read_sentences(train_file2);
    g_assert(train2->len == train1->len);
  }

  if (optind < argc) {
    test_file2 = argv[optind++];
    msg2("Reading test2 from %s...", test_file2);
    test2 = read_sentences(test_file2);
    g_assert(test2->len == test1->len);
  }
  g_assert(optind == argc);

  GHashTable *features1 = NULL;
  GHashTable *features2 = NULL;
  guint bigram_cnt1 = 0;
  guint bigram_cnt2 = 0;
  msg2("init_features1");
  features1 = init_features(test1, &bigram_cnt1);
  if (test2 != NULL) {
    msg2("init_features2");
    features2 = init_features(test2, &bigram_cnt2);
  }
  msg2("init_train_count");
  guint train_size1 = (idf_exponent == 0) ? 0 : init_train_count(features1, train1);
  msg2("init_feature_scores");
  g_hash_table_foreach(features1, init_feature_scores, &train_size1);
  msg2("init_sentence_heap");
  std::vector<Hpair> heap = init_sentence_heap(features1, train1);
  
  guint nword1 = 0;
  guint nword2 = 0;
  guint bigram_match1 = 0;
  guint bigram_match2 = 0;
  guint numsents = 0;
  
  msg2("Writing...");
  FILE *out = (output_file ? fopen(output_file, "w") : stdout);
  while (1) {
    if (heap.size() == 0) break;
    gfloat best_score = 0;
    guint best_sentence = next_best_training_instance(&heap, train1, features1, &best_score);
    Sentence s1 = NULL;
    Sentence s2 = NULL;
    s1 = (Sentence) g_ptr_array_index(train1, best_sentence);
    nword1 += sentence_size(s1);
    bigram_match1 += update_counts(features1, s1);
    fprint_sentence(s1, out);
    if (train2 != NULL) {
      s2 = (Sentence) g_ptr_array_index(train2, best_sentence);
      nword2 += sentence_size(s2);
      bigram_match2 += update_counts(features2, s2);
      fputc('\t', out); fprint_sentence(s2, out);
    }
    fprintf(out, "\t%g\t%d\t%d\t%d", best_score, nword1, bigram_cnt1, bigram_match1);
    if (train2 != NULL) fprintf(out, "\t%d\t%d\t%d", nword2, bigram_cnt2, bigram_match2);
    fputc('\n', out);
    numsents += 1;
    if (max_output_words > 0 && nword1 >= max_output_words) break;
  }
  minialloc_free_all();
  
  msg1("-v%d -n%d -t%d -o%s -i%g -l%g -d%g -c%g -s%g %s %s %s %s\t%d\t%d\t%d\t%d\t%d\t%d\t%d",
       verbosity_level, ngram_order, max_output_words, (output_file ? output_file : "-"),
       idf_exponent, ngram_length_exponent, decay_factor, decay_exponent, sentence_length_exponent, 
       train_file1, test_file1, (train_file2 ? train_file2 : ""), (test_file2 ? test_file2 : ""), 
       numsents, nword1, bigram_cnt1, bigram_match1, nword2, bigram_cnt2, bigram_match2);
}

static GHashTable *init_features(GPtrArray *sent, guint *bigram_cnt) {
  GHashTable *feats = g_hash_table_new(ngram_hash, ngram_equal);
  *bigram_cnt = 0;
  for (int si = 0; si < (int) sent->len; si++) {
    Sentence s = (Sentence) g_ptr_array_index(sent, si);
    foreach_ngram(ng, s) {
      if (g_hash_table_lookup(feats, ng) == NULL) {
	feat_t f = (feat_t) minialloc(sizeof(struct feat_s));
	f->output_cnt = 0; f->train_cnt = 0; f->logscore0 = 0; f->logscore1 = 0;
	g_hash_table_insert(feats, ngram_dup(ng), f);
	if (ngram_order > 1) {
	  if (ngram_size(ng) == 2) (*bigram_cnt)++;
	}
	else {
	  if (ngram_size(ng) == 1) (*bigram_cnt)++;
	}
      }
    }
  }
  return feats;
}

static guint init_train_count(GHashTable *feats, GPtrArray *sent) {
  guint nwords = 0;
  for (int si = 0; si < (int) sent->len; si++) {
    Sentence s = (Sentence) g_ptr_array_index(sent, si);
    nwords += sentence_size(s);
    foreach_ngram(ng, s) {
      feat_t f = (feat_t) g_hash_table_lookup(feats, ng);
      if (f != NULL) f->train_cnt++;
    }
  }
  return nwords;
}

static void init_feature_scores(gpointer key, gpointer val, gpointer dat) {
  Ngram ng = (Ngram) key;
  feat_t f = (feat_t) val;
  f->logscore0 = 0;
  if (ngram_length_exponent != 0) {
    f->logscore0 += log(ngram_size(ng)) * ngram_length_exponent;
  }
  if (idf_exponent != 0) {
    guint *train_size1 = (guint *) dat;
    guint fcnt = f->train_cnt;
    if (fcnt == 0) fcnt = 1;
    double idf = -log((double) fcnt / (double) (*train_size1));
    f->logscore0 += log(idf) * idf_exponent;
  }
  f->logscore1 = f->logscore0;
}

static std::vector<Hpair> init_sentence_heap(GHashTable *feat, GPtrArray *sent) {
  std::vector<Hpair> heap;
  for (guint si = 0; si < ((int) sent->len); si++) {
    Sentence s = (Sentence) g_ptr_array_index(sent, si);
    gfloat logscore = sentence_logscore(s, feat);
    Hpair hp;
    hp.key = si;
    hp.val = logscore;
    if (isfinite(logscore)) heap.push_back(hp);
  }
  std::make_heap(heap.begin(), heap.end(), cmp);
  return heap;
}

static gfloat sentence_logscore(Sentence s, GHashTable *feat) {
  double sscore = -INFINITY;
  foreach_ngram(ng, s) {
    feat_t f = (feat_t) g_hash_table_lookup(feat, ng);
    if (f != NULL) {
      double fscore = f->logscore1;
      if (!isfinite(sscore)) {
	sscore = fscore;
      } else if (fscore <= sscore) {
	sscore = sscore + log(1 + exp(fscore - sscore));
      } else {
	sscore = fscore + log(1 + exp(sscore - fscore));
      }
    }
  }
  if ((sentence_length_exponent != 0) && (sentence_size(s) > 0)) {
    sscore -= log(sentence_size(s)) * sentence_length_exponent;
  }
  return ((gfloat) sscore);
}

static guint next_best_training_instance(std::vector<Hpair> *heap, GPtrArray *sent, GHashTable *feat, gfloat *score_ptr) {
  g_assert(heap->size() > 0);
  guint best_sentence = 0;
  gfloat best_score = 0;
  while (1) {
    std::pop_heap(heap->begin(), heap->end(), cmp);
    Hpair hp = heap->back();
    best_sentence = hp.key;
    best_score = sentence_logscore((Sentence) g_ptr_array_index(sent, best_sentence), feat);
    g_assert(isfinite(best_score));
    heap->pop_back();
    if (heap->size() == 0) break;
    if (best_score - heap->front().val >= -G_MINFLOAT) break;
    hp.key = best_sentence;
    hp.val = best_score;
    heap->push_back(hp);
    std::push_heap(heap->begin(), heap->end(), cmp);
  }
  *score_ptr = best_score;
  return best_sentence;
}

static guint update_counts(GHashTable *feat, Sentence s) {
  guint bigram_match = 0;
  foreach_ngram(ng, s) {
    feat_t f = (feat_t) g_hash_table_lookup(feat, ng);
    if (f != NULL) {
      if ((ngram_order > 1) && (ngram_size(ng) == 2) && (f->output_cnt == 0)) 
	bigram_match++;
      else if ((ngram_order == 1) && (ngram_size(ng) == 1) && (f->output_cnt == 0)) 
	bigram_match++;
      f->output_cnt++;
      f->logscore1 = f->logscore0 + f->output_cnt * log(decay_factor) - decay_exponent * log(1.0 + f->output_cnt);
    }
  }
  return bigram_match;
}

bool cmp(Hpair hp1, Hpair hp2) {
  return (hp1.val < hp2.val);
}

