#pragma once

#include <Windows.h>

#include "GWCAServer.h"

namespace GWCACom {
	class CommServer;
	struct BaseRequest;

	class GWCAConnection {
	public:
		static const DWORD BUF_SIZE = 0x1000;

	public:
		GWCAConnection(HANDLE pipe, CommServer* parent);

		~GWCAConnection() { Close(); }

		void Close();

		void HandleCommands(BaseRequest* req);

		DWORD MainRoutine();

		inline HANDLE threadhandle() const { return thread_handle_; }
		inline CommServer* parent() const { return parent_; }

		// send reply by void* reference
		void Send(void* buffer, size_t size_of_buffer);

		// send reply by typed reference
		template <class Reply_t>
		void Send(Reply_t* reply){
			WriteFile(pipe_handle_, reply, sizeof(Reply_t),
				&last_reply_size_, NULL);
		}

		// send reply by value
		template <class Reply_t>
		void Send(Reply_t reply){
			WriteFile(pipe_handle_, &reply, sizeof(Reply_t),
				&last_reply_size_, NULL);
		}

		HANDLE pipe_handle() const { return pipe_handle_; }

	private:

		static DWORD WINAPI ThreadEntry(GWCAConnection* thisptr);

		char request_buffer_[BUF_SIZE];
		char reply_buffer_[BUF_SIZE];
		DWORD last_request_size_;
		DWORD last_reply_size_;
		DWORD last_bytes_written_;
		CommServer* parent_;
		HANDLE pipe_handle_;
		HANDLE thread_handle_;
	};
}