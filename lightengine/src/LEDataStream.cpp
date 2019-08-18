#include "LEDataStream.h"

#include "LEStringUtil.h"
#include "log.h"

template<typename T> LEDataStream& LEDataStream::operator >> (T& val) {
	read(static_cast<void*>(&val), sizeof(T));
}

String LEDataStream::getLine(bool trimAfter) {
	LEStringUtil::StrStreamType str;
	size_t c = LE_STREAM_TELE_SIZE - 1;

	// Keep the loop until you encounter a separator
	while (c == LE_STREAM_TELE_SIZE - 1) {
		c = readLine(m_TmpArea, LE_STREAM_TELE_SIZE - 1);
		str << m_TmpArea;
	}

	String retString(str.str());
	if (trimAfter) {
		LEStringUtil::trim(retString);
	}

	return retString;
}

String LEDataStream::getAsString(void) {

	// Read the entire buffer
	char* pBuf = new char[m_Size + 1];
	read(pBuf, m_Size);
	pBuf[m_Size] = '\0';
	String str;
	str.insert(0, pBuf, m_Size);
	delete[] pBuf;
	return str;
}

// LEMemoryDataStream
LEMemoryDataStream::LEMemoryDataStream(void* pMem, size_t size, bool freeOnClose) : LEDataStream() {
	m_Data = m_Pos = static_cast<uchar*>(pMem);
	m_Size = size;
	m_End = m_Data + m_Size;
	m_FreeOnClose = freeOnClose;
}

LEMemoryDataStream::LEMemoryDataStream(const String& name, void* pMem, size_t size, bool freeOnClose) : LEDataStream(name) {
	m_Data = m_Pos = static_cast<uchar*>(pMem);
	m_Size = size;
	m_End = m_Data + m_Size;
	m_FreeOnClose = freeOnClose;
}

LEMemoryDataStream::LEMemoryDataStream(LEDataStream& sourceStream, bool freeOnClose) : LEDataStream() {

	// Copy the data from another stream
	m_Size = sourceStream.size();
	m_Data = new uchar[m_Size];
	sourceStream.read(m_Data, m_Size);
	m_Pos = m_Data;
	m_End = m_Data + m_Size;
	m_FreeOnClose = freeOnClose;
}

LEMemoryDataStream::LEMemoryDataStream(LEDataStreamPtr& sourceStream, bool freeOnClose) : LEDataStream() {

	// Copy the data from another stream
	m_Size = sourceStream->size();
	m_Data = new uchar[m_Size];
	sourceStream->read(m_Data, m_Size);
	m_Pos = m_Data;
	m_End = m_Data + m_Size;
	m_FreeOnClose = freeOnClose;
}

LEMemoryDataStream::LEMemoryDataStream(const String& name, LEDataStream& sourceStream, bool freeOnClose) : LEDataStream(name) {

	// Copy the data from another stream
	m_Size = sourceStream.size();
	m_Data = new uchar[m_Size];
	sourceStream.read(m_Data, m_Size);
	m_Pos = m_Data;
	m_End = m_Data + m_Size;
	m_FreeOnClose = freeOnClose;
}

LEMemoryDataStream::LEMemoryDataStream(const String& name, const LEDataStreamPtr& sourceStream, bool freeOnClose) : LEDataStream(name) {

	// Copy the data from another stream
	m_Size = sourceStream->size();
	m_Data = new uchar[m_Size];
	sourceStream->read(m_Data, m_Size);
	m_Pos = m_Data;
	m_End = m_Data + m_Size;
	m_FreeOnClose = freeOnClose;
}

LEMemoryDataStream::LEMemoryDataStream(size_t size, bool freeOnClose) : LEDataStream() {
	m_Size = size;
	m_Data = new uchar[m_Size];
	m_Pos = m_Data;
	m_End = m_Data + m_Size;
	m_FreeOnClose = freeOnClose;
}

LEMemoryDataStream::LEMemoryDataStream(const String& name, size_t size, bool freeOnClose) : LEDataStream(name) {
	m_Size = size;
	m_Data = new uchar[m_Size];
	m_Pos = m_Data;
	m_End = m_Data + m_Size;
	m_FreeOnClose = freeOnClose;
}

LEMemoryDataStream::~LEMemoryDataStream() {
	close();
}

size_t LEMemoryDataStream::read(void* buf, size_t count) {
	size_t cnt = count;

	if (m_Pos + cnt > m_End) {
		cnt = m_End - m_Pos;
	}

	if (cnt == 0) {
		return 0;
	}

	memcpy(buf, m_Pos, cnt);
	m_Pos += cnt;
	return cnt;
}

size_t LEMemoryDataStream::readLine(char* buf, size_t maxCount, const String& delim) {

	// here deal with Unix and Windows files (based on DOS / windows text files at the end of each line)
	// there is a CR (carriage return) and LF (newline), and UNIX text has only one line break)

	bool trimCR = false;
	if (delim.find_first_of('\n') != String::npos) {
		trimCR = true;
	}

	// Find the location of the next delimiter ("\n")
	size_t pos = strcspn((const char*)m_Pos, delim.c_str());
	if (pos > maxCount) {
		pos = maxCount;
	}

	// Make sure that pos will not go beyond eof
	if (m_Pos + pos > m_End) {
		pos = m_End - m_Pos;
	}

	if (pos > 0) {
		memcpy(buf, (const void*)m_Pos, pos);
	}

	// Resets the current pointer position
	m_Pos += pos + 1;

	// Exclude CR (carriage return) and LF (line feed)
	if (trimCR && buf[pos - 1] == '\r') {

		// Delete '\r'
		--pos;
	}

	// Stop character
	buf[pos] = '\0';

	return pos;
}

size_t LEMemoryDataStream::skipLine(const String& delim) {

	// Find the location of the next delimiter ("\n")
	size_t pos = strcspn((const char*)m_Pos, delim.c_str());

	// Make sure that pos will not go beyond eof
	if (m_Pos + pos > m_End) {
		pos = m_End - m_Pos;
	}

	m_Pos += pos + 1;

	return pos;
}

void LEMemoryDataStream::skip(long count) {
	size_t newpos = (size_t)((m_Pos - m_Data) + count);
	assert(m_Data + newpos <= m_End);
	m_Pos = m_Data + newpos;
}

void LEMemoryDataStream::seek(size_t pos) {
	assert(m_Data + pos <= m_End);
	m_Pos = m_Data + pos;
}

size_t LEMemoryDataStream::tell(void) const {
	return m_Pos - m_Data;
}

bool LEMemoryDataStream::eof(void) const {
	return m_Pos >= m_End;
}

void LEMemoryDataStream::close(void) {
	if (m_FreeOnClose && m_Data) {
		delete[] m_Data;
		m_Data = 0;
	}
}

// LEFileStreamDataStream
LEFileStreamDataStream::LEFileStreamDataStream(std::ifstream* s, bool freeOnClose) : LEDataStream() {
	
	// Calculate the size
	m_pStream->seekg(0, std::ios_base::end);
	m_Size = m_pStream->tellg();
	m_pStream->seekg(0, std::ios_base::beg);
}

LEFileStreamDataStream::LEFileStreamDataStream(const String& name, std::ifstream* s, bool freeOnClose) : LEDataStream(name) {

	// Calculate the size
	m_pStream->seekg(0, std::ios_base::end);
	m_Size = m_pStream->tellg();
	m_pStream->seekg(0, std::ios_base::beg);
}

LEFileStreamDataStream::LEFileStreamDataStream(const String& name, std::ifstream* s, size_t size, bool freeOnClose) : LEDataStream(name) {
	m_Size = size;
}

LEFileStreamDataStream::~LEFileStreamDataStream() {
	close();
}

size_t LEFileStreamDataStream::read(void* buf, size_t count) {
	m_pStream->read(static_cast<char*>(buf), count);
	return m_pStream->gcount();
}

size_t LEFileStreamDataStream::readLine(char* buf, size_t maxCount, const String& delim) {
	if (delim.empty()) {
		LG("No delimiter provided", "FileStreamDataStream::readLine");
		assert(0);
	}

	if (delim.size() > 1) {
		LG("WARNING: FileStreamDataStream::readLine", "using only first delimeter");
	}

	// Here deal with Unix and Windows files
	bool trimCR = false;
	if (delim.at(0) == '\n') {
		trimCR = true;
	}

	// Because it does not include the stop character, all to maxCount + 1
	m_pStream->getline(buf, maxCount + 1, delim.at(0));
	size_t ret = m_pStream->gcount();

	// 3 cases
	// 1) encountered eof
	// 2) exceeds the cache space
	// 3) read an entire line - in this case the delimiter is in the stream but is not written to the cache, so
	// read the length of the data ret-1, ending at ret-2
	// All cases cache ends with null

	if (m_pStream->eof()) {

		// No problem
	}
	else if (m_pStream->fail()) {

		// Did we fail because of maxCount hit? No - no terminating character
		// in included in the count in this case
		if (ret == maxCount) {

			// clear failbit for next time 
			m_pStream->clear();
		}
		else {
			LG("Streaming error occurred", "FileStreamDataStream::readLine");
			assert(0);
		}
	}
	else {
		--ret;
	}

	// Exclude CR (carriage return) and LF (line feed)
	if (trimCR && buf[ret - 1] == '\r') {
		--ret;
		buf[ret] = '\0';
	}

	return ret;
}

size_t LEFileStreamDataStream::skipLine(const String& delim) {
	size_t c = LE_STREAM_TELE_SIZE - 1;
	size_t total = 0;
	while (c == LE_STREAM_TELE_SIZE - 1) {
		c = readLine(m_TmpArea, LE_STREAM_TELE_SIZE - 1);
		total += c;
	}
	return total;
}

void LEFileStreamDataStream::skip(long count) {

	// If the eof has been set, clear it
	m_pStream->clear();
	m_pStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios::cur);
}

void LEFileStreamDataStream::seek(size_t pos) {

	// If the eof has been set, clear it
	m_pStream->clear();
	m_pStream->seekg(static_cast<std::ifstream::pos_type>(pos), std::ios::beg);
}

size_t LEFileStreamDataStream::tell(void) const {

	// If eof has been set, then clear
	m_pStream->clear();
	return m_pStream->tellg();
}

bool LEFileStreamDataStream::eof(void) const {
	return m_pStream->eof();
}

void LEFileStreamDataStream::close(void) {
	if (m_pStream) {
		m_pStream->close();
		if (m_FreeOnClose) {
			delete m_pStream;
			m_pStream = 0;
		}
	}
}

// LEFileHandleDataStream
LEFileHandleDataStream::LEFileHandleDataStream(FILE* handle) : LEDataStream(), m_FileHandle(handle) {

	// Calculate the size
	fseek(m_FileHandle, 0, SEEK_END);
	m_Size = ftell(m_FileHandle);
	fseek(m_FileHandle, 0, SEEK_SET);
}

LEFileHandleDataStream::LEFileHandleDataStream(const String& name, FILE* handle) : LEDataStream(name), m_FileHandle(handle) {

	// Calculate the size
	fseek(m_FileHandle, 0, SEEK_END);
	m_Size = ftell(m_FileHandle);
	fseek(m_FileHandle, 0, SEEK_SET);
}

LEFileHandleDataStream::~LEFileHandleDataStream() {
	close();
}


size_t LEFileHandleDataStream::read(void* buf, size_t count) {
	return fread(buf, count, 1, m_FileHandle);
}

size_t LEFileHandleDataStream::readLine(char* buf, size_t maxCount, const String& delim) {
	
	// Here deal with Unix and Windows files
	bool trimCR = false;
	if (delim.find_first_of('\n') != String::npos) {
		trimCR = true;
	}

	size_t chunkSize = min(maxCount, (size_t)LE_STREAM_TELE_SIZE - 1);
	size_t totalCount = 0;
	size_t readCount;
	while (chunkSize && (readCount = fread(m_TmpArea, chunkSize, 1, m_FileHandle))) {

		// Stop character
		m_TmpArea[readCount] = '\0';

		// Find the location of the first delimiter
		size_t pos = strcspn(m_TmpArea, delim.c_str());

		if (pos < readCount) {

			// Find the delimiter and reposition the location
			fseek(m_FileHandle, pos - readCount + 1, SEEK_CUR);
		}

		if (pos > 0) {

			// If you find CR, stop early
			if (trimCR && m_TmpArea[pos - 1] == '\r') {
				--pos;
			}

			if (buf) {
				memcpy(buf, (const void*)m_TmpArea, pos);
				buf[pos] = '\0';
			}

			totalCount += pos;
		}

		if (pos < readCount) {
			break;
		}

		// Resize the block for the next lookup
		chunkSize = min(maxCount - totalCount, (size_t)LE_STREAM_TELE_SIZE - 1);
	}

	return totalCount;
}

size_t LEFileHandleDataStream::skipLine(const String& delim) {

	// Reuse readLine (), but not copy the data
	char* nullBuf = 0;
	return readLine(nullBuf, 1024, delim);
}

void LEFileHandleDataStream::skip(long count) {
	fseek(m_FileHandle, count, SEEK_CUR);
}

void LEFileHandleDataStream::seek(size_t pos) {
	fseek(m_FileHandle, pos, SEEK_SET);
}

size_t LEFileHandleDataStream::tell(void) const {
	return ftell(m_FileHandle);
}

bool LEFileHandleDataStream::eof(void) const {
	return feof(m_FileHandle) != 0;
}

void LEFileHandleDataStream::close(void) {
	fclose(m_FileHandle);
	m_FileHandle = 0;
}
