#ifndef _LOCK_H
#define _LOCK_H

class CriticalSection : public boost::noncopyable
{
public:
	CriticalSection() { InitializeCriticalSection(&_critical); }
	~CriticalSection() { DeleteCriticalSection(&_critical); }
	inline void Enter() { EnterCriticalSection(&_critical); }
	inline void Leave() { LeaveCriticalSection(&_critical); }
private:
	CRITICAL_SECTION _critical;
};

class Lock
{
public:
	Lock(CriticalSection* critical) : _critical(critical) { _critical->Enter(); }
	~Lock() { _critical->Leave(); }
private:
	CriticalSection* _critical;
};



class RWLock
{
public:
	RWLock() 
	{
		_readcount = 0; 
	}
	void EnterRead()
	{
		_writer.Enter();
		_mutex.Enter();
		if( ++_readcount == 1 ) _reader.Enter();
		_mutex.Leave();
		_writer.Leave();
	}
	void LeaveRead()
	{
		_mutex.Enter();
		if( --_readcount == 0 ) _reader.Leave();
		_mutex.Leave();
	}
	void EnterWrite()
	{
		_writer.Enter();
		_reader.Enter();
	}
	void LeaveWrite()
	{
		_reader.Leave();
		_writer.Leave();
	}	
private:
	CriticalSection _reader;
	CriticalSection _writer;
	CriticalSection _mutex;
	int _readcount;
};

#endif
