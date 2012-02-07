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

#pragma once

#include "str.h"

namespace parser
{
	/// Returns non-zero if the character is a valid html character
	template< typename T >
		static bool IsCppChar( T x_ch )
	{
		switch( x_ch )
		{	case tcTC( T, '"' ) :
			case tcTC( T, '\\' ) :
				return false;
		} // end switch

		return ( 0 > x_ch || tcTC( T, ' ' ) <= x_ch ) ? true : false;
	}

	template< typename T, typename T_STR >
		static T_STR CppEncodeChar( T x_ch )
	{
		switch( x_ch )
		{
			case tcTC( T, '"' ) :
				return tcTT( T, "\\\"" );

			case tcTC( T, '\'' ) :
				return tcTT( T, "\\\\" );

			case tcTC( T, '\t' ) :
				return tcTT( T, "\\t" );

			case tcTC( T, '\r' ) :
				return tcTT( T, "\\r" );

			case tcTC( T, '\n' ) :
				return tcTT( T, "\\n" );

		} // end switch

		// Convert to two byte character
		T s[ 16 ] = { '"', ' ', '"', '\\', 'x', 0, 0, '"', ' ', '"', 0 };
		str::ntoa< char >( &s[ 5 ], (char)x_ch );

		return T_STR( s, 10 );
	}

	template< typename T, typename T_STR >
		static T_STR CppEncode( const T *x_pStr, typename T_STR::size_type x_lSize = 0 )
	{
		if ( !x_pStr || !*x_pStr || 0 >= x_lSize )
			return T_STR();

		T_STR ret;
		typename T_STR::size_type nStart = 0, nPos = 0;

		while ( nPos < x_lSize )
		{
			// Must we encode this one?
			if ( !IsCppChar( x_pStr[ nPos ] ) )
			{
				// Copy data that's ok
				if ( nStart < nPos )
					ret.append( &x_pStr[ nStart ], nPos - nStart );

				// Encode this character
				ret.append( CppEncodeChar< T, T_STR >( x_pStr[ nPos ] ) );

				// Next
				nStart = ++nPos;

			} // end if

			else
				nPos++;

		} // end while

		// Copy remaining data
		if ( nStart < nPos )
			ret.append( &x_pStr[ nStart ], nPos - nStart );

		return ret;
	}

	template< typename T, typename T_STR >
		static T_STR CppEncode( const T_STR &x_str )
	{	return CppEncode< T, T_STR >( x_str.data(), x_str.length() ); }


	template< typename T, typename T_STR >
		static T_STR EncodeJsonStr( const T *p, typename T_STR::size_type nLen )
	{
		// Sanity check
		if ( !p || 0 >= nLen )
			return T_STR();

		// Need at least this much space
		T_STR ret;
		ret.reserve( nLen );

		// Hex string buffer
		T hex[] = { tcTC( T, '\\' ), tcTC( T, 'u' ), 0, 0, 0, 0, 0 };

		while ( 0 < nLen-- )
		{
			switch( *p )
			{
				case tcTC( T, '\"' ) : ret += tcTT( T, "\\\"" ); break;
				case tcTC( T, '\\' ) : ret += tcTT( T, "\\\\" ); break;
				case tcTC( T, '\b' ) : ret += tcTT( T, "\\b" ); break;
				case tcTC( T, '\f' ) : ret += tcTT( T, "\\f" ); break;
				case tcTC( T, '\n' ) : ret += tcTT( T, "\\n" ); break;
				case tcTC( T, '\r' ) : ret += tcTT( T, "\\r" ); break;
				case tcTC( T, '\t' ) : ret += tcTT( T, "\\t" ); break;
				default :
					if ( *p < tcTC( T, ' ' ) || *p > tcTC( T, '~' ) )
						str::ntoa( &hex[ 2 ], (unsigned short)*p ), ret.append( hex, 6 );
					else
						ret += *p;
					break;

			} // end switch

			// Next character
			p++;

		} // end while

		return ret;
	}

	template< typename T, typename T_STR >
		static T_STR EncodeJsonStr( const T_STR &x_str )
	{	return EncodeJsonStr< T, T_STR >( x_str.data(), x_str.length() ); }

	template< typename T, typename T_STR, typename T_PB >
		static T_STR EncodeJson( const T_PB &x_pb, long x_depth = 0 )
	{
		T_STR sTab, sTab1;
		for ( long t = 0; t < x_depth; t++ )
			sTab += tcTC( T, '\t' );
		sTab1 += tcTC( T, '\t' ), sTab1 += sTab;

		bool bList = x_pb.is_list();
		typename T_STR::size_type i = 0;
		T_STR sStr; sStr += bList ? tcTC( T, '[' ) : tcTC( T, '{' ); sStr += tcTTEXT( T, tcNL8 );

		// stdForeach( typename T_PB::const_iterator, it, x_pb )
		for ( typename T_PB::const_iterator it = x_pb.begin(); it != x_pb.end(); it++ )
		{
			// Add comma if needed
			if ( !i )
				i++;
			else
				sStr += tcTT( T, "," ), sStr += tcTTEXT( T, tcNL8 );

			sStr += sTab1;

			// Add key
			if ( !bList )
				sStr += tcTC( T, '\"' ),
				sStr += EncodeJsonStr< T, T_STR >( it->first ),
				sStr += tcTT( T, "\": " );

			// Single value
			if ( it->second->length() )
				sStr += tcTC( T, '\"' ), sStr += EncodeJsonStr< T, T_STR >( it->second->str() ), sStr += tcTC( T, '\"' );

			// Recurse for array
			else if ( it->second->size() )
				sStr += EncodeJson< T, T_STR, T_PB >( *it->second, x_depth + 1 );

			// Empty
			else
				sStr += tcTT( T, "\"\"" );

		} // end for

		sStr += tcNL8; sStr += sTab; sStr += bList ? tcTC( T, ']' ) : tcTC( T, '}' );

		return sStr;
	}

	template< typename T, typename T_STR >
		static T_STR DecodeJsonStr( const T *p, typename T_STR::size_type nLen )
	{
		if ( !p || 0 >= nLen )
			return T_STR();

		T_STR ret;
		unsigned long n;

		while ( 0 < nLen-- )
		{
			// Escaped?
			if ( tcTC( T, '\\' ) == *p )
			{
				p++, nLen--;
				if ( 0 < nLen )
				{	switch( *p )
					{
						case tcTC( T, '"' ) :	ret += tcTC( T, '"' ); break;
						case tcTC( T, '\'' ) :	ret += tcTC( T, '\'' ); break;
						case tcTC( T, '\\' ) :	ret += tcTC( T, '\\' ); break;
						case tcTC( T, '/' ) :	ret += tcTC( T, '/' ); break;
						case tcTC( T, 'b' ) :	ret += tcTC( T, '\b' ); break;
						case tcTC( T, 'f' ) :	ret += tcTC( T, '\f' ); break;
						case tcTC( T, 'n' ) :	ret += tcTC( T, '\n' ); break;
						case tcTC( T, 'r' ) :	ret += tcTC( T, '\r' ); break;
						case tcTC( T, 't' ) :	ret += tcTC( T, '\t' ); break;
						case tcTC( T, 'v' ) :	ret += tcTC( T, '\v' ); break;
						case tcTC( T, 'u' ) :
							if ( 4 <= nLen )
								p++, ret += (T)str::aton( p, &n, 4 ), nLen -= 4;
							break;
						default: break;
					} // end switch

					p++;

				} // end if

			} // end if

			else
				ret += *p; p++;

		} // end while

		return ret;
	}

	template< typename T, typename T_STR >
		static T_STR DecodeJsonStr( const T_STR x_str )
	{	return DecodeJsonStr< T, T_STR >( x_str.data(), x_str.length() ); }

	template< typename T, typename T_STR, typename T_PB >
		static str::t_size _DecodeJson( const T *p, str::t_size nLen, T_PB &x_pb, long x_lArrayType = 0, bool x_bMerge = false, long *x_pLast = 0 )
	{
		// Lose previous contents
		if ( !x_bMerge )
			x_pb.clear();

		// Sanity check
		if ( !p || 0 >= nLen )
			return 0;

		// Find start of array
		str::t_size nPos = 0;
		while ( !x_lArrayType && nPos < nLen )
		{
			// Look for array start character
			switch( *p )
			{	case tcTC( T, '{' ) : x_lArrayType = 1; break;
				case tcTC( T, '[' ) : x_lArrayType = 2; break;
			} // end switch

			// Next character
			p++, nPos++;

		} // end while

		// Unknown type
		if ( !x_lArrayType )
			return nPos;

		// Mark list
		if ( 2 == x_lArrayType )
			x_pb.is_list( true );

		T_STR sKey;
		long lMode = 0, lItems = 0;
		while ( nPos < nLen )
		{
			// Skip whitespace
			str::t_size nSkip = str::FindInRange( p, nLen - nPos, tcTC( T, '!' ), tcTC( T, '~' ) );
			if ( 0 > nSkip || nSkip >= ( nLen - nPos ) )
				return nPos;
			p += nSkip, nPos += nSkip;

			// The character we will deal with
			const T ch = *p;

			// Check for array
			if ( tcTC( T, '{' ) == ch || tcTC( T, '[' ) == ch )
			{
				p++;

				if ( !lMode )
					sKey = str::ToString< T, T_STR >( lItems++ );

				str::t_size sz = _DecodeJson< T, T_STR, T_PB >( p, nLen - nPos, x_pb[ sKey ], tcTC( T, '{' ) == ch ? 1 : 2 );
				if ( 0 <= sz )
					p += sz, nPos += sz;

				lMode = 0;

			} // end if

			// Key / Value separator
			else if ( tcTC( T, ':' ) == ch )
				p++, nPos++;

			// Value separator / end array
			else if ( tcTC( T, ',' ) == ch || tcTC( T, '}' ) == ch || tcTC( T, ']' ) == ch )
			{
				if ( lMode )
				{
					if ( 1 == x_lArrayType )
						x_pb[ sKey ] = tcTT( T, "" );
					else
						x_pb[ str::ToString< T, T_STR >( lItems ) ] = sKey;

					// Count an item
					lItems++;

					// Nothing read yet
					lMode = 0;

				} // end if

				// Skip
				p++, nPos++;

				// End array
				if ( tcTC( T, '}' ) == ch || tcTC( T, ']' ) == ch )
					return nPos;

			} // end else if

			// Parse token
			else if ( tcTC( T, ' ' ) < ch )
			{
				// Find the end of the token
				str::t_size end = ( tcTC( T, '"' ) == ch )
								  ? str::FindTerm( ++p, nLen - ++nPos, tcTT( T, "\"" ), 1, tcTT( T, "\\" ), 1 )
								  : str::FindTerm( p, nLen - nPos, tcTT( T, ",:{}\r\n\t\"" ), 8, tcTT( T, "" ), 0 );

				// Key?
				if ( !lMode )
					lMode = 1,
					sKey = ( 0 < end ) ? DecodeJsonStr< T, T_STR >( p, end ) : T_STR();

				// Value?
				else if ( lMode )
					lItems++,
					lMode = ( 1 == lMode ) ? 0 : lMode,
					x_pb[ sKey ] = ( 0 < end ) ? DecodeJsonStr< T, T_STR >( p, end ) : T_STR();

				// Skip string
				p += end, nPos += end;

				// Skip closing quote
				if ( tcTC( T, '"' ) == *p )
					p++, nPos++;

			} // end if

			// Skip white space
			else
				p++, nPos++;

		} // end while

		return nPos;
	}

	template< typename T, typename T_STR, typename T_PB >
		static T_PB DecodeJson( const T_STR x_str, long x_lArrayType = 0, bool x_bMerge = false, long *x_pLast = 0 )
		{	T_PB pb;
			_DecodeJson< T, T_STR, T_PB >( x_str.data(), x_str.length(), pb, x_lArrayType, x_bMerge, x_pLast );
			return pb;
		}

	template< typename T, typename T_STR, typename T_PB >
		static long DecodeJson( const T_STR x_str, T_PB &x_pb, long x_lArrayType = 0, bool x_bMerge = false, long *x_pLast = 0 )
		{	return _DecodeJson< T, T_STR, T_PB >( x_str.data(), x_str.length(), x_pb, x_lArrayType, x_bMerge, x_pLast ); }

}; // namespace parser
