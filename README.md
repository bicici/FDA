FDA
===

Feature Decay Algorithms

Citation:
Ergun Bicici and Deniz Yuret, “Optimizing Instance Selection for Statistical Machine Translation 
with Feature Decay Algorithms”, IEEE/ACM Transactions On Audio, Speech, and Language Processing (TASLP), 2014.



Usage: fda [opts] train1 test1 [train2] [test2]\n"
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
  
