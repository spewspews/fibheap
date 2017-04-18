#include <u.h>
#include <libc.h>
#include "../fibheap.h"

typedef struct Int Int;
struct Int {
	Fibnode;
	int i;
};

int
intcmp(Fibnode *x, Fibnode *y)
{
	Int *s, *t;

	s = (Int*)x;
	t = (Int*)y;

	if(s->i < t->i)
		return -1;
	if(s->i > t->i)
		return 1;
	return 0;
}

void
main(void)
{
	Fibheap *h;
	Int pool[10], *i;
	int j;

	h = fibcreate(intcmp);
	for(i = pool; i < pool+10; i++) {
		i->i = i-pool + 10;
		fibinsert(h, i);
	}

	i = pool+4;
	i->i = 4;
	fibdecreasekey(h, i);

	i = (Int*)h->min;
	print("%d %p\n", i->i, i);
	fibdeletemin(h);

	print("FOO\n");

	i = pool+7;
	i->i = 7;
	fibdecreasekey(h, i);
	i = pool+6;
	i->i = 6;
	fibdecreasekey(h, i);

	while(h->min != nil) {
		i = (Int*)h->min;
		print("%d %p\n", i->i, i);
		fibdeletemin(h);
	}

	exits(nil);
}
