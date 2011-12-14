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

#if defined( _WIN32 )
#	include <windows.h>
#else
#	include <errno.h>
#	include <sys/stat.h>
#	include <sys/types.h>
#	if !defined( CII_NODIRENT )
#		include <dirent.h>
#	endif
#endif

#include <stdio.h>
#include <string>
#include <string.h>
#include "str.h"
#include "disk.h"

namespace disk
{
	typedef FILE* tHFILE;

	HFILE Open( const char *x_pFile, const char *x_pMode )
	{
		tHFILE res = fopen( x_pFile, x_pMode );
		if ( !res )
			return c_invalid_hfile;
		return (HFILE)res;
	}

	t_size Write( const void *x_pData, t_size x_nSize, t_size x_nCount, HFILE x_hFile )
	{
		return fwrite( x_pData, x_nSize, x_nCount, (tHFILE)x_hFile );
	}

	t_size Read( void *x_pData, t_size x_nSize, t_size x_nCount, HFILE x_hFile )
	{
		return fread( x_pData, x_nSize, x_nCount, (tHFILE)x_hFile );
	}

	t_size Close( HFILE x_hFile )
	{
		return fclose( (tHFILE)x_hFile );
	}

#if defined( _WIN32 )
#	include "disk-windows.hpp"
#else
#	include "disk-posix.hpp"
#endif

}; // namespace disk

