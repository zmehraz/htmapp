
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <htmapp.h>

const char* response_msg( int code )
{
	switch( code )
	{
		case 200 : return "OK";
		case 404 : return "File not found";
		case 500 : return "Internal server error";

	} // end switch
	
	return "Unknown error";
}

int send_response( t_pb8 &headers, int code, const void *p, long sz )
{

#if defined( _WIN32 ) || defined( WIN32 )

	// Ensure stdout is in binary mode
	_setmode( fileno( stdout ), O_BINARY );

#endif

	// NULL terminated?
	if ( p && 0 >= sz )
		while ( ((char*)p)[ sz ] ) sz++;

	// Set content size
	if ( 0 <= sz )
		headers[ "content-length" ] = sz;

	// Send response headers
	printf( "HTTP/1.1 %d %s\n%s\n\n", 
			code, response_msg( code ),
			parser::EncodeHttpHeaders( headers ).c_str() );

	// Write the data
	if ( p && 0 < sz )
		fwrite( p, 1, sz, stdout );

	return 0;
}

int send_error( int code )
{
	t_pb8 headers;
	send_response( headers, code, response_msg( code ), 0 );
	return code;
}

// parser::DecodeJson< t_pb >( CII_PARAMS );
int process_cgi_request( int argc, char* argv[], const char *params )
{
	// Supported environment variables
	static const char *pEnvVars[] = 
	{
		"CONTENT_LENGTH",
		"DOCUMENT_ROOT",
		"HTTP_REFERER",
		"HTTP_USER_AGENT",
		"PATH_INFO",
		"PATH_TRANSLATED",
		"QUERY_STRING",
		"REMOTE_ADDR",
		"REMOTE_HOST",
		"REQUEST_METHOD",
		"SCRIPT_NAME",
		"SERVER_NAME",
		"SERVER_PORT",
		0
	};

	t_pb8 in, out;

	// Read in environment vars
	t_pb &r = in[ "REQUEST" ];
	for ( long i = 0; pEnvVars[ i ]; i++ )
	{	const char *p = getenv( pEnvVars[ i ] );
		if ( p && *p )
			r[ pEnvVars[ i ] ] = p;
	} // end for

	// Decode GET parameters if any
	if ( r.isSet( "QUERY_STRING" ) )
		in[ "GET" ] = parser::DecodeUrl< t_pb8 >( r[ "QUERY_STRING" ].ToString() );

	// Add browser headers
//	in[ "HEADERS" ] = parser::DecodeHttpHeaders< t_pb8 >( pHeaders );

	// What page does the caller want?
	str::t_string8 sPath = r[ "PATH_INFO" ].ToString();

	// Create full path
	str::t_string8 full = disk::WebPath< str::t_string8 >( "res", r[ "PATH_INFO" ].ToString() );

	// Check for resource
	HMRES hRes = 0;
	CHmResources res;
	if ( !res.IsValid() || 0 == ( hRes = res.FindResource( 0, full.c_str() ) ) )
		return -send_error( 404 );

	t_pb8 headers;
	switch ( res.Type( hRes ) )
	{
		default :
			break;

		case 1 :
		{
			// Set MIME type
			headers[ "content-type" ] = disk::GetMimeType( full );

			// Get data pointers
			const void *ptr = res.Ptr( hRes );
			unsigned long sz = res.Size( hRes );

			// Verify data
			if ( !ptr || 0 >= sz )
				return -send_error( 500 );

			// Send the binary data
			return send_response( headers, 200, ptr, sz );

		} break;

		case 2 :
		{
			// Set MIME type
			headers[ "content-type" ] = "text/html";

			// Get function pointer
			CHmResources::t_fn pFn = res.Fn( hRes );
			if ( pFn )
			{
				// Execute the page
				pFn( in, out );

				// Send the response
				return send_response( headers, 200, out.data(), out.length() );

			} // end if

		} break;

	} // end switch

	// Not found
	return -send_error( 404 );
}
