
/* Copyright Massachusetts Institute of Technology 1990,1991 */

#ifndef lint
static char rcsid[] = "$Header: _free_space.c,v 1.2 91/06/06 13:45:33 dcurtis Exp $";
#endif
/* $Log:	_free_space.c,v $
 * Revision 1.2  91/06/06  13:45:33  dcurtis
 * added copyright notice
 * 
 * Revision 1.1  91/02/04  23:20:26  mtv
 * Initial revision
 * 
 */


/*							*/
/*							*/
/*		IMPLEMENTATION OF			*/
/*			_free_space			*/
/*							*/
/*		returns the number of machine words	*/
/*		that remain available for future	*/
/*		allocations from the heap		*/

#include "pclu_err.h"
#include "pclu_sys.h"

#if !defined(LINUX) && !defined(FREEBSD)
extern int maxheapsize;
extern int composite_in_use;

errcode _free_space(ans)
CLUREF *ans;
{
	ans->num = blks_hard_limit*BYTES_TO_WORDS(HBLKSIZE) - composite_in_use;
	signal(ERR_ok);
	}

#else
/* Boehm GC 8.x modern API */
errcode _free_space(ans)
CLUREF *ans;
{
	struct GC_prof_stats_s stats;
	GC_get_prof_stats(&stats, sizeof(stats));
	/* free_bytes_full includes both free bytes and unmapped bytes */
	ans->num = BYTES_TO_WORDS(stats.free_bytes_full);
	signal(ERR_ok);
}
#endif
