CC = gcc
CFLAGS = -std=gnu99 -Wall -g -pthread
OBJS = list.o threadpool.o main.o mergesort.o

.PHONY: all clean

GIT_HOOKS := .git/hooks/pre-commit

ifeq ($(strip $(CHECK)),1)
CFLAGS += -DCHECK
endif

all: $(GIT_HOOKS) sort random_gen

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF .$@.d -c $<

sort: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic

random_gen: random_gen.o
	$(CC) $(CFLAGS) -o random_gen random_gen.c

check: random_gen
	for i in `seq 1 5 500`; do\
		./random_gen $$i;\
		./sort 4 $$i;\
		sort -n random > sorted;\
		diff output sorted;\
	done
	@echo "OK"

clean:
	rm -f $(OBJS) sort random_gen random output sorted
	@rm -rf $(deps)

-include $(deps)
