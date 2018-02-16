#pragma once

#include <Windows.h>

#include <GWCA\Constants\Constants.h>
#include <GWCA\Constants\Skills.h>
#include <GWCA\GWStructures.h>
#include <GWCA\GameEntities\Position.h>

#include "CommandEnums.h"

using namespace GW;

namespace GWCACom {

	/*
	Each specific Request should have a default constructor.
	The constructor must set up the cmd field to the correct CMD value;
	
	Additional constructors that set up some or all fields are optionals
	and should be made for your convenience only. They also must set up 
	the cmd field.

	A correspondent Reply should be present for every function that 
	returns a value (e.g. non-void).
	*/
	
	struct BaseRequest {
	protected:
		BaseRequest(CMD _cmd) : cmd(_cmd) {}
	public:
		const CMD cmd;
	};

	struct BaseReply {
	protected:
		BaseReply() {}
	};

	struct Heartbeat {
		struct Request : BaseRequest {
			Request() : BaseRequest(HEARTBEAT) {}
		};
		typedef DWORD Reply;
	};

	struct Disconnect {
		struct Request : BaseRequest {
			Request() : BaseRequest(DISCONNECT_PIPE_CONNECTION) {}
		};
	};

	struct Move {
		struct Request : BaseRequest {
			Request(float _x, float _y, DWORD _zplane)
				: BaseRequest(MOVE), x(_x), y(_y), zplane(_zplane) {}
			Request(float _x, float _y) : Request(_x, _y, 0) {}
			Request() : Request(0, 0, 0) {}
			float x;
			float y;
			DWORD zplane;
		};
	};
	
	struct UseSkill {
		struct Request : BaseRequest {
			Request(DWORD _slot, DWORD _target, BOOL _call)
				: BaseRequest(USE_SKILL), slot(_slot), target(_target), calltarget(_call) {}
			Request() : Request(0, 0, false) {}
			DWORD slot;
			DWORD target;
			BOOL calltarget;
		};
	};

	struct GetCastQueueSize {
		struct Request : BaseRequest {
			DWORD heroindex;
		};

		struct Reply : BaseReply {
			DWORD castqueuesize;
		};
	};

	struct GetSkillRecharge {
		struct Request : BaseRequest {
			Request(DWORD _slot) : slot(_slot), BaseRequest(GET_SKILL_RECHARGE) {}
			Request() : Request(0) {}
			DWORD slot;
		};
		typedef DWORD Reply; // note: 0 if recharged
	};

	struct GetSkillAdrenaline {
		struct Request : BaseRequest {

			Request(DWORD _heroindex,DWORD _slot) 
				: BaseRequest(GET_SKILL_ADRENALINE), heroindex(_heroindex), slot(_slot){}

			DWORD heroindex;
			DWORD slot;
		};

		struct Reply : BaseReply {
			Reply(DWORD _adren) : adrenaline(_adren){}
			DWORD adrenaline;
		};
	};

	struct GetEffectTimeRemaining {
		struct Request : BaseRequest {
			Request(Constants::SkillID _effect) : effect(_effect), BaseRequest(GET_EFFECT_TIME_REMAINING) {}
			Constants::SkillID effect;
		};
		typedef long Reply; // in milliseconds. note: 0 if effect was not found
	};
	
	struct SendChat {
		struct Request : BaseRequest {
			Request() : BaseRequest(SEND_CHAT) {}
			wchar_t channel;
			wchar_t message[137];
		};
	};
	
	struct BasePointers {
		struct Request : BaseRequest {
			Request() : BaseRequest(REQUEST_BASE_POINTERS) {}
		};
		struct Reply : BaseReply {
			void* ctx;
			void* agarray;
			void* mapidptr;
			void* skillconstptr;
		};
	};

	struct PseudoAgent {
		PseudoAgent() : Id(0) {}
		// not a proper Agent struct, we ignore pointer/useless/unknown fields
		// to minimize data transfer. Feel free to add/uncomment fields if needed.
		long Id; //AgentId
		float Rotation; //Rotation in radians from East (-pi to pi)
		long NameProperties; //Bitmap basically telling what the agent is
		union {
			struct {
				float X; //X coord in float
				float Y; //Y coord in float
				DWORD Ground;
			};
			struct {
				GW::GamePos pos;
			};
		};
		long Type; //0xDB = players, npc's, monsters etc. 0x200 = signpost/chest/object (unclickable). 0x400 = item to pick up
		float MoveX; //If moving, how much on the X axis per second
		float MoveY; //If moving, how much on the Y axis per second
		long ExtraType;
		//float WeaponAttackSpeed; //The base attack speed in float of last attacks weapon. 1.33 = axe, sWORD, daggers etc.
		//float AttackSpeedModifier; //Attack speed modifier of the last attack. 0.67 = 33% increase (1-.33)
		WORD PlayerNumber; //Selfexplanatory. All non-players have identifiers for their type. Two of the same mob = same number
		BYTE Primary; //Primary profession 0-10 (None,W,R,Mo,N,Me,E,A,Rt,P,D)
		BYTE Secondary; //Secondary profession 0-10 (None,W,R,Mo,N,Me,E,A,Rt,P,D)
		BYTE Level; //Duh!
		BYTE TeamId; //0=None, 1=Blue, 2=Red, 3=Yellow
		float Energy; //Only works for yourself
		long MaxEnergy; //Only works for yourself
		float HPPips; //Regen/degen as float
		float HP; //Health in % where 1=100% and 0=0%
		long MaxHP; //Only works for yourself
		long Effects; //Bitmap for effects to display when targetted. DOES include hexes
		BYTE Hex; //Bitmap for the hex effect when targetted (apparently obsolete!)
		long ModelState; //Different values for different states of the model.
		long TypeMap; //Odd variable! 0x08 = dead, 0xC00 = boss, 0x40000 = spirit, 0x400000 = player
		long InSpiritRange; //Tells if agent is within spirit range of you. Doesn't work anymore?
		long LoginNumber; //Unique number in instance that only works for players
		WORD Allegiance; //0x100 = ally/non-attackable, 0x300 = enemy, 0x400 = spirit/pet, 0x500 = minion, 0x600 = npc/minipet
						 //WORD WeaponType; //1=bow, 2=axe, 3=hammer, 4=daggers, 5=scythe, 6=spear, 7=sWORD, 10=wand, 12=staff, 14=staff
		WORD Skill; //0 = not using a skill. Anything else is the Id of that skill
					//WORD WeaponItemId;
					//WORD OffhandItemId;

		inline float GetHealth() const { return HP * MaxHP; }
		inline float GetEnergy() const { return Energy * MaxEnergy; }

		// Health Bar Effect Bitmasks.
		inline bool GetIsBleeding() const { return (Effects & 1) != 0; }
		inline bool GetIsConditioned() const { return (Effects & 2) != 0; }
		inline bool GetIsDead() const { return (Effects & 16) != 0; }
		inline bool GetIsDeepWounded() const { return (Effects & 32) != 0; }
		inline bool GetIsPoisoned() const { return (Effects & 64) != 0; }
		inline bool GetIsEnchanted() const { return (Effects & 128) != 0; }
		inline bool GetIsDegenHexed() const { return (Effects & 1024) != 0; }
		inline bool GetIsHexed() const { return (Effects & 2048) != 0; }
		inline bool GetIsWeaponSpelled() const { return (Effects & 32768) != 0; }

		// Agent Type Bitmasks.
		inline bool GetIsLivingType() const { return (Type & 0xDB) != 0; }
		inline bool GetIsSignpostType() const { return (Type & 0x200) != 0; }
		inline bool GetIsItemType() const { return (Type & 0x400) != 0; }

		// Agent TypeMap Bitmasks.
		inline bool GetInCombatStance() const { return (TypeMap & 1) != 0; }
		inline bool GetHasQuest() const { return (TypeMap & 2) != 0; }
		inline bool GetIsDeadByTypeMap() const { return (TypeMap & 8) != 0; }
		inline bool GetIsFemale() const { return (TypeMap & 512) != 0; }
		inline bool GetHasBossGlow() const { return (TypeMap & 1024) != 0; }
		inline bool GetIsHidingCape() const { return (TypeMap & 4096) != 0; }
		inline bool GetCanBeViewedInPartyWindow() const { return (TypeMap & 131072) != 0; }
		inline bool GetIsSpawned() const { return (TypeMap & 262144) != 0; }
		inline bool GetIsBeingObserved() const { return (TypeMap & 4194304) != 0; }

		// Modelstates.
		inline bool GetIsKnockedDown() const { return ModelState == 1104; }
		inline bool GetIsMoving() const { return ModelState == 12 || ModelState == 76 || ModelState == 204; }
		inline bool GetIsAttacking() const { return ModelState == 96 || ModelState == 1088 || ModelState == 1120; }
	};
	struct GetAgentByID {
		struct Request : BaseRequest {
			Request(int _id) : BaseRequest(GET_AGENT_BY_ID), id(_id) {}
			Request() : Request(0) {}
			int id;
		};
		typedef PseudoAgent Reply;
	};
	

	struct GetPlayer {
		struct Request : BaseRequest {
			Request() : BaseRequest(GET_PLAYER) {}
		};
		typedef PseudoAgent Reply;
	};
	

	// works as follows: first send request, then send
	// multiple replies, each with id and position of an agent
	// reply stream ends when id = -1
	struct GetAgentsPos {
		struct Request : BaseRequest {
			Request() : BaseRequest(GET_AGENTS_POS) {}
		};
		struct Reply : BaseReply {
			Reply(long _id, float _x, float _y) :
				id(_id), x(_x), y(_y) {}
			Reply() : Reply(0, 0, 0) {}
			long id;
			float x;
			float y;
		};
	};

	struct AllocateMemory {
		struct Request : BaseRequest {
			Request(DWORD _size) : BaseRequest(ALLOCATE_MEMORY), size(_size) {}
 			DWORD size;
		};
		struct Reply : BaseReply {
			Reply(void* _base) : base(_base){}
			void* base;
		};
	};

	struct FreeMemory {

		struct Request : BaseRequest {
			Request(void* _base) :BaseRequest(FREE_MEMORY), base(_base){}
			void* base;
		};

	};
	
	struct GetMapID {
		struct Request : BaseRequest {
			Request() : BaseRequest(GET_MAP_ID) {}
		};
		typedef DWORD Reply;
	};

	struct GetInstanceType {
		struct Request : BaseRequest {
			Request() : BaseRequest(GET_INSTANCE_TYPE) {}
		};
		typedef DWORD Reply;
	};

	struct GetInstanceTime {
		struct Request : BaseRequest {
			Request() : BaseRequest(GET_INSTANCE_TIME) {}
		};
		typedef DWORD Reply;
	};

	struct ChangeTarget {
		struct Request : BaseRequest {
			Request(DWORD _target_id) : BaseRequest(CHANGE_TARGET), target_id(_target_id) {}
			DWORD target_id;
		};
		// no reply
	};

	struct GoNpc {
		struct Request : BaseRequest {
			Request(DWORD _npc_id, BOOL _call_target) : BaseRequest(GO_NPC), npc_id(_npc_id), call_target(_call_target) {}
			DWORD npc_id;
			BOOL call_target;
		};
	};

	struct SendDialog {
		struct Request : BaseRequest {
			Request(DWORD _dialogid) : BaseRequest(SEND_DIALOG), dialogid(_dialogid) {}
			DWORD dialogid;
		};
	};

	struct GetAgentPos {
		struct Request : BaseRequest {
			Request(GW::AgentID _agentid) : BaseRequest(GET_AGENT_POS), agentid(_agentid) {}
			GW::AgentID agentid;
		};
		struct Reply : BaseReply {
			Reply(GW::GamePos _pos) : BaseReply(), pos(_pos) {}
			GW::GamePos pos;
		};
	};

	struct GetPlayerId {
		struct Request : BaseRequest {
			Request() : BaseRequest(GET_PLAYER_ID) {}
		};
		typedef GW::AgentID Reply;
	};

	struct GetTargetId {
		struct Request : BaseRequest {
			Request() : BaseRequest(GET_TARGET_ID) {}
		};
		typedef GW::AgentID Reply;
	};

	struct MapTravel {
		struct Request : BaseRequest {
			Request(DWORD _mapid, int _district, int _region, int _language) : BaseRequest(MAP_TRAVEL),
				mapid(_mapid), district(_district), region(_region), language(_language) {}
			DWORD mapid;
			int district;
			int region; 
			int language;
		};
	};

	struct MapTravelRandom {
		struct Request : BaseRequest {
			Request(DWORD _mapid) : BaseRequest(MAP_TRAVEL_RANDOM), mapid(_mapid) {}
			DWORD mapid;
		};
	};

	struct GetMapLoaded {
		struct Request : BaseRequest {
			Request() : BaseRequest(GET_MAP_LOADED) {}
		};
		typedef DWORD Reply;
	};

	struct ChangeSecondary {
		struct Request : BaseRequest {
			DWORD heroindex;
			DWORD profession;
		};
	};
	struct LoadSkillbar {
		struct Request : BaseRequest {
			DWORD heroindex;
			DWORD skillid[8];
		};
	};
	struct SetAttributes {
		struct Request : BaseRequest {
			DWORD heroindex;
			DWORD attributecount;
			DWORD attributeids[16];
			DWORD attributevalues[16];
		};
	};

	struct SetRenderHook {
		struct Request : BaseRequest {
			DWORD flag;
		};
	};

	struct TravelGuildHall {
		struct Request : BaseRequest {

		};
	};
	struct LeaveGuildHall {
		struct Request : BaseRequest {

		};
	};
	struct GetMapInfo {
		struct Request : BaseRequest {

		};
		struct Reply : BaseReply {
			DWORD district;
			DWORD region;
			DWORD language;
		};
	};

	struct SendPacket {
		struct Request : BaseRequest {
			DWORD size;
		};
	};

	struct GetCharacterName {
		struct Request : BaseRequest {
		};
		struct Reply : BaseReply {
			wchar_t name[20];
		};
	};

	struct GetAgentPtr {
		struct Request : BaseRequest {
			int id;
		};
		struct Reply : BaseReply {
			GW::Agent* ptr;
		};
	};

	struct GetFilteredAgentArray {
		struct Request : BaseRequest {
			DWORD type;
			DWORD allegiance;
			DWORD effects;
			float X;
			float Y;
			float minDist;
			float maxDist;
			DWORD playernumbercount;
			DWORD playernumbers[16];
		};
		struct Reply : BaseReply {
			DWORD agentcount;
			//... agents
		};
	};

	struct GetItemBySlot {
		struct Request : BaseRequest {
			BYTE bagindex;
			BYTE slot;
		};
		struct Reply : BaseReply {
			GW::Item* item;
		};
	};

	struct GetItemByAgentID {
		struct Request : BaseRequest {
			GW::AgentID id;
		};
		struct Reply : BaseReply {
			GW::Item* item;
		};
	};

	struct GetItemsByModelID {
		struct Request : BaseRequest {
			BYTE bagstart;
			BYTE bagend;
			DWORD modelid;
		};
		// Custom reply
	};
	struct GetItemName {
		struct Request : BaseRequest {
			GW::Item* item;
		};
		// custom reply
	};

	/*BUY_MERCHANT_ITEM,
		SELL_MERCHANT_ITEM,
		COLLECT_ITEM,
		CRAFT_ITEM,
		BUY_TRADER_ITEM,
		SELL_TRADER_ITEM,

		FIX_AGENT_POSITION,
		GET_EFFECT_HANDLE,
		GET_SKILLBAR_HANDLE,
		GET_SKILLBAR_INFO,*/

	struct BuyMerchantItem {
		struct Request : BaseRequest {
			DWORD modelid;
			BYTE count;
		};
	};

	struct SellMerchantItem {
		struct Request : BaseRequest {
			GW::Item* item;
			INT8 count;
		};
	};

	struct CollectItem {
		struct Request : BaseRequest {
			DWORD modelidgive;
			BYTE countgive;
			DWORD modelidrecv;
		};
	};

	struct CraftItem {
		struct Request : BaseRequest {
			DWORD modelid;
			BYTE count;
			DWORD coingive;
			DWORD matsgive[4];
			BYTE matgivecount[4];
		};
	};

	struct BuyTraderItem {
		struct Request : BaseRequest {
			GW::ItemID itemid;
		};
	};

	struct SellTraderItem {
		struct Request : BaseRequest {
			GW::ItemID itemid;
		};
	};

	struct RequestBuyQuote {
		struct Request : BaseRequest {
			DWORD modelid;
		};
		struct Reply : BaseReply {
			GW::ItemID item;
			DWORD price;
		};
	};

	struct RequestSellQuote {
		struct Request : BaseRequest {
			GW::ItemID item;
		};
		struct Reply : BaseReply {
			DWORD price;
		};
	};

	struct FixAgentPosition {
		struct Request : BaseRequest {
			GW::AgentID agentid;
		};
	};

	struct GetEffectHandle {
		struct Request : BaseRequest {
			DWORD index;
			DWORD skillid;
		};
		struct Reply : BaseReply {
			GW::Effect* effectid;
		};
	};

	struct GetSkillbarHandle {
		struct Request : BaseRequest {
			DWORD index;
		};
		struct Reply : BaseReply {
			GW::Skillbar* skillbar;
		};
	};

	struct GetSkillbarInfo {
		struct Request : BaseRequest {
			GW::Skillbar* skillbar;
		};
		struct Reply : BaseReply {
#pragma pack(1)
			struct Info{
				BYTE isRecharged;
				BYTE energyMet;
				BYTE adrenalineMet;
				BYTE effectOff;
			}skillinfo[8];
#pragma pack()
		};

	};

	enum e_InteractType : DWORD {
		kAttackEnemy,
		kFollowPlayer,
		kGoNPC,
		kPickupItem,
		kUnknown,
		kGoSignpost
	};

	struct InteractAgent {
		struct Request : BaseRequest {
			e_InteractType type;
			GW::AgentID agent;
		};
	};


	struct PerformAction {
		struct Request : BaseRequest {
			DWORD type;
			DWORD flag;
		};
	};
}
