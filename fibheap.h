typedef struct Fibheap Fibheap;
typedef struct Fibnode Fibnode;
typedef int (*Fibcmp)(Fibnode*, Fibnode*);

struct Fibheap {
	Fibcmp cmp;
	Fibnode *min;
	Fibnode **arr;
	int arrlen;
};

struct Fibnode {
	Fibnode *p, *c, *next, *prev;
	int rank;
	char mark;
};

int      fibdeletemin(Fibheap*);
Fibheap *fibcreate(Fibcmp);
Fibnode *fibfree(Fibheap*);
void     fibinsert(Fibheap*, Fibnode*);
