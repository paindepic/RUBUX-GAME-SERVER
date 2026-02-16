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
            if (!BusLaunched && GameState->WarmupCountdownEndTime > 0 && GameState->GamePhase == EAthenaGamePhase::Warmup)
            {
                float TimeSeconds = UGameplayStatics::GetTimeSeconds(World);
                if (TimeSeconds >= GameState->WarmupCountdownEndTime)
                {
                    BusLaunched = true;
                    GameState->GamePhase = EAthenaGamePhase::Aircraft;
                    GameState->GamePhaseStep = EAthenaGamePhaseStep::BusLocked;

                    for (auto PlayerState : GameState->PlayerArray)
                    {
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

    return TickFlushOriginal(NetDriver);
}
