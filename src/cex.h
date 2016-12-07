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


typedef unsigned int cex_t;

typedef struct cex_exc {
	cex_t code;
	void* data;
} cex_exc_t;

struct _cex_exc {
	cex_exc_t exc;
	void (*free)(void*);
};

struct _cex_exc_buf {
	void* prev;
	jmp_buf jmp;
	struct _cex_exc* exc;
};

extern struct _cex_exc_buf* _cex_cur_buf;

#define CEX_TRY { \
	struct _cex_exc_buf __cex_buf = { .prev =  _cex_cur_buf, .exc = 0 }; \
	_cex_cur_buf = &__cex_buf; \
	if ( !setjmp( __cex_buf.jmp ) ) {
#define _CEX_CATCH \
		_cex_cur_buf = (struct _cex_exc_buf*)_cex_cur_buf->prev; \
	} else { \
		_cex_cur_buf = (struct _cex_exc_buf*)_cex_cur_buf->prev;
#define CEX_CATCH \
	_CEX_CATCH \
	const cex_exc_t* e = &__cex_buf.exc->exc; \
	void (*e_free)(void*) = __cex_buf.exc->free;
#define CEX_CATCHN( NAME ) \
	_CEX_CATCH \
	const cex_exc_t* NAME = &__cex_buf.exc->exc; \
	void (*NAME##_free)(void*) = __cex_buf.exc->free;
#define CEX_FINISH } \
	if ( __cex_buf.exc != 0 ) \
		__cex_buf.exc->free( __cex_buf.exc->exc.data ); } \

#define _CEX_REL_BUF { \
	struct _cex_exc_buf* old = _cex_cur_buf; \
	_cex_cur_buf = (struct _cex_exc_buf*)_cex_cur_buf->prev; \
	if ( old->exc != 0 ) \
		old->exc->free( old->exc->exc.data ); }

#define CEX_RAISE( EXC, FREE ) \
	_cex_cur_buf->exc = malloc( sizeof( cex_exc_t ) ); \
	_cex_cur_buf->exc->exc = EXC; \
	_cex_cur_buf->exc->free = FREE; \
	longjmp( _cex_cur_buf->jmp, 1 );
#define CEX_RAISEC( CODE ) \
	_cex_cur_buf->exc = malloc( sizeof( cex_exc_t ) ); \
	_cex_cur_buf->exc->exc.code = CODE; \
	_cex_cur_buf->exc->exc.data = 0; \
	_cex_cur_buf->exc->free = free; \
	longjmp( _cex_cur_buf->jmp, 1 );
#define CEX_RAISED( CODE, DATA ) \
	_cex_cur_buf->exc = malloc( sizeof( cex_exc_t ) ); \
	_cex_cur_buf->exc->exc.code = CODE; \
	_cex_cur_buf->exc->exc.data = DATA; \
	_cex_cur_buf->exc->free = free; \
	longjmp( _cex_cur_buf->jmp, 1 );
#define CEX_RERAISE \
	_cex_cur_buf->exc = __cex_buf.exc; \
	longjmp( _cex_cur_buf->jmp, 1 );


#define CATCH CEX_CATCH
#define CATCHN( NAME ) CEX_CATCHN( NAME )
#define FINISH CEX_FINISH
#define RAISE( EXC, FREE ) CEX_RAISE( EXC, FREE )
#define RAISEC( CODE ) CEX_RAISEC( CODE )
#define RAISED( CODE, DATA ) CEX_RAISED( CODE, DATA )
#define RERAISE CEX_RERAISE
#define TRY CEX_TRY

#endif//_CEX_H
