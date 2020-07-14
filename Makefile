CC=cc
CFLAGS=-std=c99
CFLAGS+=-W -Wall
#CFLAGS+=-O3
CFLAGS+=-O0 -g -ggdb
CFLAGS+=-MMD  # generate dependency .d files
LDLIBS=
LDFLAGS=

SRCS=src/Analyzer_test.c src/Analyzer.c
TARGETS=src/Analyzer_test Analyzer.a
OUT_DIR=./output

# Analyzer.o: src/Analyzer.c
Analyzer.a: src/Analyzer.o
src/Analyzer_test: src/Analyzer_test.o Analyzer.a

TEST_SUITE=src/Analyzer_test

.DEFAULT_GOAL=all
.PHONY: all
all: $(TARGETS)

SRC_DIR ?= $(patsubst %/,%, $(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
CFLAGS+=-I$(SRC_DIR)/include

.PHONY: clean
clean:
	$(RM) $(TARGETS)
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) cscope.out cscope.out.in cscope.out.po
	$(RM) tags TAGS
ifeq ($(COVERAGE), 1)
	$(RM) -r coverage
	$(RM) coverage.info
	$(RM) $(OBJS:.o=.gcno)
	$(RM) $(OBJS:.o=.gcda)
endif
ifeq ($(PROFILE), 1)
	$(RM) gmon.out
	$(RM) gprof.out
endif
ifneq ($(SRC_DIR), $(CURDIR))
	-@rmdir $(OBJDIRS)
endif

.PHONY: test
test: $(TEST_SUITE)
	$(CURDIR)/$(TEST_SUITE)

#SRCS=$(notdir $(wildcard $(SRC_DIR)src/*.c))
OBJS=$(SRCS:.c=.o)
DEPS=$(OBJS:.o=.d)

# Object file subdirectories
ifneq ($(SRC_DIR), $(CURDIR))
vpath %.c $(SRC_DIR)

OBJDIRS=$(filter-out $(CURDIR)/, $(sort $(dir $(abspath $(OBJS)))))
$(OBJDIRS): ; @mkdir $@
$(DEPS): | $(OBJDIRS)
$(OBJS): | $(OBJDIRS)
endif

-include $(DEPS)

# implicit rules for building archives not parallel safe (e.g. make -j 3)
%.a: ; ar rcs $@ $^

# cscope.out
cscope.out: $(SRCS)
	cscope -f $@ -I$(SRC_DIR)/include -bq $^

# ctags
tags: $(SRCS)
	ctags -f $@ -R $(SRC_DIR)/include $^

# etags
TAGS: $(SRCS)
	etags -f $@ -R $(SRC_DIR)/include $^

# Profile (gprof)
ifeq ($(PROFILE), 1)
CFLAGS+=-pg
LDFLAGS+=-pg

gmon.out: test

gprof.out: gmon.out
	gprof $(CURDIR)/$(TEST_SUITE) > $@

.PHONY: profile
profile: gprof.out
endif

# Coverage (gcov, lcov)
ifeq ($(COVERAGE), 1)
CFLAGS+=-ftest-coverage -fprofile-arcs
LDFLAGS+=-coverage
LDLIBS+=-lgcov

$(OBJS:.o=.gcda): test

coverage.info: $(OBJS:.o=.gcda)
	lcov --capture --base-directory $(SRC_DIR) --directory $(CURDIR) --output-file $@

coverage/index.html: coverage.info
	genhtml -o coverage $<

.PHONY: coverage
coverage: coverage/index.html
endif
