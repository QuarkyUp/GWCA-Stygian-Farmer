#pragma once

#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <assert.h>

#include "..\GWCAServer\CommandEnums.h"
#include "..\GWCAServer\CommandStructs.h"


#include <string>

using namespace GWCACom;

struct AgentPosition {
	static AgentPosition Invalid() { return AgentPosition(-1, 0, 0); }

	AgentPosition(long _id, float _x, float _y) :
		id(_id), x(_x), y(_y) {}

	bool valid() const { return id > 0; }

	long id;
	float x;
	float y;
};

class GWCAClient {
public:
	static void Initialize() { instance_ = new GWCAClient(); }
	static void Destroy() { delete instance_; }
	static GWCAClient& Api() { return *instance_; }
private:
	static GWCAClient* instance_;

private:
	GWCAClient() :
		connected_(false),
		pipe_handle_(INVALID_HANDLE_VALUE) {}

public:
	// Connect to a pipe. Must be currently disconnected.
	bool Connect(TCHAR* pipe_name);
	bool ConnectByPID(DWORD pid);

	// Checks if connected
	bool IsConnected() { return connected_; }

	// Sends heartbeat packet. Returns true if successful.
	bool Heartbeat();

	// Disconnects from pipe. Must be currently connected.
	void Disconnect();

	// Moves your character to given coordinates
	void Move(float x, float y, DWORD zplane = 0) {
		Send(Move::Request(x, y, zplane));
	}

	// Use a skill
	void UseSkill(DWORD slot, DWORD target, BOOL calltarget = false) {
		Send(UseSkill::Request(slot, target, calltarget));
	}

	// Returns the recharge time remaining of an equipped
	// skill in milliseconds (returns 0 if recharged)
	int GetSkillRecharge(DWORD slot) {
		Send(GetSkillRecharge::Request(slot));
		return Receive<GetSkillRecharge::Reply>();
	}

	// Checks if the skill is recharged
	bool IsSkillRecharged(DWORD slot) {
		return GetSkillRecharge(slot) == 0;
	}

	long GetEffectTimeRemaining(Constants::SkillID effect) {
		Send(GetEffectTimeRemaining::Request(effect));
		return Receive<GetEffectTimeRemaining::Reply>();
	}

	// Sends a message to chat
	void SendChat(wchar_t channel, std::wstring message);

	// Requests base pointers. Check return struct for info.
	BasePointers::Reply RequestBasePointers() {
		Send(BasePointers::Request());
		return Receive<BasePointers::Reply>();
	}

	// Gets a PseudoAgent struct by agent ID.
	// ID can be -1 for player or -2 for current target.
	PseudoAgent GetAgentByID(long id) {
		Send(GetAgentByID::Request(id));
		return Receive<GetAgentByID::Reply>();
	}

	DWORD GetMapLoaded() {
		Send(GetMapLoaded::Request());
		return Receive<GetMapLoaded::Reply>();
	}

	// Get player PseudoAgent
	PseudoAgent GetPlayer() {
		Send(GetPlayer::Request());
		return Receive<GetPlayer::Reply>();
	}

	std::vector<AgentPosition> GetAgentsPos();

	DWORD GetMapID() {
		Send(GetMapID::Request());
		return Receive<GetMapID::Reply>();
	}

	Constants::InstanceType GetInstancType() {
		Send(GetInstanceType::Request());
		return static_cast<Constants::InstanceType>(Receive<GetInstanceType::Reply>());
	}

	DWORD GetInstanceTime() {
		Send(GetInstanceTime::Request());
		return Receive<GetInstanceType::Reply>();
	}

	void ChangeTarget(DWORD target_id) {
		Send(ChangeTarget::Request(target_id));
	}

	void MapTravelRandom(DWORD mapid) {
		Send(MapTravelRandom::Request(mapid));
	}

	void MapTravel(DWORD mapid, int district, int region, int language) {
		Send(MapTravel::Request(mapid, district, region, language));
	}


private:
	// template functions should stay in header file as long as they are referenced from the header file (above)

	// send a request, returns true if successful
	template<class Request_t>
	BOOL Send(Request_t req) {
		assert(connected_);
		BOOL success = WriteFile(pipe_handle_, &req, sizeof(Request_t),
			&bytes_written_, NULL);
		if (!success) printf("Error sending request\n");
		return success;
	}


	// receives a reply and stores it in the pointer passed to the func
	// returns true if successful
	template<class Reply_t>
	BOOL Receive(Reply_t * reply) {
		assert(connected_);
		BOOL success = ReadFile(pipe_handle_, reply, sizeof(Reply_t),
			&bytes_read_, NULL);
		if (!success) printf("Error receiving reply\n");
		return success;
	}

	// receives a reply and returns it by value
	template<class Reply_t>
	Reply_t Receive() {
		assert(connected_);
		Reply_t reply;
		BOOL success = ReadFile(pipe_handle_, &reply, sizeof(Reply_t),
			&bytes_read_, NULL);
		if (!success) printf("Error receiving reply\n");
		return reply;
	}
	
	bool connected_;
	HANDLE pipe_handle_;
	DWORD bytes_written_;
	DWORD bytes_read_;
};
