#ifndef lint
static char rcsid[] = "$Header: /pm/src/site/pclu/code/base/RCS/string.c,v 1.7 91/08/29 16:09:59 dcurtis Exp $";
#endif

#include "pclu_err.h"
#include "pclu_sys.h"
#include <stdio.h>
#include <string.h>

extern void bzero();
extern errcode stringOPprint();
extern errcode stringOPcons();

errcode stringOPsize(s, ans)
CLUREF s, *ans;
{
	ans->num = s.str->size;
	signal(ERR_ok);
	}

errcode stringOPindexs(find, s, ans)
CLUREF find, s, *ans;
{
int i, j, size, fsize;
	size = s.str->size;
	fsize = find.str->size;
	if (fsize == 0) {
		ans->num = 1;
		signal(ERR_ok);
		}
	for (i = 0; i <= size - fsize; i++) {
		for (j = 0; j < fsize; j++) {
			if (s.str->data[i+j] != find.str->data[j]) break;
			}
		if (j == fsize) {
			ans->num = i + 1;
			signal(ERR_ok);
			}
		}
	ans->num = 0;
	signal(ERR_ok);
	}

errcode stringOPindexc(find, s, ans)
CLUREF find, s, *ans;
{
int i, size;
	size = s.str->size;
	for (i = 0; i < size; i++) {
		if (s.str->data[i] == find.ch) {
			ans->num = i + 1;
			signal(ERR_ok);
			}
		}
	ans->num = 0;
	signal(ERR_ok);
	}

errcode stringOPcons(buf, start, len, ans)
char *buf;
CLUREF start, len, *ans;
{
long i, alloc_size;
    alloc_size = (len.num + 1 + 2 * sizeof(CLUREF) + 7) & ~7;
	clu_alloc_atomic(alloc_size, &ans->str);
    bzero(ans->str, alloc_size);
	ans->str->size = len.num;
	ans->str->typ.val = CT_STRING;
	ans->str->typ.mark = 0;
	ans->str->typ.refp = 0;
	for (i = 0; i < len.num; i++) {
		ans->str->data[i] = buf[start.num + i - 1];
		}
	ans->str->data[len.num] = '\0';
	signal(ERR_ok);
	}

errcode stringOPcons0(len, ans)
CLUREF len, *ans;
{
long alloc_size;
    alloc_size = (len.num + 1 + 2 * sizeof(CLUREF) + 7) & ~7;
	clu_alloc_atomic(alloc_size, &ans->str);
    bzero(ans->str, alloc_size);
	ans->str->size = len.num;
	ans->str->typ.val = CT_STRING;
	ans->str->typ.mark = 0;
	ans->str->typ.refp = 0;
	ans->str->data[len.num] = '\0';
	signal(ERR_ok);
	}

errcode stringOPc2s(ch, ans)
CLUREF ch, *ans;
{
errcode err;
CLUREF temp;
	err = stringOPcons0(CLU_1, &temp);
	if (err != ERR_ok) resignal(err);
	temp.str->data[0] = ch.ch;
	ans->str = temp.str;
	signal(ERR_ok);
	}

errcode stringOPconcat(s1, s2, ans)
CLUREF s1, s2, *ans;
{
errcode err;
CLUREF temp;
long size, i, j, alloc_size;
	if (s1.str->size == 0) {
		ans->str = s2.str;
		signal(ERR_ok);
		}
	if (s2.str->size == 0) {
		ans->str = s1.str;
		signal(ERR_ok);
		}
	size = s1.str->size + s2.str->size;
	if (size > MAX_STR) {
		elist[0].str = huge_allocation_request_STRING;
		signal(ERR_failure);
		}
    alloc_size = (size + 1 + 2 * sizeof(CLUREF) + 7) & ~7;

	clu_alloc_atomic(alloc_size, &temp);
    bzero(temp.str, alloc_size);
	temp.str->size = size;
	temp.str->typ.val = CT_STRING;
	temp.str->typ.mark = 0;
	temp.str->typ.refp = 0;

	for (i = 0; i < s1.str->size; i++)
		temp.str->data[i] = s1.str->data[i];

	for (i = 0, j = s1.str->size; i < s2.str->size; i++, j++)
		temp.str->data[j] = s2.str->data[i];
    temp.str->data[size] = '\0';
	ans->str = temp.str;
	signal(ERR_ok);
	}

errcode stringOPappend(s, c, ans)
CLUREF s, c, *ans;
{
long size, alloc_size;
errcode err;
CLUREF temp;
long i;
	size = s.str->size + 1;
	if (size > MAX_STR) {
		elist[0].str = huge_allocation_request_STRING;
		signal(ERR_failure);
		}
    alloc_size = (size + 1 + 2 * sizeof(CLUREF) + 7) & ~7;

	clu_alloc_atomic(alloc_size, &temp);
    bzero(temp.str, alloc_size);
	temp.str->size = size;
	temp.str->typ.val = CT_STRING;
	temp.str->typ.mark = 0;
	temp.str->typ.refp = 0;

	for (i = 0; i < s.str->size; i++)
		temp.str->data[i] = s.str->data[i];
	temp.str->data[size-1] = c.ch;
    temp.str->data[size] = '\0';
	ans->str = temp.str;
	signal(ERR_ok);
	}

errcode stringOPempty(s, ans)
CLUREF s, *ans;
{
	if (s.str->size == 0) ans->tf = true;
	else ans->tf = false;
	signal(ERR_ok);
	}

errcode stringOPsubstr(s, start, len, ans)
CLUREF s, start, len, *ans;
{
errcode err;
	if (start.num < 1 || len.num < 0 || start.num + len.num - 1 > s.str->size)
		signal(ERR_bounds);
	err = stringOPcons(s.str->data, start, len, ans);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

errcode stringOPrest(s, start, ans)
CLUREF s, start, *ans;
{
errcode err;
CLUREF len;
	if (start.num < 1) signal(ERR_bounds);
	if (start.num > s.str->size) {
		len.num = 0;
		}
	else {
		len.num = s.str->size - start.num + 1;
		}
	err = stringOPcons(s.str->data, start, len, ans);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

errcode stringOPs2ac(s, ans)
CLUREF s, *ans;
{
errcode err;
CLUREF a;
long i;
	err = arrayOPnew(&a);
	if (err != ERR_ok) resignal(err);
	for (i = 0; i < s.str->size; i++) {
        CLUREF temp_ch; temp_ch.ch = s.str->data[i];
		err = arrayOPaddh(a, temp_ch);
		if (err != ERR_ok) resignal(err);
		}
	ans->array = a.array;
	signal(ERR_ok);
	}

errcode stringOPac2s(a, ans)
CLUREF a, *ans;
{
errcode err;
CLUREF s, len;
long i;
	len.num = a.array->ext_size;
	err = stringOPcons0(len, &s);
	if (err != ERR_ok) resignal(err);
	for (i = 0; i < len.num; i++) {
        CLUREF *p;
        p = (CLUREF *)&a.array->store->data[i + a.array->int_low];
		s.str->data[i] = p->ch;
		}
	ans->str = s.str;
	signal(ERR_ok);
	}

errcode stringOPs2sc(s, ans)
CLUREF s, *ans;
{
errcode err;
CLUREF a;
	err = stringOPs2ac(s, &a);
	if (err != ERR_ok) resignal(err);
	ans->num = a.num;
	signal(ERR_ok);
	}

errcode stringOPsc2s(a, ans)
CLUREF a, *ans;
{
errcode err;
	err = stringOPac2s(a, ans);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

errcode stringOPchars(s, iterbod, locals, ecode2)
CLUREF s;
errcode (*iterbod)();
char **locals;
errcode *ecode2;
{
errcode err;
long i;
CLUREF iv_1;
	for (i = 0; i < s.str->size; i++) {
		iv_1.ch = s.str->data[i];
		err = iterbod(iv_1, locals, ecode2);
		if (err == ERR_ok) continue;
		if (err == ERR_break) signal(ERR_ok);
		if (err == ERR_iterbodyreturn) signal(ERR_iterbodyreturn);
		if (err == ERR_iterbodysignal) signal(ERR_iterbodysignal);
		if (err == ERR_iterbodyexit) signal(ERR_iterbodyexit);
		signal(err);
		}
	signal(ERR_ok);
	}

errcode stringOPlt(s1, s2, ans)
CLUREF s1, s2, *ans;
{
long size, i;
	size = s1.str->size;
	if (s2.str->size < size) size = s2.str->size;
	for (i = 0; i < size; i++) {
		if (s1.str->data[i] < s2.str->data[i]) {
			ans->tf = true;
			signal(ERR_ok);
			}
		if (s1.str->data[i] > s2.str->data[i]) {
			ans->tf = false;
			signal(ERR_ok);
			}
		}
	if (s1.str->size < s2.str->size) ans->tf = true;
	else ans->tf = false;
	signal(ERR_ok);
	}

errcode stringOPle(s1, s2, ans)
CLUREF s1, s2, *ans;
{
long size, i;
	size = s1.str->size;
	if (s2.str->size < size) size = s2.str->size;
	for (i = 0; i < size; i++) {
		if (s1.str->data[i] < s2.str->data[i]) {
			ans->tf = true;
			signal(ERR_ok);
			}
		if (s1.str->data[i] > s2.str->data[i]) {
			ans->tf = false;
			signal(ERR_ok);
			}
		}
	if (s1.str->size <= s2.str->size) ans->tf = true;
	else ans->tf = false;
	signal(ERR_ok);
	}

errcode stringOPge(s1, s2, ans)
CLUREF s1, s2, *ans;
{
long size, i;
	size = s1.str->size;
	if (s2.str->size < size) size = s2.str->size;
	for (i = 0; i < size; i++) {
		if (s1.str->data[i] > s2.str->data[i]) {
			ans->tf = true;
			signal(ERR_ok);
			}
		if (s1.str->data[i] < s2.str->data[i]) {
			ans->tf = false;
			signal(ERR_ok);
			}
		}
	if (s1.str->size >= s2.str->size) ans->tf = true;
	else ans->tf = false;
	signal(ERR_ok);
	}

errcode stringOPgt(s1, s2, ans)
CLUREF s1, s2, *ans;
{
long size, i;
	size = s1.str->size;
	if (s2.str->size < size) size = s2.str->size;
	for (i = 0; i < size; i++) {
		if (s1.str->data[i] > s2.str->data[i]) {
			ans->tf = true;
			signal(ERR_ok);
			}
		if (s1.str->data[i] < s2.str->data[i]) {
			ans->tf = false;
			signal(ERR_ok);
			}
		}
	if (s1.str->size > s2.str->size) ans->tf = true;
	else ans->tf = false;
	signal(ERR_ok);
	}

errcode stringOPfetch(s, i, ans)
CLUREF s, i, *ans;
{
	if (i.num < 1 || i.num > s.str->size) signal(ERR_bounds);
	ans->ch = s.str->data[i.num - 1];
	signal(ERR_ok);
	}

errcode stringOPequal(s1, s2, ans)
CLUREF s1, s2, *ans;
{
long i;
	if (s1.str == s2.str) {
		ans->tf = true;
		signal(ERR_ok);
		}
	if (s1.str->size != s2.str->size) {
		ans->tf = false;
		signal(ERR_ok);
		}
	for (i = 0; i < s1.str->size; i++) {
		if (s1.str->data[i] != s2.str->data[i]) {
			ans->tf = false;
			signal(ERR_ok);
			}
		}
	ans->tf = true;
	signal(ERR_ok);
	}

errcode stringOPsimilar(s1, s2, ans)
CLUREF s1, s2, *ans;
{
	return(stringOPequal(s1, s2, ans));
	}

errcode stringOPcopy(s1, ans)
CLUREF s1, *ans;
{
	ans->str = s1.str;
	signal(ERR_ok);
	}

errcode stringOPdebug_print(s, pst)
CLUREF s, pst;
{
errcode err;
	err = stringOPprint(s, pst);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

errcode stringOPprint(s, pst)
CLUREF s, pst;
{
errcode err;
long i;
CLUREF ans, temp_str;
	stringOPcons("\"", CLU_1, CLU_1, &temp_str);
	err = pstreamOPtext(pst, temp_str, &ans);
	if (err != ERR_ok) resignal(err);
	for (i = 0; i < s.str->size; i++) {
		if (s.str->data[i] == '\"' || s.str->data[i] == '\\') {
			stringOPcons("\\", CLU_1, CLU_1, &temp_str);
			err = pstreamOPtext(pst, temp_str, &ans);
			if (err != ERR_ok) resignal(err);
			}
		CLUREF tc; tc.ch = s.str->data[i];
		err = pstreamOPtextc(pst, tc, &ans);
		if (err != ERR_ok) resignal(err);
		}
	stringOPcons("\"", CLU_1, CLU_1, &temp_str);
	err = pstreamOPtext(pst, temp_str, &ans);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

extern errcode istreamOPputw(), istreamOPputs(), istreamOPgetw(), istreamOPgets();

errcode stringOPencode(s, ist)
CLUREF s, ist;
{
errcode err;
    CLUREF sz; sz.num = s.str->size;
	err = istreamOPputw(ist, sz);
	if (err != ERR_ok) resignal(err);
	err = istreamOPputs(ist, s);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

errcode stringOPdecode(ist, ans)
CLUREF ist, *ans;
{
errcode err;
CLUREF size, s;
	err = istreamOPgetw(ist, &size);
	if (err != ERR_ok) resignal(err);
	err = stringOPcons0(size, &s);
	if (err != ERR_ok) resignal(err);
	err = istreamOPgets(ist, s, &s);
	if (err != ERR_ok) resignal(err);
	ans->str = s.str;
	signal(ERR_ok);
	}

extern errcode gcd_tabOPinsert();

errcode stringOP_gcd(s, tab, ans)
CLUREF s, tab, *ans;
{
errcode err;
    CLUREF ctstr; ctstr.num = CT_STRING;
	err = gcd_tabOPinsert(tab, s, ctstr, ans);
	if (err != ERR_ok) resignal(err);
	signal(ERR_ok);
	}

typedef struct {
long count;
    OWNPTR type_owns;
    OWNPTR op_owns;
struct OP_ENTRY entry[27];
} string_OPS;

OWN_ptr string_own_init = {1, 0};

CLU_proc string_oe_size = {{0,0,0,0}, stringOPsize, &string_own_init, &string_own_init};
CLU_proc string_oe_indexs = {{0,0,0,0}, stringOPindexs, &string_own_init, &string_own_init};
CLU_proc string_oe_indexc = {{0,0,0,0}, stringOPindexc, &string_own_init, &string_own_init};
CLU_proc string_oe_c2s = {{0,0,0,0}, stringOPc2s, &string_own_init, &string_own_init};
CLU_proc string_oe_concat = {{0,0,0,0}, stringOPconcat, &string_own_init, &string_own_init};
CLU_proc string_oe_append = {{0,0,0,0}, stringOPappend, &string_own_init, &string_own_init};
CLU_proc string_oe_empty = {{0,0,0,0}, stringOPempty, &string_own_init, &string_own_init};
CLU_proc string_oe_substr = {{0,0,0,0}, stringOPsubstr, &string_own_init, &string_own_init};
CLU_proc string_oe_rest = {{0,0,0,0}, stringOPrest, &string_own_init, &string_own_init};
CLU_proc string_oe_s2ac = {{0,0,0,0}, stringOPs2ac, &string_own_init, &string_own_init};
CLU_proc string_oe_ac2s = {{0,0,0,0}, stringOPac2s, &string_own_init, &string_own_init};
CLU_proc string_oe_s2sc = {{0,0,0,0}, stringOPs2sc, &string_own_init, &string_own_init};
CLU_proc string_oe_sc2s = {{0,0,0,0}, stringOPsc2s, &string_own_init, &string_own_init};
CLU_proc string_oe_chars = {{0,0,0,0}, stringOPchars, &string_own_init, &string_own_init};
CLU_proc string_oe_lt = {{0,0,0,0}, stringOPlt, &string_own_init, &string_own_init};
CLU_proc string_oe_le = {{0,0,0,0}, stringOPle, &string_own_init, &string_own_init};
CLU_proc string_oe_ge = {{0,0,0,0}, stringOPge, &string_own_init, &string_own_init};
CLU_proc string_oe_gt = {{0,0,0,0}, stringOPgt, &string_own_init, &string_own_init};
CLU_proc string_oe_fetch = {{0,0,0,0}, stringOPfetch, &string_own_init, &string_own_init};
CLU_proc string_oe_equal = {{0,0,0,0}, stringOPequal, &string_own_init, &string_own_init};
CLU_proc string_oe_similar = {{0,0,0,0}, stringOPsimilar, &string_own_init, &string_own_init};
CLU_proc string_oe_copy = {{0,0,0,0}, stringOPcopy, &string_own_init, &string_own_init};
CLU_proc string_oe_print = {{0,0,0,0}, stringOPprint, &string_own_init, &string_own_init};
CLU_proc string_oe_encode = {{0,0,0,0}, stringOPencode, &string_own_init, &string_own_init};
CLU_proc string_oe_decode = {{0,0,0,0}, stringOPdecode, &string_own_init, &string_own_init};
CLU_proc string_oe__gcd = {{0,0,0,0}, stringOP_gcd, &string_own_init, &string_own_init};
CLU_proc string_oe_debug_print = {{0,0,0,0}, stringOPdebug_print, &string_own_init, &string_own_init};

string_OPS string_ops_actual = {27,
    &string_own_init,
    &string_own_init, {
{&string_oe_copy, "copy"},
{&string_oe_equal, "equal"},
{&string_oe_similar, "similar"},
{&string_oe_debug_print, "debug_print"},
{&string_oe_size, "size"},
{&string_oe_indexs, "indexs"},
{&string_oe_indexc, "indexc"},
{&string_oe_c2s, "c2s"},
{&string_oe_concat, "concat"},
{&string_oe_append, "append"},
{&string_oe_empty, "empty"},
{&string_oe_substr, "substr"},
{&string_oe_rest, "rest"},
{&string_oe_s2ac, "s2ac"},
{&string_oe_ac2s, "ac2s"},
{&string_oe_s2sc, "s2sc"},
{&string_oe_sc2s, "sc2s"},
{&string_oe_chars, "chars"},
{&string_oe_lt, "lt"},
{&string_oe_le, "le"},
{&string_oe_ge, "ge"},
{&string_oe_gt, "gt"},
{&string_oe_fetch, "fetch"},
{&string_oe_print, "print"},
{&string_oe_encode, "encode"},
{&string_oe_decode, "decode"},
{&string_oe__gcd, "_gcd"}}
};

string_OPS *string_ops = &string_ops_actual;
