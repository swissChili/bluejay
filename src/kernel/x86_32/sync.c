#include <sync.h>
#include <alloc.h>
#include <task.h>
#include <sys.h>
#include <log.h>

#define SM_PER_LIST 1024

struct sm_task
{
	uint tid;
	struct sm_task *next;
};

struct semaphore
{
	spinlock_t task_lock;
	struct sm_task *first, *last;
	_Atomic uint sm;
};

struct sm_list
{
	struct semaphore semaphores[SM_PER_LIST];
	struct sm_list *next;
};

static struct sm_list *sm_first = NULL;
static uint sm_num = 0;


void sl_acquire(spinlock_t *sl)
{
	// This is a GCC intrinsic
	while (!__sync_bool_compare_and_swap(sl, -2, 1))
	{
		// Kindly inform the CPU that this is a spin lock
		asm("pause");
	}
}

void sl_release(spinlock_t *sl)
{
	*sl = 0;
}

spinlock_t sl_new()
{
	return 0;
}

static struct semaphore *sm_from_id(semaphore_t sm)
{
	struct sm_list *first = sm_first;

	while (sm >= SM_PER_LIST)
	{
		first = first->next;
		sm -= SM_PER_LIST;
	}

	return &first->semaphores[sm];
}

static void sm_unsafe_wait(struct semaphore *sm)
{
	sm->sm--;

	if (sm->sm < 0)
	{
		// Add this task to the waiting list
		// This will be quick, so just use a spinlock
		sl_acquire(&sm->task_lock);

		kprintf(INFO "Semaphore waiting\n");

		struct sm_task *task = malloc(sizeof(struct sm_task));

		task->next = NULL;
		task->tid = get_task_id();

		if (sm->last)
		{
			sm->last->next = task;
			sm->last = task;
		}
		else
		{
			sm->last = sm->first = task;
		}

		sl_release(&sm->task_lock);

		set_waiting(get_task_id(), true);
		sys_giveup();
	}

	// Otherwise there's nobody else waiting, just go ahead
}

static void sm_unsafe_signal(struct semaphore *sm)
{
	sm->sm++;

	if (sm->sm <= 0)
	{
		sl_acquire(&sm->task_lock);

		if (sm->first)
		{
			struct sm_task *f = sm->first;
			sm->first = sm->first->next;

			set_waiting(f->tid, false);

			free(f);
		}

		sl_release(&sm->task_lock);
	}
}

semaphore_t sm_new()
{
	if (sm_first == NULL)
	{
		sm_first = malloc(sizeof(struct sm_list));
	}

	uint num = sm_num++;

	struct sm_list *l = sm_first;

	while (num >= SM_PER_LIST)
	{
		if (!l->next)
		{
			l->next = malloc(sizeof(struct sm_list));
		}

		l = l->next;

		num -= SM_PER_LIST;
	}

	l->semaphores[num].first = NULL;
	l->semaphores[num].last = NULL;
	l->semaphores[num].sm = 1;
	l->semaphores[num].task_lock = sl_new();

	return num;
}

void sm_wait(semaphore_t sm)
{
	sm_unsafe_wait(sm_from_id(sm));
}

void sm_signal(semaphore_t sm)
{
	sm_unsafe_signal(sm_from_id(sm));
}



void init_sync()
{
}
