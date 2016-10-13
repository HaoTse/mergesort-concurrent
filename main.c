#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "threadpool.h"
#include "list.h"
#include "mergesort.h"

#define USAGE "usage: ./sort [thread_count] [input_count]\n"

struct {
    pthread_mutex_t mutex;
} data_context;

static llist_t *tmp_list;
static llist_t *the_list = NULL;

static int thread_count = 0, data_count = 0;
static tpool_t *pool = NULL;

void merge_thread_list(void *data)
{
    llist_t *_list = (llist_t *) data;
    if (_list->size < (uint32_t) data_count) {
        pthread_mutex_lock(&(data_context.mutex));
        llist_t *_t = tmp_list;
        if (!_t) {
            tmp_list = _list;
            pthread_mutex_unlock(&(data_context.mutex));
        } else {
            tmp_list = NULL;
            pthread_mutex_unlock(&(data_context.mutex));
            task_t *_task = (task_t *) malloc(sizeof(task_t));
            _task->func = merge_thread_list;
            _task->arg = merge_list(_list, _t);
            tqueue_push(pool->queue, _task);
        }
    } else {
        the_list = _list;
        task_t *_task = (task_t *) malloc(sizeof(task_t));
        _task->func = NULL;
        tqueue_push(pool->queue, _task);
        list_print(_list);
    }
}

void merge_sort(void *data)
{
    llist_t *list = (llist_t *) data;
    pthread_mutex_lock(&(data_context.mutex));
    if (list->size > 1) {
        pthread_mutex_unlock(&(data_context.mutex));

        /* cut list */
        int mid = list->size / 2;
        llist_t *_list = list_new();
        _list->head = list_nth(list, mid);
        _list->size = list->size - mid;
        list_nth(list, mid - 1)->next = NULL;
        list->size = mid;

        /* create new task: left */
        task_t *_task = (task_t *) malloc(sizeof(task_t));
        _task->func = merge_sort;
        _task->arg = _list;
        tqueue_push(pool->queue, _task);

        /* create new task: right */
        _task = (task_t *) malloc(sizeof(task_t));
        _task->func = merge_sort;
        _task->arg = list;
        tqueue_push(pool->queue, _task);
    } else {
        pthread_mutex_unlock(&(data_context.mutex));
        merge_thread_list(list);
    }
}

static void *task_run(void *data)
{
    (void) data;
    while (1) {
        task_t *_task = tqueue_pop(pool->queue);
        if (_task) {
            if (!_task->func) {
                tqueue_push(pool->queue, _task);
                break;
            } else {
                _task->func(_task->arg);
                free(_task);
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    if (argc < 3) {
        printf(USAGE);
        return -1;
    }

    FILE *fin = freopen("dictionary/words.txt", "r", stdin);

    thread_count = atoi(argv[1]);
    data_count = atoi(argv[2]);

    /* Read data */
    the_list = list_new();

    /* FIXME: remove all all occurrences of printf and scanf
     * in favor of automated test flow.
     */
    for (int i = 0; i < data_count; ++i) {
        val_t data;
        scanf("%s", data);
        list_add(the_list, data);
    }

    fclose(fin);


    /* initialize tasks inside thread pool */
    pthread_mutex_init(&(data_context.mutex), NULL);
    tmp_list = NULL;
    pool = (tpool_t *) malloc(sizeof(tpool_t));
    tpool_init(pool, thread_count, task_run);

    /* launch the first task */
    task_t *_task = (task_t *) malloc(sizeof(task_t));
    _task->func = merge_sort;
    _task->arg = the_list;
    tqueue_push(pool->queue, _task);

    /* release thread pool */
    tpool_free(pool);
    return 0;
}
