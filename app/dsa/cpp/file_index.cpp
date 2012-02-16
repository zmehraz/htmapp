// pnet_video.cpp

#include <string>
#include <vector>
#include <string.h>

#include "htmapp.h"
#include "file_index.h"

CFileIndex::CFileIndex()
{
	m_deleted = 0;
	m_blob_offset = 0;
	m_block_offset = 0;
	memset( &m_root, 0, sizeof( m_root ) );
}

CFileIndex::~CFileIndex()
{
	Destroy();
}

void CFileIndex::Destroy()
{
	// Clear data
	m_deleted = 0;
	m_blob_offset = 0;
	m_block_offset = 0;
	memset( &m_root, 0, sizeof( m_root ) );

	// Release memory
	m_blocks.clear();
	m_blobs.clear();
}

bool CFileIndex::Init( t_size nBlock, t_size nBlob )
{
	// Out with the old
	Destroy();

	// Impose reasonable minimums
	nBlob = std::max( nBlob, (t_size)1024 );
	nBlock = std::max( nBlock, (t_size)1024 );

	try { m_blocks.resize( nBlock ); }
	catch ( ... ) { Destroy(); return false; }

	try { m_blobs.resize( nBlob ); }
	catch ( ... ) { Destroy(); return false; }

	return true;
}

CFileIndex::SBlockItem* CFileIndex::NewBlock()
{
	// Is there a deleted block we can use?
	if ( m_deleted )
	{	SBlockItem *p = m_deleted;
		m_deleted = p->next;
		return p;
	} // end if

	// Ensure space
	if ( ( m_block_offset + sizeof( SBlockItem ) ) < m_blocks.size() )
	{
		// Are we at least initialized?
		if ( 0 >= m_blocks.size() )
			return 0;

		// Attempt to allocate space
		try { m_blocks.resize( m_blocks.size() << 1 ); }
		catch ( ... ) { return 0; }

	} // end if

	// Allocate a block
	SBlockItem *p = (SBlockItem*)&m_blocks[ m_block_offset ];
	m_block_offset += sizeof( SBlockItem );

	return p;
}

void CFileIndex::RemoveCount( SBlockItem *pBlock )
{
	if ( !pBlock || !pBlock->size )
		return;

	// Remove size from the parent chain
	t_size size = pBlock->size;
	while ( pBlock->parent )
		pBlock->parent -= size,
		pBlock = pBlock->parent;
}

void CFileIndex::RemoveChildren( SBlockItem *pBlock )
{
	// Remove all children
	if ( !pBlock || !pBlock->child )
		return;

	// Remove children of the children
	if ( pBlock->child->child )
		RemoveCount( pBlock->child ),
		RemoveChildren( pBlock->child );

	// Remove children in the list
	SBlockItem *p = pBlock->child;
	while ( p->next ) 
		RemoveCount( p->child ),
		RemoveChildren( p = p->next );

	// Put this list in the deleted items
	p = m_deleted;
	m_deleted = pBlock->child;
	pBlock->child = 0;
}

void CFileIndex::RemoveBlock( SBlockItem *pBlock )
{
	if ( !pBlock )
		return;

	// Remove children
	RemoveChildren( pBlock );

	// Update parent
	if ( pBlock->parent )
	{
		// Are we the first in the list?
		if ( pBlock->parent->child == pBlock )
			pBlock->parent->child = pBlock->next;

		// Find us in the sibling list
		else
		{	SBlockItem *p = pBlock->parent->child;
			while ( p )
			{
				// Is this the previous node?
				if ( p->next == pBlock )
				{	p->next = pBlock->next;
					break;
				} // end if

				// Next sibling
				p = p->next;

			} // end while

		} // end else

	} // end if

	// Update counts
	RemoveCount( pBlock->child ),

	// Add us to the deleted chain
	pBlock->next = m_deleted;
	m_deleted = pBlock;
}

void* CFileIndex::NewBlob( const void* p, t_size size )
{
	if ( 0 >= size )
	{
		// Must have valid pointer if no size
		if ( !p )
			return 0;

		// NULL terminiated
		size = 0;
		while ( ((char*)p)[ size ] )
			size++;

		// Non-zero size?
		if ( 0 >= size )
			return 0;

	} // end if

	// We're allocating an implicit null character
	t_size asize = size + 1;
	
	// Ensure space
	if ( ( m_blob_offset + asize ) < m_blobs.size() )
	{
		// Are we at least initialized?
		if ( 0 >= m_blobs.size() )
			return 0;

		// Increase size / we're allocating an implicit null character
		t_size newsize = m_blobs.size();
		while ( newsize && newsize < asize )
			newsize <<= 1;

		// Did it overflow?
		if ( newsize < asize )
			return 0;
 
		// Attempt to allocate space
		try{ m_blobs.resize( newsize ); }
		catch( ... ) { return 0; }

	} // end if

	// Allocate a block
	void *pBlob = (void*)&m_blobs[ m_blob_offset ];
	m_blob_offset += asize;

	// Copy data if needed
	if ( p )
		memcpy( pBlob, p, size );
	
	// Null terminate
	((char*)pBlob)[ size ] = 0;

	return pBlob;
}

CFileIndex::SBlockItem* CFileIndex::AddSibling( SBlockItem *pBlock, const t_char *name, t_size sz_name, t_size size )
{
	// Note, the root node can't have siblings
	if ( !pBlock || pBlock == &m_root )
		return 0;

	SBlockItem *p = NewBlock();
	if ( !p )
		return 0;

	// Add as sibling
	pBlock->next = p;

	// Initialize block
	p->parent = pBlock->parent;
	p->next = 0;
	p->child = 0;
	p->size = size;
	p->name = name  ? (const t_char*)NewBlob( name, sz_name ) : 0;

	// Update sizes
	pBlock = p->parent;
	while ( pBlock )
		pBlock->size += size, pBlock = pBlock->parent;

	return p;
}

CFileIndex::SBlockItem* CFileIndex::AddChild( SBlockItem *pBlock, t_char *name, t_size sz_name, t_size size )
{
	// NULL means child of the root
	if ( !pBlock )
		return pBlock = &m_root;

	SBlockItem *p = NewBlock();
	if ( !p )
		return 0;

	// Add as child
	p->next = pBlock->child;
	pBlock->child = p;

	// Initialize block
	p->parent = pBlock;
	p->next = 0;
	p->child = 0;
	p->size =- size;
	p->name = name  ? (const t_char*)NewBlob( name, sz_name ) : 0;

	// Update sizes
	while ( pBlock )
		pBlock->size += size, pBlock = pBlock->parent;

	return p;
}

long CFileIndex::Index( const t_string &sRoot, long lDepth, SBlockItem *p )
{
	// Start with root if needed
	if ( !p )
		p = &m_root;

	// Assume no additions
	long lAdded = 0;

	str::Print( "%s\n", sRoot.c_str() );

	// Is this our working depth?
	if ( !lDepth )
	{
		disk::SFindData fd; disk::HFIND hFind;
		if ( disk::c_invalid_hfind != ( hFind = disk::FindFirst( sRoot.c_str(), "*", &fd ) ) )
		{
			do { 

				lAdded++;

				str::Print( "%s\n", disk::FilePath< t_char, t_string >( sRoot, fd.szName ).c_str() );
				// AddChild( p, fd.szName, 0, fd.llSize );

			} while ( disk::FindNext( hFind, &fd ) );

			disk::FindClose( hFind );

		} // end if

	} // end if

	// Keep digging
	else
	{
		// For every child
		p = p->child;
		while ( p )
		{
			// Process this item if it has a valid name
			if ( p->name )
				lAdded += Index( disk::FilePath< t_char, t_string >( sRoot, p->name ), lDepth - 1, p );

			// Next item
			p = p->next;

		} // end while

	} // end else

	return lAdded;
}

