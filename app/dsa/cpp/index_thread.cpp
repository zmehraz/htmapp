
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "htmapp.h"
#include "cpp/file_index.h"

	static volatile unsigned long g_lLastUpdate = 0;
	
	bool replace_smallest( CFileIndex::SBlockItem *p, CFileIndex::SBlockItem **lst, long sz_lst )
	{
		long smallest = 0;
		for ( long i = 0; i < sz_lst; i++ )
		{
			// Empty slot?
			if ( !lst[ i ] )
			{	lst[ i ] = p;
				return true;
			} // end if
			
			// Track the smallest item
			if ( smallest != i && lst[ i ]->size < lst[ smallest ]->size )
				smallest = i;
		
		} // end for
		
		// Punt if new item is smaller than the smallest
		if ( p->size <= lst[ smallest ]->size )
			return false;

		// Replace smallest
		lst[ smallest ] = p;
		
		return true;		
	}
	
	void add_item( str::t_string sRoot, t_pb &pb, CFileIndex &fi, CFileIndex::SBlockItem *p );
	long add_items( str::t_string sRoot, t_pb &pb, CFileIndex &fi, CFileIndex::t_block b, long lMaxTop, long lMaxDepth, long lDepth = 0 )
	{
		if ( !b || lMaxDepth < lDepth || 0 >= lMaxTop )
			return 0;

		CFileIndex::SBlockItem *p = fi.getItem( b );
		if ( !p )
			return 0;
		
		// If it's the root
		if ( 0 >= lDepth )
		{	pb[ "name" ] = "used";
			pb[ "colour" ] = "#800000",
			pb[ "hi_colour" ] = "#a00000";
			pb[ "size" ] = ( 0 < p->size ) ? p->size : 1;
			pb[ "path" ] = sRoot;
			pb[ "szstr" ] = str::SizeStr< str::t_char, str::t_string >( (double)p->size, 1024., 2 ) + "B";
			tq::set( "indexer.progress", p->size, "." );
			return add_items( sRoot, pb[ "children" ], fi, p->child, lMaxTop, lMaxDepth, lDepth + 1 );
		} // end if

		// This will be a list
		pb.is_list( true );
			
		// Top array
		lMaxTop = cmn::Min( lMaxTop, 256l );
		CFileIndex::SBlockItem *tdir[ 256 ], *tfile[ 256 ];
		memset( tdir, 0, sizeof( CFileIndex::SBlockItem ) * lMaxTop );
		memset( tfile, 0, sizeof( CFileIndex::SBlockItem ) * lMaxTop );
		
		// Find the top 'lMaxTop' items
		CFileIndex::SBlockItem *it = fi.getItem( b );
		while ( it )
		{
			// Tree?
			if ( 0 != ( it->flags & disk::eFileAttribDirectory ) )
				replace_smallest( it, tdir, lMaxTop );
			else
				replace_smallest( it, tfile, lMaxTop );
				
			// Next sibling
			it = it->next ? fi.getItem( it->next ) : 0;
		
		} // end while

		long idx = 0;

		// Add top directories
		for ( long i = 0; i < lMaxTop && tdir[ i ]; i++ )
		{	t_pb &r = pb[ idx++ ];
			add_item( sRoot, r, fi, tdir[ i ] );
			add_items( disk::FilePath< str::t_char, str::t_string >( sRoot, fi.getBlob( tdir[ i ]->name ) ),
					   r[ "children" ], fi, tdir[ i ]->child, lMaxTop, lMaxDepth, lDepth + 1 );
		} // end for

		// Add top files
		for ( long i = 0; i < lMaxTop && tfile[ i ]; i++ )		
			add_item( sRoot, pb[ idx++ ], fi, tfile[ i ] );

		return idx;
	}
	
	void add_item( str::t_string sRoot, t_pb &pb, CFileIndex &fi, CFileIndex::SBlockItem *p )
	{
//		CFileIndex::SBlockItem *p = fi.getItem( b );
		if ( !p || 0 > p->size )
			return;

		// Set our name
		pb[ "name" ] = fi.getBlob( p->name );
		pb[ "path" ] = disk::FilePath< str::t_char, str::t_string >( sRoot, fi.getBlob( p->name ) );
		pb[ "szstr" ] = str::SizeStr< str::t_char, str::t_string >( (double)p->size, 1024., 2 ) + "B";
		
		if ( 0 != ( p->flags & disk::eFileAttribDirectory ) )
//			pb[ "colour" ] = "#40a000",
//			pb[ "hi_colour" ] = "#a0ff00";
			pb[ "colour" ] = "#804000",
			pb[ "hi_colour" ] = "#ffa000";
		else
			pb[ "colour" ] = "#004080",
			pb[ "hi_colour" ] = "#00a0FF";

			// Item size, some renderers have a problem with zero
			pb[ "size" ] = ( 0 < p->size ) ? p->size : 1;
	}
	
	long index_callback( CFileIndex *pFi, void *p )
	{
		// Ensure valid
		if ( !pFi || !p )
			return 1;

		// Update once per second
		const unsigned long t = (const unsigned long)time( 0 );
		if ( t == g_lLastUpdate )
			return 0;
		g_lLastUpdate = t;

		// Check for cancel
		if ( tq::get( "indexer.job.cancel", "." ).ToLong() )
			return 1;

		// Recover job pointer
		t_pb &job = *((t_pb*)p);

		// Create a new root
		t_pb pb;
		t_pb &r = pb[ "children" ]; r.is_list( 1 );

		str::tc_int64 nFree = job[ "drive" ][ "bytes_free" ].ToInt64();
		
		// Add free space info
		t_pb &pbFree = r[ "free" ];
		pbFree[ "name" ] = "used";
		pbFree[ "colour" ] = "#008000",
		pbFree[ "hi_colour" ] = "#00a000";
		pbFree[ "size" ] = ( 0 < nFree ) ? nFree : 1;
		pbFree[ "path" ] = "[ Free Space ]";
		pbFree[ "szstr" ] = str::SizeStr< str::t_char, str::t_string >( (double)job[ "drive" ][ "bytes_free" ].ToInt64(), 1024., 2 ) + "B";

		// Add disk items
		add_items( job[ "params" ][ "root" ].str(), r[ "used" ], *pFi, pFi->getRoot(), job[ "top" ].ToLong(), job[ "depth" ].ToLong() );
		
		// Set it into the thread queue
		tq::set( "indexer.out", pb, "." );

		return 0;
	}

	long index_thread( CThread *t, void *p )
	{
		// Indexing object
		CFileIndex fi;
		
		// Until we get the kill signal
		while ( t->getStopEvent().Wait( 100 ) )
		{
			// Do we want to run
			t_pb job = tq::get( "indexer.job", "." );
			if ( job[ "run" ].ToLong() )
			{
				// Clear run and cancel flags
				tq::set( "indexer.job.cancel", 0, "." );
				tq::set( "indexer.job.run", 0, "." );
				
				str::t_string root = job[ "params" ][ "root" ].str();
				if ( root.length() )
				{
					long lMin = 0;
					fi.Init( 1024 * 1024, 0, root.c_str() );
					fi.setCallback( index_callback, &job );
					while ( 0 < fi.Index( fi.getRoot(), root.c_str(), lMin, lMin + 1, t->getStopFlag() ) ) 
						lMin++;

					// Final update
					g_lLastUpdate = 0;
					index_callback( &fi, 0 );
					fi.Destroy();
					tq::set( "indexer.progress", 0, "." );

				} // end if

			} // end if

		} // end while
		
		// Clear indexer data on exit
		tq::set( "indexer", "" );

		return 0;
	}
