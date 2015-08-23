#ifndef TLSDLL_GLOBAL_H
#define TLSDLL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TLSDLL_LIBRARY)
#  define TLSDLLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TLSDLLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TLSDLL_GLOBAL_H
