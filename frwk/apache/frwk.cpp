/*------------------------------------------------------------------
// Copyright (c) 1997 - 2011
// Robert Umbehant
// htmapp@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <list>
#include <map>
#include <vector>

#include "htmapp.h"

extern "C" 
{
#	include <httpd.h>
#	include <http_protocol.h>
#	include <http_config.h>
#	include "frwk.h"
};

extern "C" void init_frwk()
{
	// +++ Get thread pool working in apache module
	// tq::init( true );
	tq::init( false );
}

extern "C" void uninit_frwk()
{
	tq::uninit();
}

void get_params( t_pb8 &pb, request_rec* r, str::t_string8 &path )
{
	if ( !r )
		return;

	// Fill in the request paraeters
#	define CPSTR( s, p ) rq[ s ] = r->p ? r->p : ""
	t_pb &rq = pb[ "REQUEST" ];
	rq[ "PATH_INFO" ] = path;
	rq[ "CONTENT_LENGTH" ] = r->bytes_sent;
	CPSTR( "DOCUMENT_ROOT", filename );
	CPSTR( "SERVER_NAME", hostname );
	CPSTR( "QUERY_STRING", args );
	CPSTR( "REQUEST_METHOD", method );

	// +++ Figure out these parameters
	CPSTR( "proto", protocol );
	CPSTR( "the_request", the_request );
	CPSTR( "content_type", content_type );
	CPSTR( "handler", handler );
	CPSTR( "content_encoding", content_encoding );
	CPSTR( "unparsed_uri", unparsed_uri );
	CPSTR( "path_info", path_info );

	// GET parameters
	if ( r->args && *r->args )
		pb[ "GET" ] = parser::DecodeUrl< t_pb8 >( r->args );
}

extern "C" int process_request( request_rec* r, const char *prj_name )
{
	// Ensure it's for us
	if ( !r || str::CompareI( str::t_string8( r->handler ), str::t_string8( prj_name ) ) )
		return DECLINED;

	// Users request path
	str::t_string8 path = r->uri 
						  ? ( prj_name ? disk::SkipRoot< str::t_string8 >( r->uri, prj_name ) : r->uri )
						  : "";

	// Create full path
	str::t_string8 full = disk::WebPath< str::t_string8 >( "res", path );

/*	t_pb8 pb;
	get_params( pb, r, path );
	ap_rputs( parser::EncodeJson( pb ).c_str(), r );
	return OK;
*/
	
	// Check for resource
	HMRES hRes = 0;
	CHmResources res;
	if ( !res.IsValid() || 0 == ( hRes = res.FindResource( 0, full.c_str() ) ) )
		return HTTP_NOT_FOUND;

	t_pb8 headers;
	switch ( res.Type( hRes ) )
	{
		default :
			break;

		case 1 :
		{
			// Set MIME type
			ap_set_content_type( r, disk::GetMimeType( full ).c_str() );

			// Get data pointers
			const void *ptr = res.Ptr( hRes );
			unsigned long sz = res.Size( hRes );

			// Verify data
			if ( !ptr || 0 >= sz )
				return HTTP_INTERNAL_SERVER_ERROR;

			// Send the data
			ap_rwrite( ptr, sz, r );
				
			return OK;

		} break;

		case 2 :
		{
			// Get function pointer
			CHmResources::t_fn pFn = res.Fn( hRes );
			if ( pFn )
			{
				t_pb8 in, out;
				
				// Format request params
				get_params( in, r, path );
			
				// Execute the page
				pFn( in, out );
				
				// Set MIME type
				ap_set_content_type( r, out[ "HEADERS" ].isSet( "CONTENT_TYPE" )
										? out[ "HEADERS" ][ "CONTENT_TYPE" ].ToString().c_str()
										: "text/html" );

				// Send the data
				ap_rwrite( out.data(), out.length(), r );
				
				return OK;

			} // end if

		} break;

	} // end switch

    return HTTP_NOT_FOUND;
}

