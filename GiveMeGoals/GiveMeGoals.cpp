#include "GiveMeGoals.h"
#include "bakkesmod\wrappers\includes.h"

BAKKESMOD_PLUGIN(GiveMeGoals, "Give yourself a goal", "1.0", PLUGINTYPE_FREEPLAY)

void GiveMeGoals::onLoad()
{
	cvarManager->registerNotifier("GiveMeGoal", [this](std::vector<std::string> params){AddGoalToMyScore();}, "Give yourself a goal", PERMISSION_ALL);
	cvarManager->registerNotifier("GiveMeGoal_TakeFromOpponent", [this](std::vector<std::string> params){RemoveOpponentScore();}, "Take a goal from a random opponent", PERMISSION_ALL);
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
    PlayerControllerWrapper MyPCW = gameWrapper->GetPlayerController();
    if(MyPCW.IsNull()) { return; }
    PriWrapper MyPRI = MyPCW.GetPRI();
    if(MyPRI.IsNull()) { return; }
    int MyTeamNum = MyPRI.GetTeamNum2();

    //Add point to team, and add new stats to your scoreboard
    ArrayWrapper<TeamWrapper> Teams = Server.GetTeams();
    for(int i = 0; i < Teams.Count(); ++i)
    {
        TeamWrapper Team = Teams.Get(i);
        if(Team.IsNull()) { continue; }

        if(Team.GetTeamNum2() == MyTeamNum)
        {
            MyPRI.SetMatchScore(MyPRI.GetMatchScore() + 100);
            MyPRI.SetMatchGoals(MyPRI.GetMatchGoals() + 1);
            MyPRI.SetMatchShots(MyPRI.GetMatchShots() + 1);
            Team.ScorePoint(1);
        }
    }
}

void GiveMeGoals::RemoveOpponentScore()
{
    if(gameWrapper->IsInOnlineGame()) { return; }

    //Get information about the local player and the game state
    ServerWrapper Server = gameWrapper->GetCurrentGameState();
    if(Server.IsNull()) { return; }
    PlayerControllerWrapper MyPCW = gameWrapper->GetPlayerController();
    if(MyPCW.IsNull()) { return; }
    PriWrapper MyPRI = MyPCW.GetPRI();
    if(MyPRI.IsNull()) { return; }
    int MyTeamNum = MyPRI.GetTeamNum2();
    if(MyTeamNum != 0 && MyTeamNum != 1) { return; }

    //Remove point from other team
    ArrayWrapper<TeamWrapper> Teams = Server.GetTeams();
    for(int i = 0; i < Teams.Count(); ++i)
    {
        TeamWrapper Team = Teams.Get(i);
        if(!Team.IsNull() && Team.GetTeamNum2() != MyTeamNum && Team.GetScore() > 0)
        {
            Team.ScorePoint(-1);
        }
    }

    //Choose the first opponent with a goal and assist and remove score from them
    bool bHaveRemovedGoal = false;
    bool bHaveRemovedAssist = false;
    ArrayWrapper<PriWrapper> PRIs = Server.GetPRIs();
    for(int i = 0; i < PRIs.Count(); ++i)
    {
        PriWrapper PRI = PRIs.Get(i);
        if(PRI.IsNull()) { continue; }
        int TeamNum = PRI.GetTeamNum2();
        if(TeamNum != 0 && TeamNum != 1) { continue; }

        if(TeamNum != MyTeamNum)
        {
            if(!bHaveRemovedGoal && PRI.GetMatchGoals() > 0)
            {
                bHaveRemovedGoal = true;
                PRI.SetMatchScore(PRI.GetMatchScore() - 100);
                PRI.SetMatchGoals(PRI.GetMatchGoals() - 1);
                if(PRI.GetMatchShots() > 0)
                {
                    PRI.SetMatchShots(PRI.GetMatchShots() - 1);
                }
                continue;
            }

            if(!bHaveRemovedAssist && PRI.GetMatchAssists() > 0)
            {
                bHaveRemovedAssist = true;
                PRI.SetMatchScore(PRI.GetMatchScore() - 50);
                PRI.SetMatchAssists(PRI.GetMatchAssists() - 1);
                continue;
            }
        }
    }

}
