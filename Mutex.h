#include <windows.h>
class Mutex {
private:
	HANDLE handle_;
public:
	Mutex(const char* name = "__my_mutex__")
	{
		handle_ = CreateMutex(nullptr, false, (LPWSTR)(name));
	}

	/*Mutex(const char* name){
	handle_ = CreateMutex(nullptr, false, (LPWSTR)(name));
	}
	*/
	~Mutex()
	{
		ReleaseMutex(handle_);
	}
	void Lock(DWORD milliseconds = INFINITE)
	{
		WaitForSingleObject(handle_, milliseconds);
	}
	void Unlock()
	{
		ReleaseMutex(handle_);
	}
};
