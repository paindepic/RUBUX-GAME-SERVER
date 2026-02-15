#pragma once
#include "framework.h"
#include "BotNames.h"
#include "POI_Locs.h"

#include "Quests.h"
#include "PlayerQuests.h"
#include "Misc.h"
#include "BotBuilding.h"
#include "BotDriving.h"
#include "BotVaultSystem.h"

static std::vector<UAthenaCharacterItemDefinition*> CIDs{};
static std::vector<UAthenaPickaxeItemDefinition*> Pickaxes{};
static std::vector<UAthenaBackpackItemDefinition*> Backpacks{};
static std::vector<UAthenaGliderItemDefinition*> Gliders{};
static std::vector<UAthenaSkyDiveContrailItemDefinition*> Contrails{};
inline std::vector<UAthenaDanceItemDefinition*> Dances{};

enum class EBotState : uint8 {
    Warmup,
    PreBus, // Dont handle this, just there to stop bots from killing eachother before bus enters dropzone
    Bus,
    Skydiving,
    Gliding,
    Landed,
    Fleeing,
    Looting,
    LookingForPlayers,
    MovingToSafeZone,
    Stuck
};

enum class EBotWarmupChoice {
    Emote,
    MoveToPlayerEmote,
    MAX
};

enum class EBotStrafeType {
    StrafeLeft,
    StrafeRight
};

enum class ELootableType {
    None = -1,
    Chest = 0,
    Pickup = 1
};

enum class ELobbyBotState {
    Idle,
    Moving,
    Emoting,
    PickingUpWeapon,
    Shooting
};

std::vector<class PlayerBot*> PlayerBotArray{};
struct PlayerBot
{
public:
    // Additional State Enums : Seprated so its cleaner
    EBotWarmupChoice BotWarmupChoice = EBotWarmupChoice::MAX;

public:
    // So we can track the current tick that the bot is doing
    uint64_t tick_counter = 0;

    // The Pawn of the bot
    AFortPlayerPawnAthena* Pawn = nullptr;

    // The playercontroller of the bot
    ABP_PhoebePlayerController_C* PC = nullptr;

    // The playerstate of the bot
    AFortPlayerStateAthena* PlayerState = nullptr;

    // The current botstate, has different behaviours for different states
    EBotState BotState = EBotState::Warmup;

    // The cached botstate, usually if a botstate needs to revert back to the previous one
    EBotState CachedBotState = EBotState::Warmup;

    // The building that the bot collided with (doesent even works smd)
    ABuildingActor* StuckBuildingActor = nullptr;

    // Reservation of lootables, stops pileup and tracks current lootable
    AActor* TargetLootable = nullptr;

    // Just incase we have to do specific stuff based on the type of lootable it is
    ELootableType TargetLootableType = ELootableType::None;

    // The nearest player, bot, factionbot
    AActor* NearestPlayerActor = nullptr;

    // The displayname of the bot
    FString DisplayName = L"Bot";

    // Is the bot stressed, will be determined every tick.
    bool bIsStressed = false;

    // Is the bot dead, basically marking the bot to be removed from the playerbotarray later
    bool bIsDead = false;

    // Has the bot thanked the bus driver
    bool bHasThankedBusDriver = false;

    // Has the bot jumped from the bus, if not then set the botstate to bus.
    bool bHasJumpedFromBus = false;

    // The dropzone that the bot will attempt to land at
    FVector TargetDropZone = FVector();

    // The closest distance achieved to the targetdropzone, will be used mostly for determining bus drop
    float ClosestDistToDropZone = FLT_MAX;

    // Is the bot currently strafing (combat technique & Unstuck method)
    bool bIsCurrentlyStrafing = false;

    // The strafe type used by the bot, determines what direction
    EBotStrafeType StrafeType = EBotStrafeType::StrafeLeft;

    // When should the current strafe end?
    float StrafeEndTime = 0.0f;

    // General purpose timer
    float TimeToNextAction = 0.f;

    // The start time of this current lootable
    float LootTargetStartTime = 0.f;

    // The distance between the bot and the lootable
    float LastLootTargetDistance = 0.f;

    // OGSM Chapter 2 Season 2 - New AI Fields
    // Building system
    EBotBuildState BuildState = EBotBuildState::Idle;
    bool bIsBuilding = false;
    float LastBuildTime = 0.f;
    int32 BuildMaterialCount = 0;

    // Vehicle system
    AActor* CurrentVehicle = nullptr;
    EBotVehicleState VehicleState = EBotVehicleState::NoVehicle;
    FVector VehicleTargetLocation = FVector();

    // Vault system
    EVaultState VaultState = EVaultState::None;
    EBossType TargetBoss = EBossType::None;
    bool bHasKeycard = false;

    // Strategic dropping
    bool bTargetingRoofChest = false;
    FVector RoofChestLocation = FVector();
    FBossPOI* TargetBossPOI = nullptr;

    // Lobby bot behavior
    ELobbyBotState LobbyState = ELobbyBotState::Idle;
    float LobbyActionTimer = 0.f;
    AFortPickup* TargetWeaponPickup = nullptr;

    // Reactive defense
    float LastDamageTime = 0.f;
    FVector LastDamageDirection;
    bool bIsUnderFire = false;
    int32 ConsecutiveShotsTaken = 0;

    // Farming
    bool bNeedsFarming = false;
    float FarmingTargetAmount = 500.f;
    AActor* FarmingTarget = nullptr;
    float LastFarmTime = 0.f;
    int32 WoodCount = 0;
    int32 StoneCount = 0;
    int32 MetalCount = 0;

    // Human-like behavior
    float ReactionTime = 0.2f;
    float AimAccuracy = 0.8f;
    bool bMakesMistakes = true;
    float MistakeChance = 0.1f;
    float AggressionLevel = 0.5f;
    float LastAimTime = 0.f;


    // OGSM - Initialize bot personality with variations
    inline void InitializeBotPersonality() {
        ReactionTime = 0.15f + (rand() % 20) / 100.f;
        AimAccuracy = 0.6f + (rand() % 40) / 100.f;
        AggressionLevel = 0.3f + (rand() % 50) / 100.f;
        MistakeChance = 0.05f + (rand() % 15) / 100.f;
    }

    // OGSM - On take damage for reactive defense
    inline void OnTakeDamage(float Damage, AActor* DamageCauser, FVector HitDirection) {
        LastDamageTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
        LastDamageDirection = HitDirection;
        bIsUnderFire = true;
        ConsecutiveShotsTaken++;

        if (HasEnoughResources(10) && Pawn) {
            BuildDefensiveWall(HitDirection);
        }

        if (ConsecutiveShotsTaken >= 3 && HasEnoughResources(50)) {
            BuildBox();
        }

        if (BotState == EBotState::LookingForPlayers) {
            CachedBotState = BotState;
            BotState = EBotState::Stuck;
        }
    }

    // OGSM - Build defensive wall against threat
    inline void BuildDefensiveWall(FVector ThreatDirection) {
        if (!Pawn || !HasEnoughResources(10)) return;

        FVector BotLoc = Pawn->K2_GetActorLocation();
        ThreatDirection.Z = 0;
        ThreatDirection.Normalize();

        FRotator WallRot = ThreatDirection.Rotation();
        WallRot.Yaw += 90.f;

        FVector WallLoc = BotLoc + ThreatDirection * 100.f;
        WallLoc.Z = BotLoc.Z;

        if (Globals::bBotBuildingEnabled) {
            BotBuilding::BuildWallAt(this, WallLoc, WallRot);
        }
    }

    // OGSM - Check if has enough materials
    inline bool HasEnoughResources(int32 Amount) {
        int32 Total = WoodCount + StoneCount + MetalCount;
        return Total >= Amount;
    }

    // OGSM - Update material counts from inventory
    inline void UpdateMaterialCounts() {
        if (!PC || !PC->Inventory) return;

        WoodCount = 0;
        StoneCount = 0;
        MetalCount = 0;

        for (int32 i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
            if (!Entry.ItemDefinition) continue;

            std::string Name = Entry.ItemDefinition->Name.ToString();
            if (Name.contains("Wood")) WoodCount = Entry.Count;
            else if (Name.contains("Stone")) StoneCount = Entry.Count;
            else if (Name.contains("Metal")) MetalCount = Entry.Count;
        }

        bNeedsFarming = NeedsFarming();
    }

    // OGSM - Check if bot needs farming
    inline bool NeedsFarming() {
        int32 Total = WoodCount + StoneCount + MetalCount;
        return Total < 200;
    }

    // OGSM - Find nearest farmable object
    inline AActor* FindFarmableObject() {
        if (!Pawn) return nullptr;

        static auto BuildingClass = ABuildingSMActor::StaticClass();
        TArray<AActor*> Array;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), BuildingClass, &Array);

        AActor* BestTarget = nullptr;
        float MinDist = FLT_MAX;

        for (size_t i = 0; i < Array.Num(); i++) {
            ABuildingSMActor* Building = (ABuildingSMActor*)Array[i];
            if (!Building || Building->bDestroyed) continue;

            float Health = Building->GetHealth();
            if (Health > 0 && Health < 2000) {
                float Dist = Pawn->GetDistanceTo(Building);
                if (Dist < MinDist && Dist < 1000.f) {
                    MinDist = Dist;
                    BestTarget = Building;
                }
            }
        }

        Array.Free();
        return BestTarget;
    }

    // OGSM - Farm tick
    inline void FarmTick() {
        if (!bNeedsFarming || !Pawn) return;

        float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
        if (CurrentTime - LastFarmTime < 1.f) return;

        if (!FarmingTarget || (FarmingTarget->IsA(ABuildingSMActor::StaticClass()) && ((ABuildingSMActor*)FarmingTarget)->bDestroyed)) {
            FarmingTarget = FindFarmableObject();
        }

        if (FarmingTarget) {
            EquipPickaxe();
            LookAt(FarmingTarget);
            Pawn->PawnStartFire(0);
            LastFarmTime = CurrentTime;
        } else {
            bNeedsFarming = false;
        }
    }

    // OGSM - Find nearest weapon pickup
    inline AFortPickup* FindNearestWeaponPickup() {
        if (!Pawn) return nullptr;

        static auto PickupClass = AFortPickupAthena::StaticClass();
        TArray<AActor*> Array;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), PickupClass, &Array);

        AFortPickup* Nearest = nullptr;
        float MinDist = FLT_MAX;

        for (size_t i = 0; i < Array.Num(); i++) {
            AFortPickup* Pickup = (AFortPickup*)Array[i];
            if (Pickup->bHidden) continue;

            auto Def = Pickup->PrimaryPickupItemEntry.ItemDefinition;
            if (!Def) continue;

            if (Def->IsA(UFortWeaponItemDefinition::StaticClass()) &&
                !Def->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) {

                float Dist = Pawn->GetDistanceTo(Pickup);
                if (Dist < MinDist && Dist < 5000.f) {
                    MinDist = Dist;
                    Nearest = Pickup;
                }
            }
        }

        Array.Free();
        return Nearest;
    }

    // OGSM - Find nearest real player
    inline AActor* FindNearestRealPlayer() {
        if (!Pawn) return nullptr;

        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        AActor* Nearest = nullptr;
        float MinDist = FLT_MAX;

        for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++) {
            auto PC = GameMode->AlivePlayers[i];
            if (!PC || !PC->Pawn) continue;

            AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)PC->PlayerState;
            if (PS && PS->bIsABot) continue;

            float Dist = Pawn->GetDistanceTo(PC->Pawn);
            if (Dist < MinDist) {
                MinDist = Dist;
                Nearest = PC->Pawn;
            }
        }

        return Nearest;
    }

    // OGSM - Update lobby behavior for warmup
    inline void UpdateLobbyBehavior() {
        if (BotState != EBotState::Warmup) return;

        float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());

        switch (LobbyState) {
        case ELobbyBotState::Idle:
            if (rand() % 100 < 30) {
                LobbyState = ELobbyBotState::Moving;
                FVector Target = Pawn->K2_GetActorLocation();
                Target.X += (rand() % 2000) - 1000;
                Target.Y += (rand() % 2000) - 1000;
                PC->MoveToLocation(Target, 50.f, true, false, false, true, nullptr, true);
            }
            else if (rand() % 100 < 20) {
                LobbyState = ELobbyBotState::Emoting;
                Emote();
                LobbyActionTimer = CurrentTime + 3.f;
            }
            else if (rand() % 100 < 25) {
                TargetWeaponPickup = FindNearestWeaponPickup();
                if (TargetWeaponPickup) {
                    LobbyState = ELobbyBotState::PickingUpWeapon;
                    PC->MoveToActor(TargetWeaponPickup, 100.f, true, false, true, nullptr, true);
                }
            }
            break;

        case ELobbyBotState::Moving:
            if (PC->PathFollowingComponent && PC->PathFollowingComponent->DidMoveReachGoal()) {
                LobbyState = ELobbyBotState::Idle;
            }
            break;

        case ELobbyBotState::Emoting:
            if (CurrentTime > LobbyActionTimer) {
                LobbyState = ELobbyBotState::Idle;
            }
            break;

        case ELobbyBotState::PickingUpWeapon:
            if (TargetWeaponPickup && Pawn->GetDistanceTo(TargetWeaponPickup) < 300.f) {
                Pickup((AFortPickup*)TargetWeaponPickup);
                SimpleSwitchToWeapon();
                LobbyState = ELobbyBotState::Shooting;
                LobbyActionTimer = CurrentTime + 5.f;
            }
            break;

        case ELobbyBotState::Shooting:
            if (CurrentTime <= LobbyActionTimer) {
                AActor* NearestPlayer = FindNearestRealPlayer();
                if (NearestPlayer) {
                    LookAt(NearestPlayer);
                    Pawn->PawnStartFire(0);
                }
            } else {
                Pawn->PawnStopFire(0);
                LobbyState = ELobbyBotState::Idle;
            }
            break;
        }
    }

    // OGSM - Update damage status (reset after delay)
    inline void UpdateDamageStatus() {
        float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
        if (CurrentTime - LastDamageTime > 3.f) {
            bIsUnderFire = false;
            ConsecutiveShotsTaken = 0;
        }
    }

    // OGSM - Fire with imperfection (human-like)
    inline void FireWithImperfection(AActor* Target) {
        if (!Pawn || !Target) return;

        FVector TargetLoc = Target->K2_GetActorLocation();

        float ErrorRange = (1.f - AimAccuracy) * 200.f;
        TargetLoc.X += (rand() % (int)(ErrorRange * 2)) - ErrorRange;
        TargetLoc.Y += (rand() % (int)(ErrorRange * 2)) - ErrorRange;
        TargetLoc.Z += (rand() % (int)(ErrorRange * 2)) - ErrorRange;

        float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());

        if (CurrentTime - LastAimTime > ReactionTime) {
            FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(
                Pawn->K2_GetActorLocation(), TargetLoc);
            PC->SetControlRotation(AimRot);
            LastAimTime = CurrentTime;
        }

        if (bMakesMistakes && (rand() % 100) < (MistakeChance * 100)) {
            return;
        }

        Pawn->PawnStartFire(0);
    }

    // OGSM - Post looting behavior
    inline void PostLootBehavior() {
        if (BotState != EBotState::Looting) return;

        float Choice = (float)(rand() % 100) / 100.f;

        if (Choice < 0.3f && HasEnoughResources(50)) {
            BuildBox();
        }
        else if (Choice < 0.5f && bNeedsFarming) {
            FarmTick();
        }
        else {
            BotState = EBotState::LookingForPlayers;
        }
    }

    // OGSM - Build box protection
    inline void BuildBox() {
        if (!Pawn || !HasEnoughResources(30)) return;

        if (Globals::bBotBuildingEnabled) {
            FVector BotLoc = Pawn->K2_GetActorLocation();
            BotBuilding::BuildBoxAt(this, BotLoc);
        }
    }

public:
    void OnDied(AFortPlayerStateAthena* KillerState, AActor* DamageCauser, FName BoneName)
    {
        static auto Acc_DistanceShot = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_DistanceShot.AccoladeId_DistanceShot");
        static auto Acc_LongShot = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_051_LongShot.AccoladeId_051_LongShot");
        static auto Acc_LudicrousShot = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_052_LudicrousShot.AccoladeId_052_LudicrousShot");
        static auto Acc_ImpossibleShot = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_053_ImpossibleShot.AccoladeId_053_ImpossibleShot");
        static auto Acc_Headshot = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_048_HeadshotElim.AccoladeId_048_HeadshotElim");

        static auto Acc_Survival_Bronze = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_026_Survival_Default_Bronze.AccoladeId_026_Survival_Default_Bronze");
        static auto Acc_Survival_Silver = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_027_Survival_Default_Silver.AccoladeId_027_Survival_Default_Silver");
        static auto Acc_Survival_Gold = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_028_Survival_Default_Gold.AccoladeId_028_Survival_Default_Gold");

        Log("Bot Died");

        bIsDead = true;

        if (!KillerState || !DamageCauser || !PlayerState || !Pawn)
            return;

        FDeathInfo& DeathInfo = PlayerState->DeathInfo;

        DeathInfo.bDBNO = Pawn->bWasDBNOOnDeath;
        DeathInfo.DeathLocation = Pawn->K2_GetActorLocation();
        DeathInfo.DeathTags = Pawn->DeathTags;
        DeathInfo.Downer = KillerState ? KillerState : nullptr;
        AFortPawn* KillerPawn = KillerState ? KillerState->GetCurrentPawn() : nullptr;
        DeathInfo.Distance = (KillerPawn && Pawn) ? KillerPawn->GetDistanceTo(Pawn) : 0.f;
        DeathInfo.FinisherOrDowner = KillerState ? KillerState : nullptr;
        DeathInfo.DeathCause = PlayerState->ToDeathCause(DeathInfo.DeathTags, DeathInfo.bDBNO);
        DeathInfo.bInitialized = true;
        PlayerState->OnRep_DeathInfo();

        bool bIsKillerABot = KillerState->bIsABot;

        if (!PC->Inventory)
            return;

        auto KillerPC = (AFortPlayerControllerAthena*)KillerState->GetOwner();

        if (KillerPC && KillerPC->IsA(AFortPlayerControllerAthena::StaticClass()) && !bIsKillerABot)
        {
            KillerState->KillScore++;

            Quests::GiveAccolade(KillerPC, StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_012_Elimination.AccoladeId_012_Elimination"));
            Quests::GiveAccolade(KillerPC, GetDefFromEvent(EAccoladeEvent::Kill, KillerState->KillScore));


            // OGSM - Update quest progress for eliminations
            PlayerQuests::OnPlayerEliminatedBot(KillerPC, PlayerState);
            // giving assist accolade cuz idfk how to track assists
            Quests::GiveAccolade((AFortPlayerControllerAthena*)KillerState->Owner, StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_013_Assist.AccoladeId_013_Assist"));

            for (size_t i = 0; i < KillerState->PlayerTeam->TeamMembers.Num(); i++)
            {
                ((AFortPlayerStateAthena*)KillerState->PlayerTeam->TeamMembers[i]->PlayerState)->TeamKillScore++;
                ((AFortPlayerStateAthena*)KillerState->PlayerTeam->TeamMembers[i]->PlayerState)->OnRep_TeamKillScore();
            }

            KillerState->ClientReportKill(PlayerState);
            KillerState->ClientReportTeamKill(KillerState->KillScore); 
            KillerState->OnRep_Kills();

            /*auto KillerPawn = KillerPC->MyFortPawn;

            if (GameState->PlayersLeft && GameState->PlayerBotsLeft <= 1 && !GameState->IsRespawningAllowed(PlayerState)) // This crashes it lol
            {
                if (KillerPawn != Pawn)
                {
                    UFortWeaponItemDefinition* KillerWeaponDef = nullptr;

                    if (auto ProjectileBase = Cast<AFortProjectileBase>(DamageCauser))
                        KillerWeaponDef = ((AFortWeapon*)ProjectileBase->GetOwner())->WeaponData;
                    if (auto Weapon = Cast<AFortWeapon>(DamageCauser))
                        KillerWeaponDef = Weapon->WeaponData;

                    KillerPC->PlayWinEffects(KillerPawn, KillerWeaponDef, DeathInfo.DeathCause, false);
                    KillerPC->ClientNotifyWon(KillerPawn, KillerWeaponDef, DeathInfo.DeathCause);
                }

                KillerState->Place = 1;
                KillerState->OnRep_Place();
                GameState->WinningPlayerState = KillerState;
                GameState->WinningTeam = KillerState->TeamIndex;
                GameState->OnRep_WinningPlayerState();
                GameState->OnRep_WinningTeam();
                GameMode->EndMatch();
            }*/
        }

        if (bIsKillerABot) {
            Log("got to the end!");
            return;
        }

        if (!bFirstElimTriggered) {
            Quests::GiveAccolade(KillerPC, StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_017_First_Elimination.AccoladeId_017_First_Elimination"));
            bFirstElimTriggered = true;
        }

        float Distance = DeathInfo.Distance / 100.0f;

        if (Distance >= 100.0f && Distance < 150.0f)
        {
            Quests::GiveAccolade(KillerPC, Acc_DistanceShot); // 100-149m accolade
        }
        else if (Distance >= 150.0f && Distance < 200.0f)
        {
            Quests::GiveAccolade(KillerPC, Acc_LongShot); // 150-199m accolade
        }
        else if (Distance >= 200.0f && Distance < 250.0f)
        {
            Quests::GiveAccolade(KillerPC, Acc_LudicrousShot); // 200-249m accolade
        }
        else if (Distance >= 250.0f)
        {
            Quests::GiveAccolade(KillerPC, Acc_ImpossibleShot); // 250+m accolade
        }

        if (BoneName.ToString() == "head")
        {
            KillerPC->HeadShots++;
            Quests::GiveAccolade(KillerPC, Acc_Headshot); // headshot accolade

            if (KillerPC->HeadShots == 4) {
                Quests::GiveAccolade(KillerPC, StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_049_Headhunter.AccoladeId_049_Headhunter"));
            }
        }

        if (KillerPC) {
            float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());

            if (UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld()) > KillerPC->LastKillTimeWindow) {
                KillerPC->KillsInKillTimeWindow = 0;
                KillerPC->LastRecordedKillsInKillTimeWindow = 0;
            }
            KillerPC->LastKillTimeWindow = CurrentTime + 10.f;
            KillerPC->KillsInKillTimeWindow++;
        }

        Log("Got to the end");
    }

public:
    void SetStuck(AActor* OtherActor, FHitResult& Hit) {
        if (BotState < EBotState::Landed)
            return;

        if (BotState == EBotState::Stuck)
            return;

        if (OtherActor && OtherActor->IsA(ABuildingSMActor::StaticClass()))
        {
            ABuildingSMActor* Actor = (ABuildingSMActor*)OtherActor;
            float Health = Actor->GetHealth();
            FFindFloorResult Res;

            if (Actor->bCanBeDamaged == 1 && Health > 1 && Health < 2500)
            {
                Pawn->CharacterMovement->K2_FindFloor(Pawn->CapsuleComponent->K2_GetComponentLocation(), &Res);
                if (Res.HitResult.Actor.Get() == OtherActor)
                    return;
                StuckBuildingActor = Actor;
            }
        }

        CachedBotState = BotState;
        BotState = EBotState::Stuck;
    }

public:
    // Give an item to the bot
    void GiveItemBot(UFortItemDefinition* Def, int Count = 1, int LoadedAmmo = 0)
    {
        if (!Def) {
            return;
        }

        UFortWorldItem* Item = (UFortWorldItem*)Def->CreateTemporaryItemInstanceBP(Count, 0);
        Item->OwnerInventory = PC->Inventory;
        Item->ItemEntry.LoadedAmmo = LoadedAmmo;
        PC->Inventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
        PC->Inventory->Inventory.ItemInstances.Add(Item);
        PC->Inventory->Inventory.MarkItemDirty(Item->ItemEntry);
        PC->Inventory->HandleInventoryLocalUpdate();
    }

    FFortItemEntry* GetEntry(UFortItemDefinition* Def)
    {
        for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            if (PC->Inventory->Inventory.ReplicatedEntries[i].ItemDefinition == Def)
                return &PC->Inventory->Inventory.ReplicatedEntries[i];
        }

        return nullptr;
    }

    void Emote()
    {
        auto EmoteDef = Dances[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, Dances.size() - 1)];
        if (!EmoteDef)
            return;

        static UClass* EmoteAbilityClass = StaticLoadObject<UClass>("/Game/Abilities/Emotes/GAB_Emote_Generic.GAB_Emote_Generic_C");

        FGameplayAbilitySpec Spec{};
        AbilitySpecConstructor(&Spec, reinterpret_cast<UGameplayAbility*>(EmoteAbilityClass->DefaultObject), 1, -1, EmoteDef);
        GiveAbilityAndActivateOnce(reinterpret_cast<AFortPlayerStateAthena*>(PC->PlayerState)->AbilitySystemComponent, &Spec.Handle, Spec);
    }

    void Run()
    {
        for (size_t i = 0; i < PlayerState->AbilitySystemComponent->ActivatableAbilities.Items.Num(); i++)
        {
            if (PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].Ability->IsA(UFortGameplayAbility_Sprint::StaticClass()))
            {
                if (PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].ActivationInfo.PredictionKeyWhenActivated.bIsStale) {
                    continue;
                }
                if (PlayerState->AbilitySystemComponent->CanActivateAbilityWithMatchingTag(PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].Ability->AbilityTags)) {
                    PlayerState->AbilitySystemComponent->ServerTryActivateAbility(PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].Handle, PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].InputPressed, PlayerState->AbilitySystemComponent->ActivatableAbilities.Items[i].ActivationInfo.PredictionKeyWhenActivated);
                }
                break;
            }
        }
    }

    void ForceStrafe(bool override) {
        if (!bIsCurrentlyStrafing && override)
        {
            bIsCurrentlyStrafing = true;
            if (UKismetMathLibrary::RandomBool()) {
                StrafeType = EBotStrafeType::StrafeLeft;
            }
            else {
                StrafeType = EBotStrafeType::StrafeRight;
            }
            StrafeEndTime = Statics->GetTimeSeconds(UWorld::GetWorld()) + Math->RandomFloatInRange(2.0f, 5.0f);
        }
        else
        {
            if (Statics->GetTimeSeconds(UWorld::GetWorld()) < StrafeEndTime)
            {
                if (StrafeType == EBotStrafeType::StrafeLeft) {
                    Pawn->AddMovementInput((Pawn->GetActorRightVector() * -1.0f), 1.5f, true);
                }
                else {
                    Pawn->AddMovementInput(Pawn->GetActorRightVector(), 1.5f, true);
                }
            }
            else
            {
                bIsCurrentlyStrafing = false;
            }
        }
    }

    void LookAt(AActor* Actor)
    {
        if (!Pawn || PC->GetFocusActor() == Actor)
            return;

        if (!Actor)
        {
            PC->K2_ClearFocus();
            return;
        }

        PC->K2_SetFocus(Actor);
    }

    bool IsPickaxeEquiped() {
        if (!Pawn || !Pawn->CurrentWeapon)
            return false;

        if (Pawn->CurrentWeapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
        {
            return true;
        }
        return false;
    }

    bool HasGun()
    {
        for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
            if (Entry.ItemDefinition) {
                std::string ItemName = Entry.ItemDefinition->Name.ToString();
                if (ItemName.contains("Shotgun") || ItemName.contains("SMG") || ItemName.contains("Assault")
                    || ItemName.contains("Sniper") || ItemName.contains("Rocket") || ItemName.contains("Pistol")) {
                    return true;
                    break;
                }
            }
        }
        return false;
    }

    void EquipPickaxe()
    {
        if (!Pawn || !Pawn->CurrentWeapon)
            return;

        if (!Pawn->CurrentWeapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
        {
            for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++)
            {
                if (PC->Inventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
                {
                    Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)PC->Inventory->Inventory.ReplicatedEntries[i].ItemDefinition, PC->Inventory->Inventory.ReplicatedEntries[i].ItemGuid);
                    break;
                }
            }
        }
    }

    void SimpleSwitchToWeapon() {
        if (!HasGun()) {
            return;
        }

        if (!Pawn || !Pawn->CurrentWeapon || !Pawn->CurrentWeapon->WeaponData || !PC || !PC->Inventory || bIsDead)
            return;

        if (!Pawn->CurrentWeapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass())) {
            return;
        }

        if (Pawn->CurrentWeapon->WeaponData->IsA(UFortWeaponMeleeItemDefinition::StaticClass()))
        {
            for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++)
            {
                auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
                if (Entry.ItemDefinition) {
                    std::string ItemName = Entry.ItemDefinition->Name.ToString();
                    if (ItemName.contains("Shotgun") || ItemName.contains("SMG") || ItemName.contains("Assault")
                        || ItemName.contains("Grenade") || ItemName.contains("Sniper") || ItemName.contains("Rocket") || ItemName.contains("Pistol")) {
                        Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry.ItemDefinition, Entry.ItemGuid);
                        break;
                    }
                }
            }
        }
    }

    // OGSM - Switch weapon based on distance to target
    void WeaponSwitchByDistance(float Distance) {
        if (!Pawn || !PC || !PC->Inventory || bIsDead) return;
        if (!HasGun()) return;

        UFortWeaponItemDefinition* BestWeapon = nullptr;
        FGuid BestWeaponGuid;

        // Find best weapon for distance
        for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
            if (!Entry.ItemDefinition) continue;

            std::string ItemName = Entry.ItemDefinition->Name.ToString();
            std::transform(ItemName.begin(), ItemName.end(), ItemName.begin(), ::tolower);

            // Close range - Shotgun or SMG
            if (Distance < 500.0f) {
                if (ItemName.contains("shotgun")) {
                    BestWeapon = (UFortWeaponItemDefinition*)Entry.ItemDefinition;
                    BestWeaponGuid = Entry.ItemGuid;
                    break;
                }
                else if (ItemName.contains("smg") || ItemName.contains("pistol")) {
                    BestWeapon = (UFortWeaponItemDefinition*)Entry.ItemDefinition;
                    BestWeaponGuid = Entry.ItemGuid;
                }
            }
            // Medium range - Assault Rifle
            else if (Distance < 1500.0f) {
                if (ItemName.contains("assault") || ItemName.contains("ar")) {
                    BestWeapon = (UFortWeaponItemDefinition*)Entry.ItemDefinition;
                    BestWeaponGuid = Entry.ItemGuid;
                    break;
                }
            }
            // Long range - Sniper
            else {
                if (ItemName.contains("sniper")) {
                    BestWeapon = (UFortWeaponItemDefinition*)Entry.ItemDefinition;
                    BestWeaponGuid = Entry.ItemGuid;
                    break;
                }
                else if (ItemName.contains("assault") || ItemName.contains("ar")) {
                    BestWeapon = (UFortWeaponItemDefinition*)Entry.ItemDefinition;
                    BestWeaponGuid = Entry.ItemGuid;
                }
            }
        }

        if (BestWeapon && Pawn->CurrentWeapon && Pawn->CurrentWeapon->WeaponData != BestWeapon) {
            Pawn->EquipWeaponDefinition(BestWeapon, BestWeaponGuid);
        }
    }

    // OGSM - Crouch spam during combat
    void CrouchSpam() {
        if (!Pawn || bIsDead) return;
        
        if (UKismetMathLibrary::RandomBoolWithWeight(0.15f)) {
            if (Pawn->bIsCrouched) {
                Pawn->UnCrouch(false);
            } else {
                Pawn->Crouch(false);
            }
        }
    }

    // I will add more stuff to this over time based on what the bot needs/dont needs
    bool ShouldPickup(AFortPickup* Pickup) {
        if (!Pickup) {
            return false;
        }
        auto Def = Pickup->PrimaryPickupItemEntry.ItemDefinition;
        if (!Def) return false;

        std::string ItemName = Def->Name.ToString();

        // Dont bother with ammo cuz bots dont use it and bots usually pickup stuff in range anyway
        if (Def->IsA(UFortAmmoItemDefinition::StaticClass()))
            return false;

        return true;
    }

    void Pickup(AFortPickup* Pickup) {
        if (!Pickup)
            return;

        GiveItemBot(Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count, Pickup->PrimaryPickupItemEntry.LoadedAmmo);

        Pickup->PickupLocationData.bPlayPickupSound = true;
        Pickup->PickupLocationData.FlyTime = 0.3f;
        Pickup->PickupLocationData.ItemOwner = Pawn;
        Pickup->PickupLocationData.PickupGuid = Pickup->PrimaryPickupItemEntry.ItemGuid;
        Pickup->PickupLocationData.PickupTarget = Pawn;
        Pickup->OnRep_PickupLocationData();

        Pickup->bPickedUp = true;
        Pickup->OnRep_bPickedUp();
    }

    void PickupAllItemsInRange(float Range = 320.f) {
        if (!Pawn || !PC) {
            return;
        }

        static auto PickupClass = AFortPickupAthena::StaticClass();
        TArray<AActor*> Array;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), PickupClass, &Array);

        for (size_t i = 0; i < Array.Num(); i++)
        {
            if (!Array[i] || Array[i]->bHidden)
                continue;

            if (!ShouldPickup((AFortPickupAthena*)Array[i])) {
                continue;
            }

            if (Array[i]->GetDistanceTo(Pawn) < Range)
            {
                Pickup((AFortPickupAthena*)Array[i]);
            }
        }

        Array.Free();
    }

    ABuildingActor* FindNearestChest()
    {
        static auto ChestClass = StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
        static auto FactionChestClass = StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena_FactionChest.Tiered_Chest_Athena_FactionChest_C");
        TArray<AActor*> Array;
        TArray<AActor*> FactionChests;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), ChestClass, &Array);
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), FactionChestClass, &FactionChests);

        AActor* NearestChest = nullptr;

        for (size_t i = 0; i < Array.Num(); i++)
        {
            AActor* Chest = Array[i];
            if (Chest->bHidden && Chest != TargetLootable)
                continue;

            if (!NearestChest || Chest->GetDistanceTo(Pawn) < NearestChest->GetDistanceTo(Pawn))
            {
                NearestChest = Array[i];
            }
        }

        for (size_t i = 0; i < FactionChests.Num(); i++)
        {
            AActor* Chest = FactionChests[i];
            if (Chest->bHidden && Chest != TargetLootable)
                continue;

            if (!NearestChest || Chest->GetDistanceTo(Pawn) < NearestChest->GetDistanceTo(Pawn))
            {
                NearestChest = FactionChests[i];
            }
        }
        Array.Free();
        FactionChests.Free();
        return (ABuildingActor*)NearestChest;
    }

    AFortPickupAthena* FindNearestPickup()
    {
        static auto PickupClass = AFortPickupAthena::StaticClass();
        TArray<AActor*> Array;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), PickupClass, &Array);
        AActor* NearestPickup = nullptr;

        for (size_t i = 0; i < Array.Num(); i++)
        {
            if (Array[i]->bHidden && Array[i] != TargetLootable)
                continue;

            if (!ShouldPickup((AFortPickupAthena*)Array[i])) {
                continue;
            }

            if (!NearestPickup || Array[i]->GetDistanceTo(Pawn) < NearestPickup->GetDistanceTo(Pawn))
            {
                NearestPickup = Array[i];
            }
        }

        Array.Free();
        return (AFortPickupAthena*)NearestPickup;
    }

    bool GetNearestLootable() {
        static auto ChestClass = StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena.Tiered_Chest_Athena_C");
        static auto FactionChestClass = StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Containers/Tiered_Chest_Athena_FactionChest.Tiered_Chest_Athena_FactionChest_C");
        TArray<AActor*> ChestArray;
        TArray<AActor*> FactionChests;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), ChestClass, &ChestArray);
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), FactionChestClass, &FactionChests);

        static auto PickupClass = AFortPickupAthena::StaticClass();
        TArray<AActor*> PickupArray;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), PickupClass, &PickupArray);

        AActor* NearestPickup = nullptr;
        AActor* NearestChest = nullptr;

        for (size_t i = 0; i < ChestArray.Num(); i++)
        {
            AActor* Chest = ChestArray[i];
            if (Chest->bHidden && Chest != TargetLootable)
                continue;

            if (!NearestChest || Chest->GetDistanceTo(Pawn) < NearestChest->GetDistanceTo(Pawn))
            {
                NearestChest = ChestArray[i];
            }
        }

        for (size_t i = 0; i < FactionChests.Num(); i++)
        {
            AActor* Chest = FactionChests[i];
            if (Chest->bHidden && Chest != TargetLootable)
                continue;

            if (!NearestChest || Chest->GetDistanceTo(Pawn) < NearestChest->GetDistanceTo(Pawn))
            {
                NearestChest = FactionChests[i];
            }
        }

        for (size_t i = 0; i < PickupArray.Num(); i++)
        {
            if (PickupArray[i]->bHidden && PickupArray[i] != TargetLootable)
                continue;

            if (!ShouldPickup((AFortPickupAthena*)PickupArray[i])) {
                continue;
            }

            if (!NearestPickup || PickupArray[i]->GetDistanceTo(Pawn) < NearestPickup->GetDistanceTo(Pawn))
            {
                NearestPickup = PickupArray[i];
            }
        }

        ChestArray.Free();
        FactionChests.Free();
        PickupArray.Free();
        return NearestPickup->GetDistanceTo(Pawn) > NearestChest->GetDistanceTo(Pawn);
    }

    ABuildingSMActor* FindNearestBuildingSMActor()
    {
        static TArray<AActor*> Array;
        static bool PopulatedArray = false;
        if (!PopulatedArray)
        {
            PopulatedArray = true;
            UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), ABuildingSMActor::StaticClass(), &Array);
        }

        AActor* NearestActor = nullptr;

        for (size_t i = 0; i < Array.Num(); i++)
        {
            if (!NearestActor || (((ABuildingSMActor*)NearestActor)->GetHealth() < 1500 && ((ABuildingSMActor*)NearestActor)->GetHealth() > 1 && Array[i]->GetDistanceTo(Pawn) < NearestActor->GetDistanceTo(Pawn)))
            {
                NearestActor = Array[i];
            }
        }

        return (ABuildingSMActor*)NearestActor;
    }

    FVector FindNearestPlayerOrBot() {
        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

        AActor* NearestPlayer = nullptr;

        for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++)
        {
            if (!NearestPlayer || (GameMode->AlivePlayers[i]->Pawn && GameMode->AlivePlayers[i]->Pawn->GetDistanceTo(Pawn) < NearestPlayer->GetDistanceTo(Pawn)))
            {
                AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)GameMode->AlivePlayers[i]->PlayerState;
                if (PS->TeamIndex != PlayerState->TeamIndex) {
                    NearestPlayer = GameMode->AlivePlayers[i]->Pawn;
                }
            }
        }

        for (size_t i = 0; i < GameMode->AliveBots.Num(); i++)
        {
            if (GameMode->AliveBots[i]->Pawn != Pawn)
            {
                if (!NearestPlayer || (GameMode->AliveBots[i]->Pawn && GameMode->AliveBots[i]->Pawn->GetDistanceTo(Pawn) < NearestPlayer->GetDistanceTo(Pawn)))
                {
                    AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)GameMode->AlivePlayers[i]->PlayerState;
                    if (PS->TeamIndex != PlayerState->TeamIndex) {
                        NearestPlayer = GameMode->AliveBots[i]->Pawn;
                    }
                }
            }
        }

        for (size_t i = 0; i < FactionBots.size(); i++) {
            if (!NearestPlayer || (FactionBots[i]->Pawn && FactionBots[i]->Pawn->GetDistanceTo(Pawn) < NearestPlayer->GetDistanceTo(Pawn)))
            {
                NearestPlayer = FactionBots[i]->Pawn;
            }
        }

        return NearestPlayer->K2_GetActorLocation();
    }

    AActor* GetNearestPlayerActor() {
        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

        AActor* NearestPlayer = nullptr;

        for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++)
        {
            if (!NearestPlayer || (GameMode->AlivePlayers[i]->Pawn && GameMode->AlivePlayers[i]->Pawn->GetDistanceTo(Pawn) < NearestPlayer->GetDistanceTo(Pawn)))
            {
                AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)GameMode->AlivePlayers[i]->PlayerState;
                if (PS->TeamIndex != PlayerState->TeamIndex) {
                    NearestPlayer = GameMode->AlivePlayers[i]->Pawn;
                }
            }
        }

        for (size_t i = 0; i < GameMode->AliveBots.Num(); i++)
        {
            if (GameMode->AliveBots[i]->Pawn != Pawn)
            {
                if (!NearestPlayer || (GameMode->AliveBots[i]->Pawn && GameMode->AliveBots[i]->Pawn->GetDistanceTo(Pawn) < NearestPlayer->GetDistanceTo(Pawn)))
                {
                    AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)GameMode->AliveBots[i]->PlayerState;
                    if (PS->TeamIndex != PlayerState->TeamIndex) {
                        NearestPlayer = GameMode->AliveBots[i]->Pawn;
                    }
                }
            }
        }

        for (size_t i = 0; i < FactionBots.size(); i++) {
            if (!NearestPlayer || (FactionBots[i]->Pawn && FactionBots[i]->Pawn->GetDistanceTo(Pawn) < NearestPlayer->GetDistanceTo(Pawn)))
            {
                NearestPlayer = FactionBots[i]->Pawn;
            }
        }

        return NearestPlayer;
    }

    void UpdateLootableReservation(AActor* Lootable, bool RemoveReservation) {
        if (RemoveReservation && !TargetLootable) {
            return;
        }

        if (!RemoveReservation) {
            if (!Lootable) {
                return;
            }
            TargetLootable = Lootable;
            Lootable->bHidden = true;
        }
        else {
            TargetLootable->bHidden = false;
            TargetLootable = nullptr;
        }
    }

    void __fastcall GiveLategameLoadout()
    {
        if (UFortItemDefinition* AssaultRifleDef = Inventory::LoadWeapon(Assault_rifle))
        {
            GiveItemBot(AssaultRifleDef, 1, Looting::GetClipSize(AssaultRifleDef));

            if (auto* RangedDef = (UFortWeaponRangedItemDefinition*)AssaultRifleDef)
            {
                UFortWorldItemDefinition* AmmoDef = RangedDef->GetAmmoWorldItemDefinition_BP();
                if (AmmoDef)
                    GiveItemBot(AmmoDef, 200, 0);
            }
        }

        if (UFortItemDefinition* ShotGunDef = Inventory::LoadWeapon(Shotgun))
        {
            GiveItemBot(ShotGunDef, 1, Looting::GetClipSize(ShotGunDef));

            if (auto* RangedDef = (UFortWeaponRangedItemDefinition*)ShotGunDef)
            {
                UFortWorldItemDefinition* AmmoDef = RangedDef->GetAmmoWorldItemDefinition_BP();
                if (AmmoDef)
                    GiveItemBot(AmmoDef, 120, 0);
            }
        }

        if (UFortItemDefinition* RandomDef = Inventory::LoadWeapon(Mixed))
        {
            GiveItemBot(RandomDef, 1, Looting::GetClipSize(RandomDef));

            if (auto* RangedDef = (UFortWeaponRangedItemDefinition*)RandomDef)
            {
                UFortWorldItemDefinition* AmmoDef = RangedDef->GetAmmoWorldItemDefinition_BP();
                if (AmmoDef)
                    GiveItemBot(AmmoDef, 120, 0);
            }
        }

        if (auto Consumable1Def = Inventory::LoadWeapon(Consumables))
            GiveItemBot(Consumable1Def, 3, 0);

        if (auto Consumable2Def = Inventory::LoadWeapon(Consumables))
            GiveItemBot(Consumable2Def, 3, 0);

        if (auto TrapDef = Inventory::LoadWeapon(Traps))
            GiveItemBot(TrapDef, 3, 0);

        static UFortItemDefinition* WoodDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
        GiveItemBot(WoodDef, 500, 0);

        static UFortItemDefinition* StoneDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
        GiveItemBot(StoneDef, 500, 0);

        static UFortItemDefinition* MetalDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");
        GiveItemBot(MetalDef, 500, 0);
    }
};

class BotsBTService_AIEvaluator {
public:
    // When stressed the bot will handle combat situations with players or other bots differently
    bool IsStressed(AFortPlayerPawnAthena* Pawn, ABP_PhoebePlayerController_C* PC) {
        // If the bots health is 75 or below then they are stressed
        if (Pawn->GetHealth() <= 75) {
            return true;
        }
        return false;
    }

public:
    void Tick(PlayerBot* bot) {
        FVector BotPos = bot->Pawn->K2_GetActorLocation();
        FVector Vel = bot->Pawn->GetVelocity();
        float Speed = sqrtf(Vel.X * Vel.X + Vel.Y * Vel.Y);

        if (bot->tick_counter % 60 == 0) {
            // Lets update the reservation every 2 seconds because its cleaner
            AActor* NearestChest = bot->FindNearestChest();
            AActor* NearestPickup = (AActor*)bot->FindNearestPickup();
            if (!NearestChest || !NearestPickup) {}
            else {
                AActor* Nearest = NearestChest;
                ELootableType NearestLootable = ELootableType::Chest;
                if (NearestChest->GetDistanceTo(bot->Pawn) > NearestPickup->GetDistanceTo(bot->Pawn)) {
                    NearestLootable = ELootableType::Pickup;
                    Nearest = NearestPickup;
                }
                if (bot->TargetLootable) {
                    if (Nearest != bot->TargetLootable) {
                        bot->UpdateLootableReservation(nullptr, true);
                        bot->UpdateLootableReservation(Nearest, false);
                    }
                }
                else {
                    bot->UpdateLootableReservation(Nearest, false);
                }
                bot->TargetLootableType = NearestLootable;
            }

            bot->NearestPlayerActor = bot->GetNearestPlayerActor();
        }

        if (bot->tick_counter % 60 == 0) {
            // Every 2 seconds clear the focus just incase the bot is doing something else
            bot->PC->K2_ClearFocus();
        }

        if (bot->Pawn->bIsCrouched && (bot->tick_counter % 30) == 0) {
            bot->Pawn->UnCrouch(false);
        }

        bot->bIsStressed = IsStressed(bot->Pawn, bot->PC);

        if (bot->bIsCurrentlyStrafing) {
            bot->ForceStrafe(false);
        }

        if ((bot->tick_counter % 90 == 0) && Speed >= 100 && bot->BotState > EBotState::Landed) { // Works but mostly just spams error logs
            bot->Run();
        }
    }
};

class BotsBTService_Warmup{
public:
    void Tick(PlayerBot* bot) {
        // OGSM - Use new detailed lobby behavior
        bot->UpdateLobbyBehavior();
    }
};
};

class BotsBTService_AIDropZone {

    // OGSM - Check if location is in bus path
    inline bool IsInBusPath(FVector Location) {
        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
        AActor* Bus = GameState->GetAircraft(0);
        if (!Bus) return true;

        FVector BusLoc = Bus->K2_GetActorLocation();
        FVector BusForward = Bus->GetActorForwardVector();

        FVector ToPOI = Location - BusLoc;
        ToPOI.Z = 0;
        BusForward.Z = 0;

        float DotProduct = (ToPOI.X * BusForward.X + ToPOI.Y * BusForward.Y);
        return DotProduct > 0;
    }

    // OGSM - Check if location is in future safe zone
    inline bool IsInFutureSafeZone(FVector Location, AFortSafeZoneIndicator* CurrentSafeZone) {
        if (!CurrentSafeZone) return true;

        float Dist = Location.DistanceTo(CurrentSafeZone->NextCenter);
        return Dist < CurrentSafeZone->NextRadius * 0.8f;
    }

    void ChooseDropZone(PlayerBot* bot) {
        if (!bot->TargetDropZone.IsZero()) return;

        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
        auto SafeZone = GameState->SafeZoneIndicator;

        int32 Choice = rand() % 100;
        FVector SelectedLocation;

        // 40% chance - Grande ville avec Boss
        if (Choice < 40 && !::BossPOIs.empty()) {
            std::map<FString, int32> BotCountPerPOI;
            for (auto& OtherBot : PlayerBotArray) {
                if (OtherBot != bot && !OtherBot->TargetDropZone.IsZero()) {
                    for (auto& POI : ::BossPOIs) {
                        if (OtherBot->TargetDropZone.DistanceTo(POI.Location) < 2000.0f) {
                            BotCountPerPOI[POI.Name]++;
                        }
                    }
                }
            }

            FBossPOI* BestPOI = nullptr;
            int32 MinBots = 999;

            for (auto& POI : ::BossPOIs) {
                int32 CurrentBots = BotCountPerPOI[POI.Name];
                if (CurrentBots < POI.MaxBots && CurrentBots < MinBots) {
                    if (IsInBusPath(POI.Location)) {
                        MinBots = CurrentBots;
                        BestPOI = &POI;
                    }
                }
            }

            if (BestPOI) {
                SelectedLocation = BestPOI->Location;
                bot->TargetBossPOI = BestPOI;
                Log("Bot targeting boss POI: " + BestPOI->Name.ToString());
            }
        }
        // 35% chance - Moyenne ville avec toits
        else if (Choice < 75 && !::MediumPOIs.empty()) {
            std::map<FString, int32> BotCountPerPOI;
            for (auto& OtherBot : PlayerBotArray) {
                if (OtherBot != bot && !OtherBot->TargetDropZone.IsZero()) {
                    for (auto& POI : ::MediumPOIs) {
                        if (OtherBot->TargetDropZone.DistanceTo(POI.Center) < POI.Radius) {
                            BotCountPerPOI[POI.Name]++;
                        }
                    }
                }
            }

            FMediumPOI* BestPOI = nullptr;
            int32 MinBots = 999;

            for (auto& POI : ::MediumPOIs) {
                int32 CurrentBots = BotCountPerPOI[POI.Name];
                if (CurrentBots < POI.MaxBots && CurrentBots < MinBots) {
                    if (IsInBusPath(POI.Center) && IsInFutureSafeZone(POI.Center, SafeZone)) {
                        MinBots = CurrentBots;
                        BestPOI = &POI;
                    }
                }
            }

            if (BestPOI) {
                if (BestPOI->bHasRoofChests) {
                    FRoofChestLocation* BestRoof = nullptr;
                    float MinDist = FLT_MAX;

                    for (auto& Roof : ::RoofChestLocations) {
                        if (Roof.bIsAvailable && Roof.Location.DistanceTo(BestPOI->Center) < BestPOI->Radius) {
                            float Dist = Roof.Location.DistanceTo(BestPOI->Center);
                            if (Dist < MinDist) {
                                MinDist = Dist;
                                BestRoof = &Roof;
                            }
                        }
                    }

                    if (BestRoof) {
                        SelectedLocation = BestRoof->Location;
                        BestRoof->bIsAvailable = false;
                        bot->bTargetingRoofChest = true;
                        bot->RoofChestLocation = BestRoof->Location;
                    } else {
                        SelectedLocation = BestPOI->Center;
                    }
                } else {
                    SelectedLocation = BestPOI->Center;
                }
                Log("Bot targeting medium POI: " + BestPOI->Name.ToString());
            }
        }
        // 20% chance - Petite zone
        else if (Choice < 95 && !::SmallPOIs.empty()) {
            int32 Idx = rand() % ::SmallPOIs.size();
            if (IsInFutureSafeZone(::SmallPOIs[Idx].Location, SafeZone)) {
                SelectedLocation = ::SmallPOIs[Idx].Location;
                Log("Bot targeting small POI: " + ::SmallPOIs[Idx].Name.ToString());
            }
        }
        // 5% chance - Drop zone normale
        else {
            SelectedLocation = DropZoneLocations[rand() % DropZoneLocations.size()];
        }

        if (SelectedLocation.IsZero()) {
            SelectedLocation = DropZoneLocations[rand() % DropZoneLocations.size()];
        }

        bot->TargetDropZone.X = SelectedLocation.X + (rand() % 800) - 400;
        bot->TargetDropZone.Y = SelectedLocation.Y + (rand() % 800) - 400;
        bot->TargetDropZone.Z = SelectedLocation.Z;
    }

public:

public:
    void Tick(PlayerBot* bot) {
        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
        auto Math = (UKismetMathLibrary*)UKismetMathLibrary::StaticClass()->DefaultObject;
        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        auto Statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;

        if (bot->TargetDropZone.IsZero()) {
            ChooseDropZone(bot);
            return;
        }

        if (bot->BotState == EBotState::Bus) {
            bot->Pawn->SetShield(0);
            if (bot->bHasJumpedFromBus) {
                bot->BotState = EBotState::Skydiving;
                return;
            }

            if (!bot->bHasThankedBusDriver && GameState->GamePhase == EAthenaGamePhase::Aircraft && Math->RandomBoolWithWeight(0.0005f))
            {
                bot->bHasThankedBusDriver = true;
                bot->PC->ThankBusDriver();
            }

            AActor* Bus = GameState->GetAircraft(0);
            if (!Bus) {
                return;
            }

            FVector BusLocation = Bus->K2_GetActorLocation();
            FVector DropTarget = bot->TargetDropZone;
            DropTarget.Z = BusLocation.Z;

            if (GameState->GamePhase > EAthenaGamePhase::Aircraft) {
                Log("Force Jump");
                bot->Pawn->K2_TeleportTo(DropTarget, {});
                bot->Pawn->BeginSkydiving(true);
                bot->BotState = EBotState::Skydiving;

                bot->bHasJumpedFromBus = true;

                return;
            }

            float DistanceToDrop = Math->Vector_Distance(BusLocation, DropTarget);
            //Log("DistanceToDrop: " + std::to_string(DistanceToDrop));
            //Log("ClosestDistToDropZone: " + std::to_string(bot->ClosestDistToDropZone));
            if (DistanceToDrop < bot->ClosestDistToDropZone) {
                bot->ClosestDistToDropZone = DistanceToDrop;
            }
            else {
                if (!bot->bHasThankedBusDriver && Math->RandomBoolWithWeight(0.5f)) {
                    bot->bHasThankedBusDriver = true;
                    bot->PC->ThankBusDriver();
                }
                if (Math->RandomBoolWithWeight(0.75)) {
                    bot->Pawn->K2_TeleportTo(GameState->GetAircraft(0)->K2_GetActorLocation(), {});
                    bot->Pawn->BeginSkydiving(true);
                    bot->BotState = EBotState::Skydiving;

                    bot->bHasJumpedFromBus = true;
                }
            }

            return;
        }

        auto BotPos = bot->Pawn->K2_GetActorLocation();
        if (!bot->TargetDropZone.IsZero()) {
            bot->TargetDropZone.Z = BotPos.Z;
        }

        if (bot->BotState == EBotState::Skydiving) {
            if (!bot->Pawn->bIsSkydiving) {
                bot->BotState = EBotState::Gliding;
            }

            if (!bot->TargetDropZone.IsZero()) {
                float Height = BotPos.Z;
                float DistToTarget = Math->Vector_Distance(FVector(BotPos.X, BotPos.Y, 0), FVector(bot->TargetDropZone.X, bot->TargetDropZone.Y, 0));

                // OGSM - Deploy glider at optimal height (300m)
                if (Height < 300.0f && DistToTarget < 500.0f) {
                    bot->BotState = EBotState::Gliding;
                }
                // OGSM - 90s diving for optimal speed (~60 m/s)
                else if (Globals::bStrategicDroppingEnabled && Height > 500.0f) {
                    FVector Direction = (bot->TargetDropZone - BotPos).GetNormalized();
                    FVector DiveDirection = Direction;
                    DiveDirection.Z = -0.7f; // Steep dive angle
                    FRotator TargetRot = Math->FindLookAtRotation(BotPos, bot->TargetDropZone);
                    bot->PC->SetControlRotation(TargetRot);
                    bot->PC->K2_SetActorRotation(TargetRot, true);
                    bot->Pawn->CharacterMovement->Velocity = DiveDirection * 6000.0f;
                }
                else {
                    // Standard skydiving
                    bot->Pawn->AddMovementInput(Math->NegateVector(bot->Pawn->GetActorUpVector()), 1, true);

                    float Dist = Math->Vector_Distance(BotPos, bot->TargetDropZone);
                    auto TestRot = Math->FindLookAtRotation(BotPos, bot->TargetDropZone);

                    bot->PC->SetControlRotation(TestRot);
                    bot->PC->K2_SetActorRotation(TestRot, true);

                    bot->PC->MoveToLocation(bot->TargetDropZone, 200.f, true, false, false, true, nullptr, true);
                }
            }
        }
        else if (bot->BotState == EBotState::Gliding) {
            if (bot->Pawn->bIsSkydiving) {
                bot->BotState = EBotState::Skydiving;
            }

            FVector Vel = bot->Pawn->GetVelocity();
            float Speed = Vel.Z;
            if (Speed == 0.f || bot->Pawn->bIsInWaterVolume) {
                bot->BotState = EBotState::Landed;
            }

            if (!bot->TargetDropZone.IsZero()) {
                float Dist = Math->Vector_Distance(BotPos, bot->TargetDropZone);
                auto TestRot = Math->FindLookAtRotation(BotPos, bot->TargetDropZone);

                bot->PC->SetControlRotation(TestRot);
                bot->PC->K2_SetActorRotation(TestRot, true);

                bot->PC->MoveToLocation(bot->TargetDropZone, 200.f, true, false, false, true, nullptr, true);
            }
        }
    }
};

class BotsBTService_Loot {
public:

public:
    void Tick(PlayerBot* bot) {
        if (bot->TargetLootable) {
            if (bot->HasGun()) {
                bot->BotState = EBotState::LookingForPlayers;
                return;
            }
            FVector BotLoc = bot->Pawn->K2_GetActorLocation();
            if (!BotLoc.IsZero()) {
                float Dist = Math->Vector_Distance(BotLoc, bot->TargetLootable->K2_GetActorLocation());

                auto BotPos = bot->Pawn->K2_GetActorLocation();
                auto TestRot = Math->FindLookAtRotation(BotPos, bot->TargetLootable->K2_GetActorLocation());

                bot->PC->SetControlRotation(TestRot);
                bot->PC->K2_SetActorRotation(TestRot, true);
                bot->LookAt(bot->TargetLootable);

                float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());

                if (bot->TargetLootable) {
                    FVector BotLoc = bot->Pawn->K2_GetActorLocation();
                    float Dist = Math->Vector_Distance(BotLoc, bot->TargetLootable->K2_GetActorLocation());

                    if (bot->LootTargetStartTime == 0.f) {
                        bot->LootTargetStartTime = CurrentTime;
                        bot->LastLootTargetDistance = Dist;
                    }

                    float Elapsed = CurrentTime - bot->LootTargetStartTime;

                    // if the bot is not getting closer or stuck for more than 8 seconds then we should try go for another lootable
                    if ((Elapsed > 8.f && Dist > bot->LastLootTargetDistance - 100.f)) {
                        if (Math->RandomBool()) {
                            bot->Pawn->K2_TeleportTo(bot->TargetLootable->K2_GetActorLocation(), {});
                        }
                        else {
                            bot->TargetLootable = nullptr;
                            bot->LootTargetStartTime = 0.f;
                        }
                        return;
                    }

                    if (Dist < 300.f) {
                        bot->LootTargetStartTime = 0.f;
                    }

                    bot->LastLootTargetDistance = Dist;
                }

                if (Dist < 300.f) {
                    bot->PC->StopMovement();
                    bot->Pawn->PawnStopFire(0);
                    if (!bot->TimeToNextAction || !bot->Pawn->bStartedInteractSearch && bot->TargetLootableType == ELootableType::Chest) {
                        bot->TimeToNextAction = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
                        bot->Pawn->bStartedInteractSearch = true;
                        bot->Pawn->OnRep_StartedInteractSearch();
                    }
                    else if (UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld()) - bot->TimeToNextAction >= 1.5f && bot->TargetLootableType == ELootableType::Chest) {
                        Looting::SpawnLoot((ABuildingContainer*)bot->TargetLootable);
                        bot->TargetLootable->bHidden = true;
                        bot->TargetLootable = nullptr;
                        AFortPickup* Pickup = bot->FindNearestPickup();
                        if (Pickup)
                        {
                            bot->PickupAllItemsInRange();
                            bot->SimpleSwitchToWeapon();
                        }

                        bot->Pawn->bStartedInteractSearch = false;
                        bot->Pawn->OnRep_StartedInteractSearch();
                        bot->TimeToNextAction = 0;
                        bot->BotState = EBotState::LookingForPlayers;
                    }
                    else if (bot->TargetLootableType == ELootableType::Pickup) {
                        //bot->PickupAllItemsInRange(400.f);
                        AFortPickup* Pickup = bot->FindNearestPickup();
                        if (Pickup)
                        {
                            bot->Pickup(Pickup);
                        }
                        bot->TimeToNextAction = 0;
                        bot->BotState = EBotState::LookingForPlayers;
                    }
                }
                else if (Dist < 2000.f) {
                    bot->Pawn->PawnStartFire(0);
                    bot->PC->MoveToActor(bot->TargetLootable, 50, true, false, true, nullptr, true);
                    //bot->Pawn->AddMovementInput(bot->Pawn->GetActorForwardVector(), 1.1f, true);
                }
                else {
                    bot->PC->MoveToActor(bot->TargetLootable, 50, true, false, true, nullptr, true);
                    //bot->Pawn->AddMovementInput(bot->Pawn->GetActorForwardVector(), 1.1f, true);
                }
            }
        }
        else {
            //Log("No targetlootable dummy!");
        }
    }
};

namespace PlayerBots {
    void FreeDeadBots() {
        for (size_t i = 0; i < PlayerBotArray.size();)
        {
            if (PlayerBotArray[i]->bIsDead) {
                delete PlayerBotArray[i];
                PlayerBotArray.erase(PlayerBotArray.begin() + i);
                Log("Freed a dead bot from the array!");
            }
            else {
                ++i;
            }
        }
    }

    void SpawnPlayerBots(AActor* SpawnLocator, EBotState StartingState = EBotState::Warmup, AFortPlayerControllerAthena* TeamPC = nullptr)
    {
        if (!Globals::bBotsEnabled)
            return;

        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;

        static auto BotBP = StaticLoadObject<UClass>("/Game/Athena/AI/Phoebe/BP_PlayerPawn_Athena_Phoebe.BP_PlayerPawn_Athena_Phoebe_C");
        static UBehaviorTree* BehaviorTree = StaticLoadObject<UBehaviorTree>("/Game/Athena/AI/Phoebe/BehaviorTrees/BT_Phoebe.BT_Phoebe");
        static UBlackboardData* BlackboardData = StaticLoadObject<UBlackboardData>("/Game/Athena/AI/Phoebe/BehaviorTrees/BB_Phoebe.BB_Phoebe");

        if (!BotBP || !BehaviorTree)
            return;

        PlayerBot* bot = new PlayerBot{};

        AFortGameModeAthena* GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;

        static auto BotMutator = (AFortAthenaMutator_Bots*)GameMode->ServerBotManager->CachedBotMutator;

        bot->Pawn = BotMutator->SpawnBot(BotBP, SpawnLocator, SpawnLocator->K2_GetActorLocation(), SpawnLocator->K2_GetActorRotation(), false);

        if (!bot->Pawn || !bot->Pawn->Controller)
            return;

        bot->PC = Cast<ABP_PhoebePlayerController_C>(bot->Pawn->Controller);
        bot->PlayerState = Cast<AFortPlayerStateAthena>(bot->PC->PlayerState);

        if (!bot->PC || !bot->PlayerState)
            return;

        if (!CIDs.empty() && UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(1)) { // use the random bool so that some bots will be defaults (more realistic)
            // as you could probably tell, i did not write this!
            auto CID = CIDs[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, CIDs.size() - 1)];
            if (CID->HeroDefinition)
            {
                if (CID->HeroDefinition->Specializations.IsValid())
                {
                    for (size_t i = 0; i < CID->HeroDefinition->Specializations.Num(); i++)
                    {
                        UFortHeroSpecialization* Spec = StaticLoadObject<UFortHeroSpecialization>(UKismetStringLibrary::GetDefaultObj()->Conv_NameToString(CID->HeroDefinition->Specializations[i].ObjectID.AssetPathName).ToString());
                        if (Spec)
                        {
                            for (size_t j = 0; j < Spec->CharacterParts.Num(); j++)
                            {
                                UCustomCharacterPart* Part = StaticLoadObject<UCustomCharacterPart>(UKismetStringLibrary::GetDefaultObj()->Conv_NameToString(Spec->CharacterParts[j].ObjectID.AssetPathName).ToString());
                                if (Part)
                                {
                                    bot->PlayerState->CharacterData.Parts[(uintptr_t)Part->CharacterPartType] = Part;
                                }
                            }
                        }
                    }
                }
            }
            if (CID) {
                bot->PC->CosmeticLoadoutBC.Character = CID;
            }
        }
        if (!Backpacks.empty() && UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.5)) { // less likely to equip than skin cause lots of ppl prefer not to use backpack
            auto Backpack = Backpacks[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, Backpacks.size() - 1)];
            for (size_t j = 0; j < Backpack->CharacterParts.Num(); j++)
            {
                UCustomCharacterPart* Part = Backpack->CharacterParts[j];
                if (Part)
                {
                    bot->PlayerState->CharacterData.Parts[(uintptr_t)Part->CharacterPartType] = Part;
                }
            }
        }
        if (!Gliders.empty()) {
            auto Glider = Gliders[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, Gliders.size() - 1)];
            bot->PC->CosmeticLoadoutBC.Glider = Glider;
        }
        if (!Contrails.empty() && UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.95)) {
            auto Contrail = Contrails[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, Contrails.size() - 1)];
            bot->PC->CosmeticLoadoutBC.SkyDiveContrail = Contrail;
        }
        for (size_t i = 0; i < Dances.size(); i++)
        {
            bot->PC->CosmeticLoadoutBC.Dances.Add(Dances.at(i));
        }

        bot->Pawn->CosmeticLoadout = bot->PC->CosmeticLoadoutBC;
        bot->Pawn->OnRep_CosmeticLoadout();

        if (Pickaxes.empty()) {
            Log("Pickaxes array is empty!");
            UFortWeaponMeleeItemDefinition* PickDef = StaticLoadObject<UFortWeaponMeleeItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
            if (PickDef) {
                bot->GiveItemBot(PickDef);
                auto Entry = bot->GetEntry(PickDef);
                bot->Pawn->EquipWeaponDefinition((UFortWeaponMeleeItemDefinition*)Entry->ItemDefinition, Entry->ItemGuid);
            }
            else {
                Log("Default Pickaxe dont exist!");
            }
        }
        else {
            auto PickDef = Pickaxes[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, Pickaxes.size() - 1)];
            if (!PickDef)
            {
                Log("Cooked!");
                UFortWeaponMeleeItemDefinition* PickDef = StaticLoadObject<UFortWeaponMeleeItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
                if (PickDef) {
                    bot->GiveItemBot(PickDef);
                    auto Entry = bot->GetEntry(PickDef);
                    bot->Pawn->EquipWeaponDefinition((UFortWeaponMeleeItemDefinition*)Entry->ItemDefinition, Entry->ItemGuid);
                }
                else {
                    Log("Default Pickaxe dont exist!");
                }
            }
            else {
                if (PickDef && PickDef->WeaponDefinition)
                {
                    bot->GiveItemBot(PickDef->WeaponDefinition);
                }

                auto Entry = bot->GetEntry(PickDef->WeaponDefinition);
                bot->Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Entry->ItemDefinition, Entry->ItemGuid);
            }
        }

        if (BotDisplayNames.size() != 0) {
            std::srand(static_cast<unsigned int>(std::time(0)));
            int randomIndex = std::rand() % BotDisplayNames.size();
            std::string rdName = BotDisplayNames[randomIndex];
            BotDisplayNames.erase(BotDisplayNames.begin() + randomIndex);

            int size_needed = MultiByteToWideChar(CP_UTF8, 0, rdName.c_str(), (int)rdName.size(), NULL, 0);
            std::wstring wideString(size_needed, 0);
            MultiByteToWideChar(CP_UTF8, 0, rdName.c_str(), (int)rdName.size(), &wideString[0], size_needed);


            FString CVName = FString(wideString.c_str());
            GameMode->ChangeName(bot->PC, CVName, true);
            bot->DisplayName = CVName;

            bot->PlayerState->OnRep_PlayerName();
        }

        for (auto SkillSet : bot->PC->DigestedBotSkillSets)
        {
            if (!SkillSet)
                continue;

            if (auto AimingSkill = Cast<UFortAthenaAIBotAimingDigestedSkillSet>(SkillSet))
                bot->PC->CacheAimingDigestedSkillSet = AimingSkill;

            if (auto AttackingSkill = Cast<UFortAthenaAIBotAttackingDigestedSkillSet>(SkillSet))
                bot->PC->CacheAttackingSkillSet = AttackingSkill;

            if (auto HarvestSkill = Cast<UFortAthenaAIBotHarvestDigestedSkillSet>(SkillSet))
                bot->PC->CacheHarvestDigestedSkillSet = HarvestSkill;

            if (auto InventorySkill = Cast<UFortAthenaAIBotInventoryDigestedSkillSet>(SkillSet))
                bot->PC->CacheInventoryDigestedSkillSet = InventorySkill;

            if (auto LootingSkill = Cast<UFortAthenaAIBotLootingDigestedSkillSet>(SkillSet))
                bot->PC->CacheLootingSkillSet = LootingSkill;

            if (auto MovementSkill = Cast<UFortAthenaAIBotMovementDigestedSkillSet>(SkillSet))
                bot->PC->CacheMovementSkillSet = MovementSkill;

            if (auto PerceptionSkill = Cast<UFortAthenaAIBotPerceptionDigestedSkillSet>(SkillSet))
                bot->PC->CachePerceptionDigestedSkillSet = PerceptionSkill;

            if (auto PlayStyleSkill = Cast<UFortAthenaAIBotPlayStyleDigestedSkillSet>(SkillSet))
                bot->PC->CachePlayStyleSkillSet = PlayStyleSkill;
        }

        bot->PC->BehaviorTree = BehaviorTree;
        bot->PC->RunBehaviorTree(BehaviorTree);
        bot->PC->UseBlackboard(BehaviorTree->BlackboardAsset, &bot->PC->Blackboard);
        bot->PC->UseBlackboard(BehaviorTree->BlackboardAsset, &bot->PC->Blackboard1);

        static auto Name1 = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Global_GamePhaseStep"));
        static auto Name2 = UKismetStringLibrary::Conv_StringToName(TEXT("AIEvaluator_Global_GamePhase"));
        bot->PC->Blackboard->SetValueAsEnum(Name1, (uint8)EAthenaGamePhaseStep::Warmup);
        bot->PC->Blackboard->SetValueAsEnum(Name2, (uint8)EAthenaGamePhase::Warmup);

        bot->PC->PathFollowingComponent->MyNavData = ((UAthenaNavSystem*)UWorld::GetWorld()->NavigationSystem)->MainNavData;
        bot->PC->PathFollowingComponent->OnNavDataRegistered(((UAthenaNavSystem*)UWorld::GetWorld()->NavigationSystem)->MainNavData);
        if (((UAthenaNavSystem*)UWorld::GetWorld()->NavigationSystem)->MainNavData) {
            //Log("NavData!");
        }
        else {
            Log("No NavData!");
        }

        bot->PlayerState->OnRep_CharacterData();

        bot->Pawn->CapsuleComponent->SetGenerateOverlapEvents(true);
        bot->Pawn->CharacterMovement->bCanWalkOffLedges = true;

        bot->Pawn->SetMaxHealth(100);
        bot->Pawn->SetHealth(100);
        bot->Pawn->SetMaxShield(100);
        bot->Pawn->SetShield(0);

        bot->BotState = StartingState;

        if (TeamPC) {
            AFortPlayerStateAthena* TeamPS = (AFortPlayerStateAthena*)TeamPC->PlayerState;

            uint8 OldTeamIdx = bot->PlayerState->TeamIndex;

            bot->PlayerState->bIsABot = false;

            bot->PlayerState->TeamIndex = TeamPS->TeamIndex;
            bot->PlayerState->SquadId = TeamPS->SquadId;
            bot->PlayerState->OnRep_TeamIndex(OldTeamIdx);
            bot->PlayerState->OnRep_SquadId();

            TeamPS->PlayerTeam->TeamMembers.Add(bot->PC);
            bot->PlayerState->PlayerTeam = TeamPS->PlayerTeam;

            bot->PlayerState->OnRep_PlayerTeam();
            TeamPS->OnRep_PlayerTeam();

            bot->PlayerState->bIsABot = true;
        }

        
        // OGSM - Initialize bot personality with variations
        bot->InitializeBotPersonality();
        bot->UpdateMaterialCounts();
        PlayerBotArray.push_back(bot); // gotta do this so we can tick them all
        //Log("Bot Spawned With DisplayName: " + bot->DisplayName.ToString());
    }

    void Tick() {
        if (!PlayerBotArray.empty()) {
            if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.001f))
            {
                FreeDeadBots();
            }
        }
        else {
            return;
        }

        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
        auto Math = (UKismetMathLibrary*)UKismetMathLibrary::StaticClass()->DefaultObject;
        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        auto Statics = (UGameplayStatics*)UGameplayStatics::StaticClass()->DefaultObject;

        for (auto bot : PlayerBotArray) {
            if (!bot || !bot->Pawn || !bot->PC || !bot->PlayerState)
                continue;

            if (GameState->GamePhase <= EAthenaGamePhase::Warmup) {
                if (bot->tick_counter <= 150) {
                    bot->tick_counter++;
                    continue;
                }
            }

            if (bot->BotState > EBotState::Bus) {
                BotsBTService_AIEvaluator Evaluator;
                Evaluator.Tick(bot); // tick the evaluator after the bot is out of the bus so we dont mess up anything or cause potential crash
            }
            
            // OGSM - Update damage status and material counts
            bot->UpdateDamageStatus();
            if (bot->tick_counter % 120 == 0) {
                bot->UpdateMaterialCounts();
            }

            if (bot->BotState == EBotState::Warmup) {
                BotsBTService_Warmup Warmup;
                Warmup.Tick(bot);
            }
            else if (bot->BotState == EBotState::PreBus) {
                bot->Pawn->SetHealth(100);
                bot->Pawn->SetShield(100);
                if (!bot->bHasThankedBusDriver && UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.0005f))
                {
                    bot->bHasThankedBusDriver = true;
                    bot->PC->ThankBusDriver();
                }
            }
            else if (bot->BotState == EBotState::Bus || bot->BotState == EBotState::Skydiving || bot->BotState == EBotState::Gliding) {
                if (Globals::LateGame && UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.5f)) {
                    if (bot->bHasJumpedFromBus) {
                        bot->BotState = EBotState::LookingForPlayers;
                        return;
                    }

                    bot->Pawn->K2_TeleportTo(GameState->GetAircraft(0)->K2_GetActorLocation(), {});
                    bot->Pawn->BeginSkydiving(true);
                    bot->GiveLategameLoadout();
                    bot->BotState = EBotState::LookingForPlayers;

                    bot->bHasJumpedFromBus = true;

                    return;
                }
                
                BotsBTService_AIDropZone DropZoneEv;
                DropZoneEv.Tick(bot);
            }
            else if (bot->BotState == EBotState::Landed) {
                FVector BotLoc = bot->Pawn->K2_GetActorLocation();
                if (bot->NearestPlayerActor) {
                    float Dist = Math->Vector_Distance(BotLoc, bot->NearestPlayerActor->K2_GetActorLocation());
                    if (Dist < 6000.f) {
                        // When the bot lands the bot will have no guns so we need to get away before dying
                        //bot->BotState = EBotState::Fleeing; // Fleeing is kinda cooked atm

                        bot->BotState = EBotState::Looting;
                    }
                    else {
                        bot->BotState = EBotState::Looting;
                    }
                }
                else {
                    bot->BotState = EBotState::Looting;
                }
            }
            else if (bot->BotState == EBotState::Fleeing) {
                FVector BotLoc = bot->Pawn->K2_GetActorLocation();
                FVector Nearest = bot->FindNearestPlayerOrBot();
                if (!Nearest.IsZero()) {
                    float Dist = Math->Vector_Distance(BotLoc, Nearest);
                    if (bot->HasGun()) {
                        bot->BotState = EBotState::LookingForPlayers;
                        continue;
                    }

                    if (Dist < 3000.f) {
                        auto TestRot = Math->FindLookAtRotation(Nearest, BotLoc);

                        bot->PC->SetControlRotation(TestRot);
                        bot->PC->K2_SetActorRotation(TestRot, true);
                        bot->Pawn->AddMovementInput(bot->Pawn->GetActorForwardVector(), 1.2f, true);
                    }
                    else {
                        bot->BotState = EBotState::Looting;
                    }
                }
                else {
                    bot->BotState = EBotState::Looting;
                }
            }
            else if (bot->BotState == EBotState::Looting) {
                // OGSM - Check if still needs to loot
                if (!bot->HasGun() || !bot->TargetLootable) {
                    BotsBTService_Loot LootAI;
                    LootAI.Tick(bot);
                } else {
                    bot->PostLootBehavior();
                }
            }
            else if (bot->BotState == EBotState::Looting) {
                // OGSM - Check if still needs to loot
                if (!bot->HasGun() || !bot->TargetLootable) {
                    BotsBTService_Loot LootAI;
                    LootAI.Tick(bot);
                } else {
                    bot->PostLootBehavior();
                }
            }
            else if (bot->BotState == EBotState::Looting) {
                // OGSM - Check if still needs to loot
                if (!bot->HasGun() || !bot->TargetLootable) {
                    BotsBTService_Loot LootAI;
                    LootAI.Tick(bot);
                } else {
                    bot->PostLootBehavior();
                }
            }
            else if (bot->BotState == EBotState::Looting) {
                // OGSM - Check if still needs to loot
                if (!bot->HasGun() || !bot->TargetLootable) {
                    BotsBTService_Loot LootAI;
                    LootAI.Tick(bot);
                } else {
                    bot->PostLootBehavior();
                }
            }
            else if (bot->BotState == EBotState::LookingForPlayers) {
                if (!bot->HasGun()) {
                    bot->BotState = EBotState::Looting;
                }
                if (bot->IsPickaxeEquiped()) {
                    bot->SimpleSwitchToWeapon();
                }

                // OGSM - Update vault system
                if (Globals::bVaultSystemEnabled) {
                    BotVaultSystem::UpdateVaultState(bot);
                }

                // OGSM - Update vehicle system
                if (Globals::bBotVehicleEnabled) {
                    BotDriving::UpdateVehicleState(bot);
                }

                // OGSM - Update building system
                if (Globals::bBotBuildingEnabled) {
                    BotBuilding::UpdateBuildState(bot);
                }
                
                // OGSM - Farm materials if needed
                if (bot->bNeedsFarming) {
                    bot->FarmTick();
                    continue;
                }

                FVector BotLoc = bot->Pawn->K2_GetActorLocation();
                if (bot->NearestPlayerActor) {
                    FVector Nearest = bot->NearestPlayerActor->K2_GetActorLocation();

                    FRotator TestRot;
                    FVector TargetPosmod = Nearest;

                    if (!Nearest.IsZero()) {
                        float Dist = Math->Vector_Distance(BotLoc, Nearest);

                        // OGSM - Weapon switch by distance
                        if (bot->tick_counter % 60 == 0) {
                            bot->WeaponSwitchByDistance(Dist);
                        }

                        // OGSM - Crouch spam during combat
                        if (Dist < 800.0f && bot->tick_counter % 20 == 0) {
                            bot->CrouchSpam();
                        }

                        if (bot->PC->LineOfSightTo(bot->NearestPlayerActor, BotLoc, true)) {
                            if (true) {
                                float RandomXmod = Math->RandomFloatInRange(-180, 180);
                                float RandomYmod = Math->RandomFloatInRange(-180, 180);
                                float RandomZmod = Math->RandomFloatInRange(-180, 180);

                                FVector TargetPosMod{ Nearest.X + RandomXmod, Nearest.Y + RandomYmod, Nearest.Z + RandomZmod };

                                FRotator Rot = Math->FindLookAtRotation(BotLoc, TargetPosMod);

                                bot->PC->SetControlRotation(Rot);
                                bot->PC->K2_SetActorRotation(Rot, true);

                                //bot->PC->K2_SetFocalPoint(TargetPosMod);
                            }

                            if (UKismetMathLibrary::GetDefaultObj()->RandomBoolWithWeight(0.001)) {
                                TestRot = Math->FindLookAtRotation(BotLoc, Nearest);

                                bot->PC->SetControlRotation(TestRot);
                                bot->PC->K2_SetActorRotation(TestRot, true);
                            }

                            if (!bot->Pawn->bIsCrouched && Math->RandomBoolWithWeight(0.025f)) {
                                bot->Pawn->Crouch(false);
                            }

                            bot->ForceStrafe(true);

                            if (Dist < 1000) {
                                FVector BackVector = bot->Pawn->GetActorForwardVector() * -1.0f;
                                bot->Pawn->AddMovementInput(BackVector, 1.1f, true);
                            }

                            if (!bot->bIsStressed) {
                                bot->PC->MoveToActor(bot->NearestPlayerActor, Math->RandomFloatInRange(400, 1500), true, false, true, nullptr, true);
                            }
                            else {
                                // Somewhat retreating
                                bot->Pawn->AddMovementInput((bot->Pawn->GetActorForwardVector() * -1.0f), 1.2f, true);
                            }

                            //bot->PC->StopMovement();
                            if (bot->PC->LineOfSightTo(bot->NearestPlayerActor, BotLoc, true)) {
                                bot->Pawn->PawnStartFire(0);
                            }
                            else {
                                bot->Pawn->PawnStopFire(0);
                            }
                        }
                        else {
                            bot->BotState = EBotState::MovingToSafeZone;
                        }
                    }
                    else {}
                }
            }
            else if (bot->BotState == EBotState::MovingToSafeZone) {
                FVector BotLoc = bot->Pawn->K2_GetActorLocation();
                if (bot->NearestPlayerActor) {
                    float Dist = Math->Vector_Distance(BotLoc, bot->NearestPlayerActor->K2_GetActorLocation());

                    if (Dist < 4000.f) {
                        if (!bot->HasGun()) {
                            //bot->BotState = EBotState::Fleeing;
                            bot->BotState = EBotState::Looting;
                        }
                        else {
                            bot->BotState = EBotState::LookingForPlayers;
                        }
                    }
                }
                
                if (GameState && GameState->SafeZoneIndicator)
                {
                    if (Math->RandomBoolWithWeight(0.025f)) {
                        bot->ForceStrafe(true);
                    }
                    bot->PC->MoveToLocation(GameState->SafeZoneIndicator->NextCenter, GameState->SafeZoneIndicator->Radius, true, false, false, true, nullptr, true);
                }
            }
            else if (bot->BotState == EBotState::Stuck) {
                if (!bot->IsPickaxeEquiped()) {
                    bot->EquipPickaxe();
                }
                bot->Pawn->PawnStartFire(0);

                if (!bot->Pawn->bIsCrouched && Math->RandomBoolWithWeight(0.025f)) {
                    bot->Pawn->Crouch(false);
                }

                if (Math->RandomBoolWithWeight(0.05f)) {
                    bot->Pawn->UnCrouch(false);
                    bot->Pawn->Jump();
                }

                if (bot->StuckBuildingActor) {
                    if (bot->StuckBuildingActor->GetHealth() <= 1) {
                        bot->Pawn->PawnStopFire(0);
                        bot->StuckBuildingActor = nullptr;
                        bot->BotState = bot->CachedBotState;
                    }
                    else {
                        if (!bot->IsPickaxeEquiped()) {
                            bot->EquipPickaxe();
                        }
                        bot->LookAt(bot->StuckBuildingActor);
                        bot->Pawn->PawnStartFire(0);
                    }
                }
                else {
                    bot->ForceStrafe(true);
                    bot->BotState = bot->CachedBotState;
                }

                bot->Pawn->PawnStopFire(0);
            }
            
            bot->tick_counter++;
        }
    }
}