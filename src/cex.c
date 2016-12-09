#include "cex.h"

struct _cex_exc_ctx* _cex_cur_ctx_raise = 0;
struct _cex_exc_ctx* _cex_cur_ctx_stat = 0;
cex_exc_t cex_exc_def = { .code = 0, .data = 0, .free = free };
cex_exc_t cex_exc_oom = { .code = 1, .data = 0, .free = free };
struct _cex_exc_ptr _cex_exc_ptr = { .exc = 0, .managed = 0 };
cex_exc_t* cex = 0;


void cex_raise_jump();


void cex_raise_jump() {
	/* Release the context of the current try-catch statements' context. */
	while ( _cex_cur_ctx_stat != _cex_cur_ctx_raise ) {	/* This should only happen once. */
		struct _cex_exc_ctx* prev = _cex_cur_ctx_stat->prev;
		free( _cex_cur_ctx_stat );
		_cex_cur_ctx_stat = prev;
	}

	longjmp( _cex_cur_ctx_raise->jmp, _cex_mode_raise );
}

void _cex_raise( cex_t code, void* data, cex_free_t free ) {
	CEX_ASSERT( _cex_cur_ctx_raise == 0, "Can not raise outside any try block." );

	/* Free current exception. */
	if ( _cex_exc_ptr.exc != 0 )
		_cex_exc_ptr.exc->free( _cex_exc_ptr.exc->data );
	
	/* Make sure we have some managed space for our exception. */
	if ( !_cex_exc_ptr.managed ) {
	
		_cex_exc_ptr.exc = malloc( sizeof( struct _cex_exc_ptr ) );
		if ( _cex_exc_ptr.exc == 0 ) {
			_cex_exc_ptr.exc = &cex_exc_oom;	/* If the exception could not be allocated, point to the out-of-memory exception and pretend that that exception has been raised. */
			_cex_exc_ptr.managed = 0;
		}
		else
			_cex_exc_ptr.managed = 1;
	}

	CEX->code = code;
	CEX->data = data;
	CEX->free = free;
	
	cex_raise_jump();
}

void _cex_raise_exc( cex_exc_t* exc ) {
	CEX_ASSERT( _cex_cur_ctx_raise == 0, "Can not raise outside any try block." );
	
	if ( _cex_exc_ptr.exc != exc ) {
	
		/* Free current exception. */
		if ( _cex_exc_ptr.exc != 0 )
			_cex_exc_ptr.exc->free( _cex_exc_ptr.exc->data );

		_cex_exc_ptr.exc = exc;
		_cex_exc_ptr.managed = 0;
	}

	cex_raise_jump();
}

void _cex_reraise() {
	CEX_ASSERT( _cex_cur_ctx_raise == 0, "Can not raise outside any try block." );

	cex_raise_jump();
}

/* Creates a new subcontext and returns true. */
int _cex_new_ctx() {
	struct _cex_exc_ctx* new_ctx = malloc( sizeof( struct _cex_exc_ctx ) );
	CEX_ASSERT( new_ctx == 0, "No memory left to create new try-catch context" );

	new_ctx->prev = _cex_cur_ctx_raise;
	new_ctx->mode = _cex_mode_begin;

	_cex_cur_ctx_raise = new_ctx;
	_cex_cur_ctx_stat = new_ctx;

	return 1;
}

int _cex_worker() {printf( "jallos %d\n", _cex_cur_ctx_stat->mode );
	/* The beginning */
	if ( _cex_cur_ctx_stat->mode == _cex_mode_begin ) {
		_cex_cur_ctx_stat->mode = _cex_mode_try;
		return 1;
	}

	/* The try block has completed succesfully. */
	else if ( _cex_cur_ctx_stat->mode == _cex_mode_try )
		_cex_cur_ctx_raise = (struct _cex_exc_ctx*)_cex_cur_ctx_raise->prev;

//Fix: The _cex_cur_ctx_raise variable is not pointing to the context to which to read to mode from.
	/* An exception has been raised. */
	else if ( _cex_cur_ctx_stat->mode == _cex_mode_raise ) {
		_cex_cur_ctx_raise = (struct _cex_exc_ctx*)_cex_cur_ctx_raise->prev;
		_cex_cur_ctx_stat->mode = _cex_mode_catch;
		return 1;
	}

	/* The catch block has been completed succesfully. */
	else if ( _cex_cur_ctx_stat->mode == _cex_mode_catch ) {
		CEX->free( CEX->data );
		CEX = &cex_exc_def;
	}

	/* Cleanup */
	free( _cex_cur_ctx_stat );
	_cex_cur_ctx_stat = _cex_cur_ctx_raise;

	return 0;
}
