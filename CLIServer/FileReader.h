#pragma once
#include <stdlib.h>
#include <stdio.h>

class CFileReader
{
public:
	CFileReader() = delete;
	CFileReader(CFileReader&) = delete;
	CFileReader(int buffer_size, char * path);
	~CFileReader();

public:
	const char const * GetBuffer();
	const size_t GetBufferSize();
	const size_t FileSize();

private:
	FILE* fd;
	char* buffer;
	size_t read_size;

private:
	size_t buffer_size;
};

