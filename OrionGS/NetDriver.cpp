#include "NetDriver.h"
#include "PlayerController.h"
#include "Utils.h"

void NetDriver::TickFlush(UNetDriver* NetDriver)
{
    if (NetDriver && NetDriver->ClientConnections.Num() > 0 && NetDriver->ReplicationDriver)
        ServerReplicateActors(NetDriver->ReplicationDriver);

    auto World = UWorld::GetWorld();
    if (World)
    {
        auto GameState = Utils::Cast<AFortGameStateAthena>(World->GameState);
        auto GameMode = Utils::Cast<AFortGameModeAthena>(World->AuthorityGameMode);
        if (GameState && GameMode)
        {
            static bool BusLaunched = false;
            
            // Better countdown monitoring - ensure countdown is set and monitor it
            if (!BusLaunched && GameState->GamePhase == EAthenaGamePhase::Warmup)
            {
                // Ensure countdown is set
                if (GameState->WarmupCountdownEndTime <= 0.f && GameState->TotalPlayers > 0)
                {
                    float TimeSeconds = UGameplayStatics::GetTimeSeconds(World);
                    GameState->WarmupCountdownEndTime = TimeSeconds + 90.f;
                    printf("Set WarmupCountdownEndTime to %f (current time: %f)\n", GameState->WarmupCountdownEndTime, TimeSeconds);
                }
                
                // Check if countdown has expired
                if (GameState->WarmupCountdownEndTime > 0)
                {
                    float TimeSeconds = UGameplayStatics::GetTimeSeconds(World);
                    float TimeRemaining = GameState->WarmupCountdownEndTime - TimeSeconds;
                    
                    // Launch bus when countdown reaches 0 or below
                    if (TimeRemaining <= 0.f)
                    {
                        printf("Launching bus! Time remaining: %f\n", TimeRemaining);
                        BusLaunched = true;
                        GameState->GamePhase = EAthenaGamePhase::Aircraft;
                        GameState->GamePhaseStep = EAthenaGamePhaseStep::BusLocked;

                        for (auto PlayerState : GameState->PlayerArray)
                        {
                            if (!PlayerState)
                                continue;
                            auto AthenaPlayerState = Utils::Cast<AFortPlayerStateAthena>(PlayerState);
                            if (!AthenaPlayerState || AthenaPlayerState->bInAircraft)
                                continue;
                            PlayerController::ServerSetInAircraft(AthenaPlayerState, true);
                        }

                        for (auto Bot : GameMode->AliveBots)
                        {
                            if (!Bot || !Bot->PlayerState)
                                continue;
                            auto BotState = Utils::Cast<AFortPlayerStateAthena>(Bot->PlayerState);
                            if (!BotState || BotState->bInAircraft)
                                continue;
                            PlayerController::ServerSetInAircraft(BotState, true);
                        }
                    }
                }
            }
        }
    }

    return TickFlushOriginal(NetDriver);
}
