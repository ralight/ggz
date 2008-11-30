#ifndef KGGZLIB_EXPORT_H
#define KGGZLIB_EXPORT_H

#include <kdemacros.h>

#ifndef KGGZLIB_EXPORT
# if defined(MAKE_KGGZLIB_LIB)
   /* We are building this library */
#  define KGGZLIB_EXPORT KDE_EXPORT
# else
   /* We are using this library */
#  define KGGZLIB_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KGGZLIB_EXPORT_DEPRECATED
#  define KGGZLIB_EXPORT_DEPRECATED KDE_DEPRECATED KGGZLIB_EXPORT
# endif

#endif
