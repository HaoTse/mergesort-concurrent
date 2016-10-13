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

bench: all
	rm -f runtime
	for i in 2 4 8 16 32 64 128 256;do\
		sort -R dictionary/words.txt | ./sort $$i $(shell wc -l dictionary/words.txt);\
	done

plot: 
	gnuplot scripts/runtime.gp

clean:
	rm -f $(OBJS) sort output runtime *.png
	@rm -rf $(deps)

-include $(deps)
