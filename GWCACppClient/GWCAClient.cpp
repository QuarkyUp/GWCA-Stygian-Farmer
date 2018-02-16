#include "GWCAClient.h"

GWCAClient* GWCAClient::instance_ = nullptr;

bool GWCAClient::Connect(TCHAR* pipe_name) {
	assert(!connected_);
	assert(pipe_handle_ == INVALID_HANDLE_VALUE);

	pipe_handle_ = CreateFile(pipe_name,
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (pipe_handle_ == INVALID_HANDLE_VALUE) {
		printf("Failed to connect to pipe, error %d\n", GetLastError());
		return false;
	} else {
		connected_ = true;
		return true;
	}
}

bool GWCAClient::ConnectByPID(DWORD pid) {
	TCHAR buf[MAX_PATH];
	swprintf_s(buf, L"\\\\.\\pipe\\GWComm_%d", pid);
	return Connect(buf);
}

bool GWCAClient::Heartbeat() {
	BOOL sent = Send(Heartbeat::Request());
	DWORD reply;
	BOOL received = Receive(&reply);
	return sent && received && reply == 1;
}

void GWCAClient::Disconnect() {
	Send(Disconnect::Request());
	connected_ = false;
	pipe_handle_ = INVALID_HANDLE_VALUE;
}

void GWCAClient::SendChat(wchar_t channel, std::wstring message) {
	SendChat::Request req;
	req.channel = channel;
	wcscpy_s(req.message, message.c_str());
	Send(req);
}


std::vector<AgentPosition> GWCAClient::GetAgentsPos() {
	Send(GetAgentsPos::Request());
	std::vector<AgentPosition> positions;
	while (true) {
		GetAgentsPos::Reply reply;
		if (!Receive(&reply)) break;

		if (reply.id > 0) {
			positions.push_back(AgentPosition(reply.id, reply.x, reply.y));
		} else {
			break;
		}
	}
	return positions;
}