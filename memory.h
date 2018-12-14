

class Memory{

private:
	unsigned int base;
	HANDLE hwow;
	unsigned int pid;

public:

	template <class T>
	T read (INT64 addr) {
		T result;
		DWORD old;
		ReadProcessMemory(hwow,reinterpret_cast<LPCVOID>(addr),&result,sizeof(T),NULL);

		return (result);
	}

	char* ReadString(INT64 address, int _Size)
	{
		DWORD old;
		char* Value = new char[_Size];
		ReadProcessMemory(hwow, (LPCVOID)address, Value, _Size, 0);
		return Value;
	}

	bool init(char * name){

		HANDLE hsnap=NULL;
		PROCESSENTRY32 pe;
		MODULEENTRY32 me;
		pe.dwSize=sizeof(pe);
		me.dwSize=sizeof(me);
		bool flag=0;
		BYTE buffer=0;

		hsnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);

		Process32First(hsnap,&pe);

		while(Process32Next(hsnap,&pe))
		{
			if(!strcmp(name,pe.szExeFile))
			{
				if(hwow) CloseHandle(hwow);

				hwow=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pe.th32ProcessID);

				pid=pe.th32ProcessID;

				CloseHandle(hsnap);

				hsnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pe.th32ProcessID);

				Module32First(hsnap,&me);

				base=(unsigned int)me.modBaseAddr;

				CloseHandle(hsnap);

				flag=1;

				break;
			}
		}

		if(!flag)
		{
			CloseHandle(hsnap);

			return false;
		}

		CalcBase(pid);

	}

	void CalcBase(unsigned int UserSelectedPid)
	{		
		MODULEENTRY32 me32;
		HANDLE hProcessSnap;

		me32.dwSize=sizeof(me32);
		hProcessSnap = CreateToolhelp32Snapshot(0x8,UserSelectedPid);
		Module32First(hProcessSnap,&me32);
		base=(unsigned int)me32.modBaseAddr;		

		CloseHandle( hProcessSnap );
	}

	DWORD BaseAddress(){
		return base;
	}

	static bool IsValid(INT64 Address)
	{
		//if (ptr && HIWORD(ptr) != NULL) return true; else return false;
		//return (ptr >= 0x10000 && ptr < 0x000F000000000000);
		return (Address < 0x10000 ? false : Address < 0x000F000000000000);
	}
};