#include "GiveMeGoals.h"
#include "bakkesmod\wrappers\includes.h"

BAKKESMOD_PLUGIN(GiveMeGoals, "Give yourself a goal", "1.0", PLUGINTYPE_FREEPLAY)

void GiveMeGoals::onLoad()
{
	cvarManager->registerNotifier("GiveMeGoal", [this](std::vector<std::string> params){AddGoalToMyScore();}, "DESCRIPTION", PERMISSION_ALL);
}
void GiveMeGoals::onUnload() {}

void GiveMeGoals::AddGoalToMyScore()
{
    //Removing this check won't actually update your score in an online match.
    //At most it would only update it on your screen and then you would be confused about why you lost even though you had more points.
    //Don't bother trying to cheat by disabling this. It's only here to help avoid that confusion.
    if(gameWrapper->IsInOnlineGame()) { return; }

    //Get information about the local player and the game state
    ServerWrapper Server = gameWrapper->GetCurrentGameState();
    if(Server.IsNull()) { return; }
    PlayerControllerWrapper PCW = gameWrapper->GetPlayerController();
    if(PCW.IsNull()) { return; }
    PriWrapper PRI = PCW.GetPRI();
    if(PRI.IsNull()) { return; }
    int TeamNum = PRI.GetTeamNum2();

    //Add point to team, and add new stats to your scoreboard
    ArrayWrapper<TeamWrapper> Teams = Server.GetTeams();
    for(int i = 0; i < Teams.Count(); ++i)
    {
        TeamWrapper Team = Teams.Get(i);
        if(Team.IsNull()) { continue; }

        if(Team.GetTeamNum2() == TeamNum)
        {
            PRI.SetMatchScore(PRI.GetMatchScore() + 100);
            PRI.SetMatchGoals(PRI.GetMatchGoals() + 1);
            PRI.SetMatchShots(PRI.GetMatchShots() + 1);
            Team.ScorePoint(1);
        }
    }
}
