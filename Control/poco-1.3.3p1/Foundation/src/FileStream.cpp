//
// FileStream.cpp
//
// $Id: //poco/1.3/Foundation/src/FileStream.cpp#1 $
//
// Library: Foundation
// Package: Streams
// Module:  FileStream
//
// Copyright (c) 2007, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/FileStream.h"
#include "Poco/Exception.h"
#if defined(POCO_OS_FAMILY_WINDOWS)
#include "FileStream_WIN32.cpp"
#else
#include "FileStream_POSIX.cpp"
#endif


namespace Poco {


FileIOS::FileIOS(std::ios::openmode defaultMode):
	_defaultMode(defaultMode)
{
	poco_ios_init(&_buf);
}


FileIOS::~FileIOS()
{
}


void FileIOS::open(const std::string& path, std::ios::openmode mode)
{
	clear();
	_buf.open(path, mode | _defaultMode);
}


void FileIOS::close()
{
	_buf.close();
}


FileStreamBuf* FileIOS::rdbuf()
{
	return &_buf;
}


FileInputStream::FileInputStream():
	FileIOS(std::ios::in),
	std::istream(&_buf)
{
}


FileInputStream::FileInputStream(const std::string& path, std::ios::openmode mode):
	FileIOS(std::ios::in),
	std::istream(&_buf)
{
	open(path, mode);
}


FileInputStream::~FileInputStream()
{
}


FileOutputStream::FileOutputStream():
	FileIOS(std::ios::out),
	std::ostream(&_buf)
{
}


FileOutputStream::FileOutputStream(const std::string& path, std::ios::openmode mode):
	FileIOS(std::ios::out),
	std::ostream(&_buf)
{
	open(path, mode);
}


FileOutputStream::~FileOutputStream()
{
}


FileStream::FileStream():
	FileIOS(std::ios::in | std::ios::out),
	std::iostream(&_buf)
{
}


FileStream::FileStream(const std::string& path, std::ios::openmode mode):
	FileIOS(std::ios::in | std::ios::out),
	std::iostream(&_buf)
{
	open(path, mode);
}


FileStream::~FileStream()
{
}


} // namespace Poco
