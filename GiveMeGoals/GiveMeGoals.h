#pragma once
#pragma comment(lib, "PluginSDK.lib")
#include "bakkesmod/plugin/bakkesmodplugin.h"

class GiveMeGoals : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	void onLoad() override;
	void onUnload() override;

    void AddGoalToMyScore();
};
