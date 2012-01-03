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

HFILE Open( const char *x_pFile, const char *x_pMode )
{
#if defined( CII_NOSTAT64 )
	tHFILE res = fopen( x_pFile, x_pMode );
#else
	tHFILE res = fopen64( x_pFile, x_pMode );
#endif
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

bool mkdir( const char *x_pPath )
{
	if ( !x_pPath || !*x_pPath )
		return false;

	bool bRet = !::mkdir( x_pPath, 0755 ) ? true : false;
	if ( bRet )
	{	::chmod( x_pPath, 0755 );
		return true;
	} // end if

	return false;
}

bool exists( const char *x_pPath )
{
#if defined( CII_NOSTAT64 )
	struct stat s;
	return ::stat( x_pPath, &s ) ? false : true;
#else
	struct stat64 s64;
	return	::stat64( x_pPath, &s64 ) ? false : true;
#endif
}

bool unlink( const char *x_pFile )
{
	if ( !x_pFile )
		return false;

	::chmod( x_pFile, 0xffff );
	return ::unlink( x_pFile ) ? false : true;
}

bool rmdir( const char *x_pPath )
{
	if ( !x_pPath )
		return false;

	::chmod( x_pPath, 0xffff );
	return ::rmdir( x_pPath ) ? false : true;
}

str::tc_int64 Size( const char *x_pFile )
{
	if ( !x_pFile || !*x_pFile )
		return 0;

#if defined( CII_NOSTAT64 )

	struct stat s;
	if ( stat( x_pFile, &s ) )
		return 0;

	return s.st_size;

#else

	// +++ Ensure this works correctly
	struct stat64 s64;
	if ( stat64( x_pFile, &s64 ) )
		return 0;

	return s64.st_size;

#endif
}

str::tc_int64 Size( HFILE x_hFile )
{
	// Ensure valid handle
	if ( c_invalid_hfile == x_hFile ) 
		return 0;

#if defined( CII_NOSTAT64 )

	int size = 0;
	int pos = ftell( (tHFILE)x_hFile );
	if ( !fseek( (tHFILE)x_hFile, 0, SEEK_END ) )
		size = ftell( (tHFILE)x_hFile ),
		fseek( (tHFILE)x_hFile, pos, SEEK_SET );
	return size;	

#else

	off64_t size = 0;
	off64_t pos = ftello64( (tHFILE)x_hFile );
	if ( !fseeko64( (tHFILE)x_hFile, 0, SEEK_END ) )
		size = ftello64( (tHFILE)x_hFile ),
		fseeko64( (tHFILE)x_hFile, pos, SEEK_SET );
	return (str::tc_int64)size;	

#endif
}


static void disk_InitFindData( SFindData *x_pFd )
{   x_pFd->llSize = 0;
    *x_pFd->szName = 0;
    x_pFd->uFileAttributes = 0;
    x_pFd->ftCreated = 0;
    x_pFd->ftLastAccess = 0;
    x_pFd->ftLastModified = 0;
}

static void disk_SetFindData( SFindData *x_pFd, const dirent *x_pD )
{
	if ( ! x_pFd || !x_pD )
		return;

	// Init structure
	disk_InitFindData( x_pFd );

	// Save file name
	zstr::Copy( x_pFd->szName, sizeof( x_pFd->szName ), x_pD->d_name );

	// Is it a directory?
	if ( DT_DIR & x_pD->d_type )
		x_pFd->uFileAttributes |= disk::eFileAttribDirectory;
}

HFIND FindFirst( const char *x_pPath, const char *x_pMask, SFindData *x_pFd )
{
#if defined( CII_NODIRENT )
	return c_invalid_hfind;
#else

    // Sanity checks
    if ( !x_pPath || ! x_pMask || ! x_pFd )
        return c_invalid_hfind;

	DIR *hDir = opendir( x_pPath );
	if ( !hDir )
		return c_invalid_hfind;

	errno = 0;
	struct dirent *pD = readdir( hDir );
	if ( !pD || errno )
	{	closedir( hDir );
		return c_invalid_hfind;
	} // end if

	// Set file data
	disk_SetFindData( x_pFd, pD );

    return (HFIND)hDir;
#endif
}

bool FindNext( HFIND x_hFind, SFindData *x_pFd )
{
#if defined( CII_NODIRENT )
	return false;
#else
	DIR *hDir = (DIR*)x_hFind;

	errno = 0;
	struct dirent *pD = readdir( hDir );
	if ( !pD || errno )
		return false;

	disk_SetFindData( x_pFd, pD );

    return true;
#endif
}

bool FindClose( HFIND x_hFind )
{
#if defined( CII_NODIRENT )
	return false;
#else
	return closedir( (DIR*)x_hFind ) ? false : true;
#endif
}
