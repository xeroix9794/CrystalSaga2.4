#ifndef LEDataStream_H
#define LEDataStream_H

#include "LEEffPrerequisites.h"
#include "LESharedPtr.h"

/*
The generic class is used to package the read data

@remarks
This class performs basically the same tasks as std::basic_istream,
except that it does not have any formatting capabilities, and is
designed to be subclassed to receive data from multiple sources,
including libraries which have no compatiblity with the STL's
stream interfaces. As such, this is an abstraction of a set of
wrapper classes which pretend to be standard stream classes but
can actually be implemented quite differently.
@par
Usually if the plug-in or application to provide ArchiveFactory, it should also provide access to that kind of
Archive data stream of the derived class of LEDataStream, unless provided by default to meet the requirements
@note
Due to the performance requirements, there is no guarantee that it is thread safe. If you want to access the data stream asynchronously
You have to organize your own mutex to avoid competition
*/

class LIGHTENGINE_API LEDataStream {
public:

	// The constructor is used to create an anonymous stream
	LEDataStream() : m_Size(0) {}

	// The constructor is used to create named streams
	LEDataStream(const String& name) : m_Name(name), m_Size(0) {}
	virtual ~LEDataStream() {}

	// Flow operation
	template<typename T> LEDataStream& operator >> (T& val);
	
	/* 
	Read the data of the desired byte from the stream and stop when you encounter eof.
	@param buf Cached pointer
	@param count Need to read the number of bytes
	@returns Read the number of bytes
	*/
	virtual size_t read(void* buf, size_t count) = 0;
	
	/*
	Get a row of data from the stream
	@remarks
	The delimiter is not included in the returned data and is skipped, so the next time it is not read 
	Will appear. The cache will contain a stop character.
	@param buf Cached pointer
	@param maxCount Read the maximum length of a row of data, excluding the stop character
	@param delim Separator
	@returns The number of bytes read, excluding the stop character
	*/
	virtual size_t readLine(char* buf, size_t maxCount, const String& delim = "\n") = 0;

	/*
	Returns a String containing the next line of data, optionally
	trimmed for whitespace.
	@remarks
	This is a convenience method for text streams only, allowing you to
	retrieve a String object containing the next line of data. The data
	is read up to the next newline character and the result trimmed if
	required.
	@param
	trimAfter If true, the line is trimmed for whitespace (as in
	String.trim(true,true))
	*/
	virtual String getLine(bool trimAfter = true);

	virtual String getAsString(void);

	/*
	Skip the line from the stream
	@param delim Line delimiter
	@returns Returns the number of bytes skipped
	*/
	virtual size_t skipLine(const String& delim = "\n") = 0;

	// Skip the specified number of bytes, it can also be a negative value, then skip the data to set the byte
	virtual void skip(long count) = 0;

	// Move the stream read pointer to the specified byte
	virtual void seek(size_t pos) = 0;

	// Returns the offset of the current pointer to the end
	virtual size_t tell(void) const = 0;

	// Returns true if eof is reached
	virtual bool eof(void) const = 0;

	// Close stream
	virtual void close(void) = 0;

	// Returns the size of the stream's data, if it is not possible.
	size_t size(void) const { return m_Size; }

	// Returns the name of the stream, if any
	const String& getName(void) const { return m_Name; }

protected:

	// The name used to identify the resource (such as the resource name) (optional)
	String m_Name;
	
	// The size of the data in the stream (if it can not determine the size of the data can be 0)
	size_t m_Size;

#define LE_STREAM_TELE_SIZE 128
	
	// Format read the temporary buffer
	char m_TmpArea[LE_STREAM_TELE_SIZE];
};

typedef LESharedPtr<LEDataStream> LEDataStreamPtr;
typedef std::list<LEDataStreamPtr> LEDataStreamList;
typedef LESharedPtr<LEDataStreamList> LEDataStreamListPtr;

// A generic derived class for DataStream that handles stream data in memory.
class LIGHTENGINE_API LEMemoryDataStream : public LEDataStream {
public:

	/* 
	Wrap existing blocks of memory into streams
	@param pMem Existing memory pointer
	@param size The size of the memory block (bytes)
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/

	LEMemoryDataStream(void* pMem, size_t size, bool freeOnClose = false);
	/*
	Wrap existing blocks of memory into named streams
	@param name The name of the stream
	@param pMem Existing memory pointer
	@param size The size of the memory block (bytes)
	@param freeOnClose The size of the memory block (if the word is true, the memory will be freed when the stream is released.
	*/

	LEMemoryDataStream(const String& name, void* pMem, size_t size, bool freeOnClose = false);

	/*
	Create a stream based on another stream.
	@param sourceStream Another data stream
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/
	LEMemoryDataStream(LEDataStream& sourceStream, bool freeOnClose = true);
	
	/*
	Create a stream based on another stream.
	@param sourceStream Another data stream (weak reference)
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/
	LEMemoryDataStream(LEDataStreamPtr& sourceStream, bool freeOnClose = true);
	
	/*
	Create a named stream based on another stream
	@param name The name of the stream
	@param sourceStream Another data strea
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/
	LEMemoryDataStream(const String& name, LEDataStream& sourceStream, bool freeOnClose = true);

	/* 
	Create a named stream based on another stream
	@param name The name of the stream
	@param sourceStream Another data stream (weak reference)
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/
	LEMemoryDataStream(const String& name, const LEDataStreamPtr& sourceStream, bool freeOnClose = true);
	
	/* 
	Create an empty stream
	@param size The size of the stream
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/
	LEMemoryDataStream(size_t size, bool freeOnClose = true);
	
	/*
	Create an empty named stream
	@param name The name of the stream
	@param size The size of the stream
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/
	LEMemoryDataStream(const String& name, size_t size, bool freeOnClose = true);

	~LEMemoryDataStream();

	// Get the start pointer of this stream memory block
	uchar* getPtr(void) { return m_Data; }

	// Get the pointer to the current point
	uchar* getCurrentPtr(void) { return m_Pos; }

	// Sets whether to free memory when the stream is off
	void setFreeOnClose(bool free) { m_FreeOnClose = free; }

	size_t read(void* buf, size_t count);
	size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");
	size_t skipLine(const String& delim = "\n");
	void skip(long count);
	void seek(size_t pos);
	size_t tell(void) const;
	bool eof(void) const;
	void close(void);

protected:

	// Pointer to the beginning of the data field
	uchar* m_Data;

	// A pointer to the current location
	uchar* m_Pos;

	// Pointer to the end of the data field
	uchar* m_End;

	// Whether or not to delete the word data when closed
	bool m_FreeOnClose;
};

typedef LESharedPtr<LEMemoryDataStream> LEMemoryDataStreamPtr;

// A generic derived class for DataStream that handles the data for std :: basic_istream
class LIGHTENGINE_API LEFileStreamDataStream : public LEDataStream {
public:

	/*
	Build streams from standard stl streams
	@param STL stream pointer
	@param freeOnClose If true, the memory will be freed when the stream is released
	*/
	LEFileStreamDataStream(std::ifstream* s, bool freeOnClose = true);
	
	/* 
	Build a naming stream from the standard stl stream
	@param name The name of the stream
	@param STL Flow pointer
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/
	LEFileStreamDataStream(const String& name, std::ifstream* s, bool freeOnClose = true);

	/*
	Build a name stream from the standard STL stream and tell its size
	@remarks
	This variant tells the class the size of the stream too, which
	means this class does not need to seek to the end of the stream
	to determine the size up-front. This can be beneficial if you have
	metadata about the contents of the stream already.
	@param name The name of the stream
	@param STL Flow pointer
	@param size Stream size (bytes)
	@param freeOnClose If true, the memory will be freed when the stream is released.
	*/
	LEFileStreamDataStream(const String& name, std::ifstream* s, size_t size, bool freeOnClose = true);

	~LEFileStreamDataStream();

	size_t read(void* buf, size_t count);
	size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");
	size_t skipLine(const String& delim = "\n");
	
	void skip(long count);
	void seek(size_t pos);
	size_t tell(void) const;
	bool eof(void) const;
	void close(void);

protected:
	std::ifstream* m_pStream;
	bool m_FreeOnClose;
};

// A generic derived class for DataStream that handles data for C-style file processing.
class LIGHTENGINE_API LEFileHandleDataStream : public LEDataStream {
public:

	// Build streams from c style files
	LEFileHandleDataStream(FILE* handle);

	// Create a naming stream from the C style file
	LEFileHandleDataStream(const String& name, FILE* handle);
	~LEFileHandleDataStream();

	size_t read(void* buf, size_t count);
	size_t readLine(char* buf, size_t maxCount, const String& delim = "\n");
	size_t skipLine(const String& delim = "\n");
	
	void skip(long count);
	void seek(size_t pos);
	size_t tell(void) const;
	bool eof(void) const;
	void close(void);

protected:
	FILE* m_FileHandle;
};

#endif