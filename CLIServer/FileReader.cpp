#include "FileReader.h"


CFileReader::CFileReader(int buffer_size, char * path)
{
	this->buffer_size = buffer_size;
	buffer = (char*)malloc(buffer_size);
	fopen_s(&fd, path, "rb");
}


CFileReader::~CFileReader()
{
	fclose(fd);
	fd = nullptr;

	delete buffer;
	buffer = nullptr;
}

char const* CFileReader::GetBuffer()
{
	read_size = fread(buffer, 1, buffer_size, fd);
	return buffer;
}

const size_t CFileReader::GetBufferSize()
{
	return read_size;
}

const size_t CFileReader::FileSize()
{
	const size_t pos = ftell(fd);
	fseek(fd, 0, SEEK_END);

	const size_t size = ftell(fd);
	fseek(fd, pos, SEEK_SET);

	return size;
}
