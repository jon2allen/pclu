
/* Copyright Massachusetts Institute of Technology 1990,1991 */

#ifndef lint
static char rcsid[] = "$Header: _wordvec.c,v 1.6 91/06/06 13:52:31 dcurtis Exp $";
#endif

#include <stdio.h>
#include "pclu_err.h"
#include "pclu_sys.h"

/* Prototypes for internal safety */
errcode _wordvecOPcreate(CLUREF sz, CLUREF *ans);
errcode _wordvecOPcopy(CLUREF wv1, CLUREF *wv2);
errcode _wordvecOPfetch(CLUREF wv, CLUREF i, CLUREF *ans);
errcode _wordvecOPstore(CLUREF wv, CLUREF i, CLUREF w);
errcode _wordvecOPfetch2(CLUREF wv, CLUREF i, CLUREF *ans1, CLUREF *ans2);
errcode _wordvecOPstore2(CLUREF wv, CLUREF i, CLUREF l, CLUREF r);
errcode _wordvecOPxstore(CLUREF wv, CLUREF i, CLUREF b, CLUREF l);
errcode _wordvecOPbfetch(CLUREF wv, CLUREF i, CLUREF *ans);
errcode _wordvecOPbstore(CLUREF wv, CLUREF i, CLUREF c);
errcode _wordvecOPwfetch(CLUREF wv, CLUREF i, CLUREF *ans);
errcode _wordvecOPwstore(CLUREF wv, CLUREF i, CLUREF n);
errcode _wordvecOPlfetch(CLUREF wv, CLUREF i, CLUREF *ans);
errcode _wordvecOPlstore(CLUREF wv, CLUREF i, CLUREF n);
errcode _wordvecOPget_byte(CLUREF wv, CLUREF wrd, CLUREF bit, CLUREF len, CLUREF *ans);
errcode _wordvecOPset_byte(CLUREF wv, CLUREF byte, CLUREF wrd, CLUREF bit, CLUREF len);
errcode _wordvecOPsize(CLUREF wv, CLUREF *ans);
errcode _wordvecOPequal(CLUREF wv1, CLUREF wv2, CLUREF *ans);
errcode _wordvecOPindexc(CLUREF c, CLUREF wv, CLUREF first, CLUREF *ans);
errcode _wordvecOPindexv(CLUREF wv1, CLUREF wv2, CLUREF first, CLUREF *ans);
errcode _wordvecOPnc_indexv(CLUREF wv1, CLUREF wv2, CLUREF first, CLUREF *ans);
errcode _wordvecOPmove_lr(CLUREF wv1, CLUREF s1, CLUREF wv2, CLUREF s2, CLUREF len);
errcode _wordvecOPmove_v(CLUREF qc1, CLUREF s1, CLUREF wv2, CLUREF s2, CLUREF len);
errcode _wordvecOPmove_rl(CLUREF w1, CLUREF s1, CLUREF w2, CLUREF s2, CLUREF len);
errcode _wordvecOPbyte_size(CLUREF *ans);
errcode _wordvecOPword_size(CLUREF *ans);
errcode _wordvecOPbytes_per_word(CLUREF *ans);
errcode _wordvecOPmove_b2w(CLUREF b, CLUREF bpos, CLUREF w, CLUREF wpos, CLUREF n);
errcode _wordvecOPmove_w2b(CLUREF w, CLUREF wpos, CLUREF b, CLUREF bpos, CLUREF n);
errcode _wordvecOP_gcd(CLUREF w, CLUREF tab, CLUREF *ans);
errcode _wordvecOPprint(CLUREF w, CLUREF pst);
errcode _wordvecOPdebug_print(CLUREF w, CLUREF pst);

errcode _wordvecOPcreate(CLUREF sz, CLUREF *ans)
{
    CLUREF temp;
    int size = (int)sz.num * 8;
    int rounded_size;

    if (size > MAX_STR) signal(ERR_toobig);
    rounded_size = ((size + 7) / 8) * 8 + sizeof(CLU_string);
    clu_alloc(rounded_size, &temp); bzero(temp.str, rounded_size);
    temp.str->size = size;
    temp.str->typ.val = CT_STRING;
    temp.str->typ.mark = 0;
    temp.str->typ.refp = 0;

    ans->str = temp.str;
    signal(ERR_ok);
}

errcode _wordvecOPcopy(CLUREF wv1, CLUREF *wv2)
{
    CLUREF temp;
    int i;
    int sz = (int)wv1.str->size;
    int rounded_size = (sz + 7) / 8 * 8 + sizeof(CLU_string);
    clu_alloc(rounded_size, &temp); bzero(temp.str, rounded_size);
    temp.str->size = sz;
    temp.str->typ.val = CT_STRING;
    temp.str->typ.mark = 0;
    temp.str->typ.refp = 0;

    for (i = 0; i < sz; i++) {
        temp.str->data[i] = wv1.str->data[i];
    }
    if (i < (sz + 7) / 8 * 8) temp.str->data[i] = '\000';
    
    wv2->str = temp.str;
    signal(ERR_ok);
}

errcode _wordvecOPfetch(CLUREF wv, CLUREF i, CLUREF *ans)
{
    if (i.num < 1 || (i.num - 1) * 8 >= (long)wv.str->size) signal(ERR_bounds);
    ans->num = wv.vec->data[i.num - 1];
    signal(ERR_ok);
}

errcode _wordvecOPstore(CLUREF wv, CLUREF i, CLUREF w)
{
    if (i.num < 1 || (i.num - 1) * 8 >= (long)wv.str->size) signal(ERR_bounds);
    wv.vec->data[i.num - 1] = w.num;
    signal(ERR_ok);
}

errcode _wordvecOPfetch2(CLUREF wv, CLUREF i, CLUREF *ans1, CLUREF *ans2)
{
    unsigned long temp;
    if (i.num < 1 || (i.num - 1) * 8 >= (long)wv.str->size) signal(ERR_bounds);
    temp = (unsigned long)wv.vec->data[i.num - 1];
    ans1->num = (long)((temp >> 32) & 0xffffffffUL);
    ans2->num = (long)(temp & 0xffffffffUL);
    signal(ERR_ok);
}

errcode _wordvecOPstore2(CLUREF wv, CLUREF i, CLUREF l, CLUREF r)
{
    if (i.num < 1 || (i.num - 1) * 8 >= (long)wv.str->size) signal(ERR_bounds);
    wv.vec->data[i.num - 1] = ((unsigned long)r.num & 0xffffffffUL) | (((unsigned long)l.num & 0xffffffffUL) << 32);
    signal(ERR_ok);
}

errcode _wordvecOPxstore(CLUREF wv, CLUREF i, CLUREF b, CLUREF l)
{
    unsigned long temp;
    if (i.num < 1 || (i.num - 1) * 8 >= (long)wv.str->size) signal(ERR_bounds);
    temp = (unsigned long)wv.vec->data[i.num - 1];
    temp = (temp & ~0xffffffffUL) | ((l.num & 0xffffffUL) | ((unsigned long)(b.num & 0xff) << 24));
    wv.vec->data[i.num - 1] = (long)temp;
    signal(ERR_ok);
}

errcode _wordvecOPbfetch(CLUREF wv, CLUREF i, CLUREF *ans)
{
    int bi = (int)i.num - 1;
    int wi, sub;
    unsigned long temp;
    if (bi < 0 || bi >= (int)wv.str->size) signal(ERR_bounds);
    wi = bi / 8;
    sub = bi % 8;
    temp = (unsigned long)wv.vec->data[wi];
    ans->num = (long)((temp >> (sub * 8)) & 0xffUL);
    signal(ERR_ok);
}

errcode _wordvecOPbstore(CLUREF wv, CLUREF i, CLUREF c)
{
    int bi = (int)i.num - 1;
    int wi, sub;
    unsigned long temp, mask;
    if (bi < 0 || bi >= (int)wv.str->size) signal(ERR_bounds);
    wi = bi / 8;
    sub = bi % 8;
    temp = (unsigned long)wv.vec->data[wi];
    mask = ~(0xffUL << (sub * 8));
    temp = (temp & mask) | (((unsigned long)c.num & 0xffUL) << (sub * 8));
    wv.vec->data[wi] = (long)temp;
    signal(ERR_ok);
}

errcode _wordvecOPwfetch(CLUREF wv, CLUREF i, CLUREF *ans)
{
    int bi = (int)i.num - 1;
    int wi, sub;
    unsigned long temp, temp2;
    if (bi < 0 || bi > (int)wv.str->size - 2) signal(ERR_bounds);
    wi = bi / 8;
    sub = bi % 8;
    temp = (unsigned long)wv.vec->data[wi];
    if (sub <= 6) {
        ans->num = (long)((temp >> (sub * 8)) & 0xffffUL);
    } else {
        temp = (temp >> 56) & 0xff;
        temp2 = (unsigned long)wv.vec->data[wi + 1];
        ans->num = (long)(temp | ((temp2 & 0xff) << 8));
    }
    signal(ERR_ok);
}

errcode _wordvecOPwstore(CLUREF wv, CLUREF i, CLUREF n)
{
    int bi = (int)i.num - 1;
    int wi, sub;
    unsigned long temp, temp2, mask;
    if (bi < 0 || bi > (int)wv.str->size - 2) signal(ERR_bounds);
    wi = bi / 8;
    sub = bi % 8;
    temp = (unsigned long)wv.vec->data[wi];
    if (sub <= 6) {
        mask = ~(0xffffUL << (sub * 8));
        temp = (temp & mask) | (((unsigned long)n.num & 0xffffUL) << (sub * 8));
        wv.vec->data[wi] = (long)temp;
    } else {
        temp = (temp & 0x00ffffffffffffffUL) | (((unsigned long)n.num & 0xffUL) << 56);
        wv.vec->data[wi] = (long)temp;
        temp2 = (unsigned long)wv.vec->data[wi + 1];
        temp2 = (temp2 & ~0xffUL) | (((unsigned long)n.num >> 8) & 0xffUL);
        wv.vec->data[wi + 1] = (long)temp2;
    }
    signal(ERR_ok);
}

errcode _wordvecOPlfetch(CLUREF wv, CLUREF i, CLUREF *ans)
{
    int bi = (int)i.num - 1;
    int wi, sub;
    unsigned long temp, temp2;
    if (bi < 0 || bi > (int)wv.str->size - 4) signal(ERR_bounds);
    wi = bi / 8;
    sub = bi % 8;
    temp = (unsigned long)wv.vec->data[wi];
    if (sub <= 4) {
        ans->num = (long)((temp >> (sub * 8)) & 0xffffffffUL);
    } else {
        int bits_in_first = (8 - sub) * 8;
        temp = (temp >> (sub * 8));
        temp2 = (unsigned long)wv.vec->data[wi + 1];
        ans->num = (long)(temp | ((temp2 & ((1UL << (32 - bits_in_first)) - 1)) << bits_in_first));
    }
    signal(ERR_ok);
}

errcode _wordvecOPlstore(CLUREF wv, CLUREF i, CLUREF n)
{
    int bi = (int)i.num - 1;
    int wi, sub;
    unsigned long temp, temp2, mask;
    if (bi < 0 || bi > (int)wv.str->size - 4) signal(ERR_bounds);
    wi = bi / 8;
    sub = bi % 8;
    temp = (unsigned long)wv.vec->data[wi];
    if (sub <= 4) {
        mask = ~(0xffffffffUL << (sub * 8));
        temp = (temp & mask) | (((unsigned long)n.num & 0xffffffffUL) << (sub * 8));
        wv.vec->data[wi] = (long)temp;
    } else {
        int bits_in_first = (8 - sub) * 8;
        unsigned long val = (unsigned long)n.num & 0xffffffffUL;
        mask = (1UL << (sub * 8)) - 1;
        temp = (temp & mask) | (val << (sub * 8));
        wv.vec->data[wi] = (long)temp;
        temp2 = (unsigned long)wv.vec->data[wi + 1];
        mask = ~((1UL << (32 - bits_in_first)) - 1);
        temp2 = (temp2 & mask) | (val >> bits_in_first);
        wv.vec->data[wi + 1] = (long)temp2;
    }
    signal(ERR_ok);
}

errcode _wordvecOPsize(CLUREF wv, CLUREF *ans)
{
    ans->num = (long)wv.str->size / 8;
    signal(ERR_ok);
}

errcode _wordvecOPequal(CLUREF wv1, CLUREF wv2, CLUREF *ans)
{
    ans->tf = (wv1.str == wv2.str);
    signal(ERR_ok);
}

errcode _wordvecOPindexc(CLUREF c, CLUREF wv, CLUREF first, CLUREF *ans)
{
    int i;
    if (first.num < 1) signal(ERR_bounds);
    for (i = (int)first.num; i <= (int)wv.str->size; i++) {
        if (wv.str->data[i-1] == c.ch) {
            ans->num = (long)i;
            signal(ERR_ok);
        }
    }
    ans->num = 0;
    signal(ERR_ok);
}

errcode _wordvecOPindexv(CLUREF wv1, CLUREF wv2, CLUREF first, CLUREF *ans)
{
    int i, j;
    int sz1 = (int)wv1.str->size;
    int sz2 = (int)wv2.str->size;
    if (first.num < 1) signal(ERR_bounds);
    for (i = (int)first.num; i + sz1 - 1 <= sz2; i++) {
        bool found = true;
        for (j = 0; j < sz1; j++) {
            if (wv2.str->data[i + j - 1] != wv1.str->data[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            ans->num = (long)i;
            signal(ERR_ok);
        }
    }
    ans->num = 0;
    signal(ERR_ok);
}

errcode _wordvecOPnc_indexv(CLUREF wv1, CLUREF wv2, CLUREF first, CLUREF *ans)
{
    int i, j;
    int sz1 = (int)wv1.str->size;
    int sz2 = (int)wv2.str->size;
    if (first.num < 1) signal(ERR_bounds);
    for (i = (int)first.num; i + sz1 - 1 <= sz2; i++) {
        bool found = true;
        for (j = 0; j < sz1; j++) {
            char c1 = wv1.str->data[j];
            char c2 = wv2.str->data[i + j - 1];
            if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
            if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';
            if (c1 != c2) {
                found = false;
                break;
            }
        }
        if (found) {
            ans->num = (long)i;
            signal(ERR_ok);
        }
    }
    ans->num = 0;
    signal(ERR_ok);
}

errcode _wordvecOPmove_lr(CLUREF wv1, CLUREF s1, CLUREF wv2, CLUREF s2, CLUREF len)
{
    int i, count = (int)len.num;
    if (count < 0) signal(ERR_negative_size);
    if (s1.num < 1 || s2.num < 1) signal(ERR_bounds);
    if (s1.num + count - 1 > (long)wv1.str->size || s2.num + count - 1 > (long)wv2.str->size) signal(ERR_bounds);
    for (i = 0; i < count; i++) {
        wv2.str->data[s2.num - 1 + i] = wv1.str->data[s1.num - 1 + i];
    }
    signal(ERR_ok);
}

errcode _wordvecOPmove_v(CLUREF qc1, CLUREF s1, CLUREF wv2, CLUREF s2, CLUREF len)
{
    int i, count = (int)len.num;
    if (count < 0) signal(ERR_negative_size);
    if (s1.num < 1 || s2.num < 1) signal(ERR_bounds);
    if (s1.num + count - 1 > (long)qc1.vec->size || s2.num + count - 1 > (long)wv2.str->size) signal(ERR_bounds);
    for (i = 0; i < count; i++) {
        wv2.str->data[s2.num - 1 + i] = (char)qc1.vec->data[s1.num - 1 + i];
    }
    signal(ERR_ok);
}

errcode _wordvecOPmove_rl(CLUREF w1, CLUREF s1, CLUREF w2, CLUREF s2, CLUREF len)
{
    int i, count = (int)len.num;
    if (count < 0) signal(ERR_negative_size);
    if (s1.num < 1 || s2.num < 1) signal(ERR_bounds);
    if (s1.num + count - 1 > (long)w1.str->size || s2.num + count - 1 > (long)w2.str->size) signal(ERR_bounds);
    for (i = count - 1; i >= 0; i--) {
        w2.str->data[s2.num - 1 + i] = w1.str->data[s1.num - 1 + i];
    }
    signal(ERR_ok);
}

errcode _wordvecOPget_byte(CLUREF wv, CLUREF wrd, CLUREF bit, CLUREF len, CLUREF *ans)
{
    unsigned long result, mask;
    if (len.num <= 0 || len.num > 64) signal(ERR_illegal_size);
    if (wrd.num < 1 || (wrd.num - 1) * 8 >= (long)wv.str->size) signal(ERR_bounds);
    if (len.num > bit.num || bit.num > 64) signal(ERR_bounds);
    
    result = (unsigned long)wv.vec->data[wrd.num - 1];
    
    result >>= (64 - (long)bit.num);
    if (len.num == 64) mask = ~0UL;
    else mask = (1UL << (long)len.num) - 1;
    
    ans->num = (long)(result & mask);
    signal(ERR_ok);
}

errcode _wordvecOPset_byte(CLUREF wv, CLUREF byte, CLUREF wrd, CLUREF bit, CLUREF len)
{
    unsigned long slot, val, mask;
    if (len.num <= 0 || len.num > 64) signal(ERR_illegal_size);
    if (wrd.num < 1 || (wrd.num - 1) * 8 >= (long)wv.str->size) signal(ERR_bounds);
    if (len.num > bit.num || bit.num > 64) signal(ERR_bounds);

    if (len.num == 64) mask = ~0UL;
    else mask = (1UL << (long)len.num) - 1;
    
    unsigned long val_bits = (unsigned long)byte.num & mask;
    val = val_bits << (64 - (long)bit.num);
    mask <<= (64 - (long)bit.num);
    
    slot = (unsigned long)wv.vec->data[wrd.num - 1];
    slot = (slot & ~mask) | val;
    wv.vec->data[wrd.num - 1] = (long)slot;
    
    signal(ERR_ok);
}

errcode _wordvecOPbyte_size(CLUREF *ans) { ans->num = 8; signal(ERR_ok); }
errcode _wordvecOPword_size(CLUREF *ans) { ans->num = 64; signal(ERR_ok); }
errcode _wordvecOPbytes_per_word(CLUREF *ans) { ans->num = 8; signal(ERR_ok); }

errcode _wordvecOPmove_b2w(CLUREF b, CLUREF bpos, CLUREF w, CLUREF wpos, CLUREF n)
{
    int i, count = (int)n.num;
    if (count < 0) signal(ERR_negative_size);
    if (count == 0) signal(ERR_ok);
    if (bpos.num < 1 || wpos.num < 1) signal(ERR_bounds);
    if (bpos.num + count - 1 > (long)b.str->size || wpos.num + count - 1 > (long)w.str->size) signal(ERR_bounds);
    for (i = 0; i < count; i++) {
        w.str->data[wpos.num - 1 + i] = b.str->data[bpos.num - 1 + i];
    }
    signal(ERR_ok);
}

errcode _wordvecOPmove_w2b(CLUREF w, CLUREF wpos, CLUREF b, CLUREF bpos, CLUREF n)
{
    int i, count = (int)n.num;
    if (count < 0) signal(ERR_negative_size);
    if (count == 0) signal(ERR_ok);
    if (wpos.num < 1 || bpos.num < 1) signal(ERR_bounds);
    if (wpos.num + count - 1 > (long)w.str->size || bpos.num + count - 1 > (long)b.str->size) signal(ERR_bounds);
    for (i = 0; i < count; i++) {
        b.str->data[bpos.num - 1 + i] = w.str->data[wpos.num - 1 + i];
    }
    signal(ERR_ok);
}

errcode _wordvecOP_gcd(CLUREF w, CLUREF tab, CLUREF *ans)
{
    errcode err;
    CLUREF temp_oneof, sz;
    err = oneofOPnew(CLU_1, CLU_0, &temp_oneof);
    if (err != ERR_ok) resignal(err);
    sz.num = (long)(2 * CLUREFSZ + ((long)w.str->size + CLUREFSZ) & ~(CLUREFSZ - 1));
    err = gcd_tabOPinsert(tab, sz, temp_oneof, w, ans);
    if (err != ERR_ok) resignal(err);
    signal(ERR_ok);
}

errcode _wordvecOPprint(CLUREF w, CLUREF pst) { return stringOPprint(w, pst); }
errcode _wordvecOPdebug_print(CLUREF w, CLUREF pst) { return stringOPprint(w, pst); }

typedef struct {
    int count;
    OWNPTR type_owns;
    OWNPTR op_owns;
    struct OP_ENTRY entry[13];
} _wordvec_OPS;

CLU_proc _wordvec_oe_create = {{0,0,0,0}, (errcode (*)())_wordvecOPcreate, 0};
CLU_proc _wordvec_oe_copy = {{0,0,0,0}, (errcode (*)())_wordvecOPcopy, 0};
CLU_proc _wordvec_oe_fetch = {{0,0,0,0}, (errcode (*)())_wordvecOPfetch, 0};
CLU_proc _wordvec_oe_store = {{0,0,0,0}, (errcode (*)())_wordvecOPstore, 0};
CLU_proc _wordvec_oe_size = {{0,0,0,0}, (errcode (*)())_wordvecOPsize, 0};
CLU_proc _wordvec_oe_equal = {{0,0,0,0}, (errcode (*)())_wordvecOPequal, 0};
CLU_proc _wordvec_oe_indexc = {{0,0,0,0}, (errcode (*)())_wordvecOPindexc, 0};
CLU_proc _wordvec_oe_indexv = {{0,0,0,0}, (errcode (*)())_wordvecOPindexv, 0};
CLU_proc _wordvec_oe_nc_indexv = {{0,0,0,0}, (errcode (*)())_wordvecOPnc_indexv, 0};
CLU_proc _wordvec_oe_move_lr = {{0,0,0,0}, (errcode (*)())_wordvecOPmove_lr, 0};
CLU_proc _wordvec_oe_move_v = {{0,0,0,0}, (errcode (*)())_wordvecOPmove_v, 0};
CLU_proc _wordvec_oe_print = {{0,0,0,0}, (errcode (*)())_wordvecOPprint, 0};
CLU_proc _wordvec_oe_debug_print = {{0,0,0,0}, (errcode (*)())_wordvecOPdebug_print, 0};

OWN_ptr _wordvec_own_init = {1,0};
_wordvec_OPS _wordvec_ops_actual = {13, &_wordvec_own_init, &_wordvec_own_init, {
    {&_wordvec_oe_create, "create"}, {&_wordvec_oe_copy, "copy"},
    {&_wordvec_oe_fetch, "fetch"}, {&_wordvec_oe_store, "store"},
    {&_wordvec_oe_size, "size"}, {&_wordvec_oe_equal, "equal"},
    {&_wordvec_oe_indexc, "indexc"}, {&_wordvec_oe_indexv, "indexv"},
    {&_wordvec_oe_nc_indexv, "nc_indexv"}, {&_wordvec_oe_move_lr, "move_lr"},
    {&_wordvec_oe_move_v, "move_v"}, {&_wordvec_oe_print, "print"},
    {&_wordvec_oe_debug_print, "debug_print"}
}};
_wordvec_OPS *_wordvec_ops = &_wordvec_ops_actual;
