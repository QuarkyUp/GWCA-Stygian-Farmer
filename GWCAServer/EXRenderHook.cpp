#include "EXRenderHook.h"

#include <GWCA\Utilities\PatternScanner.h>

bool EXRenderHook::enabled_ = false;
char* EXRenderHook::renderfunc_ = nullptr;


void stub_hookdetour();

EXRenderHook::EXRenderHook() 
{
	GW::PatternScanner scan(0x401000, 0x49a000);
	renderfunc_ = (char*)scan.FindPattern("\x53\x8D\x0C\x40\xA1", "xxxxx", -0x2B);
}




EXRenderHook::~EXRenderHook()
{
	if (state() == true) set_state(false);
}

void EXRenderHook::set_state(bool state) {
	enabled_ = state; 
	if (state) {
		DWORD old;
		char* det = renderfunc_ - 6;
		VirtualProtect(det, 9, PAGE_EXECUTE_READWRITE, &old);
		memcpy(det, stub_hookdetour, 9);
		VirtualProtect(det, 9, old, &old);
	}
	else {
		DWORD old;
		VirtualProtect(renderfunc_, 3, PAGE_EXECUTE_READWRITE, &old);
		memcpy(renderfunc_, "\x55\x8B\xEC", 3);
		VirtualProtect(renderfunc_, 3, old, &old);
	}
}

void __declspec(naked) EXRenderHook::renderHook()
{
	__asm {
		pop		ebp
		pop		eax
		push	50 
		push	eax
		jmp		Sleep
	}
}

void __declspec(naked) stub_hookdetour()
{
	__asm {
		JumpmanJumpmanJumpman:
			push	EXRenderHook::renderHook
			retn
			push	ebp
			jmp		JumpmanJumpmanJumpman
	}
}