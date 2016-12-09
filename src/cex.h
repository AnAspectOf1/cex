/*	C Exception (or Cex in short) is a C 'library' that can be used to implement exceptions in C similarily to other programming languages, consisting of one header file.
 *
 *	This header file is not licensend. The author(s) of this file consider(s) this file to be of the public domain. Use it however it pleases you.
 *	Credits to:
 *		An-Aspect-of-One (2016)
 */
#ifndef _CEX_H
#define _CEX_H

#include <setjmp.h>
#include <stdlib.h>
#ifndef NDEBUG
#include <stdio.h>
#endif


typedef unsigned int cex_t;
typedef void (*cex_free_t)(void*);

typedef struct cex_exc {
	cex_t code;
	void* data;
	cex_free_t free;
} cex_exc_t;
#define CEX_EXC_INIT_DEF \
	{ .code = 0, .data = 0, .free = free }

struct _cex_exc_ptr {
	cex_exc_t* exc;
	unsigned char managed;
};

struct _cex_exc_ctx {
	void* prev;
	jmp_buf jmp;
	int mode;
};

enum _cex_mode {
	_cex_mode_begin = 0,	/* Run for the first time. */
	_cex_mode_try,
	_cex_mode_raise,	/* An exception has been raised. */
	_cex_mode_catch	/* Ready to catch the exception. */
};

extern struct _cex_exc_ctx* _cex_cur_ctx_raise;	/* Points to the current context to raise to. */
extern struct _cex_exc_ctx* _cex_cur_ctx_stat;	/* Points to the context the current try-catch statement is in. */
extern struct _cex_exc_ptr _cex_exc_ptr;
#define CEX (_cex_exc_ptr.exc)

int _cex_new_ctx();
int _cex_worker();
void _cex_raise( cex_t, void*, cex_free_t );
void _cex_raise_exc( cex_exc_t* exc );

#ifndef NDEBUG
#define CEX_ASSERT( COND, MSG ) \
	{ if ( COND ) { fprintf( stderr, "Assertion: %s.\n", MSG ); exit(1); } }
#endif

#define CEX_TRY \
	if ( _cex_new_ctx() && (_cex_cur_ctx_stat->mode = setjmp( _cex_cur_ctx_stat->jmp )) >= 0 ) \
	while ( _cex_worker() ) \
	if ( _cex_cur_ctx_stat->mode == _cex_mode_try )

#define CEX_CATCH \
	else if ( _cex_cur_ctx_stat->mode == _cex_mode_catch )

#define CEX_RAISE( EXC ) \
	_cex_raise_exc( &(EXC) )
#define CEX_RAISEF( CODE, DATA, FREE ) \
	_cex_raise( CODE, DATA, FREE )
#define CEX_RAISEC( CODE ) \
	_cex_raise( CODE, 0, free )
#define CEX_RAISED( CODE, DATA ) \
	_cex_raise( CODE, DATA, free )
#define CEX_RAISEM( CODE, MSG ) \
	_cex_raise_msg( CODE, MSG )
#define CEX_RERAISE \
	_cex_reraise()


/* All keywords */
#define CATCH CEX_CATCH
#define RAISE( EXC, FREE ) CEX_RAISE( EXC, FREE )
#define RAISEC( CODE ) CEX_RAISEC( CODE )
#define RAISED( CODE, DATA ) CEX_RAISED( CODE, DATA )
#define RAISEF( CODE, DATA, FREE ) CEX_RAISEF( CODE, DATA, FREE )
#define RERAISE CEX_RERAISE
#define TRY CEX_TRY


#endif//_CEX_H
