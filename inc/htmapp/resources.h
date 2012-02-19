
#pragma once

struct _SHmResourceInfo
{
	const char *   name;
	unsigned long  sz_name;
	
	const char *   data;
	unsigned long  sz_data;
	
	unsigned long  type;
};

typedef const _SHmResourceInfo* HMRES;

/// Pointer to embedded resources
extern "C" const _SHmResourceInfo* _htmapp_resources_ptr;

/// Use this macro to set the resource pointer
#define HTMAPP_INIT_RESOURCES()	( _htmapp_resources_ptr = (const _SHmResourceInfo*)_htmapp_resources )

class CHmResources
{
public:

	/// Resource function type
	typedef int (*t_fn)( TPropertyBag< char > &in, TPropertyBag< char > &out );
	
	/// Resource size type
	typedef unsigned long t_size;
	
	/// Type type
	typedef unsigned long t_type;

public:

	/// Constructor
	CHmResources();

	/// Constructor
	CHmResources( HMRES hRes );

	/// Constructs class from resource name
	CHmResources( const char *pModule );
	
	/// Destructor
	~CHmResources();

	/// Unload resources
	void Unload();
	
	/// Constructs class from resource name
	int LoadFromModule( const char *pModule );
	
	/// Returns non-zero if there is a resource list
	int IsValid();

	/// Sets the pointer to the resource array
	void SetResourcePtr( HMRES hRes );

	/// Returns resource data
	HMRES FindResource( const char *pRoot, const char *pName );

	/// Returns specified resource pointer
	const void* Ptr( HMRES hRes );

	/// Returns specified resource function pointer
	t_size Size( HMRES hRes );

	/// Returns specified resource function pointer
	t_fn Fn( HMRES hRes );

	/// Retrns resource type
	t_type Type( HMRES hRes );
	
	/// Returns the resource name for the specified index
	const char* Name( HMRES hRes );

private:

	/// Pointer to resources
	const _SHmResourceInfo		*m_ptr; 
};

