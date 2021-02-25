#include "kheap.h"
#include "log.h"

static void swap_entries(struct heap_entry *a, struct heap_entry *b)
{
	// Static to not waste stack space, NOT thread safe
	static struct heap_entry tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

int heap_parent(int i)
{
	return (i - 1) / 2;
}

int heap_left(int i)
{
	return 2 * i + 1;
}

int heap_right(int i)
{
	return 2 * i + 2;
}

static void heap_sort(struct min_heap *heap, int i)
{
	int l = heap_left(i), r = heap_right(i);
	int smallest = i;
	int size = heap->size;
	struct heap_entry *e = heap->elements;

	if (l < size && e[l].key < e[smallest].key)
		smallest = l;
	if (r < size && e[r].key < e[smallest].key)
		smallest = r;

	if (smallest != i)
	{
		swap_entries(&e[i], &e[smallest]);
		heap_sort(heap, smallest);
	}
}

static void heap_delete_root(struct min_heap *heap)
{
	if (heap->size <= 0)
		return;
	if (heap->size == 1)
		heap->size--;

	heap->elements[0] = heap->elements[--heap->size];
	heap_sort(heap, 0);
}

void heap_decrease(struct min_heap *heap, int i, struct heap_entry to)
{
	heap->elements[i] = to;
	int k = to.key;

	while (i && heap->elements[heap_parent(i)].key > k)
	{
		swap_entries(&heap->elements[i], &heap->elements[heap_parent(i)]);
		i = heap_parent(i);
	}
}

void heap_delete(struct min_heap *heap, int i)
{
	heap_decrease(heap, i,
				  (struct heap_entry){
					  .key = -1,
					  .address = 0,
				  });
	heap_delete_root(heap);
}

void heap_insert(struct min_heap *heap, struct heap_entry e)
{
	kassert(heap->size < HEAP_SIZE, "Heap overflow!");

	int i = heap->size++;
	uint k = e.key;
	heap->elements[i] = e;

	// While the parent element is GREATER than the child
	while (i && heap->elements[heap_parent(i)].key > k)
	{
		swap_entries(&heap->elements[i], &heap->elements[heap_parent(i)]);
		i = heap_parent(i);
	}
}

struct heap_entry heap_lookup_min(struct min_heap *heap, int min, bool *ok,
								  bool delete, int *i)
{
	int j = 0;
	struct heap_entry *e = heap->elements;

	while (j < heap->size && e[j].key < min)
	{
		int left = heap_left(j), right = heap_right(j);

		// If left is invalid, use right, if right is invalid, use left.
		// If both are valid, chose the smallest one.
		// In the first case, right may too be invalid, but it will be
		// sorted out in the while loop (see else clause at end of function).

		if (left >= heap->size)
			j = right;
		else if (right >= heap->size)
			j = left;
		else
			j = e[left].key < e[right].key ? left : right;
	}

	if (j < heap->size)
	{
		struct heap_entry val = e[j];

		*ok = true;

		if (i)
			*i = j;

		if (delete)
			heap_delete(heap, j);

		return val;
	}
	else
	{
		*ok = false;
		return (struct heap_entry){0};
	}
}
