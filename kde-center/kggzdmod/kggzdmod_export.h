#ifndef KGGZDMOD_EXPORT_H
#define KGGZDMOD_EXPORT_H

#include <kdemacros.h>

#ifndef KGGZDMOD_EXPORT
# if defined(MAKE_KGGZDMOD_LIB)
   /* We are building this library */ 
#  define KGGZDMOD_EXPORT KDE_EXPORT
# else
   /* We are using this library */ 
#  define KGGZDMOD_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KGGZDMOD_EXPORT_DEPRECATED
#  define KGGZDMOD_EXPORT_DEPRECATED KDE_DEPRECATED KGGZDMOD_EXPORT
# endif

#endif
