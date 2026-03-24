
/* Copyright Massachusetts Institute of Technology 1990,1991 */

#ifndef lint
static char rcsid[] = "$Header: _bytevec.c,v 1.5 91/06/06 13:23:04 root Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include "pclu_err.h"
#include "pclu_sys.h"

extern void bzero();

errcode _bytevecOPcreate(sz, ans)
CLUREF sz;
CLUREF *ans;
{
CLUREF temp;
long alloc_size;

	if (sz.num > MAX_STR) signal(ERR_toobig);
    alloc_size = (sz.num + 1 + 2 * sizeof(CLUREF) + 7) & ~7;
	clu_alloc_atomic(alloc_size, &temp);
    bzero(temp.str, alloc_size);
	temp.str->size = sz.num;
	temp.str->typ.val = CT_STRING;
	temp.str->typ.mark = 0;
	temp.str->typ.refp = 0;

	ans->str = temp.str;
	signal(ERR_ok);
	}

errcode _bytevecOPcopy(bv1, ans)
CLUREF bv1;
CLUREF *ans;
{
CLUREF temp;
long i, alloc_size;

    alloc_size = (bv1.str->size + 1 + 2 * sizeof(CLUREF) + 7) & ~7;

	clu_alloc_atomic(alloc_size, &temp);
    bzero(temp.str, alloc_size);
	temp.str->size = bv1.str->size;
	temp.str->typ.val = CT_STRING;
	temp.str->typ.mark = 0;
	temp.str->typ.refp = 0;

	for (i = 0; i < bv1.str->size; i++) {
		temp.str->data[i] = bv1.str->data[i];
		}
	ans->str = temp.str;
	signal(ERR_ok);
	}

errcode _bytevecOPfetch(bv, i, ans)
CLUREF bv, i;
CLUREF *ans;
{
	if (i.num < 1 || i.num > bv.str->size) signal(ERR_bounds);
	ans->ch = bv.str->data[i.num-1];
	signal(ERR_ok);
	}

errcode _bytevecOPstore(bv, i, c)
CLUREF bv, i, c;
{
	if (i.num < 1 || i.num > bv.str->size) signal(ERR_bounds);
	bv.str->data[i.num-1] = c.ch;
	signal(ERR_ok);
	}

errcode _bytevecOPsize(bv, ans)
CLUREF bv;
CLUREF *ans;
{
	ans->num = bv.str->size;
	signal(ERR_ok);
	}

errcode _bytevecOPindexv(find, bv, start, ans)
CLUREF find, bv, start;
CLUREF *ans;
{
long i, j, size, fsize;

	size = bv.str->size;
	fsize = find.str->size;
	if (fsize == 0) {
		if (start.num < 1 || start.num > size + 1) signal(ERR_bounds);
		ans->num = start.num;
		signal(ERR_ok);
		}
	for (i = start.num - 1; i <= size - fsize; i++) {
		for (j = 0; j < fsize; j++) {
			if (bv.str->data[i+j] != find.str->data[j]) break;
			}
		if (j == fsize) {
			ans->num = i + 1;
			signal(ERR_ok);
			}
		}
	ans->num = 0;
	signal(ERR_ok);
	}

errcode _bytevecOPnc_indexv(find, bv, start, ans)
CLUREF find, bv, start;
CLUREF *ans;
{
	return(_bytevecOPindexv(find, bv, start, ans));
	}

errcode _bytevecOPmove_lr(bv1, s1, bv2, s2, len)
CLUREF bv1, s1, bv2, s2, len;
{
long i;

	if (s1.num < 1 || s2.num < 1 || len.num < 0 || 
	    s1.num + len.num - 1 > bv1.str->size ||
	    s2.num + len.num - 1 > bv2.str->size) signal(ERR_bounds);
	for (i = 0; i < len.num; i++) {
		bv2.str->data[s2.num + i - 1] = bv1.str->data[s1.num + i - 1];
		}
	signal(ERR_ok);
	}

errcode _bytevecOPequal(bv1, bv2, ans)
CLUREF bv1, bv2;
CLUREF *ans;
{
long i;

	if (bv1.str == bv2.str) {
		ans->tf = true;
		signal(ERR_ok);
		}
	if (bv1.str->size != bv2.str->size) {
		ans->tf = false;
		signal(ERR_ok);
		}
	for (i = 0; i < bv1.str->size; i++) {
		if (bv1.str->data[i] != bv2.str->data[i]) {
			ans->tf = false;
			signal(ERR_ok);
			}
		}
	ans->tf = true;
	signal(ERR_ok);
	}

typedef struct {
long count;
    OWNPTR type_owns;
    OWNPTR op_owns;
struct OP_ENTRY entry[8];
} _bytevec_OPS;

OWN_ptr _bytevec_own_init = {1, 0};

CLU_proc _bytevec_oe_create = {{0,0,0,0}, _bytevecOPcreate, &_bytevec_own_init, &_bytevec_own_init};
CLU_proc _bytevec_oe_copy = {{0,0,0,0}, _bytevecOPcopy, &_bytevec_own_init, &_bytevec_own_init};
CLU_proc _bytevec_oe_fetch = {{0,0,0,0}, _bytevecOPfetch, &_bytevec_own_init, &_bytevec_own_init};
CLU_proc _bytevec_oe_store = {{0,0,0,0}, _bytevecOPstore, &_bytevec_own_init, &_bytevec_own_init};
CLU_proc _bytevec_oe_size = {{0,0,0,0}, _bytevecOPsize, &_bytevec_own_init, &_bytevec_own_init};
CLU_proc _bytevec_oe_indexv = {{0,0,0,0}, _bytevecOPindexv, &_bytevec_own_init, &_bytevec_own_init};
CLU_proc _bytevec_oe_move_lr = {{0,0,0,0}, _bytevecOPmove_lr, &_bytevec_own_init, &_bytevec_own_init};
CLU_proc _bytevec_oe_equal = {{0,0,0,0}, _bytevecOPequal, &_bytevec_own_init, &_bytevec_own_init};

_bytevec_OPS _bytevec_ops_actual = {8,
    &_bytevec_own_init,
    &_bytevec_own_init, {
{&_bytevec_oe_create, "create"},
{&_bytevec_oe_copy, "copy"},
{&_bytevec_oe_fetch, "fetch"},
{&_bytevec_oe_store, "store"},
{&_bytevec_oe_size, "size"},
{&_bytevec_oe_indexv, "indexv"},
{&_bytevec_oe_move_lr, "move_lr"},
{&_bytevec_oe_equal, "equal"}}
};

_bytevec_OPS *_bytevec_ops = &_bytevec_ops_actual;
