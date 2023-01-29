#include "FileReader.h"


/**
 * 할당될 버퍼크기와, 파일의 경로를 입력 받는 생성자.
 * 
 * \param buffer_size - 내부적으로 파일에서 데이터를 읽을 버퍼크기.
 * \param path - 읽을 파일의 경로
 */
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

/**
 * 읽은 파일의 데이터를 갖고 있는 버퍼를 반환해주는 함수.
 * 
 * \return 
 */
char const* CFileReader::GetBuffer()
{
	read_size = fread(buffer, 1, buffer_size, fd);
	return buffer;
}

/**
 * 파일의 버퍼크기를 구하는 함수.
 * 
 * \return 
 */
const size_t CFileReader::GetBufferSize()
{
	return read_size;
}

/**
 * 파일의 크기를 구하는 함수.
 * 
 * \return 
 */
const size_t CFileReader::FileSize()
{
	const size_t pos = ftell(fd);
	fseek(fd, 0, SEEK_END);

	const size_t size = ftell(fd);
	fseek(fd, pos, SEEK_SET);

	return size;
}
