#ifndef __TOKEN_H__
#define __TOKEN_H__
#include <glib.h>

/** Tokens are represented as guint32 integers (GQuarks).  A GQuark is
 *  a non-zero integer which uniquely identifies a particular
 *  string. A GQuark value of zero is associated to NULL.  The data
 *  types like GQuark, GString etc. are defined in glib2.
 */

typedef GQuark Token;
#define NULLTOKEN 0
#define token_from_string g_quark_from_string
#define token_try_string g_quark_try_string
#define token_to_string g_quark_to_string

#endif
