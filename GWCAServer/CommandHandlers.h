#pragma once

#include <Windows.h>

#include "CommandStructs.h"


extern GW::GameContext* g__ctx;

namespace GWCACom {

	class GWCAConnection;

	static DWORD ConvertID(DWORD id);

	static PseudoAgent BuildPseudoAgent(GW::Agent* agent);

	void __fastcall HandleHeartbeat(BaseRequest* req, GWCAConnection* comm);
	void __fastcall HandlePipeDisconnect(BaseRequest* req, GWCAConnection* comm);
	void __fastcall HandleRequestBasePointers(BaseRequest* req, GWCAConnection* comm);
	void __fastcall HandleMove(Move::Request* req, GWCAConnection* comm);
	void __fastcall HandleUseSkill(UseSkill::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetSkillRecharge(GetSkillRecharge::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetEffectTimeRemaining(GetEffectTimeRemaining::Request* req, GWCAConnection* comm);
	void __fastcall HandleSendChat(SendChat::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetPlayer(GetPlayer::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetAgentByID(GetAgentByID::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetAgentsPos(GetAgentsPos::Request* req, GWCAConnection* comm);
	void __fastcall HandleCloseServer(BaseRequest* req, GWCAConnection* comm);
	void __fastcall HandleAllocateMemory(AllocateMemory::Request* req, GWCAConnection* comm);
	void __fastcall HandleFreeMemory(FreeMemory::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetMapID(GetMapID::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetInstanceType(GetInstanceType::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetInstanceTime(GetInstanceTime::Request* req, GWCAConnection* comm);
	void __fastcall HandleChangeTarget(ChangeTarget::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetCastQueueSize(GetCastQueueSize::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetSkillAdrenaline(GetSkillAdrenaline::Request* req, GWCAConnection* comm);
	void __fastcall HandleSendDialog(SendDialog::Request* req, GWCAConnection* comm);
	void __fastcall HandleGoNpc(GoNpc::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetAgentPos(GetAgentPos::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetPlayerId(GetPlayerId::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetTargetId(GetTargetId::Request* req, GWCAConnection* comm);
	void __fastcall HandleMapTravel(MapTravel::Request* req, GWCAConnection* comm);
	void __fastcall HandleMapTravelRandom(MapTravelRandom::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetMapLoaded(GetMapLoaded::Request* req, GWCAConnection* comm);
	void __fastcall HandleChangeSecondary(ChangeSecondary::Request* req, GWCAConnection* comm);
	void __fastcall HandleSetAttributes(SetAttributes::Request* req, GWCAConnection* comm);
	void __fastcall HandleLoadSkillbar(LoadSkillbar::Request* req, GWCAConnection* comm);
	void __fastcall HandleSetRenderHook(SetRenderHook::Request* req, GWCAConnection* comm);
	void __fastcall HandleTravelGuildHall(TravelGuildHall::Request* req, GWCAConnection* comm);
	void __fastcall HandleLeaveGuildHall(LeaveGuildHall::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetMapInfo(GetMapInfo::Request* req, GWCAConnection* comm);
	void __fastcall HandleSendPacket(SendPacket::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetCharacterName(GetCharacterName::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetAgentPtr(GetAgentPtr::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetFilteredAgentArray(GetFilteredAgentArray::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetItemBySlot(GetItemBySlot::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetItemByAgentId(GetItemByAgentID::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetItemsByModelId(GetItemsByModelID::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetItemName(GetItemName::Request* req, GWCAConnection* comm);
	void __fastcall HandleFixAgentPosition(FixAgentPosition::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetEffectHandle(GetEffectHandle::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetSkillbarHandle(GetSkillbarHandle::Request* req, GWCAConnection* comm);
	void __fastcall HandleInteractAgent(InteractAgent::Request* req, GWCAConnection* comm);
	void __fastcall HandleGetSkillbarInfo(GetSkillbarInfo::Request* req, GWCAConnection* comm);
	void __fastcall HandleBuyMerchantItem(BuyMerchantItem::Request* req, GWCAConnection* comm);
	void __fastcall HandleSellMerchantItem(SellMerchantItem::Request* req, GWCAConnection* comm);
	void __fastcall HandlePerformAction(PerformAction::Request* req, GWCAConnection* comm);
}