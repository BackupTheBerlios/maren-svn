BUILT_SOURCES = parse_mpll.h
AM_YFLAGS = -d -t -v
AM_LFLAGS = -d -Pmpll

AM_CFLAGS = -std=c99 -D_\(txt\)=txt -DYY_NO_UNPUT

LDADD = ../maren/libMAReN.la

lib_LTLIBRARIES = libmpll.la

libmpll_la_SOURCES = \
		parse_mpll.h parse_mpll.y scan_mpll.l \
		pi_loader.c pi_loader.h \
		plugin.c plugin.h \
		pi_interface.h

noinst_PROGRAMS = parsetest

parsetest_SOURCES = \
		parsetest.c \
		parse_mpll.h parse_mpll.y scan_mpll.l \
		pi_loader.c pi_loader.h \
		plugin.c plugin.h \
		pi_interface.h

parsetest_LDFLAGS = -ldl