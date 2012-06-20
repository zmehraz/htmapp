
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <htmapp.h>

#include <signal.h>

typedef THttpSession< CIpSocket > t_session;
typedef THttpServer< CIpSocket, t_session > t_server;

int g_show_requests = 0;

// Reply with greater than zero to authenticate the connection
int OnAuthenticate( void *x_pData, t_session *x_pSession, long lType, t_pb8 &sData )
{
	// Print request info to stdout
	if ( g_show_requests )
		str::Print( "%s : %s : %s %s\n",
					sData[ "REQUEST" ][ "REQUEST_TIME" ].str().c_str(),
					sData[ "REQUEST" ][ "REMOTE_ADDR" ].str().c_str(),
					sData[ "REQUEST" ][ "REQUEST_METHOD" ].str().c_str(),
					sData[ "REQUEST" ][ "uri" ][ "path" ].str().c_str()
				  );

	return 1;
}

int run_service( int argc, char* argv[], const char *params )
{
	t_pb8 pb;

	// Read params from make file
	if ( params )
		pb = parser::DecodeJson< t_pb8 >( params );

	// Allow override from command line
	if ( argc && argv )
		pb.merge( TCmdLine< str::t_string8 >( argc, argv ).pb() );

	// Show params
	if ( pb.isSet( "debug" ) )
		str::Print( " --- OPTIONS --- \n %s \n\n", parser::EncodeJson( pb ).c_str() );

	// Show requests?
	g_show_requests = pb.isSet( "show" ) ? 1 : 0;

	// On what port shall our server run?
	unsigned int port = pb[ "server_port" ].ToUInt();
	if ( 0 >= port )
	{	str::Print( "Invalid server port %d \n"
					"  i.  Ensure 'server_port' is set in CII_PARAMS in Makefile \n"
					"  ii. Use the --server_port command line option \n",
					port );
		return -1;
	} // end if

	// Initialize sockets
	if ( !CIpSocket::InitSockets() )
		return -1;

	// Our server object
	t_server server;

	// Has a home path been set?
	if ( pb.isSet( "home" ) )
		server.setDefaultPath( parser::DecodeUri< t_pb8 >( pb[ "home" ].str() )[ "path" ].str() );

	// Set authentication callback
	// Just using this in debug mode to show connection information
	server.SetAuthCallback( &OnAuthenticate, 0 );

	// You may want single threading if you're on a very weak
	// machine.  However, on any modern box with multiple cores,
	// multi-threaded server will perform much better.  As a bonus,
	// with multi-threaded server, you don't have to worry about
	// one transaction hanging the others.
//	server.EnableMultiThreading( 0 );

	// Map the root
	server.MapFolder( "/", "#htm", 1 );

	// Attempt to start the server
	if ( !server.StartServer( port ) )
	{	str::Print( "Server failed to start on port %d : %s\n",
					port, server.Port().GetLastErrorMsg().c_str() );
		return -2;
	} // end if

	// Show server location
	str::Print( "Server running at : http://localhost:%d/\n", port );

	// Run the sever
	while ( server.getStopEvent().Wait( 100 ) )
		sys::sleep( 100 );

	CIpSocket::UninitSockets();

	return 0;
}
