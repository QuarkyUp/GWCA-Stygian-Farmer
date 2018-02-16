#include "GWCAServer.h"

#include "..\GWCA\GWCA\GWCA.h"
#include "CommandEnums.h"

#include "Console.h"

GWCACom::CommServer::CommServer(std::wstring pipe_name,size_t amount_of_handlers)
	: pipe_name_(pipe_name), pipe_handle_(INVALID_HANDLE_VALUE), handlers_(std::vector<std::function<void(BaseRequest*, GWCAConnection*)>>(amount_of_handlers)){
	
}

DWORD GWCACom::CommServer::MainRoutine() {
	while (pipe_name_ != L"") {
		printf("Creating pipe\n");
		pipe_handle_ = CreateNamedPipe(
			pipe_name_.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			GWCAConnection::BUF_SIZE,
			GWCAConnection::BUF_SIZE,
			0,
			NULL);

		if (pipe_handle_ == INVALID_HANDLE_VALUE) {
			DBG("CreateNamedPipe failed, error=%d", GetLastError());
			return -1;
		}

		printf("Accepting connection... ");
		bool connected = ConnectNamedPipe(pipe_handle_, NULL) ? 
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (connected){
			printf("success!, creating connection thread\n");
			open_connections_.push_back(new GWCAConnection(pipe_handle_, this));
		} else {
			printf("failed!\n");
			CloseHandle(pipe_handle_);
		}



		Sleep(5);
	}
	return TRUE;
}

GWCACom::CommServer::~CommServer() {
	for (GWCAConnection* connection : open_connections_) {
		DisconnectNamedPipe(connection->pipe_handle());
		delete connection;
	}
	pipe_name_ = L"";
}

