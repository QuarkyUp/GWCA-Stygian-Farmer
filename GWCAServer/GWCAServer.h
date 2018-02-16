#pragma once

#include <Windows.h>
#include <vector>
#include <memory>
#include <string>
#include <functional>

#include "CommandEnums.h"
#include "GWCAConnection.h"

namespace GWCACom{

	class GWCAConnection;
	struct BaseRequest;
		
	class CommServer {
		friend class GWCAConnection;
	public:

		template <class T>
		using GWCAComHandler = void(__fastcall *)(T request, GWCAConnection* reciever);

		CommServer(std::wstring pipe_name,size_t amount_of_handlers);
		~CommServer();

		template <class Handler_t>
		void AddCommand(DWORD command,GWCAComHandler<Handler_t*> handler)
		{
			handlers_[command] = [handler](BaseRequest* request, GWCAConnection* reciever){
				handler((Handler_t*)request, reciever);
			};
		}

		DWORD MainRoutine();
	private:
		
		std::wstring pipe_name_;
		HANDLE pipe_handle_;
		HANDLE thread_handle_;
		std::vector<GWCAConnection*> open_connections_;
		std::vector<std::function<void(BaseRequest*, GWCAConnection*)>> handlers_;
	};

}