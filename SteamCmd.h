#pragma once

#include <string>
#include <memory>

#include "Stats.h"
#include "GameClient.h"

using namespace std;

class SteamCmd
{
public:
	void initStats(int maxSamples, int sampleRate);
	void install();
	void chdir();
	void updateGame(int appid, string branch, bool validate = false);
	void startGame(int appid, string cmdline, int port);
	void cleanUp(int appid);
	int getPid();
	void killProcess(string reason);
	void checkServer();
	int createProcess(string cmd);
private:
	int port;

	unique_ptr<Stats> stats;
	unique_ptr<GameClient> game;

	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
};

