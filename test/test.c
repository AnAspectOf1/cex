#include <cex.h>
#include <stdio.h>
#include <string.h>

const unsigned char test_count = 8;
unsigned char test_index = 0;

void fail() {
	RAISEC(3);
}

void print( const char* msg ) {
	fprintf( stdout, "%s.\n", msg );
}

void error( int code, const char* msg ) {
	fprintf( stderr, "Error: %s.\n", msg );
	exit( code );
}

void test( unsigned char index, const char* msg ) {
	fprintf( stdout, "[Test %d/%d]:\t%s.\n", index, test_count, msg );
	test_index = index;
}

int main() {

	print( "This is a test program to test whether Cex works correctly" );

	TRY {
		test( 1, "First try block" );
		RAISEC( 1 );
		error( 1, "Code in a try block after raisec should not be reached" );
	}
	CATCH {
		test( 2, "First catch block" );

		if ( CEX->code != 1 )
			error( 2, "Raised error code did not match in catch block" );
		
		TRY {
			test( 3, "Try block in a catch block" );

			TRY {
				test( 4, "Try block in a try block" );
			}
			CATCH {
				error( 4, "Catch block executed with no exception" );
			}

			TRY {
				TRY {
					const char* message = "Raised data";
					char* buffer = malloc( strlen( message ) );
					strcpy( buffer, message );
					RAISED( 2, buffer );
					error( 1, "Code in a try block after a raised should not be reached" );
				}
				CATCH {
					test( 5, CEX->data );

					RERAISE;
				}
			}
			CATCH {
				test( 6, "Reraise caught" );
				
				TRY {
					fail();
				}
				CATCH {
					test( 7, "Exception in function caught" );
				
					TRY
						RAISEM( 4, "Message raised!" );
					CATCH {
						test( 8, CEX->data );
						return 0;
					}
				}
			}
		}
		CATCH {
			error( 3, "An exception was not caught locally" );
		}
	}

	return -1;
}
