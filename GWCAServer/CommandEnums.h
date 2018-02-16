#pragma once

namespace GWCACom {
	enum CMD {
		HEARTBEAT, 
		DISCONNECT_PIPE_CONNECTION, 
		UNLOAD_SERVER,

		REQUEST_BASE_POINTERS,

		MOVE, 
		USE_SKILL, 
		GET_SKILL_RECHARGE,
		GET_CAST_QUEUE_SIZE,
		GET_SKILL_ADRENALINE,

		GET_EFFECT_TIME_REMAINING,

		SEND_CHAT,

		GET_PLAYER_ID,
		GET_TARGET_ID,

		GET_PLAYER,
		GET_AGENT_BY_ID,
		GET_AGENTS_POS,
		GET_AGENT_POS,

		ALLOCATE_MEMORY,
		FREE_MEMORY,
		
		GET_MAP_ID,
		GET_MAP_INFO,
		GET_INSTANCE_TYPE,
		GET_INSTANCE_TIME,

		CHANGE_TARGET,

		GO_NPC,
		SEND_DIALOG,

		MAP_TRAVEL,
		MAP_TRAVEL_RANDOM,

		TRAVEL_GUILD_HALL,
		LEAVE_GUILD_HALL,

		GET_MAP_LOADED,

		CHANGE_SECONDARY,
		SET_ATTRIBUTES,
		LOAD_SKILLBAR,

		SET_RENDER_HOOK,
		SEND_PACKET,

		GET_CHARACTER_NAME,

		GET_AGENT_HANDLE,
		GET_FILTERED_AGENT_ARRAY,

		GET_ITEM_BY_SLOT,
		GET_ITEM_BY_AGENTID,
		GET_ITEMS_BY_MODELID,
		GET_ITEM_NAME,
	
		BUY_MERCHANT_ITEM,
		SELL_MERCHANT_ITEM,
		COLLECT_ITEM, 
		CRAFT_ITEM, 
		BUY_TRADER_ITEM, 
		SELL_TRADER_ITEM, 

		FIX_AGENT_POSITION, 
		GET_EFFECT_HANDLE, 
		GET_SKILLBAR_HANDLE,
		GET_SKILLBAR_INFO,

		INTERACT_AGENT,
		USE_TARGET_FUNC,
		
		COMMANDS_END
	};

	const size_t kAmountofCommands = COMMANDS_END;

	enum Target {
		Self = -2,
		Current = -1
	};
}
