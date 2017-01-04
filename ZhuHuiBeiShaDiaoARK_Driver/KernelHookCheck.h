#pragma once

#include <ntddk.h>
#ifndef  _NTIAMGE_
#define _NTIAMGE_
#include <ntimage.h>
#endif 

#include "ReloadKernel.h"

VOID ReLoadNtosCALL(DWORD64 *FuncSyntax, WCHAR *lpwzFuncTion, DWORD64 ulOldNtosBase, DWORD64 ulReloadNtosBase)
{


	DWORD64 FuncAddress;
	UNICODE_STRING FunctionName;

	if (RMmIsAddressValid) {
		if (RMmIsAddressValid((PLONG64)*FuncSyntax)) {
			return;
		}
	}
	else {
		if (MmIsAddressValid((PLONG64)*FuncSyntax)) {
			return;
		}
	}
	__try
	{
		RtlInitUnicodeString(&FunctionName, lpwzFuncTion);
		FuncAddress = (DWORD64)(unsigned char *)MmGetSystemRoutineAddress(&FunctionName);
		if (FuncAddress)
		{
			*FuncSyntax = FuncAddress - ulOldNtosBase + ulReloadNtosBase;
			if (!MmIsAddressValid((PLONG64)*FuncSyntax))
			{
				*FuncSyntax = (DWORD64)NULL;
				return;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {

	}
	return;
}

PEPROCESS GetEprocessFromPid(HANDLE Pid)
{
	HANDLE hProcess;
	NTSTATUS status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	PEPROCESS Process = NULL;
	CLIENT_ID ClientId = { 0 };

	BOOLEAN bInit = FALSE;

	ReLoadNtosCALL((DWORD64 *)&RZwOpenProcess, L"ZwOpenProcess", SystemKernelModuleBase, (DWORD64)ImageModuleBase);
	ReLoadNtosCALL((DWORD64 *)&RObReferenceObjectByHandle, L"ObReferenceObjectByHandle", SystemKernelModuleBase, (DWORD64)ImageModuleBase);
	ReLoadNtosCALL((DWORD64 *)&RZwClose, L"ZwClose", SystemKernelModuleBase, (DWORD64)ImageModuleBase);
	if (RZwOpenProcess &&
		RObReferenceObjectByHandle &&
		RZwClose)
	{
		DPRINT("RZwOpenProcess:%p\n", RZwOpenProcess);
		DPRINT("ObReferenceObjectByHandle:%p\n", RObReferenceObjectByHandle);
		DPRINT("ZwClose%p\n", RZwClose);
		bInit = TRUE;
	}
	if (!bInit)
		return NULL;

	ClientId.UniqueProcess = Pid;
	InitializeObjectAttributes(
		&ObjectAttributes,
		NULL,
		OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);
	status = RZwOpenProcess(
		&hProcess,
		PROCESS_ALL_ACCESS,
		&ObjectAttributes,
		&ClientId
	);
	if (!NT_SUCCESS(status))
	{

		DPRINT("ZwOpenProcess failed:%X\n", status);
		return NULL;
	}
	status = RObReferenceObjectByHandle(
		hProcess,
		PROCESS_ALL_ACCESS,
		*PsProcessType,
		KernelMode,
		(PVOID *)&Process,
		NULL);
	if (!NT_SUCCESS(status))
	{

		DPRINT("RObReferenceObjectByHandle failed:%d\n", RtlNtStatusToDosError(status));

		RZwClose(hProcess);
		return NULL;
	}

	STATUS_UNSUCCESSFUL;
	DbgPrint("[RObReferneceObjectByHandle]%s\n", PsGetProcessImageFileName(Process));
	ObDereferenceObject(Process);
	RZwClose(hProcess);
	return Process;
}

PIMAGE_BASE_RELOCATION
LdrProcessRelocationBlockLongLong(
	IN ULONG_PTR VA,
	IN ULONG SizeOfBlock,
	IN PUSHORT NextOffset,
	IN LONGLONG Diff
)
{
	PUCHAR FixupVA;
	USHORT Offset;
	LONG Temp;

	ULONGLONG Value64;




	while (SizeOfBlock--) {

		Offset = *NextOffset & (USHORT)0xfff;
		FixupVA = (PUCHAR)(VA + Offset);

		//
		// Apply the fixups.
		//

		switch ((*NextOffset) >> 12) {

		case IMAGE_REL_BASED_HIGHLOW:
			//
			// HighLow - (32-bits) relocate the high and low half
			//      of an address.
			//
			*(LONG UNALIGNED *)FixupVA += (ULONG)Diff;
			break;

		case IMAGE_REL_BASED_HIGH:
			//
			// High - (16-bits) relocate the high half of an address.
			//
			Temp = *(PUSHORT)FixupVA << 16;
			Temp += (ULONG)Diff;
			*(PUSHORT)FixupVA = (USHORT)(Temp >> 16);
			break;

		case IMAGE_REL_BASED_HIGHADJ:
			//
			// Adjust high - (16-bits) relocate the high half of an
			//      address and adjust for sign extension of low half.
			//

			//
			// If the address has already been relocated then don't
			// process it again now or information will be lost.
			//
			if (Offset & LDRP_RELOCATION_FINAL) {
				++NextOffset;
				--SizeOfBlock;
				break;
			}

			Temp = *(PUSHORT)FixupVA << 16;
			++NextOffset;
			--SizeOfBlock;
			Temp += (LONG)(*(PSHORT)NextOffset);
			Temp += (ULONG)Diff;
			Temp += 0x8000;
			*(PUSHORT)FixupVA = (USHORT)(Temp >> 16);

			break;

		case IMAGE_REL_BASED_LOW:
			//
			// Low - (16-bit) relocate the low half of an address.
			//
			Temp = *(PSHORT)FixupVA;
			Temp += (ULONG)Diff;
			*(PUSHORT)FixupVA = (USHORT)Temp;
			break;

		case IMAGE_REL_BASED_IA64_IMM64:

			//
			// Align it to bundle address before fixing up the
			// 64-bit immediate value of the movl instruction.
			//

			FixupVA = (PUCHAR)((ULONG_PTR)FixupVA & ~(15));
			Value64 = (ULONGLONG)0;

			//
			// Extract the lower 32 bits of IMM64 from bundle
			//


			EXT_IMM64(Value64,
				(PULONG)FixupVA + EMARCH_ENC_I17_IMM7B_INST_WORD_X,
				EMARCH_ENC_I17_IMM7B_SIZE_X,
				EMARCH_ENC_I17_IMM7B_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM7B_VAL_POS_X);
			EXT_IMM64(Value64,
				(PULONG)FixupVA + EMARCH_ENC_I17_IMM9D_INST_WORD_X,
				EMARCH_ENC_I17_IMM9D_SIZE_X,
				EMARCH_ENC_I17_IMM9D_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM9D_VAL_POS_X);
			EXT_IMM64(Value64,
				(PULONG)FixupVA + EMARCH_ENC_I17_IMM5C_INST_WORD_X,
				EMARCH_ENC_I17_IMM5C_SIZE_X,
				EMARCH_ENC_I17_IMM5C_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM5C_VAL_POS_X);
			EXT_IMM64(Value64,
				(PULONG)FixupVA + EMARCH_ENC_I17_IC_INST_WORD_X,
				EMARCH_ENC_I17_IC_SIZE_X,
				EMARCH_ENC_I17_IC_INST_WORD_POS_X,
				EMARCH_ENC_I17_IC_VAL_POS_X);
			EXT_IMM64(Value64,
				(PULONG)FixupVA + EMARCH_ENC_I17_IMM41a_INST_WORD_X,
				EMARCH_ENC_I17_IMM41a_SIZE_X,
				EMARCH_ENC_I17_IMM41a_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM41a_VAL_POS_X);

			EXT_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IMM41b_INST_WORD_X),
				EMARCH_ENC_I17_IMM41b_SIZE_X,
				EMARCH_ENC_I17_IMM41b_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM41b_VAL_POS_X);
			EXT_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IMM41c_INST_WORD_X),
				EMARCH_ENC_I17_IMM41c_SIZE_X,
				EMARCH_ENC_I17_IMM41c_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM41c_VAL_POS_X);
			EXT_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_SIGN_INST_WORD_X),
				EMARCH_ENC_I17_SIGN_SIZE_X,
				EMARCH_ENC_I17_SIGN_INST_WORD_POS_X,
				EMARCH_ENC_I17_SIGN_VAL_POS_X);
			//
			// Update 64-bit address
			//

			Value64 += Diff;

			//
			// Insert IMM64 into bundle
			//

			INS_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IMM7B_INST_WORD_X),
				EMARCH_ENC_I17_IMM7B_SIZE_X,
				EMARCH_ENC_I17_IMM7B_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM7B_VAL_POS_X);
			INS_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IMM9D_INST_WORD_X),
				EMARCH_ENC_I17_IMM9D_SIZE_X,
				EMARCH_ENC_I17_IMM9D_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM9D_VAL_POS_X);
			INS_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IMM5C_INST_WORD_X),
				EMARCH_ENC_I17_IMM5C_SIZE_X,
				EMARCH_ENC_I17_IMM5C_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM5C_VAL_POS_X);
			INS_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IC_INST_WORD_X),
				EMARCH_ENC_I17_IC_SIZE_X,
				EMARCH_ENC_I17_IC_INST_WORD_POS_X,
				EMARCH_ENC_I17_IC_VAL_POS_X);
			INS_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IMM41a_INST_WORD_X),
				EMARCH_ENC_I17_IMM41a_SIZE_X,
				EMARCH_ENC_I17_IMM41a_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM41a_VAL_POS_X);
			INS_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IMM41b_INST_WORD_X),
				EMARCH_ENC_I17_IMM41b_SIZE_X,
				EMARCH_ENC_I17_IMM41b_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM41b_VAL_POS_X);
			INS_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_IMM41c_INST_WORD_X),
				EMARCH_ENC_I17_IMM41c_SIZE_X,
				EMARCH_ENC_I17_IMM41c_INST_WORD_POS_X,
				EMARCH_ENC_I17_IMM41c_VAL_POS_X);
			INS_IMM64(Value64,
				((PULONG)FixupVA + EMARCH_ENC_I17_SIGN_INST_WORD_X),
				EMARCH_ENC_I17_SIGN_SIZE_X,
				EMARCH_ENC_I17_SIGN_INST_WORD_POS_X,
				EMARCH_ENC_I17_SIGN_VAL_POS_X);
			break;

		case IMAGE_REL_BASED_DIR64:

			*(ULONGLONG UNALIGNED *)FixupVA += Diff;

			break;

		case IMAGE_REL_BASED_MIPS_JMPADDR:
			//
			// JumpAddress - (32-bits) relocate a MIPS jump address.
			//
			Temp = (*(PULONG)FixupVA & 0x3ffffff) << 2;
			Temp += (ULONG)Diff;
			*(PULONG)FixupVA = (*(PULONG)FixupVA & ~0x3ffffff) |
				((Temp >> 2) & 0x3ffffff);

			break;

		case IMAGE_REL_BASED_ABSOLUTE:
			//
			// Absolute - no fixup required.
			//
			break;

			//case IMAGE_REL_BASED_SECTION :
			//	//
			//	// Section Relative reloc.  Ignore for now.
			//	//
			//	break;

			//case IMAGE_REL_BASED_REL32 :
			//	//
			//	// Relative intrasection. Ignore for now.
			//	//
			//	break;

		default:
			//
			// Illegal - illegal relocation type.
			//

			return (PIMAGE_BASE_RELOCATION)NULL;
		}
		++NextOffset;
	}
	return (PIMAGE_BASE_RELOCATION)NextOffset;
}

NTSTATUS
NTAPI
RtlImageNtHeaderEx(
	ULONG Flags,
	PVOID Base,
	ULONG64 Size,
	OUT PIMAGE_NT_HEADERS * OutHeaders
)

/*++

Routine Description:

This function returns the address of the NT Header.

This function is a bit complicated.
It is this way because RtlImageNtHeader that it replaces was hard to understand,
and this function retains compatibility with RtlImageNtHeader.

RtlImageNtHeader was #ifed such as to act different in each of the three
boot loader, kernel, usermode flavors.

boot loader -- no exception handling
usermode -- limit msdos header to 256meg, catch any exception accessing the msdos-header
or the pe header
kernel -- don't cross user/kernel boundary, don't catch the exceptions,
no 256meg limit

Arguments:

Flags - RTL_IMAGE_NT_HEADER_EX_FLAG_NO_RANGE_CHECK -- don't be so picky
about the image, for compatibility with RtlImageNtHeader
Base - Supplies the base of the image.
Size - The size of the view, usually larger than the size of the file on disk.
This is available from NtMapViewOfSection but not from MapViewOfFile.
OutHeaders -

Return Value:

STATUS_SUCCESS -- everything ok
STATUS_INVALID_IMAGE_FORMAT -- bad filesize or signature value
STATUS_INVALID_PARAMETER -- bad parameters

--*/

{
	PIMAGE_NT_HEADERS NtHeaders = 0;
	ULONG e_lfanew = 0;
	BOOLEAN RangeCheck = 0;
	NTSTATUS Status = 0;
	const ULONG ValidFlags =
		RTL_IMAGE_NT_HEADER_EX_FLAG_NO_RANGE_CHECK;

	if (OutHeaders != NULL) {
		*OutHeaders = NULL;
	}
	if (OutHeaders == NULL) {
		Status = STATUS_INVALID_PARAMETER;
		goto Exit;
	}
	if ((Flags & ~ValidFlags) != 0) {
		Status = STATUS_INVALID_PARAMETER;
		goto Exit;
	}
	if (Base == NULL || Base == (PVOID)(LONG_PTR)-1) {
		Status = STATUS_INVALID_PARAMETER;
		goto Exit;
	}

	RangeCheck = ((Flags & RTL_IMAGE_NT_HEADER_EX_FLAG_NO_RANGE_CHECK) == 0);
	if (RangeCheck) {
		if (Size < sizeof(IMAGE_DOS_HEADER)) {
			Status = STATUS_INVALID_IMAGE_FORMAT;
			goto Exit;
		}
	}

	//
	// Exception handling is not available in the boot loader, and exceptions
	// were not historically caught here in kernel mode. Drivers are considered
	// trusted, so we can't get an exception here due to a bad file, but we
	// could take an inpage error.
	//
#define EXIT goto Exit
	if (((PIMAGE_DOS_HEADER)Base)->e_magic != IMAGE_DOS_SIGNATURE) {
		Status = STATUS_INVALID_IMAGE_FORMAT;
		EXIT;
	}
	e_lfanew = ((PIMAGE_DOS_HEADER)Base)->e_lfanew;
	if (RangeCheck) {
		if (e_lfanew >= Size
#define SIZEOF_PE_SIGNATURE 4
			|| e_lfanew >= (MAXULONG - SIZEOF_PE_SIGNATURE - sizeof(IMAGE_FILE_HEADER))
			|| (e_lfanew + SIZEOF_PE_SIGNATURE + sizeof(IMAGE_FILE_HEADER)) >= Size
			) {
			Status = STATUS_INVALID_IMAGE_FORMAT;
			EXIT;
		}
	}

	NtHeaders = (PIMAGE_NT_HEADERS)((PCHAR)Base + e_lfanew);

	//
	// In kernelmode, do not cross from usermode address to kernelmode address.
	//
	if (Base < MM_HIGHEST_USER_ADDRESS) {
		if ((PVOID)NtHeaders >= MM_HIGHEST_USER_ADDRESS) {
			Status = STATUS_INVALID_IMAGE_FORMAT;
			EXIT;
		}
		//
		// Note that this check is slightly overeager since IMAGE_NT_HEADERS has
		// a builtin array of data_directories that may be larger than the image
		// actually has. A better check would be to add FileHeader.SizeOfOptionalHeader,
		// after ensuring that the FileHeader does not cross the u/k boundary.
		//
		if ((PVOID)((PCHAR)NtHeaders + sizeof(IMAGE_NT_HEADERS)) >= MM_HIGHEST_USER_ADDRESS) {
			Status = STATUS_INVALID_IMAGE_FORMAT;
			EXIT;
		}
	}

	if (NtHeaders->Signature != IMAGE_NT_SIGNATURE) {
		Status = STATUS_INVALID_IMAGE_FORMAT;
		EXIT;
	}
	Status = STATUS_SUCCESS;

Exit:
	if (NT_SUCCESS(Status)) {
		*OutHeaders = NtHeaders;
	}
	return Status;
}


PIMAGE_NT_HEADERS
NTAPI
RtlImageNtHeader_RE(
	PVOID Base
)
{
	PIMAGE_NT_HEADERS NtHeaders = NULL;
	(VOID)RtlImageNtHeaderEx(RTL_IMAGE_NT_HEADER_EX_FLAG_NO_RANGE_CHECK, Base, 0, &NtHeaders);
	return NtHeaders;
}

PVOID
RtlpImageDirectoryEntryToData32(
	IN PVOID Base,
	IN BOOLEAN MappedAsImage,
	IN USHORT DirectoryEntry,
	OUT PULONG Size,
	PIMAGE_NT_HEADERS32 NtHeaders
)
{
	ULONG DirectoryAddress;
	if (DirectoryEntry >= NtHeaders->OptionalHeader.NumberOfRvaAndSizes) {
		return(NULL);
	}
	if (!(DirectoryAddress = NtHeaders->OptionalHeader.DataDirectory[DirectoryEntry].VirtualAddress)) {
		return(NULL);
	}
	if (Base < MM_HIGHEST_USER_ADDRESS) {
		if ((PVOID)((PCHAR)Base + DirectoryAddress) >= MM_HIGHEST_USER_ADDRESS) {
			return(NULL);
		}
	}
	*Size = NtHeaders->OptionalHeader.DataDirectory[DirectoryEntry].Size;
	if (MappedAsImage || DirectoryAddress < NtHeaders->OptionalHeader.SizeOfHeaders) {
		return((PVOID)((PCHAR)Base + DirectoryAddress));
	}
	return(RtlAddressInSectionTable((PIMAGE_NT_HEADERS)NtHeaders, Base, DirectoryAddress));
}


PVOID
RtlpImageDirectoryEntryToData64(
	IN PVOID Base,
	IN BOOLEAN MappedAsImage,
	IN USHORT DirectoryEntry,
	OUT PULONG Size,
	PIMAGE_NT_HEADERS64 NtHeaders
)
{
	ULONG DirectoryAddress;
	if (DirectoryEntry >= NtHeaders->OptionalHeader.NumberOfRvaAndSizes) {
		return(NULL);
	}
	if (!(DirectoryAddress = NtHeaders->OptionalHeader.DataDirectory[DirectoryEntry].VirtualAddress)) {
		return(NULL);
	}
	if (Base < MM_HIGHEST_USER_ADDRESS) {
		if ((PVOID)((PCHAR)Base + DirectoryAddress) >= MM_HIGHEST_USER_ADDRESS) {
			return(NULL);
		}
	}
	*Size = NtHeaders->OptionalHeader.DataDirectory[DirectoryEntry].Size;
	if (MappedAsImage || DirectoryAddress < NtHeaders->OptionalHeader.SizeOfHeaders) {
		return((PVOID)((PCHAR)Base + DirectoryAddress));
	}
	return(RtlAddressInSectionTable((PIMAGE_NT_HEADERS)NtHeaders, Base, DirectoryAddress));
}




PVOID
EasyDebugger_RtlImageDirectoryEntryToData(
	IN PVOID Base,
	IN BOOLEAN MappedAsImage,
	IN USHORT DirectoryEntry,
	OUT PULONG Size
)
{
	PIMAGE_NT_HEADERS NtHeaders;
	if (LDR_IS_DATAFILE(Base)) {
		Base = LDR_DATAFILE_TO_VIEW(Base);
		MappedAsImage = FALSE;
	}
	NtHeaders = RtlImageNtHeader_RE(Base);
	if (!NtHeaders)
		return NULL;
	if (NtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
		return (RtlpImageDirectoryEntryToData32(Base,
			MappedAsImage,
			DirectoryEntry,
			Size,
			(PIMAGE_NT_HEADERS32)NtHeaders));
	}
	else if (NtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
		return (RtlpImageDirectoryEntryToData64(Base,
			MappedAsImage,
			DirectoryEntry,
			Size,
			(PIMAGE_NT_HEADERS64)NtHeaders));
	}
	else {
		return (NULL);
	}
}

BOOLEAN
FixBaseRelocTable(
	PVOID NewImageBase,
	DWORD64 ExistImageBase
)
/*++

Routine Description:

This routine relocates an image file that was not loaded into memory
at the preferred address.

Arguments:

NewBase - Supplies a pointer to the image base.

AdditionalBias - An additional quantity to add to all fixups.  The
32-bit X86 loader uses this when loading 64-bit images
to specify a NewBase that is actually a 64-bit value.

LoaderName - Indicates which loader routine is being called from.

Success - Value to return if relocation successful.

Conflict - Value to return if can't relocate.

Invalid - Value to return if relocations are invalid.

Return Value:

Success if image is relocated.
Conflict if image can't be relocated.
Invalid if image contains invalid fixups.

--*/

{
	LONGLONG Diff;
	ULONG TotalCountBytes = 0;
	ULONG_PTR VA;
	ULONGLONG OriginalImageBase;
	ULONG SizeOfBlock;


	PUSHORT NextOffset = NULL;
	PIMAGE_NT_HEADERS NtHeaders;
	PIMAGE_BASE_RELOCATION NextBlock;


	NtHeaders = RtlImageNtHeader_RE(NewImageBase);
	if (NtHeaders == NULL)
	{
		return FALSE;
	}

	switch (NtHeaders->OptionalHeader.Magic) {

	case IMAGE_NT_OPTIONAL_HDR32_MAGIC:

		OriginalImageBase =
			((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.ImageBase;
		break;

	case IMAGE_NT_OPTIONAL_HDR64_MAGIC:

		OriginalImageBase =
			((PIMAGE_NT_HEADERS64)NtHeaders)->OptionalHeader.ImageBase;
		break;

	default:
		return FALSE;
	}

	//
	// Locate the relocation section.
	//

	NextBlock = (PIMAGE_BASE_RELOCATION)EasyDebugger_RtlImageDirectoryEntryToData(
		NewImageBase, TRUE, IMAGE_DIRECTORY_ENTRY_BASERELOC, &TotalCountBytes);

	//
	// It is possible for a file to have no relocations, but the relocations
	// must not have been stripped.
	//

	if (!NextBlock || !TotalCountBytes)
	{

		if (NtHeaders->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED)
		{
			DPRINT("Image can't be relocated, no fixup information.\n");
			return FALSE;

		}
		else
		{
			return TRUE;
		}

	}

	//
	// If the image has a relocation table, then apply the specified fixup
	// information to the image.
	//
	Diff = (ULONG_PTR)ExistImageBase - OriginalImageBase;
	while (TotalCountBytes)
	{
		SizeOfBlock = NextBlock->SizeOfBlock;
		TotalCountBytes -= SizeOfBlock;
		SizeOfBlock -= sizeof(IMAGE_BASE_RELOCATION);
		SizeOfBlock /= sizeof(USHORT);
		NextOffset = (PUSHORT)((PCHAR)NextBlock + sizeof(IMAGE_BASE_RELOCATION));

		VA = (ULONG_PTR)NewImageBase + NextBlock->VirtualAddress;

		if (!(NextBlock = LdrProcessRelocationBlockLongLong(VA,
			SizeOfBlock,
			NextOffset,
			Diff)))
		{

			DPRINT("%s: Unknown base relocation type\n");
			return FALSE;

		}
	}

	return TRUE;
}
int InsertOriginalFirstThunk(DWORD64 ImageBase, DWORD64 ExistImageBase, PIMAGE_THUNK_DATA FirstThunk)
{
	DWORD64 Offset;
	PIMAGE_THUNK_DATA OriginalFirstThunk;
	Offset = (DWORD64)FirstThunk - ImageBase;
	OriginalFirstThunk = (PIMAGE_THUNK_DATA)(ExistImageBase + Offset);
	while (OriginalFirstThunk->u1.Function)
	{

		DPRINT("Fuction Address:%X\n", OriginalFirstThunk->u1.Function);
		FirstThunk->u1.Function = OriginalFirstThunk->u1.Function;
		OriginalFirstThunk++;
		FirstThunk++;
	}
	return TRUE;

}
VOID WcharToChar(__in WCHAR *wzFuncName, __out CHAR *FuncName)
{
	UNICODE_STRING UnicodeFuncName;
	ANSI_STRING AnsiFuncName;

	RtlInitUnicodeString(&UnicodeFuncName, wzFuncName);
	if (RtlUnicodeStringToAnsiString(&AnsiFuncName, &UnicodeFuncName, TRUE) == STATUS_SUCCESS) {
		memcpy(FuncName, AnsiFuncName.Buffer, AnsiFuncName.Length);
		RtlFreeAnsiString(&AnsiFuncName);
	}
}
PVOID GetKernelModuleBase(PDRIVER_OBJECT DriverObject, char *KernelModuleName)
{
	PLDR_DATA_TABLE_ENTRY LdrDataTable;

	CHAR lpModule[260];
	//WCHAR lpSysMode[260];

	__try
	{
		LdrDataTable = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
		do
		{
			if (LdrDataTable->BaseDllName.Length>0 && LdrDataTable->BaseDllName.Buffer != NULL)
			{
				if (MmIsAddressValid(&LdrDataTable->BaseDllName.Buffer[LdrDataTable->BaseDllName.Length / 2 - 1]))
				{
					DPRINT("Module:%ws\n",LdrDataTable->BaseDllName.Buffer);
					memset(lpModule, 0, sizeof(lpModule));

					/*memset(lpModule, 0, sizeof(lpModule));
					memset(lpSysMode, 0, sizeof(lpSysMode));

					
					RtlCopyMemory(lpSysMode, LdrDataTable->BaseDllName.Buffer, LdrDataTable->BaseDllName.Length * 2);
					CharToWchar(KernelModuleName, lpModule);

					if(wcsstr(lpSysMode, lpModule))
					{
						return LdrDataTable->DllBase;
					}*/
					WcharToChar(LdrDataTable->BaseDllName.Buffer, lpModule);

					_strlwr(lpModule);
					if (_stricmp(lpModule, KernelModuleName) == 0)
					{
						return LdrDataTable->DllBase;
					}
				}
			}
			LdrDataTable = (PLDR_DATA_TABLE_ENTRY)LdrDataTable->InLoadOrderLinks.Flink;

		} while ((PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection != LdrDataTable&&LdrDataTable != NULL);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
	return NULL;
}

PIMAGE_SECTION_HEADER
RtlSectionTableFromVirtualAddress(
	IN PIMAGE_NT_HEADERS NtHeaders,
	IN PVOID Base,
	IN ULONG Address
)
{
	UNREFERENCED_PARAMETER(Base);
	ULONG i;
	PIMAGE_SECTION_HEADER NtSection;

	NtSection = IMAGE_FIRST_SECTION(NtHeaders);
	for (i = 0; i<NtHeaders->FileHeader.NumberOfSections; i++) {
		if ((ULONG)Address >= NtSection->VirtualAddress &&
			(ULONG)Address < NtSection->VirtualAddress + NtSection->SizeOfRawData
			) {
			return NtSection;
		}
		++NtSection;
	}

	return NULL;
}

PVOID
RtlAddressInSectionTable(
	IN PIMAGE_NT_HEADERS NtHeaders,
	IN PVOID Base,
	IN ULONG Address
)
{
	PIMAGE_SECTION_HEADER NtSection;
	NtSection = RtlSectionTableFromVirtualAddress(NtHeaders,
		Base,
		Address
	);
	if (NtSection != NULL) {
		return(((PCHAR)Base + ((ULONG_PTR)Address - NtSection->VirtualAddress) + NtSection->PointerToRawData));
	}
	else {
		return(NULL);
	}
}


PVOID
MiFindExportedRoutine(
	IN PVOID DllBase,
	int ByName,
	IN char *RoutineName,
	DWORD64 Ordinal
)

/*++

Routine Description:

This function searches the argument module looking for the requested
exported function name.

Arguments:

DllBase - Supplies the base address of the requested module.

AnsiImageRoutineName - Supplies the ANSI routine name being searched for.

Return Value:

The virtual address of the requested routine or NULL if not found.

--*/

{
	USHORT OrdinalNumber;
	PULONG NameTableBase;
	PUSHORT NameOrdinalTableBase;
	PULONG AddressTableBase;
	PULONG Addr;
	LONG High;
	LONG Low;
	LONG Middle;
	LONG Result;
	ULONG ExportSize;
	PVOID FunctionAddress;
	PIMAGE_EXPORT_DIRECTORY ExportDirectory;

	PAGED_CODE();

	ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)EasyDebugger_RtlImageDirectoryEntryToData(
		DllBase,
		TRUE,
		IMAGE_DIRECTORY_ENTRY_EXPORT,
		&ExportSize);

	if (ExportDirectory == NULL) {
		return NULL;
	}

	//
	// Initialize the pointer to the array of RVA-based ansi export strings.
	//

	NameTableBase = (PULONG)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfNames);

	//
	// Initialize the pointer to the array of USHORT ordinal numbers.
	//

	NameOrdinalTableBase = (PUSHORT)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfNameOrdinals);

	AddressTableBase = (PULONG)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfFunctions);

	if (!ByName)
	{
		return (PVOID)AddressTableBase[Ordinal];
	}

	//
	// Lookup the desired name in the name table using a binary search.
	//

	Low = 0;
	Middle = 0;
	High = ExportDirectory->NumberOfNames - 1;

	while (High >= Low) {

		//
		// Compute the next probe index and compare the import name
		// with the export name entry.
		//

		Middle = (Low + High) >> 1;

		Result = strcmp(RoutineName,
			(PCHAR)DllBase + NameTableBase[Middle]);

		if (Result < 0) {
			High = Middle - 1;
		}
		else if (Result > 0) {
			Low = Middle + 1;
		}
		else {
			break;
		}
	}
	//
	// If the high index is less than the low index, then a matching
	// table entry was not found. Otherwise, get the ordinal number
	// from the ordinal table.
	//

	if (High < Low) {
		return NULL;
	}

	OrdinalNumber = NameOrdinalTableBase[Middle];

	//
	// If the OrdinalNumber is not within the Export Address Table,
	// then this image does not implement the function.  Return not found.
	//

	if ((ULONG)OrdinalNumber >= ExportDirectory->NumberOfFunctions) {
		return NULL;
	}

	//
	// Index into the array of RVA export addresses by ordinal number.
	//

	Addr = (PULONG)((PCHAR)DllBase + (ULONG)ExportDirectory->AddressOfFunctions);

	FunctionAddress = (PVOID)((PCHAR)DllBase + Addr[OrdinalNumber]);

	//
	// Forwarders are not used by the kernel and HAL to each other.
	//

	// 	ASSERT ((FunctionAddress <= (PVOID)ExportDirectory) ||
	// 		(FunctionAddress >= (PVOID)((PCHAR)ExportDirectory + ExportSize)));

	if ((ULONG_PTR)FunctionAddress > (ULONG_PTR)ExportDirectory &&
		(ULONG_PTR)FunctionAddress < ((ULONG_PTR)ExportDirectory + ExportSize)) {
		FunctionAddress = NULL;
	}
	return FunctionAddress;
}

int FixImportTable(unsigned char *ImageBase, DWORD64 ExistImageBase, PDRIVER_OBJECT DriverObject)
{
	PIMAGE_IMPORT_DESCRIPTOR ImageImportDescriptor = NULL;
	PIMAGE_THUNK_DATA ImageThunkData, FirstThunk;
	PIMAGE_IMPORT_BY_NAME ImortByName;
	DWORD64 ImportSize;
	PVOID ModuleBase;
	char ModuleName[260];
	DWORD64 FunctionAddress;

	ImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)EasyDebugger_RtlImageDirectoryEntryToData(ImageBase, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, (PULONG)&ImportSize);
	if (ImageImportDescriptor == NULL)
	{
		return FALSE;
	}
	while (ImageImportDescriptor->OriginalFirstThunk&&ImageImportDescriptor->Name)
	{
		strcpy(ModuleName, (char*)(ImageBase + ImageImportDescriptor->Name));

		DPRINT("ModuleName:%s\n", ModuleName);
		
		_strlwr(ModuleName);
		//ntoskrnl.exe(NTKRNLPA.exe、ntkrnlmp.exe、ntkrpamp.exe)：
		if (_stricmp(ModuleName, "ntkrnlpa.exe") == 0 ||
			_stricmp(ModuleName, "ntoskrnl.exe") == 0 ||
			_stricmp(ModuleName, "ntkrnlmp.exe") == 0 ||
			_stricmp(ModuleName, "ntkrpamp.exe") == 0)
		{
			//bakup module name
			memset(NtosModuleName, 0, sizeof(NtosModuleName));
			memcpy(NtosModuleName, ModuleName, strlen(ModuleName));

			//set ntos base
			ModuleBase = (PVOID)SystemKernelModuleBase;

		}
		else
		{
			ModuleBase = GetKernelModuleBase(DriverObject, ModuleName);

		}
		if (ModuleBase == NULL)
		{

			DPRINT("can't find module:%s\n", ModuleName);

			FirstThunk = (PIMAGE_THUNK_DATA)(ImageBase + ImageImportDescriptor->FirstThunk);
			InsertOriginalFirstThunk((DWORD64)ImageBase, ExistImageBase, FirstThunk);
			ImageImportDescriptor++;
			continue;
		}
		ImageThunkData = (PIMAGE_THUNK_DATA)(ImageBase + ImageImportDescriptor->OriginalFirstThunk);
		FirstThunk = (PIMAGE_THUNK_DATA)(ImageBase + ImageImportDescriptor->FirstThunk);
		while (ImageThunkData->u1.Ordinal)
		{
			//序号导入
			if (IMAGE_SNAP_BY_ORDINAL32(ImageThunkData->u1.Ordinal))
			{
				FunctionAddress = (DWORD64)MiFindExportedRoutine(ModuleBase, FALSE, NULL, ImageThunkData->u1.Ordinal & ~IMAGE_ORDINAL_FLAG32);
				if (FunctionAddress == 0)
				{

					DPRINT("can't find funcion Index %d \n", ImageThunkData->u1.Ordinal & ~IMAGE_ORDINAL_FLAG32);
					break;
				}
				FirstThunk->u1.Function = FunctionAddress;
			}
			//函数名导入
			else
			{
				//
				ImortByName = (PIMAGE_IMPORT_BY_NAME)(ImageBase + ImageThunkData->u1.AddressOfData);
				FunctionAddress = (DWORD64)MiFindExportedRoutine(ModuleBase, TRUE, ImortByName->Name, 0);
				if (FunctionAddress == 0)
				{

					DPRINT(("can't Funcion Name:%s\n", ImortByName->Name));
					break;
				}
				FirstThunk->u1.Function = FunctionAddress;
			}
			FirstThunk++;
			ImageThunkData++;
		}
		ImageImportDescriptor++;
	}
	return TRUE;
}

UINT64 AlignSize(UINT64 nSize, UINT64 nAlign)
{
	return ((nSize + nAlign - 1) / nAlign * nAlign);
}

int ImageFile(unsigned char *FileBuffer, unsigned char **mImageModuleBase)
{
	__debugbreak();
	PIMAGE_DOS_HEADER ImageDosHeader;
	PIMAGE_NT_HEADERS ImageNtHeaders;
	PIMAGE_SECTION_HEADER ImageSectionHeader;
	DWORD64 FileAlignment, SectionAlignment, NumberOfSections, SizeOfImage, SizeOfHeaders;
	DWORD64 Index;
	unsigned char *ImageBase;
	DWORD64 SizeOfNtHeaders;
	ImageDosHeader = (PIMAGE_DOS_HEADER)FileBuffer;
	if (ImageDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return FALSE;
	}
	ImageNtHeaders = (PIMAGE_NT_HEADERS)(FileBuffer + ImageDosHeader->e_lfanew);
	if (ImageNtHeaders->Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}
	FileAlignment = ImageNtHeaders->OptionalHeader.FileAlignment;
	SectionAlignment = ImageNtHeaders->OptionalHeader.SectionAlignment;
	NumberOfSections = ImageNtHeaders->FileHeader.NumberOfSections;
	SizeOfImage = ImageNtHeaders->OptionalHeader.SizeOfImage;
	SizeOfHeaders = ImageNtHeaders->OptionalHeader.SizeOfHeaders;

	SizeOfImage = AlignSize(SizeOfImage, SectionAlignment);

	ImageBase = (unsigned char *)ExAllocatePool(NonPagedPool, SizeOfImage);
	if (ImageBase == NULL)
	{
		return FALSE;
	}
	DPRINT("++++++++++++++Allocate Image buffer ok:0x%llx\n", ImageBase);
	RtlZeroMemory(ImageBase, SizeOfImage);
	SizeOfNtHeaders = sizeof(ImageNtHeaders->FileHeader) + sizeof(ImageNtHeaders->Signature) + ImageNtHeaders->FileHeader.SizeOfOptionalHeader;
	ImageSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD64)ImageNtHeaders + SizeOfNtHeaders);
	DPRINT("++++++++++++++ImageSectionHeader buffer:0x%llx\n", ImageSectionHeader);
	/*for (Index = 0; Index<NumberOfSections; Index++)
	{
	ImageSectionHeader[Index].SizeOfRawData = AlignSize(ImageSectionHeader[Index].SizeOfRawData, FileAlignment);
	ImageSectionHeader[Index].Misc.VirtualSize = AlignSize(ImageSectionHeader[Index].Misc.VirtualSize, SectionAlignment);
	}
	if (ImageSectionHeader[NumberOfSections - 1].VirtualAddress + ImageSectionHeader[NumberOfSections - 1].SizeOfRawData>SizeOfImage)
	{
	ImageSectionHeader[NumberOfSections - 1].SizeOfRawData = SizeOfImage - (DWORD64)(ImageSectionHeader[NumberOfSections - 1].VirtualAddress);
	}*/
	RtlCopyMemory(ImageBase, FileBuffer, SizeOfHeaders);
	__debugbreak();
	ULONG64 temp = 0;
	PVOID  temp1 = 0, temp2 = 0;
	for (Index = 0; Index<NumberOfSections; Index++)
	{
		if (ImageSectionHeader[Index].Characteristics & (IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE))
		{
			DPRINT("==============index:%d\n", Index);
			if (ImageSectionHeader[Index].SizeOfRawData == 0)
				continue;
			DWORD64 FileOffset = ImageSectionHeader[Index].PointerToRawData;
			DPRINT("==============FileOffset:0x%llx\n", FileOffset);
			temp = FileOffset;
			ULONG64 Length = ImageSectionHeader[Index].Misc.VirtualSize;
			DPRINT("==============Length:0x%llx\n", Length);
			if (Length>ImageSectionHeader[Index].SizeOfRawData)
				temp = Length;
			else
				temp = ImageSectionHeader[Index].SizeOfRawData;
			DWORD64 ImageOffset = ImageSectionHeader[Index].VirtualAddress;
			DPRINT("==============ImageOffset:0x%llx\n", ImageOffset);
			temp1 = &ImageBase[ImageOffset];
			DPRINT("==============dest addr:0x%llx\n", temp1);
			temp2 = &FileBuffer[FileOffset];
			DPRINT("==============src addr:0x%llx\n", temp2);
			RtlCopyMemory(temp1, temp2, Length);
		}

	}
	*mImageModuleBase = ImageBase;

	return TRUE;


}


NTSTATUS
IoCompletionRoutine(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	*Irp->UserIosb = Irp->IoStatus;
	if (Irp->UserEvent)
		KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, 0);
	if (Irp->MdlAddress)
	{
		IoFreeMdl(Irp->MdlAddress);
		Irp->MdlAddress = NULL;
	}
	IoFreeIrp(Irp);
	return STATUS_MORE_PROCESSING_REQUIRED;
}


NTSTATUS
IrpReadFile(
	IN PFILE_OBJECT FileObject,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG64 Length,
	IN PLARGE_INTEGER ByteOffset OPTIONAL)
{
	NTSTATUS ntStatus;
	PIRP Irp;
	KEVENT kEvent;
	PIO_STACK_LOCATION IrpSp;
	// 


	if (ByteOffset == NULL)
	{
		if (!(FileObject->Flags & FO_SYNCHRONOUS_IO))
			return STATUS_INVALID_PARAMETER;
		ByteOffset = &FileObject->CurrentByteOffset;
	}

	Irp = IoAllocateIrp(DeviceObject->StackSize, FALSE);
	if (Irp == NULL) return STATUS_INSUFFICIENT_RESOURCES;

	RtlZeroMemory(Buffer, Length);
	if (FileObject->DeviceObject->Flags & DO_BUFFERED_IO)
	{
		Irp->AssociatedIrp.SystemBuffer = Buffer;
	}
	else if (FileObject->DeviceObject->Flags & DO_DIRECT_IO)
	{
		Irp->MdlAddress = IoAllocateMdl(Buffer, (ULONG)Length, 0, 0, 0);
		if (Irp->MdlAddress == NULL)
		{
			IoFreeIrp(Irp);
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		MmBuildMdlForNonPagedPool(Irp->MdlAddress);
	}
	else
	{
		Irp->UserBuffer = Buffer;
	}

	KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);

	Irp->UserEvent = &kEvent;
	Irp->UserIosb = IoStatusBlock;
	Irp->RequestorMode = KernelMode;
	Irp->Flags = IRP_READ_OPERATION;
	Irp->Tail.Overlay.Thread = PsGetCurrentThread();
	Irp->Tail.Overlay.OriginalFileObject = FileObject;

	IrpSp = IoGetNextIrpStackLocation(Irp);
	IrpSp->MajorFunction = IRP_MJ_READ;
	IrpSp->MinorFunction = IRP_MN_NORMAL;
	IrpSp->DeviceObject = DeviceObject;
	IrpSp->FileObject = FileObject;
	IrpSp->Parameters.Read.Length = (ULONG)Length;
	IrpSp->Parameters.Read.ByteOffset = *ByteOffset;

	IoSetCompletionRoutine(Irp, (PIO_COMPLETION_ROUTINE)IoCompletionRoutine, 0, TRUE, TRUE, TRUE);
	ntStatus = IoCallDriver(DeviceObject, Irp);
	if (ntStatus == STATUS_PENDING)
		KeWaitForSingleObject(&kEvent, Executive, KernelMode, TRUE, 0);

	return IoStatusBlock->Status;
}



int IoGetFileSystemVpbInfo(IN PFILE_OBJECT FileObject, PDEVICE_OBJECT *DeviceObject, PDEVICE_OBJECT *RealDevice)
{
	//PDEVICE_OBJECT deviceObject;
	// If the file object has a mounted Vpb, use its DeviceObject.
	if (FileObject->Vpb != NULL && FileObject->Vpb->DeviceObject != NULL)
	{
		*DeviceObject = FileObject->Vpb->DeviceObject;
		*RealDevice = FileObject->Vpb->RealDevice;

		// Otherwise, if the real device has a VPB that indicates that it is mounted,
		// then use the file system device object associated with the VPB.
	}
	else if
		(
			!(FileObject->Flags & FO_DIRECT_DEVICE_OPEN)
			&&
			FileObject->DeviceObject->Vpb != NULL
			&&
			FileObject->DeviceObject->Vpb->DeviceObject != NULL
			)
	{
		*DeviceObject = FileObject->DeviceObject->Vpb->DeviceObject;
		*RealDevice = FileObject->DeviceObject->Vpb->RealDevice;
		// Otherwise, just return the real device object.
	}
	else
	{
		*DeviceObject = FileObject->DeviceObject;
		*RealDevice = NULL;
	}
	if (*RealDevice == NULL || *DeviceObject == NULL)
	{
		return FALSE;
	}
	// Simply return the resultant file object.
	return TRUE;
}
NTSTATUS KernelReadFile(HANDLE hFile, PLARGE_INTEGER ByteOffset, ULONG64 Length, PVOID FileBuffer, PIO_STATUS_BLOCK IoStatusBlock)
{
	NTSTATUS status;
	PFILE_OBJECT FileObject;
	PDEVICE_OBJECT DeviceObject, RealDevice;

	status = ObReferenceObjectByHandle(hFile, 0, *IoFileObjectType, KernelMode, (PVOID *)&FileObject, 0);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	if (!IoGetFileSystemVpbInfo(FileObject, &DeviceObject, &RealDevice))
	{
		ObDereferenceObject(FileObject);
		return STATUS_UNSUCCESSFUL;
	}
	status = IrpReadFile(FileObject, DeviceObject, IoStatusBlock, FileBuffer, Length, ByteOffset);
	ObDereferenceObject(FileObject);
	return status;

}


NTSTATUS
IrpQueryInformationFile(
	IN PFILE_OBJECT FileObject,
	IN PDEVICE_OBJECT DeviceObject,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass)
{
	NTSTATUS ntStatus;
	PIRP Irp;
	KEVENT kEvent;
	PIO_STACK_LOCATION IrpSp;
	IO_STATUS_BLOCK IoStatusBlock;

	// 	if (FileObject->Vpb == 0 || FileObject->Vpb->DeviceObject == NULL)
	// 		return STATUS_UNSUCCESSFUL;

	Irp = IoAllocateIrp(DeviceObject->StackSize, FALSE);
	if (Irp == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);

	RtlZeroMemory(FileInformation, Length);
	Irp->AssociatedIrp.SystemBuffer = FileInformation;
	Irp->UserEvent = &kEvent;
	Irp->UserIosb = &IoStatusBlock;
	Irp->RequestorMode = KernelMode;
	Irp->Tail.Overlay.Thread = PsGetCurrentThread();
	Irp->Tail.Overlay.OriginalFileObject = FileObject;

	IrpSp = IoGetNextIrpStackLocation(Irp);
	IrpSp->MajorFunction = IRP_MJ_QUERY_INFORMATION;
	IrpSp->DeviceObject = DeviceObject;
	IrpSp->FileObject = FileObject;
	IrpSp->Parameters.QueryFile.Length = Length;
	IrpSp->Parameters.QueryFile.FileInformationClass = FileInformationClass;

	IoSetCompletionRoutine(Irp, (PIO_COMPLETION_ROUTINE)IoCompletionRoutine, 0, TRUE, TRUE, TRUE);
	ntStatus = IoCallDriver(DeviceObject, Irp);

	if (ntStatus == STATUS_PENDING)
		KeWaitForSingleObject(&kEvent, Executive, KernelMode, TRUE, 0);

	return IoStatusBlock.Status;
}
NTSTATUS  KernelGetFileSize(HANDLE hFile, PLARGE_INTEGER FileSize)
{
	NTSTATUS status;
	PFILE_OBJECT FileObject;
	PDEVICE_OBJECT DeviceObject, RealDevice;
	FILE_STANDARD_INFORMATION FileInformation;

	status = ObReferenceObjectByHandle(hFile, 0, *IoFileObjectType, KernelMode, (PVOID *)&FileObject, 0);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	if (!IoGetFileSystemVpbInfo(FileObject, &DeviceObject, &RealDevice))
	{
		ObDereferenceObject(FileObject);
		return STATUS_UNSUCCESSFUL;
	}
	status = IrpQueryInformationFile(FileObject, DeviceObject, &FileInformation, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(status))
	{
		ObDereferenceObject(FileObject);
		return status;
	}
	FileSize->HighPart = FileInformation.EndOfFile.HighPart;
	FileSize->LowPart = FileInformation.EndOfFile.LowPart;
	ObDereferenceObject(FileObject);
	return status;
}

int GetDeviceObjectFromFileFullName(WCHAR *FileFullName, PDEVICE_OBJECT *RealDevice, PDEVICE_OBJECT *DeviceObject)
{
	WCHAR wRootName[32] = { 0 };
	UNICODE_STRING RootName;
	OBJECT_ATTRIBUTES ObjectAttributes = { 0 };
	NTSTATUS status;
	HANDLE hFile;
	IO_STATUS_BLOCK IoStatus;
	PFILE_OBJECT FileObject;

	if (FileFullName[0] == 0x005C)
	{
		wcscpy(wRootName, L"\\SystemRoot");
	}
	else
	{
		wcscpy(wRootName, L"\\DosDevices\\*:\\");
		wRootName[12] = FileFullName[0];
	}
	RtlInitUnicodeString(&RootName, wRootName);

	InitializeObjectAttributes(&ObjectAttributes, &RootName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = IoCreateFile(
		&hFile,
		SYNCHRONIZE,
		&ObjectAttributes,
		&IoStatus,
		0,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0,
		CreateFileTypeNone,
		NULL,
		IO_NO_PARAMETER_CHECKING);

	if (!NT_SUCCESS(status))
	{
		return FALSE;
	}
	status = ObReferenceObjectByHandle(hFile, 1, *IoFileObjectType, KernelMode, (PVOID *)&FileObject, NULL);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hFile);
		return FALSE;
	}
	if (!IoGetFileSystemVpbInfo(FileObject, DeviceObject, RealDevice))
	{
		ObfDereferenceObject(FileObject);
		ZwClose(hFile);
		return FALSE;

	}
	ObfDereferenceObject(FileObject);
	ZwClose(hFile);

	return TRUE;

}

int GetWindowsRootName(WCHAR *WindowsRootName)
{
	UNICODE_STRING RootName, ObjectName;
	OBJECT_ATTRIBUTES ObjectAttributes;
	HANDLE hLink;
	NTSTATUS status;
	WCHAR *SystemRootName = (WCHAR*)0x7FFE0030;
	WCHAR *ObjectNameBuffer = (WCHAR*)ExAllocatePool(NonPagedPool, 260 * 2);
	if (ObjectNameBuffer == NULL)
	{
		return FALSE;
	}
	RtlZeroMemory(ObjectNameBuffer, 260 * 2);
	RtlInitUnicodeString(&RootName, L"\\SystemRoot");
	InitializeObjectAttributes(&ObjectAttributes, &RootName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwOpenSymbolicLinkObject(&hLink, 1, &ObjectAttributes);
	if (NT_SUCCESS(status))
	{
		ObjectName.Buffer = ObjectNameBuffer;
		ObjectName.Length = 0;
		ObjectName.MaximumLength = 260 * 2;
		status = ZwQuerySymbolicLinkObject(hLink, &ObjectName, NULL);
		if (NT_SUCCESS(status))
		{
			int ObjectNameLength = ObjectName.Length / 2;
			int Index;
			for (Index = ObjectNameLength - 1; Index>0; Index--)
			{
				if (ObjectNameBuffer[Index] == 0x005C)
				{
					if (!MmIsAddressValid(&WindowsRootName[ObjectNameLength - Index]))
					{
						break;

					}
					RtlCopyMemory(WindowsRootName, &ObjectNameBuffer[Index], (ObjectNameLength - Index) * 2);
					ExFreePool(ObjectNameBuffer);
					return TRUE;
				}

			}
		}

	}
	ExFreePool(ObjectNameBuffer);
	if (!MmIsAddressValid(SystemRootName))
	{
		return FALSE;
	}
	if (SystemRootName[1] != 0x003A || SystemRootName[2] != 0x005C)
	{
		return FALSE;
	}
	wcscpy(WindowsRootName, &SystemRootName[2]);

	return TRUE;


}
NTSTATUS  KernelOpenFile(wchar_t *FileFullName,
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions)
{
	WCHAR SystemRootName[32] = L"\\SystemRoot";
	WCHAR *FileNodeName = NULL;
	UNICODE_STRING FilePath;
	PDEVICE_OBJECT RealDevice, DeviceObject;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	IO_STATUS_BLOCK Io_Status_Block;
	OBJECT_ATTRIBUTES obj_attrib;

	FileNodeName = (WCHAR*)ExAllocatePool(NonPagedPool, 260 * 2);
	if (FileNodeName == NULL)
	{
		return status;
	}
	RtlZeroMemory(FileNodeName, 260 * 2);


	DPRINT("FileFullName:%ws--%ws", FileFullName, SystemRootName);

	if (_wcsnicmp(FileFullName, SystemRootName, wcslen(SystemRootName)) == 0)
	{
		size_t Len;
		if (!GetWindowsRootName(FileNodeName))
		{
			ExFreePool(FileNodeName);
			return status;
		}
		Len = wcslen(SystemRootName);
		wcscat(FileNodeName, &FileFullName[Len]);
	}
	else
	{
		if (FileFullName[1] != 0x003A || FileFullName[2] != 0x005C)
		{
			return status;

		}
		wcscpy(FileNodeName, &FileFullName[2]);
	}

	DPRINT("FileNodeName:%S\n", FileNodeName);

	if (!GetDeviceObjectFromFileFullName(FileFullName, &RealDevice, &DeviceObject))
	{

		DPRINT("get device object and real device object faild\n");
		ExFreePool(FileNodeName);
		return status;
	}
	//for test
	//RtlInitUnicodeString(&FilePath,L"\\??\\c:\\windows\\system32\\ntoskrnl.exe");
	RtlInitUnicodeString(&FilePath, FileFullName);

	InitializeObjectAttributes(
		&obj_attrib,
		&FilePath,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
	);
	status = IoCreateFile(
		FileHandle,
		DesiredAccess,
		&obj_attrib,
		&Io_Status_Block,
		0,
		FileAttributes,
		ShareAccess,
		CreateDisposition,
		CreateOptions,
		NULL,
		0,
		CreateFileTypeNone,
		NULL,
		IO_NO_PARAMETER_CHECKING
	);

	/*
	status=IrpCreateFile(&FilePath,DesiredAccess,FileAttributes,ShareAccess,CreateDisposition,CreateOptions,DeviceObject,RealDevice,&FileObject);
	if (!NT_SUCCESS(status))
	{
	if (DebugOn)
	DPRINT(("Irp create file failed\n"));
	ExFreePool(FileNodeName);
	return status;
	}
	if (DebugOn)
	DPRINT(("IrpCreate File Ok\n"));

	status=ObOpenObjectByPointer(
	FileObject,
	OBJ_KERNEL_HANDLE,    //verifier下测试要指定OBJ_KERNEL_HANDLE
	0,
	DesiredAccess|0x100000,
	*IoFileObjectType,
	0,
	FileHandle);

	ObfDereferenceObject(FileObject);

	*/
	return status;

}


BOOLEAN GetSystemKernelModuleInfo(
	__in  PDRIVER_OBJECT DriverObject,
	__out WCHAR **SystemKernelModulePath,
	__out DWORD64 *mSystemKernelModuleBase,
	__out DWORD64 *mSystemKernelModuleSize,
	__in BOOLEAN isNtSysmodule,
	__in WCHAR* szModueNama
)
{
	PLDR_DATA_TABLE_ENTRY LdrDataTable;
	BOOLEAN bRetOK = FALSE;
	int i = 0;
	WCHAR wzKernelName[4][100] = { L"ntkrnlpa.exe", L"ntoskrnl.exe", L"ntkrnlmp.exe", L"ntkrpamp.exe" };
	UNICODE_STRING UnicodeKernelString;

	__try
	{
		*SystemKernelModulePath = (WCHAR *)ExAllocatePool(NonPagedPool, 260 * 2);
		if (*SystemKernelModulePath == NULL)
		{
			*mSystemKernelModuleBase = 0;
			*mSystemKernelModuleSize = 0;
			return FALSE;
		}
		memset(*SystemKernelModulePath, 0, 260 * 2);

		LdrDataTable = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
		do
		{
			if (LdrDataTable->BaseDllName.Length>0 && LdrDataTable->BaseDllName.Buffer != NULL)
			{
				if (MmIsAddressValid(&LdrDataTable->BaseDllName.Buffer[LdrDataTable->BaseDllName.Length / 2 - 1]))
				{
					if (!isNtSysmodule)
					{
						if (wcsstr(LdrDataTable->BaseDllName.Buffer, szModueNama))
						{
							DPRINT("Found Module:%ws base:%p size:%X\n", LdrDataTable->BaseDllName.Buffer, LdrDataTable->DllBase, LdrDataTable->SizeOfImage);
							*mSystemKernelModuleBase = (DWORD64)LdrDataTable->DllBase;
							*mSystemKernelModuleSize = (DWORD64)LdrDataTable->SizeOfImage;
							memcpy(*SystemKernelModulePath, LdrDataTable->FullDllName.Buffer, LdrDataTable->FullDllName.Length);
							bRetOK = TRUE;
							break;
						}
					}
					else
					{
						for (i = 0; i<3; i++)
						{
							RtlInitUnicodeString(&UnicodeKernelString, wzKernelName[i]);
							if (RtlCompareUnicodeString(&LdrDataTable->BaseDllName, &UnicodeKernelString, TRUE) == 0)
							{

								DPRINT("Found KernelModule:%ws base:%p size:%X\n", LdrDataTable->BaseDllName.Buffer, LdrDataTable->DllBase, LdrDataTable->SizeOfImage);

								*mSystemKernelModuleBase = (DWORD64)LdrDataTable->DllBase;
								*mSystemKernelModuleSize = (DWORD64)LdrDataTable->SizeOfImage;
								memcpy(*SystemKernelModulePath, LdrDataTable->FullDllName.Buffer, LdrDataTable->FullDllName.Length);
								bRetOK = TRUE;
								break;
							}
						}
					}
					
				}
			}
			LdrDataTable = (PLDR_DATA_TABLE_ENTRY)LdrDataTable->InLoadOrderLinks.Flink;

		} while ((PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection != LdrDataTable&&LdrDataTable != NULL);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
	return bRetOK;
}

#define _OUT_
#define _IN_
/*
PeLoad
加载文件到内存
参数:
FileFullPath:加载的文件路径	带sysroot的或者是dos路径(带问号的)
ImageModuleBase:加载地址 成功返回加载地址 
pDriverObject:驱动对象 利用这个遍历模块
ExisImageBase:加载的文件在内存中的地址 (也就是所谓的老地址)
bNeedFixImportTable:是否需要修复导入表
*/

BOOLEAN PeLoad(
	_IN_ WCHAR *FileFullPath,
	_OUT_ unsigned char **ImageModeleBase,
	_IN_ PDRIVER_OBJECT pDriverObject,
	_IN_ DWORD64 ExistImageBase,
	_IN_ BOOLEAN bNeedFixImportTable
)
{
	NTSTATUS Status;
	HANDLE hFile;
	LARGE_INTEGER FileSize;
	DWORD64 Length;
	unsigned char *FileBuffer;
	unsigned char *ImageBase;
	IO_STATUS_BLOCK IoStatus;

	Status = KernelOpenFile(FileFullPath, &hFile, 0x100020, 0x80, 1, 1, 0x20);
	if (!NT_SUCCESS(Status))
	{
		DPRINT("Open File failed\n");
		return FALSE;
	}


	DPRINT("Open File ok!file handle:0x%llx\n", hFile);

	Status = KernelGetFileSize(hFile, &FileSize);
	if (!NT_SUCCESS(Status))
	{
		ZwClose(hFile);
		return FALSE;
	}

	DPRINT("+++++++++++++++++++Get File Size ok!FileSize:0x%llx\n", FileSize);

	Length = FileSize.LowPart;
	FileBuffer = (unsigned char *)ExAllocatePool(PagedPool, Length);
	if (FileBuffer == NULL)
	{
		ZwClose(hFile);
		return FALSE;
	}
	DPRINT("+++++++++++++++++++Allocte FileBuffer OK:0x%llx\n", FileBuffer);
	Status = KernelReadFile(hFile, NULL, Length, FileBuffer, &IoStatus);
	if (!NT_SUCCESS(Status))
	{
		ZwClose(hFile);
		ExFreePool(FileBuffer);
		return FALSE;
	}

	DPRINT("Read File Ok\n");
	ZwClose(hFile);

	DPRINT("ZwClose ok\n");

	if (!ImageFile(FileBuffer, &ImageBase))
	{
		DPRINT("ImageFile failed\n");
		ExFreePool(FileBuffer);
		return FALSE;
	}
	ExFreePool(FileBuffer);

	if (bNeedFixImportTable)
	{
		if (!FixImportTable(ImageBase, ExistImageBase, pDriverObject))
		{

			DPRINT("FixImportTable failed\n");
			ExFreePool(ImageBase);
			return FALSE;
		}
	}
	

	if (!FixBaseRelocTable(ImageBase, ExistImageBase))
	{

		DPRINT("FixBaseRelocTable failed\n");
		ExFreePool(ImageBase);
		return FALSE;
	}

	*ImageModeleBase = ImageBase;


	DPRINT("reload success,new kernel base:%p\n", ImageBase);

	return TRUE;
}
