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
	{
		return CppEncode< T, T_STR >( x_str.data(), x_str.length() );
	}


	template< typename T, typename T_STR >
		static T_STR EncodeJsonStr( const T_STR &x_str )
	{
		// Data pointer and length
		const T *p = x_str.c_str();
		typename T_STR::size_type nLen = x_str.length();

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

/*
	/// Decodes a json
	template< typename T >
		static TStr< T > DecodeJSON( oexCONST T *x_pStr )
		{   oexASSERT_PTR( x_pStr );
			return DecodeJSON( TStr< T >( x_pStr ) );
		}

	template< typename T >
		static TStr< T > DecodeJSON( TStr< T > x_str )
	{
		TStr< T > ret, num;
		oexINT nLen = x_str.Length();

		while ( 0 < nLen-- )
		{
			if ( oexTC( T, '\\' ) == *x_str )
			{
				x_str++, nLen--;
				if ( 0 < nLen )
				{	switch( *x_str )
					{
						case oexTC( T, '"' ) : ret << oexTC( T, '"' ); break;
						case oexTC( T, '\'' ) : ret << oexTC( T, '\'' ); break;
						case oexTC( T, '\\' ) : ret << oexTC( T, '\\' ); break;
						case oexTC( T, '/' ) : ret << oexTC( T, '/' ); break;
						case oexTC( T, 'b' ) : ret << oexTC( T, '\b' ); break;
						case oexTC( T, 'f' ) : ret << oexTC( T, '\f' ); break;
						case oexTC( T, 'n' ) : ret << oexTC( T, '\n' ); break;
						case oexTC( T, 'r' ) : ret << oexTC( T, '\r' ); break;
						case oexTC( T, 't' ) : ret << oexTC( T, '\t' ); break;
						case oexTC( T, 'v' ) : ret << oexTC( T, '\v' ); break;
						case oexTC( T, 'u' ) : 
							if ( 4 <= nLen ) 
							{	x_str++;
								ret << (T)x_str.ToNum( 4, 16 );
								x_str.LTrim( 3 ); nLen -= 4;
							} // end if
							break;
						default: break;
					} // end switch
					
					x_str++;
					
				} // end if

			} // end if

			else 
				ret << *x_str, x_str++;

		} // end while

		return ret;
	}

	template< typename T >
		static TPropertyBag< TStr< T > > DecodeJSON( TStr< T > x_sStr, oexLONG x_lArrayType = 0, oexBOOL x_bMerge = oexFALSE, oexLONG *x_pLast = oexNULL )
		{	TPropertyBag< TStr< T > > pb;
			_DecodeJSON( x_sStr, pb, x_lArrayType, x_bMerge, x_pLast );
			return pb;
		}

	template< typename T >
		static oexLONG DecodeJSON( TStr< T > x_sStr, TPropertyBag< TStr< T > > &x_pb, oexLONG x_lArrayType = 0, oexBOOL x_bMerge = oexFALSE, oexLONG *x_pLast = oexNULL )
		{	return _DecodeJSON( x_sStr, x_pb, x_lArrayType, x_bMerge, x_pLast ); }

	template< typename T >
		static oexLONG _DecodeJSON( TStr< T > &x_sStr, TPropertyBag< TStr< T > > &x_pb, oexLONG x_lArrayType = 0, oexBOOL x_bMerge = oexFALSE, oexLONG *x_pLast = oexNULL )
	{
		// Lose previous contents
		if ( !x_bMerge )
			x_pb.Destroy();

		// Punt if null string
		if ( !x_sStr.Length() )
			return 0;

		// Find array or
		if ( !x_lArrayType )
		{
			// Figure out array type
			switch( *x_sStr.Find( oexTT( T, "{[" ) ) )
			{	case oexTC( T, '{' ) : x_lArrayType = 1; break;
				case oexTC( T, '[' ) : x_lArrayType = 2; break;
				default : return 0;
			} // end switch

			x_sStr++;

		} // end if

		oexLONG lItems = 0;
		oexLONG lMode = 0;
		TStr< T > sKey;

		while ( x_sStr.Length() )
		{
			x_sStr.SkipWhiteSpace();

			oexCONST T ch = x_sStr[ 0 ];

			// Check for array
			if ( oexTC( T, '{' ) == ch || oexTC( T, '[' ) == ch )
			{
				x_sStr++;

				if ( !lMode )
					sKey = lItems++;

				_DecodeJSON( x_sStr, x_pb[ sKey ], oexTC( T, '{' ) == ch ? 1 : 2 );

				lMode = 0;

			} // end if

			// End array
			else if ( oexTC( T, '}' ) == ch || oexTC( T, ']' ) == ch )
			{
				x_sStr++;
				return lItems;

			} // end of array?

			else if ( oexTC( T, ':' ) == ch )
			{
				x_sStr++;

			} // end else if

			else if ( oexTC( T, ',' ) == ch )
			{
				if ( lMode )
				{
					if ( !x_lArrayType )
						x_pb[ sKey ] = "";
					else
						x_pb[ CStr( lItems++ ) ] = sKey;
				} // end if

				x_sStr++;

			} // end else if

			// Parse string
			else if ( oexTC( T, '"' ) == ch )
			{
				if ( !lMode )
					lMode = 1,
					sKey = JsonDecode( x_sStr.ParseQuoted( oexTT( T, "\"" ), oexTT( T, "\"" ), oexTT( T, "\\" ) ) );

				else if ( lMode )
					lMode = ( 1 == lMode ) ? 0 : lMode,
					x_pb[ sKey ] = JsonDecode( x_sStr.ParseQuoted( oexTT( T, "\"" ), oexTT( T, "\"" ), oexTT( T, "\\" ) ) );

				lItems++;

			} // end if

			// Parse token
			else
			{
				if ( !lMode )
					lMode = 1,
					sKey = x_sStr.Parse( oexTT( T, ",:{}\r\n\t" ) ).TrimWhiteSpace();
				else if ( lMode )
					lMode = ( 1 == lMode ) ? 0 : lMode,
					x_pb[ sKey ] = x_sStr.Parse( oexTT( T, ",:{}\r\n\t" ) ).TrimWhiteSpace();

				x_sStr++;
				lItems++;

			} // end if

		} // end while

		return lItems;
	}

*/
}; // namespace parser
