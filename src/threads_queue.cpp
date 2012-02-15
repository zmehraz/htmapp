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

#include "htmapp.h"

namespace tq
{

	t_pb g_htmapp_pb;

	CLock g_htmapp_pb_lock;

	t_pb& pb() 
	{ 
		return g_htmapp_pb; 
	}

	CLock& lock() 
	{ 
		return g_htmapp_pb_lock; 
	}

	bool set( const str::t_string &sKey, const str::t_string &sValue, const str::t_string &sep )
	{
		CScopeLock sl( g_htmapp_pb_lock );
		if ( !sl.isLocked() )
			return false;

		if ( sep.length() )
			g_htmapp_pb.at( sKey, sep ) = sValue;
		else
			g_htmapp_pb[ sKey ] = sValue;

		return true;
	}

	str::t_string get( const str::t_string &sKey, const str::t_string &sep )
	{
		CScopeLock sl( g_htmapp_pb_lock );
		if ( !sl.isLocked() )
			return str::t_string();

		if ( sep.length() )
			return g_htmapp_pb.at( sKey, sep ).str();

		return g_htmapp_pb[ sKey ].str();
	}

//------------------------------------------------------------------

	CThreadPool g_htmapp_tp;

	long start( t_tqfunc f )
	{
tcM;
		return g_htmapp_tp.start( f ); 
	}

	bool stop( long id )
	{	return g_htmapp_tp.stop( id ); }

	CThreadPool::CThreadPool()
	{	m_id = 0; 
		m_enable = true; 
		Start();
	}

	CThreadPool::~CThreadPool()
	{	m_enable = false;
		Stop(); 
	}

	long CThreadPool::DoThread( void* x_pData )
	{
		// While not stopped
		while( getStopEvent().Wait( m_event, 1000 ) )
		{
			// Function
			long id = -1;
			t_tqfunc f = 0;

tcM;
			// Command waiting?
			if ( m_event.Wait( 0 ) )
			{
tcM;
				// Grab first command
				CScopeLock sl( m_lock );
				if ( sl.isLocked() )
				{
tcM;
					// Grab waiting command
					if ( m_cmds.size() )
						id = m_cmds.begin()->first,
						f = m_cmds.begin()->second,
						m_cmds.erase( m_cmds.begin() );

					// Reset the event
					else
						m_event.Reset();

				} // end scope locked

			} // end if

			// Command?
			if ( 0 <= id )
			{
tcM;
				// New thread?
				if ( f )
					m_tp[ id ].Run( f, true );

				// Kill thread if that id exists
				else 
				{	t_threadpool::iterator it = m_tp.find( id );
					if ( m_tp.end() != it )
						m_tp.erase( it );
				} // end else

			} // end if

		} // end while

		// Cleanup expired thread objects
		for( t_threadpool::iterator it = m_tp.begin(); m_tp.end() != it; )
			if ( it->second.isRunning() )
				m_tp.erase( it++ );
			else
				it++;

		return 0;
	}

	long CThreadPool::EndThread( void* x_pData )
	{
		// Delete all threads
		m_tp.clear();

		return 0;
	}

	long CThreadPool::start( t_tqfunc f )
	{
tcM;
		if ( !m_enable )
			return -1;

tcM;
		CScopeLock sl( m_lock );
		if ( !sl.isLocked() )
			return -1;

tcM;
		// Create a new thread object and start it
		long id = m_id++;

		// Send start thread command
		m_cmds[ id ] = f;

		// New command waiting
		m_event.Signal();

		// Return the thread id
		return id;
	}

	bool CThreadPool::stop( long id )
	{
		if ( !m_enable )
			return false;

		CScopeLock sl( m_lock );
		if ( !sl.isLocked() )
			return false;

		// Do we have such a thread?
		if ( id <= m_id )
			return false;

		// Stop signal
		m_cmds[ id ] = 0;

		// New command waiting
		m_event.Signal();

		return true;
	}

}; // namespace tq