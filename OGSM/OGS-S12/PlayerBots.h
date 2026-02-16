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
    Warmup,             // Lobby
    PreBus,
    Bus,
    Skydiving,          // Plongee directe
    Gliding,
    Landed,
    Fleeing,
    Looting,
    LookingForPlayers,
    MovingToSafeZone,
    Stuck,
    BuildFighting,      // Combat avec builds
    TakingHighGround,   // Construction vers le haut
    DrivingVehicle,     // Conduite Choppa/Bateau
    FightingBoss,       // Combat contre Midas/etc
    OpeningVault,       // Ouverture vault
    FarmingMaterials,   // Farming
    Healing,            // Soin
    Retreating          // Retraite strategique
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

enum class EBotBuildStrategy {
    None,
    NinetyUp,
    BoxFight,
    RampRush,
    Turtle,
    HighGroundRetake,
    DefensiveWall
};

enum class EVehicleType {
    None,
    Choppa,
    Boat
};

enum class EPOIType {
    None,
    Boss,
    Medium,
    Small
};

enum class ELobbyBotState {
    Idle,
    Moving,
    Emoting,
    PickingUpWeapon,
    Shooting
};

enum class ELootableType {
    None = -1,
    Chest = 0,
    Pickup = 1
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
    // ============================================================================
    // SYSTEME DE COMBAT AVANCE (utilisant UFortAthenaAIBotAttackingSkillSet)
    // ============================================================================
    float AggressionLevel = 0.5f;           // 0.0 - 1.0
    float ReactionTime = 0.2f;              // Temps de reaction
    float AimAccuracy = 0.85f;              // Precision de visee
    bool bMakesMistakes = true;             // Fait des erreurs comme humain
    float MistakeChance = 0.1f;             // 10% erreurs
    int32 ShotsFiredWithoutBuilding = 0;    // Compteur pour build fights
    float LastCombatTime = 0.f;
    AActor* CurrentCombatTarget = nullptr;
    bool bWantsHighGround = false;

    // ============================================================================
    // SYSTEME DE BUILDING (utilisant UFortAthenaAIBotBuildingSkillSet)
    // ============================================================================
    EBotBuildState BuildState = EBotBuildState::Idle;
    bool bIsBuilding = false;
    int32 WoodCount = 0;
    int32 StoneCount = 0;
    int32 MetalCount = 0;
    float LastBuildTime = 0.f;
    int32 BuildMaterialCount = 0;
    FVector BuildTargetLocation = FVector();
    EBotBuildStrategy CurrentBuildStrategy = EBotBuildStrategy::None;
    int32 BuildSequenceStep = 0;
    float BuildSequenceStartTime = 0.f;

    // ============================================================================
    // SYSTEME DE VEHICULES (Choppa + Bateaux)
    // ============================================================================
    AActor* CurrentVehicle = nullptr;
    bool bIsInVehicle = false;
    float VehicleEnterTime = 0.f;
    EVehicleType CurrentVehicleType = EVehicleType::None;
    EBotVehicleState VehicleState = EBotVehicleState::NoVehicle;
    FVector VehicleTargetLocation = FVector();

    // ============================================================================
    // SYSTEME DE VAULT (Boss + Cartes + Coffres mythiques)
    // ============================================================================
    EVaultState VaultState = EVaultState::None;
    EBossType TargetBoss = EBossType::None;
    bool bHasKeycard = false;
    UFortItemDefinition* CurrentKeycard = nullptr;
    bool bWantsToOpenVault = false;
    FBossPOI* TargetVault = nullptr;
    bool bIsFightingBoss = false;
    AActor* CurrentBossTarget = nullptr;
    float BossFightStartTime = 0.f;

    // ============================================================================
    // SYSTEME D'ATTERRISSAGE AVANCE
    // ============================================================================
    bool bShouldDiveDirect = true;
    float DiveStartTime = 0.f;
    bool bWantsRoofChest = false;
    FVector TargetRoofLocation = FVector();
    EPOIType AssignedPOIType = EPOIType::None;
    FString AssignedPOIName;

    // ============================================================================
    // SYSTEME DE DEFENSE REACTIVE
    // ============================================================================
    float LastDamageTime = 0.f;
    FVector LastDamageDirection = FVector();
    bool bIsUnderFire = false;
    int32 ConsecutiveShotsTaken = 0;
    float PanicBuildTime = 0.f;

    // ============================================================================
    // SYSTEME DE FARMING
    // ============================================================================
    bool bNeedsFarming = false;
    AActor* FarmingTarget = nullptr;
    float LastFarmTime = 0.f;
    float FarmingStartTime = 0.f;
    int32 FarmingTargetAmount = 300;

    // ============================================================================
    // SYSTEME DE LOBBY
    // ============================================================================
    ELobbyBotState LobbyState = ELobbyBotState::Idle;
    float LobbyActionTimer = 0.f;
    AFortPickup* TargetWeaponPickup = nullptr;
    float LobbyIdleTime = 0.f;

    // ============================================================================
    // METHODES DE BUILDING
    // ============================================================================
    inline bool HasEnoughResources(int32 Amount) {
        return (WoodCount + StoneCount + MetalCount) >= Amount;
    }

    inline void UpdateMaterialCounts() {
        if (!PC || !PC->Inventory) return;
        WoodCount = StoneCount = MetalCount = 0;
        for (int32 i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
            if (!Entry.ItemDefinition) continue;
            std::string Name = Entry.ItemDefinition->Name.ToString();
            if (Name.contains("Wood")) WoodCount = Entry.Count;
            else if (Name.contains("Stone")) StoneCount = Entry.Count;
            else if (Name.contains("Metal")) MetalCount = Entry.Count;
        }
        bNeedsFarming = (WoodCount + StoneCount + MetalCount) < 200;
    }

    inline void Perform90s() {
        if (!Pawn || !HasEnoughResources(30)) return;
        CurrentBuildStrategy = EBotBuildStrategy::NinetyUp;
        BuildSequenceStep = 0;
        BuildSequenceStartTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
    }

    inline void BuildBox() {
        if (!Pawn || !HasEnoughResources(50)) return;
        CurrentBuildStrategy = EBotBuildStrategy::BoxFight;
        BuildSequenceStep = 0;
    }

    inline void BuildRampRush() {
        if (!Pawn || !HasEnoughResources(40)) return;
        CurrentBuildStrategy = EBotBuildStrategy::RampRush;
        BuildSequenceStep = 0;
    }

    inline void BuildDefensiveWall(FVector ThreatDir) {
        if (!Pawn || !HasEnoughResources(10)) return;
        CurrentBuildStrategy = EBotBuildStrategy::DefensiveWall;
        LastDamageDirection = ThreatDir;
    }

    inline bool ShouldTakeHighGround(AActor* Enemy) {
        if (!Pawn || !Enemy) return false;
        FVector BotLoc = Pawn->K2_GetActorLocation();
        FVector EnemyLoc = Enemy->K2_GetActorLocation();
        return (EnemyLoc.Z - BotLoc.Z > 150.0f) && HasEnoughResources(80);
    }

    inline bool ShouldBoxUp() {
        return bIsStressed && HasEnoughResources(50);
    }

    // ============================================================================
    // METHODES DE COMBAT
    // ============================================================================
    inline void FireWithHumanImperfection(AActor* Target) {
        if (!Pawn || !Target || !PC) return;

        FVector TargetLoc = Target->K2_GetActorLocation();
        float ErrorRange = (1.0f - AimAccuracy) * 150.0f;
        TargetLoc.X += (rand() % (int)(ErrorRange * 2)) - ErrorRange;
        TargetLoc.Y += (rand() % (int)(ErrorRange * 2)) - ErrorRange;
        TargetLoc.Z += (rand() % (int)(ErrorRange * 2)) - ErrorRange;

        FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(Pawn->K2_GetActorLocation(), TargetLoc);
        PC->SetControlRotation(AimRot);

        if (bMakesMistakes && (rand() % 100) < (int)(MistakeChance * 100)) return;

        Pawn->PawnStartFire(0);
    }

    inline void SwitchToBestWeaponForDistance(float Distance) {
        WeaponSwitchByDistance(Distance);
    }

    inline void CrouchSpam() {
        if (!Pawn) return;
        static float LastCrouch = 0;
        float Now = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
        if (Now - LastCrouch > 0.25f) {
            Pawn->bIsCrouched ? Pawn->UnCrouch(false) : Pawn->Crouch(false);
            LastCrouch = Now;
        }
    }

    // ============================================================================
    // METHODES DE VEHICULE
    // ============================================================================
    inline bool ShouldUseVehicle(FVector Target) {
        if (!Pawn || bIsInVehicle) return false;
        return Target.DistanceTo(Pawn->K2_GetActorLocation()) > 2000.0f;
    }

    inline void EnterVehicle(AActor* Vehicle) {
        if (!Pawn || !Vehicle) return;
        CurrentVehicle = Vehicle;
        bIsInVehicle = true;
        VehicleEnterTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
    }

    inline void DriveToLocationVehicle(FVector Target) {
        if (!Pawn || !bIsInVehicle) return;
        VehicleTargetLocation = Target;
    }

    // ============================================================================
    // METHODES DE DEFENSE REACTIVE
    // ============================================================================
    inline void OnTakeDamage(float Damage, AActor* DamageCauser, FVector HitDir) {
        (void)Damage;
        (void)DamageCauser;
        LastDamageTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
        LastDamageDirection = HitDir;
        bIsUnderFire = true;
        ConsecutiveShotsTaken++;

        if (HasEnoughResources(10)) BuildDefensiveWall(HitDir);
        if (ConsecutiveShotsTaken >= 3 && HasEnoughResources(50)) BuildBox();

        if (BotState == EBotState::LookingForPlayers) {
            CachedBotState = BotState;
            BotState = EBotState::BuildFighting;
        }
    }

    // ============================================================================
    // METHODES DE FARMING
    // ============================================================================
    inline AActor* FindFarmableObject() {
        if (!Pawn) return nullptr;
        return nullptr;
    }

    inline void FarmTick() {
        if (!bNeedsFarming || !Pawn) return;
    }

    // ============================================================================
    // METHODES DE LOBBY
    // ============================================================================
    inline void UpdateLobbyBehavior() {
        if (BotState != EBotState::Warmup) return;

        float Now = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());

        switch (LobbyState) {
            case ELobbyBotState::Idle:
                if (rand() % 100 < 30) {
                    FVector Target = Pawn->K2_GetActorLocation();
                    Target.X += (rand() % 2000) - 1000;
                    Target.Y += (rand() % 2000) - 1000;
                    PC->MoveToLocation(Target, 50.f, true, false, false, true, nullptr, true);
                    LobbyState = ELobbyBotState::Moving;
                } else if (rand() % 100 < 20) {
                    Emote();
                    LobbyState = ELobbyBotState::Emoting;
                    LobbyActionTimer = Now + 3.f;
                } else if (rand() % 100 < 25) {
                    TargetWeaponPickup = FindNearestWeaponPickup();
                    if (TargetWeaponPickup) {
                        PC->MoveToActor(TargetWeaponPickup, 100.f, true, false, true, nullptr, true);
                        LobbyState = ELobbyBotState::PickingUpWeapon;
                    }
                }
                break;

            case ELobbyBotState::PickingUpWeapon:
                if (TargetWeaponPickup && Pawn->GetDistanceTo(TargetWeaponPickup) < 300.f) {
                    Pickup(TargetWeaponPickup);
                    SimpleSwitchToWeapon();
                    LobbyState = ELobbyBotState::Shooting;
                    LobbyActionTimer = Now + 5.f;
                }
                break;

            case ELobbyBotState::Shooting:
                if (Now <= LobbyActionTimer) {
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

            case ELobbyBotState::Emoting:
                if (Now > LobbyActionTimer) LobbyState = ELobbyBotState::Idle;
                break;

            case ELobbyBotState::Moving:
                if (PC->PathFollowingComponent && PC->PathFollowingComponent->DidMoveReachGoal()) {
                    LobbyState = ELobbyBotState::Idle;
                }
                break;
        }
    }

    inline AFortPickup* FindNearestWeaponPickup() {
        if (!Pawn) return nullptr;
        static auto PickupClass = AFortPickupAthena::StaticClass();
        TArray<AActor*> Array;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), PickupClass, &Array);

        AFortPickupAthena* Nearest = nullptr;
        for (size_t i = 0; i < Array.Num(); i++) {
            auto Pickup = (AFortPickupAthena*)Array[i];
            if (!Pickup || Pickup->bHidden) continue;
            if (!Pickup->PrimaryPickupItemEntry.ItemDefinition) continue;
            if (!Pickup->PrimaryPickupItemEntry.ItemDefinition->IsA(UFortWeaponItemDefinition::StaticClass())) continue;
            if (!Nearest || Pickup->GetDistanceTo(Pawn) < Nearest->GetDistanceTo(Pawn)) {
                Nearest = Pickup;
            }
        }
        Array.Free();
        return Nearest;
    }

    inline AActor* FindNearestRealPlayer() {
        if (!Pawn) return nullptr;
        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        AActor* Nearest = nullptr;
        float MinDist = FLT_MAX;

        for (auto Player : GameMode->AlivePlayers) {
            if (!Player || !Player->Pawn) continue;
            AFortPlayerStateAthena* PS = (AFortPlayerStateAthena*)Player->PlayerState;
            if (PS && PS->bIsABot) continue;

            float Dist = Pawn->GetDistanceTo(Player->Pawn);
            if (Dist < MinDist) {
                MinDist = Dist;
                Nearest = Player->Pawn;
            }
        }
        return Nearest;
    }

    // ============================================================================
    // METHODES DE BOSS FIGHT
    // ============================================================================
    inline void StartBossFight(AActor* Boss) {
        if (!Boss) return;
        CurrentBossTarget = Boss;
        bIsFightingBoss = true;
        BossFightStartTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
        BotState = EBotState::FightingBoss;
    }

    inline void UpdateBossFight() {
        if (!bIsFightingBoss || !CurrentBossTarget) return;

        if (ShouldTakeHighGround(CurrentBossTarget)) {
            Perform90s();
        }

        if (bIsStressed) BuildBox();

        FireWithHumanImperfection(CurrentBossTarget);
    }

    // ============================================================================
    // INITIALISATION PERSONNALITE
    // ============================================================================
    inline void InitializeBotPersonality() {
        ReactionTime = 0.15f + (rand() % 25) / 100.0f;
        AimAccuracy = 0.65f + (rand() % 35) / 100.0f;
        AggressionLevel = 0.3f + (rand() % 50) / 100.0f;
        MistakeChance = 0.05f + (rand() % 15) / 100.0f;
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

            if (bIsFightingBoss && CurrentBossTarget) {
                Quests::UpdatePlayerQuestProgress(KillerPC, Quests::EPlayerQuestType::EliminateBoss, 1);
            } else {
                Quests::UpdatePlayerQuestProgress(KillerPC, Quests::EPlayerQuestType::Eliminations, 1);
            }

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
    void DetermineBotWarmupChoice(PlayerBot* bot) {
        if (UKismetMathLibrary::GetDefaultObj()->RandomBool()) {
            bot->BotWarmupChoice = EBotWarmupChoice::Emote;
        }
        else {
            bot->BotWarmupChoice = EBotWarmupChoice::MoveToPlayerEmote;
        }
    }

public:
    void Tick(PlayerBot* bot) {
        if (bot->BotWarmupChoice == EBotWarmupChoice::MAX) {
            DetermineBotWarmupChoice(bot);
        }
        else if (bot->BotWarmupChoice == EBotWarmupChoice::Emote) {
            if (bot->tick_counter % 300 == 0) {
                bot->Emote();
            }
        }
        else {
            if (bot->tick_counter % 150 == 0) {
                bot->NearestPlayerActor = bot->GetNearestPlayerActor();
                auto BotPos = bot->Pawn->K2_GetActorLocation();
                if (bot->NearestPlayerActor) {
                    FVector Nearest = bot->NearestPlayerActor->K2_GetActorLocation();
                    if (!Nearest.IsZero()) {
                        float Dist = Math->Vector_Distance(BotPos, Nearest);
                        if (Dist < 200.f + rand() % 300) {
                            bot->LookAt(bot->NearestPlayerActor);
                            if (UKismetMathLibrary::GetDefaultObj()->RandomBool()) {
                                bot->Emote();
                            }
                        }
                        else {
                            bot->LookAt(bot->NearestPlayerActor);
                            bot->PC->MoveToActor(bot->NearestPlayerActor, 100, true, false, true, nullptr, true);
                        }
                    }
                }
            }
        }
    }
};

class BotsBTService_AIDropZone {
public:
    void ChooseDropZone(PlayerBot* bot) {
        if (!bot || !bot->TargetDropZone.IsZero()) return;

        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
        auto SafeZone = GameState->SafeZoneIndicator;

        int32 Choice = rand() % 100;
        FVector Selected;

        if (Choice < 40 && !BossPOIs.empty()) {
            FBossPOI* Best = nullptr;
            int32 MinCount = 999;

            for (auto& POI : BossPOIs) {
                if (!IsLocationInBusPath(POI.Location)) continue;
                int32 Count = CountBotsInPOI(POI.Location, 2000.f);
                if (Count < POI.MaxBots && Count < MinCount) {
                    MinCount = Count;
                    Best = &POI;
                }
            }

            if (Best) {
                Selected = Best->Location;
                bot->AssignedPOIType = EPOIType::Boss;
                bot->AssignedPOIName = Best->Name;
                bot->TargetVault = Best;
                bot->bWantsToOpenVault = true;
            }
        } else if (Choice < 75 && !MediumPOIs.empty()) {
            FMediumPOI* Best = nullptr;
            int32 MinCount = 999;

            for (auto& POI : MediumPOIs) {
                if (!IsLocationInBusPath(POI.Center)) continue;
                if (!IsInFutureSafeZone(POI.Center, SafeZone)) continue;
                int32 Count = CountBotsInPOI(POI.Center, POI.Radius);
                if (Count < POI.MaxBots && Count < MinCount) {
                    MinCount = Count;
                    Best = &POI;
                }
            }

            if (Best) {
                if (Best->bHasRoofChests && !Best->RoofChestLocations.empty()) {
                    Selected = Best->RoofChestLocations[rand() % Best->RoofChestLocations.size()];
                    bot->bWantsRoofChest = true;
                } else {
                    Selected = Best->Center;
                }
                bot->AssignedPOIType = EPOIType::Medium;
                bot->AssignedPOIName = Best->Name;
            }
        } else if (Choice < 95 && !SmallPOIs.empty()) {
            auto& POI = SmallPOIs[rand() % SmallPOIs.size()];
            if (IsInFutureSafeZone(POI.Location, SafeZone)) {
                Selected = POI.Location;
                bot->AssignedPOIType = EPOIType::Small;
                bot->AssignedPOIName = POI.Name;
            }
        } else if (!DropZoneLocations.empty()) {
            Selected = DropZoneLocations[rand() % DropZoneLocations.size()];
        }

        bot->TargetDropZone.X = Selected.X + (rand() % 800) - 400;
        bot->TargetDropZone.Y = Selected.Y + (rand() % 800) - 400;
        bot->TargetDropZone.Z = Selected.Z;
    }

    void Tick(PlayerBot* bot) {
        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
        auto Math = (UKismetMathLibrary*)UKismetMathLibrary::StaticClass()->DefaultObject;

        if (!bot || !bot->Pawn || !bot->PC) return;

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
                bot->Pawn->K2_TeleportTo(DropTarget, {});
                bot->Pawn->BeginSkydiving(true);
                bot->BotState = EBotState::Skydiving;

                bot->bHasJumpedFromBus = true;

                return;
            }

            float DistanceToDrop = Math->Vector_Distance(BusLocation, DropTarget);
            if (DistanceToDrop < bot->ClosestDistToDropZone) {
                bot->ClosestDistToDropZone = DistanceToDrop;
            } else {
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
                FVector Target = bot->TargetDropZone;
                Target.Z = BotPos.Z;

                FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(BotPos, Target);
                bot->PC->SetControlRotation(LookAt);
                bot->PC->K2_SetActorRotation(LookAt, true);

                if (BotPos.Z > 1000.0f) {
                    bot->Pawn->AddMovementInput(FVector(0, 0, -1), 2.0f, true);
                } else {
                    bot->BotState = EBotState::Gliding;
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

        for (auto bot : PlayerBotArray) {
            if (!bot || !bot->Pawn || !bot->PC || !bot->PlayerState)
                continue;

            if (GameState->GamePhase <= EAthenaGamePhase::Warmup) {
                if (bot->tick_counter <= 150) {
                    bot->tick_counter++;
                    continue;
                }
            }

            bot->UpdateMaterialCounts();

            float Now = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
            if (Now - bot->LastDamageTime > 3.f) {
                bot->bIsUnderFire = false;
                bot->ConsecutiveShotsTaken = 0;
            }

            if (bot->BotState == EBotState::Warmup) {
                bot->UpdateLobbyBehavior();
                continue;
            }

            if (bot->BotState == EBotState::LookingForPlayers && bot->NearestPlayerActor) {
                FVector BotLoc = bot->Pawn->K2_GetActorLocation();
                FVector EnemyLoc = bot->NearestPlayerActor->K2_GetActorLocation();
                float Dist = BotLoc.DistanceTo(EnemyLoc);

                if (bot->ShouldTakeHighGround(bot->NearestPlayerActor)) {
                    bot->Perform90s();
                } else if (bot->ShouldBoxUp()) {
                    bot->BuildBox();
                }

                bot->SwitchToBestWeaponForDistance(Dist);

                if (Dist < 1000.f) bot->CrouchSpam();

                bot->FireWithHumanImperfection(bot->NearestPlayerActor);
            }

            if (bot->BotState == EBotState::FightingBoss) {
                bot->UpdateBossFight();
            }

            if (bot->bNeedsFarming && bot->BotState == EBotState::Landed) {
                bot->FarmTick();
            }

            if (bot->ShouldUseVehicle(bot->TargetDropZone)) {
            }

            if (bot->BotState > EBotState::Bus) {
                BotsBTService_AIEvaluator Evaluator;
                Evaluator.Tick(bot); // tick the evaluator after the bot is out of the bus so we dont mess up anything or cause potential crash
            }

            if (bot->BotState == EBotState::PreBus) {
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
                DropZoneEv.ChooseDropZone(bot);
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
                BotsBTService_Loot LootAI;
                LootAI.Tick(bot);
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