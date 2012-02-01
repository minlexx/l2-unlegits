#pragma once

class DebugOneStackFrameInfo
{
public:
	DebugOneStackFrameInfo():
		m_name(NULL), m_fileName(NULL), m_moduleName(NULL), m_addr(0), m_addr_offset(0), m_line(0) {}
	DebugOneStackFrameInfo( const DebugOneStackFrameInfo& other );
	~DebugOneStackFrameInfo();
	const DebugOneStackFrameInfo& operator=( const DebugOneStackFrameInfo& other );
	bool operator==( const DebugOneStackFrameInfo& other );
public:
	void set(
		const char *name,
		unsigned long long addr,
		unsigned long long addr_offset,
		const char *fileName,
		unsigned int line,
		const char *moduleName
	);
	char               *getName() const { return m_name; }
	unsigned long long  getAddr() const { return m_addr; }
	unsigned long long  getAddrOffset() const { return m_addr_offset; }
	char               *getFileName() const { return m_fileName; }
	unsigned int        getLine() const { return m_line; }
	char               *getModuleName() const { return m_moduleName; }
public:
	char               *m_name;
	unsigned long long  m_addr;
	unsigned long long  m_addr_offset;
	char               *m_fileName;
	unsigned int        m_line;
	char               *m_moduleName;
};



class DebugStackTrace
{
public:
	DebugStackTrace();
	~DebugStackTrace();
public:
	void clear();
	void addStackFrame( DebugOneStackFrameInfo& ref );
	void logStackTrace();
protected:
	std::list<DebugOneStackFrameInfo> m_list;
};



class Debug
{
protected:
	Debug();
	~Debug();
	static Debug *s_instance;

public:
	static Debug *getInstance();
	static void freeInstance();

public:
	void createStackTrace( DebugStackTrace& traceObject );

protected:
	CRITICAL_SECTION   m_lock;
	HANDLE             m_symHandle;
	bool               m_initOK;
};
