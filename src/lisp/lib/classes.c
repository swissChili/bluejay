#include "classes.h"
#include "../plat/plat.h"
#include "std.h"

value_t *class_member_ref(value_t class, int index)
{
	if (!classp(class))
		return NULL;

	struct class *c = (struct class *)(class ^ CLASS_TAG);

	if (index >= c->num_members)
		return NULL;

	return &c->members[index];
}

value_t l_class_member(value_t class, value_t index)
{
	if (!integerp(index))
		return nil;

	value_t *member = class_member_ref(class, valint(index));

	if (member)
		return *member;
	else
		return nil;
}

value_t l_set_class_member(value_t class, value_t index, value_t value)
{
	if (!integerp(index))
		return nil;

	value_t *member = class_member_ref(class, valint(index));

	if (member)
		*member = value;

	return nil;
}

// type = symbol representing this instances type
// members = list of members
value_t l_make_class(value_t type, value_t members)
{
	if (!integerp(members) || !symbolp(type))
		return nil;

	int nmemb = valint(members);
	struct class_alloc *item = malloc_aligned(sizeof(struct class_alloc) +
											  sizeof(value_t) * nmemb);
	struct class *c = &item->class;

	c->type = type;
	c->num_members = nmemb;
	c->cdata = NULL;

	for (int i = 0; i < nmemb; i++)
	{
		c->members[i] = nil;
	}

	add_this_alloc(&item->alloc, CLASS_TAG);

	return (value_t)c | CLASS_TAG;
}

void load_classes(struct environment *env)
{
	add_c_function(env, "set-class-member", l_set_class_member, 3);
	add_c_function(env, "class-member", l_class_member, 2);
	add_c_function(env, "make-class", l_make_class, 2);
}
