#include <stdlib.h>
#include <pthread.h>
#include <sys/queue.h>
#include "task.h"

#define MAX_TASK_THREADS 64
#define MAX_IDLE_TASK_THREADS 4

TAILQ_HEAD(task_queue, task);

typedef struct task {
  TAILQ_ENTRY(task) t_link;
  task_fn_t *t_fn;
  void *t_opaque;
} task_t;

static struct task_queue tasks = TAILQ_HEAD_INITIALIZER(tasks);
static unsigned int num_task_threads;
static unsigned int num_task_threads_avail;

static pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t task_cond   = PTHREAD_COND_INITIALIZER;


/**
 *
 */
static void *
task_thread(void *aux)
{
  task_t *t;

  pthread_mutex_lock(&task_mutex);
  while(1) {
    t = TAILQ_FIRST(&tasks);
    if(t == NULL) {

      if(num_task_threads_avail == MAX_IDLE_TASK_THREADS)
        break;

      num_task_threads_avail++;
      pthread_cond_wait(&task_cond, &task_mutex);
      num_task_threads_avail--;
      continue;
    }
    TAILQ_REMOVE(&tasks, t, t_link);

    pthread_mutex_unlock(&task_mutex);
    t->t_fn(t->t_opaque);
    free(t);
    pthread_mutex_lock(&task_mutex);
  }

  num_task_threads--;
  pthread_mutex_unlock(&task_mutex);
  return NULL;
}


/**
 *
 */
static void
task_launch_thread(void)
{
  num_task_threads++;

  pthread_t id;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&id, &attr, task_thread, NULL);
  pthread_attr_destroy(&attr);
}


/**
 *
 */
void
task_run(task_fn_t *fn, void *opaque)
{
  task_t *t = calloc(1, sizeof(task_t));
  t->t_fn = fn;
  t->t_opaque = opaque;
  pthread_mutex_lock(&task_mutex);
  TAILQ_INSERT_TAIL(&tasks, t, t_link);

  if(num_task_threads_avail > 0) {
    pthread_cond_signal(&task_cond);
  } else {
    if(num_task_threads < MAX_TASK_THREADS) {
      task_launch_thread();
    }
  }
  pthread_mutex_unlock(&task_mutex);
}
