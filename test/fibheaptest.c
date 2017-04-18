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

enum {
	POOLSIZ = 100,
	RANDSIZ = 500,
};

void
main(void)
{
	Fibheap *fh;
	Int pool[POOLSIZ], *i;

	srand(time(nil));

	fh = fibcreate(intcmp);
	for(i = pool; i < pool+POOLSIZ; i++) {
		i->i = nrand(RANDSIZ);
		print("Adding %d %p\n", i->i, i);
		fibinsert(fh, i);
	}
	while(fh->min != nil) {
		i = (Int*)fh->min;
		print("Heap sorted %d %p\n", i->i, i);
		if(fibdeletemin(fh) < 0)
			sysfatal("delete failed: %r");
	}
	exits(nil);
}
