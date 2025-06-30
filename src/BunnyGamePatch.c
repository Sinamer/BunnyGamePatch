#include <stdio.h>
#include <windows.h>

struct PatchInfo {
    DWORD originalOffset;
    DWORD returnOffset;
    BYTE* codeBytes;
    SIZE_T codeSize;
    BYTE* jumpBytes;
    SIZE_T jumpSize;
};

BOOL ApplyPatch(BYTE* baseAddress, struct PatchInfo* patch)
{
    // allocate new memory
    LPVOID memBlock = VirtualAlloc(NULL, patch->codeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if(!memBlock)
        return FALSE;
    
	// calculate return jump
    *(DWORD*)(patch->codeBytes + patch->codeSize - 4) = (baseAddress + patch->returnOffset) - ((BYTE*)memBlock + patch->codeSize);
    
	// write patch code to new memory
	memcpy(memBlock, patch->codeBytes, patch->codeSize);
    
	// calculate jump to new memory
    *(DWORD*)(patch->jumpBytes + 1) = (BYTE*)memBlock - (baseAddress + patch->originalOffset + 5);
    
	// write jump to new memory from the original code
	DWORD oldProtect;
	if(!VirtualProtect(baseAddress + patch->originalOffset, patch->jumpSize, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        VirtualFree(memBlock, 0, MEM_RELEASE);
		return FALSE;
	}
	memcpy(baseAddress + patch->originalOffset, patch->jumpBytes, patch->jumpSize);
	VirtualProtect(baseAddress + patch->originalOffset, patch->jumpSize, oldProtect, &oldProtect);
	return TRUE;
}

BOOL HealthPatch(BYTE* baseAddress)
{
	BYTE codeBytes[33] = {
		0x01, 0x8C, 0x33, 0xE0, 0x04, 0x00, 0x00, // add [ebx+esi+000004E0],ecx
		0x01, 0x8C, 0x33, 0xEC, 0x0B, 0x00, 0x00, // add [ebx+esi+00000BEC],ecx
		0x01, 0x8C, 0x33, 0xF8, 0x12, 0x00, 0x00, // add [ebx+esi+000012F8],ecx
		0x01, 0x8C, 0x33, 0x04, 0x1A, 0x00, 0x00, // add [ebx+esi+00001A04],ecx
		0xE9, 0x00, 0x00, 0x00, 0x00 // jmp back
	};

	BYTE jumpBytes[] = {
		0xE9, 0x00, 0x00, 0x00, 0x00,
		0x66, 0x90
	};

	struct PatchInfo patch = {
		.originalOffset = 0x1BA770,
		.returnOffset = 0x1BA777,
		.codeBytes = codeBytes,
		.codeSize = sizeof(codeBytes),
		.jumpBytes = jumpBytes,
		.jumpSize = sizeof(jumpBytes)
	};
	
	return ApplyPatch(baseAddress, &patch);
}

BOOL BPPatch(BYTE* baseAddress)
{
	BYTE codeBytes[29] = {
		0x89, 0x86, 0xE4, 0x05, 0x00, 0x00, // mov [esi+000005E4],eax
		0x89, 0x86, 0xF0, 0x0C, 0x00, 0x00, // mov [esi+00000CF0],eax
		0x89, 0x86, 0xFC, 0x13, 0x00, 0x00, // mov [esi+000013FC],eax
		0x89, 0x86, 0x08, 0x1B, 0x00, 0x00, // mov [esi+00001B08],eax
		0xE9, 0x00, 0x00, 0x00, 0x00 // jmp back
	};

	BYTE jumpBytes[] = {
		0xE9, 0x00, 0x00, 0x00, 0x00,
		0x90
	};

	struct PatchInfo patch = {
		.originalOffset = 0x26D98,
		.returnOffset = 0x26D9E,
		.codeBytes = codeBytes,
		.codeSize = sizeof(codeBytes),
		.jumpBytes = jumpBytes,
		.jumpSize = sizeof(jumpBytes)
	};
	
	return ApplyPatch(baseAddress, &patch);
}

BOOL AmuletPatch(BYTE* baseAddress)
{
	BYTE codeBytes[37] = {
		0xF3, 0x0F, 0x11, 0x86, 0x34, 0x05, 0x00, 0x00, // movss [esi+00000534],xmm0 
		0xF3, 0x0F, 0x11, 0x86, 0x40, 0x0C, 0x00, 0x00, // movss [esi+00000C40],xmm0 
		0xF3, 0x0F, 0x11, 0x86, 0x4C, 0x13, 0x00, 0x00, // movss [esi+0000134C],xmm0
		0xF3, 0x0F, 0x11, 0x86, 0x58, 0x1A, 0x00, 0x00, // movss [esi+00001A58],xmm0
		0xE9, 0x00, 0x00, 0x00, 0x00 // jmp back
	};

	BYTE jumpBytes[] = {
		0xE9, 0x00, 0x00, 0x00, 0x00,
		0x0F, 0x1F, 0x00
	};

	struct PatchInfo patch = {
		.originalOffset = 0x26DA9,
		.returnOffset = 0x26DB1,
		.codeBytes = codeBytes,
		.codeSize = sizeof(codeBytes),
		.jumpBytes = jumpBytes,
		.jumpSize = sizeof(jumpBytes)
	};
	
	return ApplyPatch(baseAddress, &patch);
}

BOOL RunPatch()
{
	BYTE *baseAddress = (BYTE *)GetModuleHandleA("rabiribi.exe");
	if(!baseAddress)
		return FALSE;
	
	if(!HealthPatch(baseAddress)) return FALSE;
	if(!BPPatch(baseAddress)) return FALSE;
	if(!AmuletPatch(baseAddress)) return FALSE;

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if(ul_reason_for_call == DLL_PROCESS_ATTACH) return RunPatch();
    return TRUE;
}