#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(MAReN, 0.1.0, marcus.perlick@gmx.de)
AC_CONFIG_SRCDIR([maren/dlist.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_VPRINTF

# Checks for package features
AC_ARG_ENABLE(build-config,
  AS_HELP_STRING([--enable-build-config],
		 [dev=Pure debugging; ship=Optimized version w/o debugging.]),
  [case "${enableval}" in
	dev) CFLAGS="-ggdb -Wall"; CXXFLAGS="-ggdb -Wall";;
	ship) CFLAGS="-O2 -DNDEBUG"; CXXFLAGS="-O2 -DNDEBUG";;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-build-config);;
   esac])

AC_ARG_ENABLE(check-node-casts,
  AS_HELP_STRING([--enable-check-node-casts],
                 [Check validity of target type when casting node-pointers. \
		  This check is supposed to be obsolete, if this \
		  software-package works correct. Enable it, if you are \
		  working on MAReN itself. As a user, that is concerned \
		  about performance, you should disable it. [[default=no]]]),
  [case "${enableval}" in
	yes) CFLAGS=${CFLAGS}" -DMAREN_CHECK_NODE_CASTS";
             CXXFLAGS=${CXXFLAGS}" -DMAREN_CHECK_NODE_CASTS";;
	no)  ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --enable-check-node-casts);;
   esac],
  [])

AC_ARG_ENABLE(pmf-conversion,
  AS_HELP_STRING([--enable-pmf-conversion],
                 [Avoid a level of function-calls in the C++ interface by \
                  using conversion of bound pointers to member-function to \
                  normal function-pointers. [[default="yes"]]]),
  [case "${enableval}" in
	yes) CXXFLAGS=${CXXFLAGS}" -Wno-pmf-conversions \
             -DMAREN_USE_PMF_CONVERSION";;
	no) ;;
	*) AC_MSG_ERROR(bad value ${enableval} for --use-pmf-conversion);;
   esac],
   [CXXFLAGS=${CXXFLAGS}" -Wno-pmf-conversions -DMAREN_USE_PMF_CONVERSION"])

AC_ARG_ENABLE(custom-as-allocator,
  AS_HELP_STRING([-enable-custom-as-allocator],
		 [Active Sets (as) are those things, which are rapidly \
                  allocated and deallocated during fact-insertion and \
                  fact-removal. So one may use a custom allocator for \
                  active-sets. [[default=no]]]),
  [case "${enableval}" in
   yes) AC_DEFINE([MAREN_ACTIVE_SET_ALLOC],[1],[Type of allocator to use \
                                                for active sets.]);;
   no)  AC_DEFINE([MAREN_ACTIVE_SET_ALLOC],[0],[Type of allocator to use \
                                                for active sets.]);;
   *) AC_MSG_ERROR(bad valuel ${enableval} for --enable-custom-as-allocator) ;;
   esac],
   [AC_DEFINE([MAREN_ACTIVE_SET_ALLOC],[0],[Type of allocator to use \
                                                     for active sets.])])

AC_CONFIG_FILES([./Makefile
                 maren/Makefile
		 mpll/Makefile
		 tst/Makefile
		 examples/Makefile
		 examples/fire-c-basic/Makefile])
AC_OUTPUT
