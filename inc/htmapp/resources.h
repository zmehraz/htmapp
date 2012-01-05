
#pragma once

#if !defined( HTMAPP_RESOURCES )
#	define HTMAPP_NORESOURCES 1
#endif

#if defined( HTMAPP_RESOURCES )
	typedef SHmResourceInfo _SHmResourceInfo;
#else
	struct _SHmResourceInfo
	{
		const char *   name;
		unsigned long  sz_name;
		
		const char *   data;
		unsigned long  sz_data;
		
		unsigned long  type;
	};
#endif

typedef const _SHmResourceInfo* HMRES;

class CHmResources
{
public:

	/// Resource function type
	typedef int (*t_fn)( const TPropertyBag< char > &in, std::basic_string< char > &out );
	
	/// Resource size type
	typedef unsigned long t_size;
	
	/// Type type
	typedef unsigned long t_type;

public:

	/// Constructor
	CHmResources()
	{
#if defined( HTMAPP_RESOURCES )
		m_ptr = _htmapp_resources;
#else
		m_ptr = 0;
#endif
	}

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
	int IsResources();

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

