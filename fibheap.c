#include <u.h>
#include <libc.h>
#include "fibheap.h"

static void
printlist(Fibnode *head) {
	Fibnode *n;

	n = head;
	do {
		fprint(2, "%p %p %p\n", n, n->prev, n->next);
		n = n->next;
	} while(n != head);
}

Fibheap*
fibcreate(Fibcmp cmp)
{
	Fibheap *heap;

	heap = malloc(sizeof(*heap));
	if(heap == nil)
		return nil;

	heap->arr = calloc(10, sizeof(*heap->arr));
	if(heap->arr == nil) {
		free(heap);
		return nil;
	}
	heap->arrlen = 10;

	heap->cmp = cmp;
	heap->min = nil;

	return heap;
}

Fibnode*
fibfree(Fibheap *h)
{
	Fibnode *min;

	min = h->min;
	free(h->arr);
	free(h);
	return min;
}

static Fibnode*
fibconcat(Fibnode *h1, Fibnode *h2)
{
	Fibnode *prev;

	if(h1 == nil)
		return h2;
	if(h2 == nil)
		return h1;

	h1->prev->next = h2;
	h2->prev->next = h1;

	prev = h1->prev;
	h1->prev = h2->prev;
	h2->prev = prev;
	return h1;
}

static Fibnode*
meld(Fibnode *h1, Fibnode *h2, Fibcmp cmp)
{
	if(h1 == nil)
		return h2;
	if(h2 == nil)
		return h1;

	fibconcat(h1, h2);
	return cmp(h1, h2) <= 0 ? h1 : h2;
}

static Fibnode*
fibinit(Fibnode *n)
{
	n->p = nil;
	n->c = nil;
	n->next = n;
	n->prev = n;
	n->rank = 0;
	n->mark = 0;
	return n;
}

void
fibinsert(Fibheap *h, Fibnode *n)
{
	h->min = meld(h->min, fibinit(n), h->cmp);
}

static Fibnode*
link1(Fibnode *x, Fibnode *y)
{
	x->c = fibconcat(x->c, y);
	y->p = x;
	y->mark = 0;
	x->rank += y->rank + 1;
	return x;
}

static Fibnode*
link(Fibnode *x, Fibnode *y, Fibcmp cmp)
{
	if(cmp(x, y) <= 0)
		return link1(x, y);
	else
		return link1(y, x);
}

static int
resizearr(Fibheap *h, int rank)
{
	Fibnode **a;
	int alen;

	a = h->arr;
	alen = h->arrlen;
	h->arrlen = 2 * rank * sizeof(*h->arr);
	h->arr = calloc(h->arrlen, sizeof(*h->arr));
	if(h->arr == nil)
		return -1;
	memcpy(h->arr, a, alen);
	return 0;
}

static int
arraylink(Fibheap *h, Fibnode *n)
{
	Fibnode *x;

	for(;;) {
		if(h->arrlen <= n->rank) {
			if(resizearr(h, n->rank) == -1)
				return -1;
		}

		n->next = n;
		n->prev = n;	
		x = h->arr[n->rank];
		if(x == nil) {
			h->arr[n->rank] = n;
			return n->rank;
		}
		h->arr[n->rank] = nil;
		n = link(x, n, h->cmp);
	}
}

static int
linkheaps(Fibheap *h, Fibnode *head)
{
	Fibnode *n, *next;
	int rank, maxrank;

	memset(h->arr, 0, sizeof(*h->arr) * h->arrlen);

	maxrank = 0;
	n = head;
	do {
		next = n->next;
		rank = arraylink(h, n);
		if(rank == -1)
			return -1;
		if(maxrank < rank)
			maxrank = rank;
		n = next;
	} while(n != head);

	return maxrank;
}

static void
meldheaps(Fibheap *h, int maxrank)
{
	Fibnode **ni;

	h->min = nil;
	for(ni = h->arr; ni <= h->arr + maxrank; ni++) {
		if(*ni != nil)
			h->min = meld(h->min, *ni, h->cmp);
	}
}

static int
linkstep(Fibheap *h, Fibnode *head)
{
	int maxrank;

	maxrank = linkheaps(h, head);
	if(maxrank == -1)
		return -1;
	meldheaps(h, maxrank);
	return 0;
}

static Fibnode*
removenode(Fibnode *n)
{
	Fibnode *next;

	n->p = nil;
	if(n->next == n)
		return nil;

	next = n->next;
	n->next->prev = n->prev;
	n->prev->next = n->next;
	return next;
}

int
fibdeletemin(Fibheap *h)
{
	Fibnode *head, *chead, *min;

	min = h->min;
	if(min == nil)
		return 0;

	head = removenode(min);
	head = fibconcat(head, min->c);
	if(head == nil) {
		h->min = nil;
		return 0;
	}

	return linkstep(h, head);
}

void
cut(Fibheap *h, Fibnode *n)
{
	Fibnode *p;

	p = n->p;
	p->rank -= n->rank + 1;
	p->c = removenode(n);
	h->min = meld(h->min, n, h->cmp);
}

void
cascadingcut(Fibheap *h, Fibnode *n)
{
	Fibnode *p;

Loop:
	p = n->p;
	cut(h, n);
	if(p->p == nil)
		return;

	if(p->mark) {
		n = p;
		goto Loop;
	}

	p->mark = 1;
}

void
fibdecreasekey(Fibheap *h, Fibnode *n)
{
	if(n->p == nil) {
		h->min = h->cmp(h->min, n) ? h->min : n;
		return;
	}

	if(h->cmp(n->p, n) < 0)
		return;

	cascadingcut(h, n);
}

int
fibdelete(Fibheap *h, Fibnode *n)
{
	if(h->min == n)
		return fibdeletemin(h);

	if(n->p == nil) {
		removenode(n);
		return 0;
	}

	cascadingcut(h, n);
	removenode(n);
	fibconcat(h->min, n->c);
	return 0;
}
