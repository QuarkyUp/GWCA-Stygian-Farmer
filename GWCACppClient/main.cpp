
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <comdef.h>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <utility>


#include "GWCAClient.h"

// Namespaces
using namespace std;

// Defines
#define GToBMapId GW::Constants::MapID::Great_Temple_of_Balthazar_outpost
#define GToBMapIdDWORD (DWORD)GW::Constants::MapID::Great_Temple_of_Balthazar_outpost

// Functions Prototypes
void sleepUntilLoadComplete(GWCAClient& client, const GW::Constants::MapID& mapId);
void travelToCity(GWCAClient& client, const GW::Constants::MapID& mapIdDest, string text);
// auto timestamp();

// Template Functions Prototypes
template<typename First, typename ...Rest>
void log(First && first, Rest && ...rest);
void log() {};




// Main Action
int main(int argc, char **argv) {
	cout << "DoA Gem Farmer v0.1" << endl;
	cout << "Taverne / QuarkyUp" << endl;

	GWCAClient::Initialize();
	GWCAClient client = GWCAClient::Api();
	HWND gw_handle = FindWindow(L"ArenaNet_Dx_Window_Class", NULL);
	DWORD gw_pid;
	GetWindowThreadProcessId(gw_handle, &gw_pid);
	cout << "Connecting to Gw.exe process : " << gw_pid << endl;
	if (!client.ConnectByPID(gw_pid)) return EXIT_FAILURE;

	travelToCity(client, GToBMapId, "Travelling to Balthazar");
	while (1) {
		PseudoAgent agent = client.GetAgentByID(-2);

		log("x= ", agent.X);
		log("y= ", agent.Y);
		cout << "x = " << agent.X << endl;
		cout << "y = " << agent.Y << endl;
		cout << "primary = " << static_cast<int>(agent.Primary) << endl;
		cout << "secondary = " << static_cast<int>(agent.Secondary) << endl;

		if (GetAsyncKeyState(VK_END) & 1) {
			GWCAClient::Destroy();
			break;
		}
		Sleep(500);
	}
	
	return EXIT_SUCCESS;
}

void sleepUntilLoadComplete(GWCAClient& client, const GW::Constants::MapID& mapId) {
	bool differentMap = client.GetMapID() != (DWORD)mapId;
	bool mapNotLoaded = !client.GetMapLoaded();
	do {
		Sleep(100);
	} while (differentMap || mapNotLoaded);
}

void travelToCity(GWCAClient& client, const GW::Constants::MapID& mapIdDest, string text) {
	if (client.GetMapID() != (DWORD)mapIdDest) {
		log(text);
		client.MapTravelRandom((DWORD)mapIdDest);
		sleepUntilLoadComplete(client, mapIdDest);
	}
}

template<typename First, typename ...Rest>
void log(First && first, Rest && ...rest)
{
	std::cout << std::forward<First>(first);
	log(std::forward<Rest>(rest)...);
}

/*
auto timestamp() {
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t  now_c = chrono::system_clock::to_time_t(now);

	return std::put_time(std::localtime(&now_c), "%m-%d %t %T");
}
*/
