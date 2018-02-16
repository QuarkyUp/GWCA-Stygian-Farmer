#pragma once
class EXRenderHook
{
public:
	EXRenderHook();
	~EXRenderHook();

public:
	bool state() const { return enabled_; }
	void set_state(bool state);

private:
	static void renderHook();

	static bool enabled_;
	static char* renderfunc_;
	static char* renderreturn_;
};

