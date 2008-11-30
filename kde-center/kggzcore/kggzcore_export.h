#ifndef KGGZCORE_EXPORT_H
#define KGGZCORE_EXPORT_H

#include <kdemacros.h>

#ifndef KGGZCORE_EXPORT
# if defined(MAKE_KGGZCORE_LIB)
   /* We are building this library */
#  define KGGZCORE_EXPORT KDE_EXPORT
# else
   /* We are using this library */
#  define KGGZCORE_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KGGZCORE_EXPORT_DEPRECATED
#  define KGGZCORE_EXPORT_DEPRECATED KDE_DEPRECATED KGGZCORE_EXPORT
# endif

#endif
