AM_CFLAGS = -std=c99

bin_PROGRAMS = ut_rule ut_add_fact ut_rt_assert ut_ctx_prio_insert \
	ut_start_hash utAddFact

utAddFact_SOURCES = utAddFact.cc

LDADD = ../maren/libMAReN.la

check:
	run_tests.sh $(bin_PROGRAMS)
