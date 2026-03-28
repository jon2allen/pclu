
/* Copyright Massachusetts Institute of Technology 1990,1991 */

#ifndef lint
static char rcsid[] = "$Header: _wordvec.c,v 1.6 91/06/06 13:52:31 dcurtis Exp $";
#endif
/* $Log:	_wordvec.c,v $
 * Revision 1.6  91/06/06  13:52:31  dcurtis
 * added copyright notice
 * 
 * Revision 1.5  91/06/03  17:32:27  root
 * sparcstation compatibility: int->CLUREF
 * 
 * Revision 1.4  91/05/31  13:09:10  root
 * fixed aggregate initialization in ops_actual
 * 
 * Revision 1.3  91/02/11  12:05:57  dcurtis
 * added lfetch and lstore
 * fixed bfetch: avoid sign extensions
 * fixed wft etch : case 4 
 * 
 * Revision 1.2  91/02/06  13:57:38  dcurtis
 * no change
 * 
 * Revision 1.1  91/02/04  23:21:09  mtv
 * Initial revision
 * 
 */

/*							*/
/*		WORDVEC IMPLEMENTATION			*/
/*							*/

/* TODO: comp, build, move_rl, move_b (not referenced) 	*/

/*   mutable aggregate of bytes				*/
/*	to hold 64-bit quantities and be accessed in    */
/*		64-bit units				*/

#include <stdio.h>
#include "pclu_err.h"
#include "pclu_sys.h"

errcode _wordvecOPcreate(sz, ans)
CLUREF sz;
CLUREF *ans;
{
errcode err;
CLUREF temp;
	long size = sz.num * 8;
	long rounded_size;
	int i;

	if (size > MAX_STR) {
		fprintf(stderr, "_wordvec$create: size %ld > MAX_STR %ld\n", size, (long)MAX_STR);
		signal(ERR_toobig);
	}
	rounded_size = ((size+7)/8)*8 + sizeof(CLU_string);
	clu_alloc(rounded_size, &temp);
	temp.str->size = size;
	temp.str->typ.val = CT_STRING;
	temp.str->typ.mark = 0;
	temp.str->typ.refp = 0;

	ans->str = temp.str;
	signal(ERR_ok);
	}

errcode _wordvecOPcopy(wv1, wv2)
CLUREF wv1;
CLUREF *wv2;
{
errcode err;
CLUREF temp;
int i;

	clu_alloc(((wv1.str->size+7)/8)*8 + sizeof(CLU_string), &temp);
	temp.str->size = wv1.str->size;
	temp.str->typ.val = CT_STRING;
	temp.str->typ.mark = 0;
	temp.str->typ.refp = 0;

	for (i = 0; i < wv1.str->size; i++) {
		temp.str->data[i] = wv1.str->data[i];
		}
		temp.str->data[i] = '\000';
	wv2->str = temp.str;
	signal(ERR_ok);
	}

errcode _wordvecOPfetch(wv, i, ans)
CLUREF wv, i, *ans;
{
int bi;
	bi = (i.num-1)*8;
	if (bi < 0 || bi >= wv.str->size) {
		fprintf(stderr, "_wordvec$fetch bounds check failed: i %ld, bi %d, sz %ld\n", i.num, bi, wv.str->size);
		signal(ERR_bounds);
	}
	ans->num = wv.vec->data[i.num-1];
	signal(ERR_ok);
	}	

errcode _wordvecOPstore(wv, i, w)
CLUREF wv, i, w;
{
int bi;
	bi = (i.num-1)*8;
	if (bi < 0 || bi >= wv.str->size) {
		fprintf(stderr, "_wordvec$store bounds check failed: i %ld, bi %d, sz %ld\n", i.num, bi, wv.str->size);
		signal(ERR_bounds);
	}
	wv.vec->data[i.num-1] = w.num;
	signal(ERR_ok);
	}

errcode _wordvecOPfetch2(wv, i, ans1, ans2)
CLUREF wv, i, *ans1, *ans2;
{
int bi;
long temp;
	bi = (i.num-1)*8;
	if (bi < 0 || bi >= wv.str->size) {
		fprintf(stderr, "_wordvec$fetch2 bounds check failed: i %ld, bi %d, sz %ld\n", i.num, bi, wv.str->size);
		signal(ERR_bounds);
	}
	temp = wv.vec->data[i.num - 1];
	ans1->num = (temp >> 32) & 0xffffffff;
	ans2->num = temp & 0xffffffff;
	signal(ERR_ok);
	}	

errcode _wordvecOPstore2(wv, i, l, r)
CLUREF wv, i, l, r;
{
int bi;
	bi = (i.num-1)*8;
	if (bi < 0 || bi >= wv.str->size) {
		fprintf(stderr, "_wordvec$store2 bounds check failed: i %ld, bi %d, sz %ld\n", i.num, bi, wv.str->size);
		signal(ERR_bounds);
	}
	wv.vec->data[i.num-1] = (r.num & 0xffffffffL) | ((l.num & 0xffffffffL) << 32);
	signal(ERR_ok);
	}

errcode _wordvecOPxstore(wv, i, b, l)
CLUREF wv, i, b, l;
{
int bi;
	bi = (i.num-1)*8;
	if (bi < 0 || bi > wv.str->size) signal(ERR_bounds);
	wv.vec->data[i.num-1] = l.num & 0xffffff | ((b.num & 0xff) << 24);
	signal(ERR_ok);
	}

errcode _wordvecOPbfetch(wv, i, ans)
CLUREF wv, i, *ans;
{
int bi, wi, sub_index;
long temp;
	bi = (i.num-1);
	if (bi < 0 || bi >= wv.str->size) {
		fprintf(stderr, "_wordvec$bfetch bounds check failed: bi %d, sz %ld\n", bi, wv.str->size);
		signal(ERR_bounds);
	}
	wi = bi/8;
	sub_index = bi % 8;
	temp = wv.vec->data[wi];
	ans->num = (temp >> (sub_index * 8)) & 0xff;
	signal(ERR_ok);
	}	

errcode _wordvecOPbstore(wv, i, c)
CLUREF wv, i, c;
{
int bi, wi, sub_index;
long temp;
	bi = (i.num-1);
	if (bi < 0 || bi >= wv.str->size) {
		fprintf(stderr, "_wordvec$bstore bounds check failed: bi %d, sz %ld\n", bi, wv.str->size);
		signal(ERR_bounds);
	}
	wi = bi/8;
	sub_index = bi % 8;
	temp = wv.vec->data[wi];
	temp &= ~(0xffL << (sub_index * 8));
	temp |= ((c.num & 0xffL) << (sub_index * 8));
	wv.vec->data[wi] = temp;
	signal(ERR_ok);
	}


errcode _wordvecOPwfetch(wv, i, ans)
CLUREF wv, i, *ans;
{
int bi, wi, sub_index;
long temp, temp2;
	bi = (i.num-1);
	if (bi < 0 || bi >= wv.str->size - 1) signal(ERR_bounds);
	wi = bi/8;
	sub_index = bi % 8;
	if (sub_index <= 6) {
		temp = wv.vec->data[wi];
		ans->num = (temp >> (sub_index * 8)) & 0xffff;
	} else {
		temp = (wv.vec->data[wi] >> 56) & 0xff;
		temp2 = (wv.vec->data[wi+1] & 0xff) << 8;
		ans->num = temp | temp2;
	}
	signal(ERR_ok);
	}	

errcode _wordvecOPwstore(wv, i, n)
CLUREF wv, i, n;
{
int bi, wi, sub_index;
long temp;
	bi = (i.num-1);
	if (bi < 0 || bi >= wv.str->size-1) signal(ERR_bounds);
	n.num = n.num & 0xffff;
	wi = bi/8;
	sub_index = bi % 8;
	temp = wv.vec->data[wi];
	if (sub_index <= 6) {
		temp &= ~(0xffffL << (sub_index * 8));
		temp |= (n.num << (sub_index * 8));
		wv.vec->data[wi] = temp;
	} else {
		temp &= ~(0xffL << (sub_index * 8));
		temp |= (n.num & 0xff) << (sub_index * 8);
		wv.vec->data[wi] = temp;
		temp = wv.vec->data[wi+1];
		temp &= ~0xffL;
		temp |= (n.num >> 8) & 0xff;
		wv.vec->data[wi+1] = temp;
	}
	signal(ERR_ok);
	}


errcode _wordvecOPlfetch(wv, i, ans)
CLUREF wv, i, *ans;
{
int bi, wi, sub_index;
long temp, temp2;
	bi = i.num - 1;
	if (bi < 0 || bi > wv.str->size - 4) signal(ERR_bounds);
	wi = bi / 8;
	sub_index = bi % 8;
	if (sub_index <= 4) {
		temp = wv.vec->data[wi];
		ans->num = (temp >> (sub_index * 8)) & 0xffffffff;
	} else {
		temp = (wv.vec->data[wi] >> (sub_index * 8));
		temp2 = (wv.vec->data[wi+1] << (64 - sub_index * 8)) >> (64 - sub_index * 8);
		/* Actually simplify: read bytes. */
		ans->num = (temp & 0xffffffff) | (temp2 << (64 - sub_index * 8));
	}
	signal(ERR_ok);
	}	

errcode _wordvecOPlstore(wv, i, n)
CLUREF wv, i, n;
{
	signal(ERR_bounds);
	}

errcode _wordvecOPsize(wv, ans)
CLUREF wv, *ans;
{
	ans->num = wv.str->size/8;
	signal(ERR_ok);
	}

errcode _wordvecOPequal(wv1, wv2, ans)
CLUREF wv1, wv2, *ans;
{
	ans->tf = (wv1.str == wv2.str);
	signal(ERR_ok);
	}

errcode _wordvecOPindexc(c, wv, first, ans)
CLUREF c, wv, first, *ans;
{
int i;
	if (first.num < 1 || first.num > wv.str->size) signal(ERR_bounds);
	for (i = first.num; i <= wv.str->size; i++) {
		if (wv.str->data[i-1] == c.ch) {
			ans->num = i;
			signal(ERR_ok);
			}
		}
	ans->num = 0;
	signal(ERR_ok);
	}

errcode _wordvecOPindexv(wv1, wv2, first, ans)
CLUREF wv1, wv2, first, *ans;
{
bool found;
int i,j;
	if (first.num < 1 || first.num > wv2.str->size) signal(ERR_bounds);
	for (i = first.num; i + wv1.str->size - 1 <= wv2.str->size; i++) {
		found = true;
		for (j = 1; j <= wv1.str->size; j++) {
			if (((unsigned char*)wv2.str->data)[i-1+j-1] != ((unsigned char*)wv1.str->data)[j-1]) {
				found = false;
				break;
				}
			}
		if (found == true) {
			ans->num = i;
			signal(ERR_ok);
			}
		}
	ans->num = 0;
	signal(ERR_ok);
	}

errcode _wordvecOPnc_indexv(wv1, wv2, first, ans)
CLUREF wv1, wv2, first, *ans;
{
	return _wordvecOPindexv(wv1, wv2, first, ans);
	}

errcode _wordvecOPmove_lr(wv1, s1, wv2, s2, len)
CLUREF wv1, s1, wv2, s2, len;
{
int copycount;
	if (len.num < 0) signal(ERR_negative_size);
	if (s1.num < 1 || (s1.num-1)*8 >= wv1.str->size) {
		fprintf(stderr, "_wordvec$move_lr src bounds check failed: s1 %ld, sz1 %ld\n", s1.num, wv1.str->size);
		signal(ERR_bounds);
	}
	if (s2.num < 1 || (s2.num-1)*8 >= wv2.str->size) {
		fprintf(stderr, "_wordvec$move_lr dst bounds check failed: s2 %ld, sz2 %ld\n", s2.num, wv2.str->size);
		signal(ERR_bounds);
	}
	copycount = len.num;
	if (copycount + s1.num-1 > wv1.str->size/8) copycount = wv1.str->size/8 - s1.num + 1;
	if (copycount + s2.num-1 > wv2.str->size/8) copycount = wv2.str->size/8 - s2.num + 1;
	if (copycount <= 0) signal(ERR_ok);
	memmove(((unsigned char*)wv2.str->data) + (s2.num-1)*8, 
	        ((unsigned char*)wv1.str->data) + (s1.num-1)*8, 
	        copycount * 8);
	signal(ERR_ok);
	}

errcode _wordvecOPmove_v(qc1, s1, wv2, s2, len)
CLUREF qc1, s1, wv2, s2, len;
{
int copycount;
int i;
	if (len.num < 0) signal(ERR_negative_size);
	if (s1.num < 1 || s1.num > qc1.vec->size) signal(ERR_bounds);
	if (s2.num < 1 || (s2.num-1) >= wv2.str->size) signal(ERR_bounds);
	copycount = len.num;
	if (copycount + s1.num-1 > qc1.vec->size) copycount = qc1.vec->size - s1.num + 1;
	if (copycount + (s2.num-1) > wv2.str->size) copycount = wv2.str->size - (s2.num-1);
	for (i = 0; i < copycount; i++) {
		((unsigned char*)wv2.str->data)[(s2.num-1)+i] = (unsigned char)qc1.vec->data[s1.num-1+i];
		}
	signal(ERR_ok);
	}

errcode _wordvecOPmove_rl(w1, s1, w2, s2, len)
CLUREF w1, s1, w2, s2, len;
{
	return _wordvecOPmove_lr(w1, s1, w2, s2, len);
	}

errcode _wordvecOPget_byte(wv, wrd, bit, len, ans)
CLUREF wv, wrd, bit, len, *ans;
{
long n1, result, mask;
	if (len.num <= 0 || len.num > 64) signal(ERR_illegal_size);
	n1 = (wrd.num-1) * 8;
	if (n1 < 0 || n1 >= wv.str->size) signal(ERR_bounds);
	result = wv.vec->data[wrd.num - 1];
	result = result >> (64 - bit.num);
	mask =  (1L << len.num) - 1;
	ans->num = result & mask;
	signal(ERR_ok);
	}

errcode _wordvecOPset_byte(wv, byte, wrd, bit, len)
CLUREF wv, byte, wrd, bit, len;
{
long n1, slot, new_field, field_mask;
	if (len.num <= 0 || len.num > 64) signal(ERR_illegal_size);
	n1 = (wrd.num-1) * 8;
	if (n1 < 0 || n1 >= wv.str->size) signal(ERR_bounds);
	field_mask = (1L << len.num) - 1;
	new_field = (byte.num & field_mask) << (64 - bit.num);
	field_mask = field_mask << (64 - bit.num);
	slot = wv.vec->data[wrd.num - 1];
	slot = (slot & ~field_mask) | new_field;
	wv.vec->data[wrd.num - 1] = slot;
	signal(ERR_ok);
	}

errcode _wordvecOPbyte_size(ans)
CLUREF *ans;
{
	ans->num = 8;
	signal(ERR_ok);
	}

errcode _wordvecOPword_size(ans)
CLUREF *ans;
{
	ans->num = 64;
	signal(ERR_ok);
	}

errcode _wordvecOPbytes_per_word(ans)
CLUREF *ans;
{
	ans->num = 8;
	signal(ERR_ok);
	}

errcode _wordvecOPmove_b2w(b, bpos, w, wpos, n)
CLUREF b, bpos, w, wpos, n;
{
int copycount;
	if (n.num < 0) signal(ERR_negative_size);
	copycount = n.num;
	if (copycount <= 0) signal(ERR_ok);
	if (bpos.num < 1 || bpos.num > b.str->size) signal(ERR_bounds);
	if (wpos.num < 1 || wpos.num > w.str->size) signal(ERR_bounds);
	if (bpos.num + copycount - 1 > b.str->size) copycount = b.str->size - bpos.num + 1;
	if (wpos.num + copycount - 1 > w.str->size) copycount = w.str->size - wpos.num + 1;
	if (copycount <= 0) signal(ERR_ok);
	memmove(((unsigned char*)w.str->data) + (wpos.num-1), b.str->data + bpos.num - 1, copycount);
	signal(ERR_ok);
	}

errcode _wordvecOPmove_w2b(w, wpos, b, bpos, n)
CLUREF w, wpos, b, bpos, n;
{
int copycount;
	if (n.num < 0) signal(ERR_negative_size);
	copycount = n.num;
	if (copycount <= 0) signal(ERR_ok);
	if (bpos.num < 1 || bpos.num > b.str->size) signal(ERR_bounds);
	if (wpos.num < 1 || wpos.num > w.str->size) signal(ERR_bounds);
	if (wpos.num + copycount - 1 > w.str->size) copycount = w.str->size - (wpos.num-1);
	if (bpos.num + copycount - 1 > b.str->size) copycount = b.str->size - bpos.num + 1;
	if (copycount <= 0) signal(ERR_ok);
	memmove(b.str->data + bpos.num - 1, ((unsigned char*)w.str->data) + (wpos.num-1), copycount);
	signal(ERR_ok);
	}

errcode _wordvecOP_gcd(w, tab, ans)
CLUREF w, tab, *ans;
{
errcode err;
CLUREF temp_oneof, sz;
        err = oneofOPnew(CLU_1, CLU_0, &temp_oneof);
        if (err != ERR_ok) resignal(err);
	sz.num = 2*CLUREFSZ + (w.str->size + CLUREFSZ) & ~(CLUREFSZ - 1);
        err = gcd_tabOPinsert(tab, sz, temp_oneof, w, ans);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

errcode _wordvecOPprint(w, pst)
CLUREF w, pst;
{
errcode err;
	err = stringOPprint(w, pst);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

errcode _wordvecOPdebug_print(w, pst)
CLUREF w, pst;
{
errcode err;
	err = stringOPprint(w, pst);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

typedef struct {
int count;
    OWNPTR type_owns;
    OWNPTR op_owns;
struct OP_ENTRY entry[13];
} _wordvec_OPS;
CLU_proc _wordvec_oe_create = {{0,0,0,0}, _wordvecOPcreate, 0};
CLU_proc _wordvec_oe_copy = {{0,0,0,0}, _wordvecOPcopy, 0};
CLU_proc _wordvec_oe_fetch = {{0,0,0,0}, _wordvecOPfetch, 0};
CLU_proc _wordvec_oe_store = {{0,0,0,0}, _wordvecOPstore, 0};
CLU_proc _wordvec_oe_size = {{0,0,0,0}, _wordvecOPsize, 0};
CLU_proc _wordvec_oe_equal = {{0,0,0,0}, _wordvecOPequal, 0};
CLU_proc _wordvec_oe_indexc = {{0,0,0,0}, _wordvecOPindexc, 0};
CLU_proc _wordvec_oe_indexv = {{0,0,0,0}, _wordvecOPindexv, 0};
CLU_proc _wordvec_oe_nc_indexv = {{0,0,0,0}, _wordvecOPnc_indexv, 0};
CLU_proc _wordvec_oe_move_lr = {{0,0,0,0}, _wordvecOPmove_lr, 0};
CLU_proc _wordvec_oe_move_v = {{0,0,0,0}, _wordvecOPmove_v, 0};
CLU_proc _wordvec_oe_print = {{0,0,0,0}, _wordvecOPprint, 0};
CLU_proc _wordvec_oe_debug_print = {{0,0,0,0}, _wordvecOPdebug_print, 0};

OWN_ptr _wordvec_own_init = {1,0};

_wordvec_OPS _wordvec_ops_actual = {13,
    &_wordvec_own_init,
    &_wordvec_own_init, {
{&_wordvec_oe_create, "create"},
{&_wordvec_oe_copy, "copy"},
{&_wordvec_oe_fetch, "fetch"},
{&_wordvec_oe_store, "store"},
{&_wordvec_oe_size, "size"},
{&_wordvec_oe_equal, "equal"},
{&_wordvec_oe_indexc, "indexc"},
{&_wordvec_oe_indexv, "indexv"},
{&_wordvec_oe_nc_indexv, "nc_indexv"},
{&_wordvec_oe_move_lr, "move_lr"},
{&_wordvec_oe_move_v, "move_v"},
{&_wordvec_oe_print, "print"},
{&_wordvec_oe_debug_print, "debug_print"}
}};

_wordvec_OPS *_wordvec_ops = &_wordvec_ops_actual;
