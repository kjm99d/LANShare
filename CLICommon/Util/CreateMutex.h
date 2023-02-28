#pragma once
#include <Windows.h>
#include <string>
#pragma hdrstop

/**
 * @brief Windows에서 중복 실행 방지를 위한 뮤텍스 클래스
 */
class CCreateMutex {
public:
    /**
     * @brief 뮤텍스 객체 생성
     * @param name 뮤텍스 이름
     */
    CCreateMutex(const std::string& name) : handle_(CreateMutexA(NULL, TRUE, name.c_str())) {}

    /**
     * @brief 뮤텍스 객체가 이미 존재하는지 확인
     * @return 이미 존재하면 true, 그렇지 않으면 false
     */
    bool exist() const { return GetLastError() == ERROR_ALREADY_EXISTS; }

    /**
     * @brief 뮤텍스 객체 해제
     */
    void Release() { ReleaseMutex(handle_); }

    /**
     * @brief 뮤텍스 객체 핸들 종료
     */
    ~CCreateMutex() { CloseHandle(handle_); }

private:
    HANDLE handle_;
};

/**
 * @brief 프로그램이 이미 실행 중인지 확인하는 예제코드
 * @param lock_file_name 락 파일 이름
 * @return 이미 실행 중이면 true, 그렇지 않으면 false

bool CheckProgramRunning(const std::string& lock_file_name)
{
    Mutex mutex(lock_file_name);

    if (mutex.IsAlreadyExist())
    {
        return true;
    }

    return false;
}
*/