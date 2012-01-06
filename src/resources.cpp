
#include "htmapp.h"

CHmResources::~CHmResources()
{
	Unload();
}

CHmResources::CHmResources( HMRES hRes )
{
	m_ptr = hRes;
}

CHmResources::CHmResources( const char *pModule )
{
	if ( pModule && *pModule )
		LoadFromModule( pModule );
	else
		m_ptr = 0;
}

void CHmResources::Unload()
{
	m_ptr = 0;
}

int CHmResources::LoadFromModule( const char *pModule )
{
	Unload();
	
	return 0;
}

int CHmResources::IsValid()
{
	return m_ptr ? 1 : 0;
}

void CHmResources::SetResourcePtr( HMRES hRes )
{
	m_ptr = hRes;
}

HMRES CHmResources::FindResource( const char *pRoot, const char *pName )
{
	if ( !pName || !*pName )
		return 0;
	
	unsigned long lName = zstr::Length( pName );
	const _SHmResourceInfo *p = m_ptr;
	while ( p && *p->name )
		if ( *p->name == *pName && p->sz_name == lName )
			if ( !str::Compare( p->name, p->sz_name, pName, lName ) )
				return p;
	
	return 0;
}

const void* CHmResources::Ptr( HMRES hRes )
{
	if ( !hRes )
		return 0;
	return (const void*)hRes->data;
}

CHmResources::t_size CHmResources::Size( HMRES hRes )
{
	if ( !hRes )
		return 0;
	return hRes->sz_data;
}

CHmResources::t_fn CHmResources::Fn( HMRES hRes )
{
	if ( !hRes )
		return 0;
	return (t_fn)hRes->data;
}

CHmResources::t_type CHmResources::Type( HMRES hRes )
{
	if ( !hRes )
		return 0;
	return hRes->type;
}

const char* CHmResources::Name( HMRES hRes )
{
	if ( !hRes )
		return 0;
	return hRes->name;
}
