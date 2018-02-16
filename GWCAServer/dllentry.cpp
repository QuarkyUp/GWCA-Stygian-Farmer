#include <GWCA\GWCA.h>


#include "GWCAServer.h"

#include <ctime>

#include "CommandEnums.h"
#include "CommandStructs.h"
#include "CommandHandlers.h"
#include <GWCA\Managers\MemoryMgr.h>

#include "Console.h"

HANDLE main_thread = nullptr;
HANDLE title_thread = nullptr;
GWCACom::CommServer* server = nullptr;
FILE* console;

DWORD WINAPI title_updater(LPVOID) {
	wchar_t buffer[0x100];
	HWND gwhwnd = GW::MemoryMgr::GetGWWindowHandle();
	time_t start_time = time(nullptr);
	while (1) {
		DWORD seconds = time(nullptr) - start_time;
		DWORD minutes = (seconds / 60) % 60;
		DWORD hours = (seconds / 3600);
		seconds = seconds % 60;

		swprintf_s(buffer, L"GW - %s - [%d:%02d:%02d]", (wchar_t*)0x00A2AE88, hours, minutes, seconds);
		SetWindowTextW(gwhwnd, buffer);

		Sleep(1000);
	}
	return TRUE;
}

void init(HMODULE hModule){

	INIT_CONSOLE(console);

	TCHAR buf[MAX_PATH];
	swprintf_s(buf, L"\\\\.\\pipe\\GWComm_%d", GetCurrentProcessId());
	printf("Pipe name: %S\n", buf);

	using namespace GWCACom;

	server = new CommServer(buf,kAmountofCommands);

	server->AddCommand<BaseRequest>(CMD::HEARTBEAT,HandleHeartbeat);
	server->AddCommand<BaseRequest>(CMD::DISCONNECT_PIPE_CONNECTION,HandlePipeDisconnect);
	server->AddCommand<BaseRequest>(CMD::UNLOAD_SERVER,HandleCloseServer);
	server->AddCommand<BaseRequest>(CMD::REQUEST_BASE_POINTERS,HandleRequestBasePointers);
	server->AddCommand<Move::Request>(CMD::MOVE,HandleMove);
	server->AddCommand<UseSkill::Request>(CMD::USE_SKILL,HandleUseSkill);
	server->AddCommand<GetSkillRecharge::Request>(CMD::GET_SKILL_RECHARGE,HandleGetSkillRecharge);
	server->AddCommand<GetEffectTimeRemaining::Request>(CMD::GET_EFFECT_TIME_REMAINING,HandleGetEffectTimeRemaining);
	server->AddCommand<SendChat::Request>(CMD::SEND_CHAT,HandleSendChat);
	server->AddCommand<GetPlayer::Request>(CMD::GET_PLAYER,HandleGetPlayer);
	server->AddCommand<GetAgentByID::Request>(CMD::GET_AGENT_BY_ID,HandleGetAgentByID);
	server->AddCommand<GetAgentsPos::Request>(CMD::GET_AGENTS_POS,HandleGetAgentsPos);
	server->AddCommand<AllocateMemory::Request>(CMD::ALLOCATE_MEMORY,HandleAllocateMemory);
	server->AddCommand<FreeMemory::Request>(CMD::FREE_MEMORY,HandleFreeMemory);
	server->AddCommand<GetMapID::Request>(CMD::GET_MAP_ID,HandleGetMapID);
	server->AddCommand<GetInstanceType::Request>(CMD::GET_INSTANCE_TYPE,HandleGetInstanceType);
	server->AddCommand<GetInstanceTime::Request>(CMD::GET_INSTANCE_TIME,HandleGetInstanceTime);
	server->AddCommand<ChangeTarget::Request>(CMD::CHANGE_TARGET,HandleChangeTarget);
	server->AddCommand<GetSkillAdrenaline::Request>(CMD::GET_SKILL_ADRENALINE,HandleGetSkillAdrenaline);
	server->AddCommand<GetCastQueueSize::Request>(CMD::GET_CAST_QUEUE_SIZE,HandleGetCastQueueSize);
	server->AddCommand<GoNpc::Request>(CMD::GO_NPC, HandleGoNpc);
	server->AddCommand<SendDialog::Request>(CMD::SEND_DIALOG, HandleSendDialog);
	server->AddCommand<GetAgentPos::Request>(CMD::GET_AGENT_POS, HandleGetAgentPos);
	server->AddCommand<GetPlayerId::Request>(CMD::GET_PLAYER_ID, HandleGetPlayerId);
	server->AddCommand<GetTargetId::Request>(CMD::GET_TARGET_ID, HandleGetTargetId);
	server->AddCommand<MapTravel::Request>(CMD::MAP_TRAVEL, HandleMapTravel);
	server->AddCommand<MapTravelRandom::Request>(CMD::MAP_TRAVEL_RANDOM, HandleMapTravelRandom);
	server->AddCommand<GetMapLoaded::Request>(CMD::GET_MAP_LOADED, HandleGetMapLoaded);
	server->AddCommand<ChangeSecondary::Request>(CMD::CHANGE_SECONDARY, HandleChangeSecondary);
	server->AddCommand<SetAttributes::Request>(CMD::SET_ATTRIBUTES, HandleSetAttributes);
	server->AddCommand<LoadSkillbar::Request>(CMD::LOAD_SKILLBAR, HandleLoadSkillbar);
	server->AddCommand<SetRenderHook::Request>(CMD::SET_RENDER_HOOK, HandleSetRenderHook);
	server->AddCommand<TravelGuildHall::Request>(CMD::TRAVEL_GUILD_HALL, HandleTravelGuildHall);
	server->AddCommand<LeaveGuildHall::Request>(CMD::LEAVE_GUILD_HALL, HandleLeaveGuildHall);
	server->AddCommand<GetMapInfo::Request>(CMD::GET_MAP_INFO, HandleGetMapInfo);
	server->AddCommand<SendPacket::Request>(CMD::SEND_PACKET, HandleSendPacket);
	server->AddCommand<GetCharacterName::Request>(CMD::GET_CHARACTER_NAME, HandleGetCharacterName);
	server->AddCommand<GetAgentPtr::Request>(CMD::GET_AGENT_HANDLE, HandleGetAgentPtr);
	server->AddCommand<GetFilteredAgentArray::Request>(CMD::GET_FILTERED_AGENT_ARRAY, HandleGetFilteredAgentArray);
	server->AddCommand<GetItemBySlot::Request>(CMD::GET_ITEM_BY_SLOT, HandleGetItemBySlot);
	server->AddCommand<GetItemByAgentID::Request>(CMD::GET_ITEM_BY_AGENTID, HandleGetItemByAgentId);
	server->AddCommand<GetItemsByModelID::Request>(CMD::GET_ITEMS_BY_MODELID, HandleGetItemsByModelId);
	server->AddCommand<GetItemName::Request>(CMD::GET_ITEM_NAME, HandleGetItemName);
	server->AddCommand<GetSkillbarHandle::Request>(CMD::GET_SKILLBAR_HANDLE, HandleGetSkillbarHandle);
	server->AddCommand<GetEffectHandle::Request>(CMD::GET_EFFECT_HANDLE, HandleGetEffectHandle);
	server->AddCommand<FixAgentPosition::Request>(CMD::FIX_AGENT_POSITION, HandleFixAgentPosition);
	server->AddCommand<InteractAgent::Request>(CMD::INTERACT_AGENT, HandleInteractAgent);
	server->AddCommand<GetSkillbarInfo::Request>(CMD::GET_SKILLBAR_INFO, HandleGetSkillbarInfo);
	server->AddCommand<BuyMerchantItem::Request>(CMD::BUY_MERCHANT_ITEM, HandleBuyMerchantItem);
	server->AddCommand<SellMerchantItem::Request>(CMD::SELL_MERCHANT_ITEM, HandleSellMerchantItem);
	server->AddCommand<PerformAction::Request>(CMD::USE_TARGET_FUNC, HandlePerformAction);

	server->MainRoutine();

	DBG("Done");
	main_thread = nullptr;
	FreeLibraryAndExitThread(hModule, EXIT_SUCCESS);
}

BOOL WINAPI DllMain(_In_ HMODULE _HDllHandle, _In_ DWORD _Reason, _In_opt_ LPVOID _Reserved){
	DisableThreadLibraryCalls(_HDllHandle);
	if (_Reason == DLL_PROCESS_ATTACH){

		if (!GW::Api::Initialize()) {
			return FALSE;
		}

		g__ctx = GW::GameContext::instance();
		main_thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)init, _HDllHandle, 0, 0);
		title_thread = CreateThread(0, 0, title_updater, 0, 0, 0);
	}
	else if (_Reason == DLL_PROCESS_DETACH) {
		delete server;
		if (main_thread) TerminateThread(main_thread, EXIT_SUCCESS);
		TerminateThread(title_thread, EXIT_SUCCESS);
		GW::Api::Destruct();
		KILL_CONSOLE(console);
	}
	return TRUE;
}
