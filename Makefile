CC = gcc
CFLAGS = -std=gnu99 -Wall -g -pthread
OBJS = list.o threadpool.o main.o mergesort.o

.PHONY: all clean

GIT_HOOKS := .git/hooks/pre-commit

ifeq ($(strip $(CHECK)),1)
CFLAGS += -DCHECK
endif

all: $(GIT_HOOKS) sort

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF .$@.d -c $<

sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic

check: all
	sort -R dictionary/words.txt | ./sort 4 $(shell wc -l dictionary/words.txt)
	diff dictionary/words.txt output && echo "OK" || echo "Failed!"

clean:
	rm -f $(OBJS) sort output
	@rm -rf $(deps)

-include $(deps)
