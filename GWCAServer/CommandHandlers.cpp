#include "CommandHandlers.h"

#include <random>
#include <winternl.h>

#include "GWCAConnection.h"
#include "EXRenderHook.h"

#include <GWCA\GWCA.h>
#include <GWCA\Managers\SkillbarMgr.h>
#include <GWCA\Managers\EffectMgr.h>
#include <GWCA\Managers\ChatMgr.h>
#include <GWCA\Managers\GuildMgr.h>
#include <GWCA\Managers\MerchantMgr.h>
#include <GWCA\Context\GameContext.h>

using namespace GW;

GW::GameContext* g__ctx = nullptr;

DWORD GWCACom::ConvertID(DWORD id) {
	if (id == Target::Self) {
		return Agents().GetPlayerId();
	}
	else if (id == Target::Current) {
		return Agents().GetTargetId();
	}
	else {
		return id;
	}
}

GWCACom::PseudoAgent GWCACom::BuildPseudoAgent(GW::Agent* agent) {
	GWCACom::PseudoAgent pseudoagent;
	pseudoagent.Id = agent->Id;
	pseudoagent.Rotation = agent->Rotation_angle;
	pseudoagent.NameProperties = agent->NameProperties;
	pseudoagent.pos = agent->pos;
	pseudoagent.Type = agent->Type;
	pseudoagent.MoveX = agent->MoveX;
	pseudoagent.MoveY = agent->MoveY;
	pseudoagent.ExtraType = agent->ExtraType;
	pseudoagent.PlayerNumber = agent->PlayerNumber;
	pseudoagent.Primary = agent->Primary;
	pseudoagent.Secondary = agent->Secondary;
	pseudoagent.Level = agent->Level;
	pseudoagent.TeamId = agent->TeamId;
	pseudoagent.Energy = agent->Energy;
	pseudoagent.MaxEnergy = agent->MaxEnergy;
	pseudoagent.HPPips = agent->HPPips;
	pseudoagent.HP = agent->HP;
	pseudoagent.MaxHP = agent->MaxHP;
	pseudoagent.Effects = agent->Effects;
	pseudoagent.Hex = agent->Hex;
	pseudoagent.ModelState = agent->ModelState;
	pseudoagent.TypeMap = agent->TypeMap;
	pseudoagent.InSpiritRange = agent->InSpiritRange;
	pseudoagent.LoginNumber = agent->LoginNumber;
	pseudoagent.Allegiance = agent->Allegiance;
	pseudoagent.Skill = agent->Skill;
	return pseudoagent;
}

void __fastcall GWCACom::HandleHeartbeat(BaseRequest* req, GWCAConnection* comm) {
	printf("Heartbeat!\n");
	comm->Send<Heartbeat::Reply>(1);
}

void __fastcall GWCACom::HandleRequestBasePointers(BaseRequest* req, GWCAConnection* comm) {
	BasePointers::Reply reply;
	reply.ctx = *(void**)GW::MemoryMgr::GetContextPtr();
	reply.agarray = GW::MemoryMgr::agArrayPtr;
	reply.mapidptr = GW::MemoryMgr::MapIDPtr;
	reply.skillconstptr = GW::MemoryMgr::SkillArray;
	comm->Send(&reply);
}

void __fastcall GWCACom::HandleMove(Move::Request* req, GWCAConnection* comm) {
	Agents().Move(req->x, req->y, req->zplane);
}

void __fastcall GWCACom::HandleUseSkill(UseSkill::Request* req, GWCAConnection* comm) {
	if (req->slot < 0 || req->slot >= 8) {
		printf("Warning, bad slot argument %d for USE_SKILL\n", req->slot);
	} else {
		GW::Skillbarmgr().UseSkill(req->slot, ConvertID(req->target), req->calltarget);
	}
}

void __fastcall GWCACom::HandleGetSkillRecharge(GetSkillRecharge::Request* req, GWCAConnection* comm) {
	GetSkillRecharge::Reply reply = 0;
	if (req->slot < 0 || req->slot >= 8) {
		printf("Warning, bad slot argument %d for GET_SKILL_RECHARGE\n", req->slot);
	} else {
		auto& sbarr = g__ctx->world->skillbar;
		auto& skill = sbarr[0].Skills[req->slot + 1];
		if (skill.SkillId != 0) reply = skill.GetRecharge();
	}
	comm->Send(reply);
}

void __fastcall GWCACom::HandleGetEffectTimeRemaining(GetEffectTimeRemaining::Request* req, GWCAConnection* comm) {
	GW::Effect effect = Effects().GetPlayerEffectById(req->effect);
	GetEffectTimeRemaining::Reply reply = 0;
	if (effect.SkillId != 0) reply = effect.GetTimeRemaining();
	comm->Send(reply);
}

void __fastcall GWCACom::HandleSendChat(SendChat::Request* req, GWCAConnection* comm) {
	Chat().SendChat(req->message, req->channel);
}

void __fastcall GWCACom::HandleGetPlayer(GetPlayer::Request* req, GWCAConnection* comm) {
	GetPlayer::Reply reply;
	GW::Agent* player = Agents().GetPlayer();
	if (player != nullptr) reply = BuildPseudoAgent(player);
	comm->Send<GetPlayer::Reply>(&reply);
}

void __fastcall GWCACom::HandleGetAgentByID(GetAgentByID::Request* req, GWCAConnection* comm) {
	GetAgentByID::Reply reply;
	DWORD id = ConvertID(req->id);
	if (id > 0 && Agents().GetAgentArray().valid()
		&& id < Agents().GetAgentArray().size()) {
		GW::Agent* agent = Agents().GetAgentArray()[id];
		reply = BuildPseudoAgent(agent);
	}
	comm->Send<GetAgentByID::Reply>(&reply);
}

void __fastcall GWCACom::HandleGetAgentsPos(GetAgentsPos::Request* req, GWCAConnection* comm) {
	GW::AgentArray agents = Agents().GetAgentArray();
	long player_id = Agents().GetPlayerId();
	if (agents.valid()) {
		for (size_t i = 0; i < agents.size(); ++i) {
			if (agents[i] == nullptr) continue;
			if (agents[i]->Id <= 0) continue;
			if (agents[i]->Id == player_id) continue;
			if (agents[i]->Allegiance != 0x300) continue;
			if (agents[i]->Type != 0xDB) continue;
			if (agents[i]->GetIsDead()) continue;
			comm->Send(GetAgentsPos::Reply(agents[i]->Id, agents[i]->X, agents[i]->Y));
		}
	}
	comm->Send(GetAgentsPos::Reply(-1, 0, 0));
}

void __fastcall GWCACom::HandlePipeDisconnect(BaseRequest* req, GWCAConnection* comm) {
	comm->Close();
}

void __fastcall GWCACom::HandleCloseServer(BaseRequest* req, GWCAConnection* comm) {
	printf("Close Server!\n");
	comm->parent()->~CommServer();
}

void __fastcall GWCACom::HandleAllocateMemory(AllocateMemory::Request* req, GWCAConnection* comm) {
	void* buffer = malloc(req->size);
	comm->Send<AllocateMemory::Reply>(AllocateMemory::Reply(buffer));
}

void __fastcall GWCACom::HandleFreeMemory(FreeMemory::Request* req, GWCAConnection* comm) {
	free(req->base);
}

void __fastcall GWCACom::HandleGetMapID(GetMapID::Request* req, GWCAConnection* comm) {
	DWORD id = static_cast<DWORD>(Map().GetMapID());
	comm->Send<DWORD>(id);
}

void __fastcall GWCACom::HandleGetInstanceType(GetInstanceType::Request* req, GWCAConnection* comm) {
	DWORD type = static_cast<DWORD>(Map().GetInstanceType());
	comm->Send<DWORD>(type);
}

void __fastcall GWCACom::HandleGetInstanceTime(GetInstanceTime::Request* req, GWCAConnection* comm) {
	comm->Send<DWORD>(Map().GetInstanceTime());
}

void __fastcall GWCACom::HandleChangeTarget(ChangeTarget::Request* req, GWCAConnection* comm) {
	GW::AgentArray agents = Agents().GetAgentArray();
	if (agents.valid()) {
		GW::Agent* target = agents[req->target_id];
		if (target) {
			Agents().ChangeTarget(target);
		}
	}
}

void __fastcall GWCACom::HandleGetCastQueueSize(GetCastQueueSize::Request* req, GWCAConnection* comm) {
	GW::SkillbarArray& sb = g__ctx->world->skillbar;
	//if (req->heroindex >= sb.size() || req->heroindex < 0){
	//	comm->Send<DWORD>(0);
	//	return;
	//}
	comm->Send<DWORD>(sb[req->heroindex].Casting);
}

void __fastcall GWCACom::HandleGetSkillAdrenaline(GetSkillAdrenaline::Request* req, GWCAConnection* comm) {
	GW::SkillbarArray& sb = g__ctx->world->skillbar;
	if (req->heroindex >= sb.size() || req->heroindex < 0){
		comm->Send<DWORD>(0);
		return;
	}
	comm->Send<GetSkillAdrenaline::Reply>(GetSkillAdrenaline::Reply(sb[req->heroindex].Skills[req->slot - 1].AdrenalineB));
}

void __fastcall GWCACom::HandleSendDialog(SendDialog::Request * req, GWCAConnection * comm) {
	Agents().Dialog(req->dialogid);
}

void __fastcall GWCACom::HandleGoNpc(GoNpc::Request* req, GWCAConnection * comm) {
	CtoSMgr::Instance().SendPacket(0xC, 0x33, req->npc_id, req->call_target);
}

void __fastcall GWCACom::HandleGetAgentPos(GetAgentPos::Request * req, GWCAConnection * comm) {
	auto& agentarray = Agents().GetAgentArray();

	if (agentarray.valid()) {
		GW::Agent* agent = agentarray[req->agentid];

		if (agent != nullptr) {
			comm->Send<GetAgentPos::Reply>(GetAgentPos::Reply(agent->pos));
			return;
		}

	}
	comm->Send<GetAgentPos::Reply>(GetAgentPos::Reply(GW::GamePos()));
}

void __fastcall GWCACom::HandleGetPlayerId(GetPlayerId::Request * req, GWCAConnection * comm) {
	comm->Send<GetPlayerId::Reply>(Agents().GetPlayerId());
}

void __fastcall GWCACom::HandleGetTargetId(GetTargetId::Request * req, GWCAConnection * comm) {
	comm->Send<GetPlayerId::Reply>(Agents().GetTargetId());
}

void __fastcall GWCACom::HandleMapTravel(MapTravel::Request * req, GWCAConnection * comm) {
	Map().Travel((Constants::MapID)req->mapid, req->district, req->region, req->language);
}

void __fastcall GWCACom::HandleMapTravelRandom(MapTravelRandom::Request * req, GWCAConnection * comm) {

	const Constants::MapRegion mapregionchoices[9] = {
		Constants::MapRegion::International,
		//Constants::MapRegion::European,
		Constants::MapRegion::European,
		//Constants::MapRegion::European,
		Constants::MapRegion::European,
		Constants::MapRegion::European,
		Constants::MapRegion::European,
		Constants::MapRegion::European,
		Constants::MapRegion::Korean,
		Constants::MapRegion::Chinese,
		Constants::MapRegion::Japanese
	};
	const Constants::MapLanguage maplanguagechoices[9] = {
		Constants::MapLanguage::English,
		//Constants::MapLanguage::English,
		Constants::MapLanguage::French,
		//Constants::MapLanguage::German,
		Constants::MapLanguage::Italian,
		Constants::MapLanguage::Polish,
		Constants::MapLanguage::Russian,
		Constants::MapLanguage::Spanish,
		Constants::MapLanguage::English,
		Constants::MapLanguage::English,
		Constants::MapLanguage::English
	};

	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0, 8);
	int seed = distribution(generator);

	Map().Travel((Constants::MapID)req->mapid, 0, (int)mapregionchoices[seed], (int)maplanguagechoices[seed]);
}

void __fastcall GWCACom::HandleGetMapLoaded(GetMapLoaded::Request * req, GWCAConnection * comm) {
	DWORD* test = MemoryMgr::ReadPtrChain<DWORD*>(MemoryMgr::GetContextPtr(), 2, 0x44, 0x124);
	comm->Send<DWORD>((DWORD)(*test == 0xC8 && Agents().GetPlayer() != nullptr));
}

void __fastcall GWCACom::HandleChangeSecondary(ChangeSecondary::Request * req, GWCAConnection * comm)
{
	GW::Skillbarmgr().ChangeSecondary(req->profession, req->heroindex);
}

void __fastcall GWCACom::HandleSetAttributes(SetAttributes::Request * req, GWCAConnection * comm)
{
	GW::Skillbarmgr().SetAttributes(req->attributecount, req->attributeids, req->attributevalues, req->heroindex);
}

void __fastcall GWCACom::HandleLoadSkillbar(LoadSkillbar::Request * req, GWCAConnection * comm)
{
	GW::Skillbarmgr().LoadSkillbar(req->skillid, req->heroindex);
}

static EXRenderHook rendering = EXRenderHook();
void __fastcall GWCACom::HandleSetRenderHook(SetRenderHook::Request * req, GWCAConnection * comm)
{
	rendering.set_state(req->flag);
}

void __fastcall GWCACom::HandleTravelGuildHall(TravelGuildHall::Request * req, GWCAConnection * comm)
{
	GW::Guildmgr().TravelGH();
}

void __fastcall GWCACom::HandleLeaveGuildHall(LeaveGuildHall::Request * req, GWCAConnection * comm)
{
	GW::Guildmgr().LeaveGH();
}

void __fastcall GWCACom::HandleGetMapInfo(GetMapInfo::Request * req, GWCAConnection * comm)
{
	GetMapInfo::Reply reply;

	DWORD* pdistrict = MemoryMgr::ReadPtrChain<DWORD*>(MemoryMgr::GetContextPtr(), 2, 0x44, 0x1B4);

	if (pdistrict) reply.district = *pdistrict;

	reply.region = Map().GetRegion();
	reply.language = Map().GetLanguage();

	comm->Send<GetMapInfo::Reply>(reply);
}

void __fastcall GWCACom::HandleSendPacket(SendPacket::Request * req, GWCAConnection * comm)
{
	CtoSMgr::gs_send_function_(MemoryMgr::GetGSObject(), req->size, (DWORD*)((BYTE*)req + 0x8));
}

void __fastcall GWCACom::HandleGetCharacterName(GetCharacterName::Request * req, GWCAConnection * comm)
{
	comm->Send((wchar_t*)0xA2AE88, 20 * sizeof(wchar_t));
}

void __fastcall GWCACom::HandleGetAgentPtr(GetAgentPtr::Request * req, GWCAConnection * comm)
{

	auto& agentarray = Agents().GetAgentArray();
	if (req->id < -2 || req->id >= agentarray.size()) {
		comm->Send<Agent*>(nullptr);
		return;
	}

	Agent* agent;
	switch (req->id) {
	case -1:
		agent = Agents().GetTarget();
		break;
	case -2:
		agent = Agents().GetPlayer();
		break;
	default:
		agent = agentarray[req->id];
	}
	comm->Send<Agent*>(agent);
}

void __fastcall GWCACom::HandleGetFilteredAgentArray(GetFilteredAgentArray::Request * req, GWCAConnection * comm)
{
	auto& agentarray = Agents().GetAgentArray();
	if (!agentarray.valid()) return;
	static DWORD* buffer = new DWORD[0x100];
	DWORD count = 0;

	for (Agent* agent : agentarray) {

		if (agent == nullptr) continue;
		if ((agent->Type & req->type) == 0) continue;
		if (req->allegiance != 0 && agent->Allegiance != req->allegiance) continue;
		if ((agent->Effects & req->effects) > 0) continue;

		if (req->X != INFINITY && req->Y != INFINITY) {
			float dist = agent->pos.DistanceTo(Vector2f(req->X, req->Y));
			if (dist > req->maxDist || dist < req->minDist) continue;
		}
		else {
			auto player = Agents().GetPlayer();
			float dist = agent->pos.DistanceTo(player->pos);
			if (dist > req->maxDist || dist < req->minDist) continue;
		}			

		if ([](Agent* ag, GetFilteredAgentArray::Request * req) -> bool {
			if (req->playernumbercount == 0) return true;
			for (DWORD i = 0; i < req->playernumbercount; ++i)
				if (ag->PlayerNumber == req->playernumbers[i]) return true;
			return false;
		}(agent,req) == false) continue;

		buffer[count++] = (DWORD)agent;
	}

	comm->Send<DWORD>(count);
	comm->Send(buffer, count * sizeof(Agent*));
}

void __fastcall GWCACom::HandleGetItemBySlot(GetItemBySlot::Request * req, GWCAConnection * comm)
{
	if (req->bagindex > 18) { comm->Send<DWORD>(0); return; }
	auto ctx = GameContext::instance();
	Inventory* inventory = ctx->items->inventory;
	Bag* bag = inventory->bags[req->bagindex];
	if(bag == nullptr) { comm->Send<DWORD>(0); return; }
	if(req->slot >= bag->Items.size()) { comm->Send<DWORD>(0); return; }
	ItemArray& items = bag->Items;


	comm->Send<GW::Item*>(items[req->slot - 1]);
}

void __fastcall GWCACom::HandleGetItemByAgentId(GetItemByAgentID::Request * req, GWCAConnection * comm)
{
	auto ctx = GameContext::instance();
	auto& agentarray = Agents().GetAgentArray();
	auto agent = agentarray[req->id];

	if(agent == nullptr) { comm->Send<DWORD>(0); return; }
	if(agent->Type != 0x400) { comm->Send<DWORD>(0); return; }
	
	auto& items = ctx->items->itemarray;
	auto item = items[agent->itemid];

	comm->Send<GW::Item*>(item);
}

void __fastcall GWCACom::HandleGetItemsByModelId(GetItemsByModelID::Request * req, GWCAConnection * comm)
{
	static DWORD* items = new DWORD[0x40];
	DWORD count = 0;

	auto ctx = GameContext::instance();
	Inventory* inventory = ctx->items->inventory;
	Bag* bag; Item* item;

	for (DWORD i = req->bagstart; i <= req->bagend; ++i) {
		bag = inventory->bags[i];
		if (bag == nullptr) continue;
		auto& itemarray = bag->Items;
		for (DWORD j = 0; j < itemarray.size(); ++j) {
			item = itemarray[j];
			if (item == nullptr) continue;
			if (item->ModelId == req->modelid) {
				items[count++] = (DWORD)item;
			}
		}
	}

	comm->Send<DWORD>(count);
	comm->Send(items, sizeof(Item*) * count);
}

void* GetItemNameAddress()
{
	const BYTE func_code[] = { 0x55, 0x8B, 0xEC, 0x53, 0x56, 0x8B, 0xF1, 0x8B, 0xDA, 0x85, 0xF6, 0x75, 0x14 };

	BYTE* start = (BYTE*)0x401000;
	const BYTE* end = (BYTE*)0x900000;

	while (start++ != end)
		if (!memcmp(start, func_code, sizeof(func_code)))
			return (void*)start;

	return nullptr;
}

bool HandleGetItemName_wait = false;
void __fastcall ItemNameHandler(GWCACom::GWCAConnection* argument, wchar_t* string) {
	DWORD sz = wcslen(string);
	static wchar_t* buffer = nullptr;
	if (buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
	buffer = new wchar_t[sz + 1];
	wcscpy(buffer, string);
	argument->Send(buffer, sizeof(wchar_t) * (sz+1));
	HandleGetItemName_wait = false;
}

void __fastcall GWCACom::HandleGetItemName(GetItemName::Request * req, GWCAConnection * comm)
{
	typedef void(__fastcall *Handler_t)(GWCAConnection* argument, wchar_t* string);
	typedef void(__fastcall *GetString_t)(void* hString, void* callback, GWCAConnection* argument);

	static GetString_t getstring = (GetString_t)GetItemNameAddress();

	auto item = req->item;
	if(item == nullptr) { comm->Send<DWORD>(0); return; }
	void* namestring = *(void**)((BYTE*)item + 0x38);
	if (namestring == nullptr) { comm->Send<DWORD>(0); return; }


	GameThreadMgr::Instance().Enqueue([=] { getstring(namestring, ItemNameHandler, comm); } );
	HandleGetItemName_wait = true;
	while (HandleGetItemName_wait) {
		Sleep(10);
	}
}

void* GetAgentPositionFunc() {
	const BYTE func_code[] = { 0xFF, 0x76, 0x04, 0x8D, 0x8F, 0xCC, 0x01, 0x00, 0x00 };

	BYTE* start = (BYTE*)0x401000;
	const BYTE* end = (BYTE*)0x900000;

	while (start++ != end)
		if (!memcmp(start, func_code, sizeof(func_code)))
			return (void*)(start - 0x1E);

	return nullptr;
}

void __fastcall GWCACom::HandleFixAgentPosition(FixAgentPosition::Request * req, GWCAConnection * comm)
{
	typedef struct _Updatepos {
		DWORD header;
		AgentID agentid;
		GamePos pos;
	}UpdatePos, *lpUpdatePos;

	typedef bool(__fastcall *UpdatePos_t)(lpUpdatePos info);


	static lpUpdatePos buf = new UpdatePos();
	static UpdatePos_t call = (UpdatePos_t)GetAgentPositionFunc();

	auto ctx = GameContext::instance();

	auto& agmvarray = ctx->agent->agentmovement;
	if (!agmvarray.valid()) return;

	auto agmv = agmvarray[req->agentid];
	if (agmv == nullptr) return;

	buf->pos = GamePos( agmv->X, agmv->Y );

	GameThreadMgr::Instance().Enqueue([=]{ call(buf); });
}

void __fastcall GWCACom::HandleGetEffectHandle(GetEffectHandle::Request * req, GWCAConnection * comm)
{
	auto& effectsctx = Effects().GetPartyEffectArray();
	if (!effectsctx.valid()) { comm->Send<Effect*>(nullptr); return; }
	auto& effectarr = effectsctx[req->index].Effects;
	if (!effectarr.valid()) { comm->Send<Effect*>(nullptr); return; }

	for (auto& effect : effectarr) {
		if (effect.SkillId == req->skillid) {
			comm->Send<Effect*>(&effect);
			return;
		}
	}

	comm->Send<Effect*>(nullptr);
}

void __fastcall GWCACom::HandleGetSkillbarHandle(GetSkillbarHandle::Request * req, GWCAConnection * comm)
{
	auto& skillbararr = g__ctx->world->skillbar;
	if (!skillbararr.valid()) { comm->Send<GW::Skillbar*>(nullptr); return; }
	auto skillbar = &skillbararr[req->index];
	comm->Send<GW::Skillbar*>(skillbar);
}


void* GetInteractAgent() {
	const BYTE func_code[] = { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x10, 0x53, 0x8B, 0xD9, 0x56, 0x83, 0xFB, 0x06 };

	BYTE* start = (BYTE*)0x401000;
	const BYTE* end = (BYTE*)0x900000;

	while (start++ != end)
		if (!memcmp(start, func_code, sizeof(func_code)))
			return (void*)start;

	return nullptr;
}

void __fastcall GWCACom::HandleInteractAgent(InteractAgent::Request * req, GWCAConnection * comm)
{
	typedef void(__fastcall *InteractAgent_t)(e_InteractType type, GW::AgentID id);
	static InteractAgent_t interactagent = (InteractAgent_t)GetInteractAgent();

	GameThreadMgr::Instance().Enqueue([=]{ interactagent(req->type, req->agent); });
}

void __fastcall GWCACom::HandleGetSkillbarInfo(GetSkillbarInfo::Request * req, GWCAConnection * comm)
{
	static GetSkillbarInfo::Reply::Info info[8];
	auto& sb = *(req->skillbar);
	auto player = Agents().GetAgentByID(sb.AgentId);
	auto& effectarr = GW::Effects().GetPartyEffectArray();
	AgentEffects* effects = nullptr;

	for (int i = 0; i < effectarr.size(); ++i)
		if (effectarr[i].AgentId == sb.AgentId)
			effects = &effectarr[i];


	for (int i = 0; i < 8; ++i) {
		auto skillinfo = GW::Skillbarmgr().GetSkillConstantData(sb.Skills[i].SkillId);

		info[i].isRecharged = sb.Skills[i].Recharge == 0;
		info[i].energyMet = (skillinfo.GetEnergyCost() < (player->Energy * player->MaxEnergy));
		info[i].adrenalineMet = (sb.Skills[i].AdrenalineB >= skillinfo.Adrenaline);
		info[i].effectOff = true;

		if (effects == nullptr || !effects->Effects.valid()) {
			continue;
		}

		for (int j = 0; j < effects->Effects.size();++i)
			if(effects->Effects[j].SkillId == sb.Skills[i].SkillId) 
				info[i].effectOff = false;
	}

	comm->Send(&info, sizeof(GetSkillbarInfo::Reply::Info) * 8);
}

void __fastcall GWCACom::HandleBuyMerchantItem(BuyMerchantItem::Request * req, GWCAConnection * comm)
{
	Merchant().BuyMerchantItem(req->modelid, req->count);
}

void __fastcall GWCACom::HandleSellMerchantItem(SellMerchantItem::Request * req, GWCAConnection * comm)
{
	Merchant().SellMerchantItem(req->item, req->count);
}



void* GetUseTargetFunc() {
	const BYTE func_code[] = { 0x55, 0x8B, 0xEC, 0x81, 0xEC, 0xF8, 0x00, 0x00, 0x00, 0x53, 0x56, 0x57, 0x8B, 0xFA, 0x8B, 0xF1 };

	BYTE* start = (BYTE*)0x401000;
	const BYTE* end = (BYTE*)0x900000;

	while (start++ != end)
		if (!memcmp(start, func_code, sizeof(func_code)))
			return (void*)start;

	return nullptr;
}

void* perform_action_func = (void*)0x0060F540;
void __stdcall perform_action(DWORD id,DWORD flag) {
	__asm {
		mov ecx, dword ptr ds : [0xA3FF30] // Should be 0xA3FF30? is 0xA3FF38
		mov ecx, dword ptr ds : [ecx + 0x250]
		mov ecx, dword ptr ds : [ecx + 0x10]
		mov ecx, dword ptr ds : [ecx]
		push 0
		push 0
		push id
		mov edx, esp
		push 0
		push edx
		push flag
		call perform_action_func
		pop eax
		pop ebx
		pop ecx
	}
}

void __fastcall GWCACom::HandlePerformAction(PerformAction::Request * req, GWCAConnection * comm)
{
	GameThreadMgr::Instance().Enqueue([=]{ perform_action(req->type, req->flag); });
}

