#ifndef MEMSTREAM_HPP
#define MEMSTREAM_HPP

#include <iostream>

struct IMemBuf : std::streambuf
{
	IMemBuf(const char* base, size_t size)
	{
		char* p(const_cast<char*>(base));
		this->setg(p, p, p + size);
	}
};

struct IMemStream : virtual IMemBuf, std::istream
{
	IMemStream(const char* mem, size_t size) :
		IMemBuf(mem, size),
		std::istream(static_cast<std::streambuf*>(this))
	{
	}
}; 

struct OMemBuf : std::streambuf
{
	OMemBuf(char* base, size_t size)
	{
		this->setp(base, base + size);
	}
};

struct OMemStream : virtual OMemBuf, std::ostream
{
	OMemStream(char* base, size_t size) :
		OMemBuf(base, size),
		std::ostream(static_cast<std::streambuf*>(this))
	{
	}
};

#endif

