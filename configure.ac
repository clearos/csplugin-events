#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([csplugin-events], [1.2], [http://www.clearfoundation.com/docs/developer/bug_tracker/])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([csplugin-events.cpp])
AC_CONFIG_SUBDIRS([inih])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([sqlite3], [sqlite3_open], [], [
        AC_MSG_ERROR([libsqlite3 not found but is required.])])

# Checks for header files.
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([clearsync/csplugin.h], [], [AC_MSG_ERROR([clearsync header files not found.])])
AC_LANG_POP([C++])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

# Output files
AC_CONFIG_FILES([Makefile csplugin-events.spec])
AC_OUTPUT
