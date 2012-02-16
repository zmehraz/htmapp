
#pragma once

class CFileIndex
{
public:

	typedef long long	t_size;
	typedef long		t_offset;
	typedef char		t_char;

	/// String type
	typedef std::basic_string< t_char >	t_string;

public:

	struct SBlockItem
	{
		/// Offset to parent block item
		SBlockItem		*parent;

		/// Offset to next block item
		SBlockItem		*next;

		/// Offset to child item
		SBlockItem		*child;

		/// Item size
		t_size			size;

		/// Item name
		const t_char	*name;
	};

public:

	/// Default constructor
	CFileIndex();

	/// Destructor
	virtual ~CFileIndex();

	/// Releases all resources
	void Destroy();

	/// Allocate initial storage for indexing
	bool Init( t_size nBlock, t_size nBlob );

	/// Initializes the specified block
	void InitBlock( SBlockItem *p );

	/// Returns a pointer to the root block
	SBlockItem* getRoot() { return &m_root; }

	/// Returns a new uninitialized block
	SBlockItem* NewBlock();

	/// Returns a new blob, blobs cannot be deleted
	void* NewBlob( const void* p, t_size size );

	/// Removes a blocks size from the parent chain counts
	void RemoveCount( SBlockItem *pBlock );

	/// Removes a blocks children
	void RemoveChildren( SBlockItem *pBlock );

	/// Removes the specified block, the block is not released, but can be reused
	void RemoveBlock( SBlockItem *pBlock );

	/// Add a sibling to the specified block
	SBlockItem* AddSibling( SBlockItem *pBlock, const t_char *name, t_size sz_name, t_size size );

	/// Adds a child item to the specified block
	SBlockItem* AddChild( SBlockItem *pBlock, t_char *name, t_size sz_name, t_size size );

	/// Indexes the specified root folder
	long Index( const t_string &sRoot, long lDepth, SBlockItem *p = 0 );

private:

	/// Root block
	SBlockItem				m_root;

	/// Deleted block chain
	SBlockItem				*m_deleted;

	/// Block array
	std::vector< char >		m_blocks;

	/// Block offset
	t_offset				m_block_offset;

	/// Blob data
	std::vector< char >		m_blobs;

	/// Blob offset
	t_offset				m_blob_offset;

};

