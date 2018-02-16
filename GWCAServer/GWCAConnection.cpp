#include "GWCAConnection.h"

#include "CommandStructs.h"
#include <GWCA\GWCA.h>

using namespace GW;

void GWCACom::GWCAConnection::HandleCommands(BaseRequest* request) {
	auto func = parent_->handlers_[request->cmd];
	if(func != NULL)
		func(request, this);
}

DWORD WINAPI GWCACom::GWCAConnection::ThreadEntry(GWCAConnection* thisptr) {
	return thisptr->MainRoutine();
}

DWORD GWCACom::GWCAConnection::MainRoutine() {
	while (pipe_handle_ != NULL) {
		//printf("Connection reading from pipe\n");
		if (ReadFile(pipe_handle_, request_buffer_, BUF_SIZE, &last_request_size_, NULL)) {
			//printf("Read %d bytes from pipe!\n", last_request_size_);
			HandleCommands((BaseRequest*)request_buffer_);
		}
		Sleep(10);
	}
	return TRUE;
}

void GWCACom::GWCAConnection::Close() {
	printf("Closing connection\n");

	if (pipe_handle_) 
	{ 
		DisconnectNamedPipe(pipe_handle_);
		CloseHandle(pipe_handle_);
	}

	TerminateThread(thread_handle_, EXIT_SUCCESS);
}

GWCACom::GWCAConnection::GWCAConnection(HANDLE pipe, CommServer* parent) : parent_(parent), pipe_handle_(pipe) {
	thread_handle_ = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadEntry, this, 0, 0);
}

void GWCACom::GWCAConnection::Send(void* buffer, size_t size_of_buffer) {
	WriteFile(pipe_handle_, buffer, size_of_buffer,
		&last_reply_size_, NULL);
}




