EXTRA_DIST = run_tests.sh

AM_CFLAGS = -std=c99

noinst_PROGRAMS = ut_rule ut_add_fact ut_rt_assert ut_ctx_prio_insert \
	ut_start_hash ut_dlist utAddFact

utAddFact_SOURCES = utAddFact.cc

LDADD = ../maren/libMAReN.la

unittest:
	@./run_tests.sh $(bin_PROGRAMS)
