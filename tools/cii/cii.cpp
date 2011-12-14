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

#include <time.h>

#include "str.h"
#include "cmd_line.h"
#include "disk.h"
#include "process_binary.h"
#include "process_cpp.h"

typedef TCmdLine< char > t_cmdline8;
typedef std::basic_string< char > t_string8;
typedef std::list< t_string8 > t_strlist8;

int process_file( t_string8 x_sInDir, t_string8 x_sOutDir, t_string8 x_sOutPre, t_string8 x_sRel, t_string8 x_sFile, t_cmdline8 &cl, t_strlist8 &lstCmp, t_string8 &sRoot, long &lI )
{
	// Create unique variable name
	t_string8 sVar = sRoot + str::ToString< char, t_string8 >( lI++ );
	
	// Create full input and output file paths
	t_string8 sIn = disk::FilePath< char >( x_sInDir, x_sFile );
	t_string8 sOut = disk::FilePath< char >( x_sOutDir, disk::Path< char >( x_sOutPre, x_sFile, '_' ) ) + t_string8( ".cpp" );

	// Is this a compiled type?
	stdForeach( t_strlist8::iterator, it, lstCmp )
		if ( 0 <= str::MatchPattern( sIn.data(), sIn.length(), it->data(), it->length(), true ) )
		{
			if ( !cl.pb().IsSet( "q" ) )
				str::Print( "c: %s -> %s\n", sIn.c_str(), sOut.c_str() );

			// Declare variables
			disk::AppendFile< char >( disk::FilePath< char, t_string8 >( x_sOutDir, "cii_resource_extern.hpp" ),
									  t_string8()
									  + "\nextern void * f_" + sVar + ";\n"
									);

			// Add to map
			disk::AppendFile< char >( disk::FilePath< char, t_string8 >( x_sOutDir, "cii_resource.cpp" ),
									  t_string8( "\n{" )
									  + "\n\t\"" + disk::WebPath< char >( x_sRel, x_sFile ) + "\","
									  + "\n\t0,"
									  + "\n\t0,"
									  + "\n\tf_" + sVar + "\n},\n"
									);

			// Process the embedded c/c++ file
			process_cpp< char, t_string8 >( sIn, sOut, sVar );
				
			return 0;
		} // end if

	if ( !cl.pb().IsSet( "q" ) )
		str::Print( "b: %s -> %s\n", sIn.c_str(), sOut.c_str() );

	// Declare variables
	disk::AppendFile< char >( disk::FilePath< char, t_string8 >( x_sOutDir, "cii_resource_extern.hpp" ),
							  t_string8()
							  + "\nextern const char *data_" + sVar + ";"
							  + "\nextern const long size_" + sVar + ";\n"
							);

	// Add to map
	disk::AppendFile< char >( disk::FilePath< char, t_string8 >( x_sOutDir, "cii_resource.cpp" ),
							  t_string8( "\n{" )
							  + "\n\t\"" + disk::WebPath< char >( x_sRel, x_sFile ) + "\","
							  + "\n\tdata_" + sVar + ","
							  + "\n\tsize_" + sVar + ",\n\t0\n},\n"
							);

	// Create the binary data file
	long lBytes = process_binary< char, t_string8 >( sIn, sOut,
													 t_string8( "\nextern const char *data_" ) + sVar + " = \n{\n\t",
													 t_string8( "\n\t0\n};\n\nextern const long size_" ) + sVar + " = ",
													 ";\n"
													);


	return lBytes;
}

int process_directory( t_string8 x_sIn, t_string8 x_sOut, t_string8 x_sOutPre, t_string8 x_sRel, t_cmdline8 &cl, t_strlist8 &lstCmp, t_string8 &sRoot, long &lI )
{
	if ( !x_sIn.length() || !x_sOut.length() )
		return 0;

	// Process directory contents
	disk::SFindData fd;
	disk::HFIND hFind = disk::FindFirst( x_sIn.c_str(), "*", &fd );
	if ( disk::c_invalid_hfind == hFind )
		return 0;

	do 
	{ 
		// Ignore relative
		if ( '.' != *fd.szName )
		{
			// Recurse if directory
			if ( disk::eFileAttribDirectory & fd.uFileAttributes )
				process_directory( disk::FilePath< char, t_string8 >( x_sIn, fd.szName ), 
								   x_sOut,
								   disk::Path< char, t_string8 >( x_sOutPre, fd.szName, '_' ),
								   disk::FilePath< char, t_string8 >( x_sRel, fd.szName ), 
								   cl, lstCmp, sRoot, lI );

			// Go process the file
			else
				process_file( x_sIn, x_sOut, x_sOutPre, x_sRel, fd.szName, cl, lstCmp, sRoot, lI );

		} // end if

	// While we can find more files
	} while ( disk::FindNext( hFind, &fd ) );

	// Close the find handle
	disk::FindClose( hFind );

	return 1;
}

int main( int argc, char* argv[] )
{
	// Parse the command line
    t_cmdline8 cl( argc, argv );

	// Dumping the command line options to STDOUT?
	if ( cl.pb().IsSet( "d" ) || cl.pb().IsSet( "debug" ) )
	{	for ( TCmdLine< char >::iterator it = cl.begin(); cl.end() != it; it++ )
			str::Print( "[%s] = '%s'\n", it->first.c_str(), it->second->c_str() );
		return 0;
	} // end if

	// Ensure all needed parameters are present
	if ( cl.pb().IsSet( "h" ) || cl.pb().IsSet( "help" )
		 || !cl.pb().IsSet( "i" ) || !cl.pb().IsSet( "o" ) )
	{	str::Print( "Options\n"
				" -i            '<comma separated input directories>'\n"
				" -o            '<output directory>'\n"
				" -c            '<semi-colon separated compiled file types>' - default is '*.cii'\n"
				" -q / --quiet  Suppress all output\n"
				" -d / --debug  'Show processed command line array'\n"
				" -h / --help   Display this information\n"
				);
		return cl.pb().IsSet( "h" ) ? 0 : -1;
	} // end if

	// Combine quiet options
	if ( cl.pb().IsSet( "quiet" ) )
		cl.pb()[ "q" ] = 1;

	// Create output folder if needed
	if ( !disk::exists( cl.pb()[ "o" ] ) )
	{	if ( !cl.pb().IsSet( "q" ) )
			str::Print( "Creating directory : %s\n", cl.pb()[ "o" ].c_str() );
		disk::mkdir( cl.pb()[ "o" ].c_str() );
	} // end if

	// res_extern.hpp
	disk::unlink( disk::FilePath< char, t_string8 >( cl.pb()[ "o" ].str(), "cii_resource_extern.hpp" ).c_str() );
	
	// res_list.hpp
	disk::WriteFile< char >( disk::FilePath< char, t_string8 >( cl.pb()[ "o" ].str(), "cii_resource.cpp" ),
							 t_string8() + "#include \"cii_resource.h\"\n"
										   "#include \"cii_resource_extern.hpp\"\n"
										   "\nextern SCiiResourceInfo _cii_resources[] = \n{\n" );

	// resource.h
	disk::WriteFile< char >( disk::FilePath< char, t_string8 >( cl.pb()[ "o" ].str(), "cii_resource.h" ),
							 t_string8() + "\nstruct SCiiResourceInfo\n{"
										   "\n\tconst char*   name;"
										   "\n\tconst char*   data;"
										   "\n\tunsigned long size;"
										   "\n\tvoid*         func;"
										   "\n};\n" 
										   "\nextern SCiiResourceInfo _cii_resources[];\n"
										 );

	// Get compiled types
	t_strlist8 lstCmp;
	if ( !cl.pb().IsSet( "c" ) )
		lstCmp.push_back( "*.cii" );
	else
		lstCmp = str::SplitQuoted< char, t_string8, t_strlist8 >
								   ( (char*)cl.pb()[ "c" ].data(), cl.pb()[ "c" ].length(), 
								     ";, \t", "\"'", "\"'", "\\", true );

	// Separate the different directories
	t_strlist8 lstDir = str::SplitQuoted< char, t_string8, t_strlist8 >
										  ( (char*)cl.pb()[ "i" ].data(), cl.pb()[ "i" ].length(), 
											";, \t", "\"'", "\"'", "\\", true );

	// Root variable name
	t_string8 sRoot( "T" );
	sRoot += str::ToString< char, t_string8 >( (unsigned long)time( 0 ) );
	sRoot += "I";

	// Process each directory
	long lI = 0;
	stdForeach( t_strlist8::iterator, it, lstDir )
		process_directory( *it, cl.pb()[ "o" ].str(), *it, "", cl, lstCmp, sRoot, lI );

	// Close up res_list.hpp
	disk::AppendFile< char >( disk::FilePath< char, t_string8 >( cl.pb()[ "o" ].str(), "cii_resource.cpp" ), 
							  t_string8( "\n{0,0,0,0}\n\n};\n" ) );

	return 0;
}
