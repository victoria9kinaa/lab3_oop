#include <iostream>
#include <string>
#include <stdexcept>
#include <conio.h>
#include <thread>
#include <windows.h>
using namespace std;

wstring GetLastErrorString(DWORD ErrorID = 0)
{
	if (!ErrorID)
		ErrorID = GetLastError();
	if (!ErrorID)
		return wstring();

	LPWSTR pBuff = nullptr;
	size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, ErrorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&pBuff, 0, NULL);
	wstring s(pBuff, size);
	LocalFree(pBuff);

	return s;
}

void Error()
{
	HANDLE hFile = CreateFile(L"file.dat", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wcout << GetLastErrorString() << endl;
	}
	CloseHandle(hFile);
}

class MyClass
{
public:
	MyClass() { wcout << "MyClass::MyClass()" << endl; }
	~MyClass() { wcout << "MyClass::~MyClass()" << endl; }
};

int n;

int f()
{
	MyClass o;
	wcout << 1 / n << endl;
	return n;
}

void Exception1()
{
	//	__try
	{
		_try
		{
			// MyClass o;
			if (f())
				return;
				//__leave;
		}
			__finally
		{
			if (AbnormalTermination())
			{
				wcout << L"something wrong" << endl;
			}
			else
			{
				wcout << L"ok" << endl;
			}
		}
		wcout << L"SEH done" << endl;
	}
	//	__except (EXCEPTION_EXECUTE_HANDLER) {}
}

DWORD GetFilter(DWORD exCode)
{
	if (exCode == EXCEPTION_INT_DIVIDE_BY_ZERO)
	{
		n = 1;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void Exception2()
{
	__try
	{
		__try
		{
			if (f())
				__leave;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
//		__except ((GetExceptionCode() == EXCEPTION_INT_DIVIDE_BY_ZERO)? EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH)
//		__except (GetFilter(GetExceptionCode()))
		{
			wcout << L"exception handled" << endl;
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			wcout << L"something wrong" << endl;
		}
		else
		{
			wcout << L"ok" << endl;
		}
	}
	wcout << L"SEH done" << endl;
}

void SETranslator(unsigned int exCode, EXCEPTION_POINTERS*)
{
	throw runtime_error("Exception: " + to_string(exCode));
}

void Exception3()
{
	_set_se_translator(SETranslator);
	try
	{
		MyClass o;
		if (f())
			return;
	}
	catch (runtime_error& ex)
	{
		wcout << ex.what() << endl;
	}
}

void Exception4()
{
	try
	{
		thread t([]()
			{
				throw runtime_error("exception from thread");
			});
		t.join();
	}
	catch (runtime_error& ex)
	{
		wcout << ex.what() << endl;
	}
}

void Exception5()
{
	try
	{
		std::exception_ptr ex = nullptr;
		thread t([&]()
			{
				try
				{
					throw runtime_error("exception from thread");
				}
				catch (...)
				{
					ex = std::current_exception();
				}
			});
		t.join();
		if (ex)
		{
			std::rethrow_exception(ex);
		}
	}
	catch (runtime_error& ex)
	{
		wcout << ex.what() << endl;
	}
}

int main()
{
	Error();
	//	cin >> n;
	//	Exception1();
	//	Exception2();
	//	Exception3();
	//	Exception4();
	//	Exception5();

	_getch();
}

