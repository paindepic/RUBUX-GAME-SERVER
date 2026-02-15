#pragma once
#include "framework.h"
#include "Building.h"

// Forward declaration
struct PlayerBot;

enum class EBotBuildState : uint8 {
    Idle,
    Building90s,
    BoxFighting,
    RampRush,
    HighGroundRetake,
    Turtling,
    Editing
};

enum class EBuildType : uint8 {
    Wall,
    Floor,
    Stair,
    Roof,
    Cone
};

struct FBuildAction {
    EBuildType Type;
    FRotator Rotation;
    FVector Offset;
    float Delay;
};

namespace BotBuilding {
    inline void Build90s(PlayerBot* bot, int32 Steps = 3);
    inline void BuildBoxFight(PlayerBot* bot, FVector Center);
    inline void BuildRampRush(PlayerBot* bot, int32 Steps = 4);
    inline void BuildTurtle(PlayerBot* bot);
    inline void BuildHighGroundRetake(PlayerBot* bot, AActor* Target);
    inline bool ShouldBuildForHighGround(PlayerBot* bot, AActor* Target);
    inline bool ShouldBoxUp(PlayerBot* bot);
    inline void UpdateBuildState(PlayerBot* bot);

    inline UFortItemDefinition* GetBuildMaterial(AFortPlayerControllerAthena* PC) {
        static UFortItemDefinition* WoodDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");
        static UFortItemDefinition* StoneDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");
        static UFortItemDefinition* MetalDef = StaticLoadObject<UFortItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

        if (!PC || !PC->Inventory) return WoodDef;

        for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
            if (Entry.ItemDefinition == MetalDef && Entry.Count >= 30) return MetalDef;
        }
        for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
            if (Entry.ItemDefinition == StoneDef && Entry.Count >= 30) return StoneDef;
        }
        return WoodDef;
    }

    inline int32 GetMaterialCount(AFortPlayerControllerAthena* PC, UFortItemDefinition* MatDef) {
        if (!PC || !PC->Inventory || !MatDef) return 0;
        for (size_t i = 0; i < PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = PC->Inventory->Inventory.ReplicatedEntries[i];
            if (Entry.ItemDefinition == MatDef) return Entry.Count;
        }
        return 0;
    }

    inline bool HasEnoughMaterials(AFortPlayerControllerAthena* PC, int32 Amount = 10) {
        UFortItemDefinition* Mat = GetBuildMaterial(PC);
        return GetMaterialCount(PC, Mat) >= Amount;
    }

    inline UClass* GetBuildingClass(EBuildType Type) {
        switch (Type) {
        case EBuildType::Wall:
            return StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Wall/Player_Wall1.Player_Wall1_C");
        case EBuildType::Floor:
            return StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Floor/Player_Floor.Player_Floor_C");
        case EBuildType::Stair:
            return StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Stairs/Player_StairW.Player_StairW_C");
        case EBuildType::Roof:
            return StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Roof/Player_RoofS.Player_RoofS_C");
        case EBuildType::Cone:
            return StaticLoadObject<UClass>("/Game/Building/ActorBlueprints/Roof/Player_RoofC.Player_RoofC_C");
        default:
            return nullptr;
        }
    }

    inline bool TryBuild(AFortPlayerControllerAthena* PC, EBuildType Type, FVector Location, FRotator Rotation, bool bMirrored = false) {
        if (!PC || !HasEnoughMaterials(PC, 10)) return false;

        UClass* BuildingClass = GetBuildingClass(Type);
        if (!BuildingClass) return false;

        char a7;
        TArray<AActor*> BuildingsToRemove;

        if (!CantBuild(UWorld::GetWorld(), BuildingClass, Location, Rotation, bMirrored, &BuildingsToRemove, &a7)) {
            auto ResDef = UFortKismetLibrary::GetDefaultObj()->K2_GetResourceItemDefinition(((ABuildingSMActor*)BuildingClass->DefaultObject)->ResourceType);
            Inventory::RemoveItem(PC, ResDef, 10);

            ABuildingSMActor* NewBuilding = SpawnActor<ABuildingSMActor>(Location, Rotation, PC, BuildingClass);
            if (NewBuilding) {
                NewBuilding->bPlayerPlaced = true;
                NewBuilding->InitializeKismetSpawnedBuildingActor(NewBuilding, PC, true);
                NewBuilding->TeamIndex = ((AFortPlayerStateAthena*)PC->PlayerState)->TeamIndex;
                NewBuilding->Team = EFortTeam(NewBuilding->TeamIndex);

                for (size_t i = 0; i < BuildingsToRemove.Num(); i++) {
                    BuildingsToRemove[i]->K2_DestroyActor();
                }
                UKismetArrayLibrary::Array_Clear((TArray<int32>&)BuildingsToRemove);
                return true;
            }
        }
        return false;
    }

    inline void Build90s(PlayerBot* bot, int32 Steps = 3) {
        if (!bot || !bot->Pawn || !bot->PC) return;
        if (!HasEnoughMaterials(bot->PC, Steps * 20)) return;

        FVector BotLoc = bot->Pawn->K2_GetActorLocation();
        FVector Forward = bot->Pawn->GetActorForwardVector();
        FVector Right = bot->Pawn->GetActorRightVector();

        for (int32 i = 0; i < Steps; i++) {
            FVector StairLoc = BotLoc + (Forward * (150.0f + i * 50.0f)) + FVector(0, 0, i * 150.0f);
            FRotator StairRot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector(0, 0, 1));

            if (TryBuild(bot->PC, EBuildType::Stair, StairLoc, StairRot)) {
                FVector WallLoc = StairLoc + (Forward * 100.0f);
                FRotator WallRot = UKismetMathLibrary::MakeRotFromXZ(-Forward, FVector(0, 0, 1));
                TryBuild(bot->PC, EBuildType::Wall, WallLoc, WallRot);

                FVector SideWallLoc1 = StairLoc + (Right * 100.0f);
                FRotator SideWallRot1 = UKismetMathLibrary::MakeRotFromXZ(-Right, FVector(0, 0, 1));
                TryBuild(bot->PC, EBuildType::Wall, SideWallLoc1, SideWallRot1);

                FVector SideWallLoc2 = StairLoc - (Right * 100.0f);
                FRotator SideWallRot2 = UKismetMathLibrary::MakeRotFromXZ(Right, FVector(0, 0, 1));
                TryBuild(bot->PC, EBuildType::Wall, SideWallLoc2, SideWallRot2);
            }
        }
    }

    inline void BuildBoxFight(PlayerBot* bot, FVector Center) {
        if (!bot || !bot->Pawn || !bot->PC) return;
        if (!HasEnoughMaterials(bot->PC, 50)) return;

        FVector Forward = bot->Pawn->GetActorForwardVector();
        FVector Right = bot->Pawn->GetActorRightVector();

        for (int32 i = -1; i <= 1; i++) {
            for (int32 j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                FVector WallLoc = Center + (Forward * i * 150.0f) + (Right * j * 150.0f);
                FRotator WallRot = UKismetMathLibrary::FindLookAtRotation(WallLoc, Center);
                TryBuild(bot->PC, EBuildType::Wall, WallLoc, WallRot);
            }
        }

        FVector FloorLoc = Center - FVector(0, 0, 100.0f);
        TryBuild(bot->PC, EBuildType::Floor, FloorLoc, FRotator(0, 0, 0));

        FVector RoofLoc = Center + FVector(0, 0, 200.0f);
        TryBuild(bot->PC, EBuildType::Roof, RoofLoc, FRotator(0, 0, 0));
    }

    inline void BuildRampRush(PlayerBot* bot, int32 Steps = 4) {
        if (!bot || !bot->Pawn || !bot->PC) return;
        if (!HasEnoughMaterials(bot->PC, Steps * 30)) return;

        FVector BotLoc = bot->Pawn->K2_GetActorLocation();
        FVector Forward = bot->Pawn->GetActorForwardVector();

        for (int32 i = 0; i < Steps; i++) {
            FVector StairLoc = BotLoc + (Forward * (150.0f + i * 50.0f)) + FVector(0, 0, i * 150.0f);
            FRotator StairRot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector(0, 0, 1));

            TryBuild(bot->PC, EBuildType::Stair, StairLoc, StairRot);

            FVector WallLoc1 = StairLoc + (Forward * 100.0f);
            FRotator WallRot1 = UKismetMathLibrary::MakeRotFromXZ(-Forward, FVector(0, 0, 1));
            TryBuild(bot->PC, EBuildType::Wall, WallLoc1, WallRot1);

            FVector WallLoc2 = StairLoc + (bot->Pawn->GetActorRightVector() * 100.0f);
            FRotator WallRot2 = UKismetMathLibrary::MakeRotFromXZ(-bot->Pawn->GetActorRightVector(), FVector(0, 0, 1));
            TryBuild(bot->PC, EBuildType::Wall, WallLoc2, WallRot2);
        }
    }

    inline void BuildTurtle(PlayerBot* bot) {
        if (!bot || !bot->Pawn || !bot->PC) return;
        if (!HasEnoughMaterials(bot->PC, 40)) return;

        FVector Center = bot->Pawn->K2_GetActorLocation();
        FVector Forward = bot->Pawn->GetActorForwardVector();
        FVector Right = bot->Pawn->GetActorRightVector();

        FVector FloorLoc = Center - FVector(0, 0, 50.0f);
        TryBuild(bot->PC, EBuildType::Floor, FloorLoc, FRotator(0, 0, 0));

        for (int32 i = 0; i < 4; i++) {
            FVector WallLoc = Center + (Forward * 150.0f);
            FRotator WallRot = UKismetMathLibrary::MakeRotFromXZ(-Forward, FVector(0, 0, 1));
            TryBuild(bot->PC, EBuildType::Wall, WallLoc, WallRot);

            Forward = Forward.RotateAngleAxis(90.0f, FVector(0, 0, 1));
        }

        FVector RoofLoc = Center + FVector(0, 0, 200.0f);
        TryBuild(bot->PC, EBuildType::Roof, RoofLoc, FRotator(0, 0, 0));
    }

    inline void BuildHighGroundRetake(PlayerBot* bot, AActor* Target) {
        if (!bot || !bot->Pawn || !bot->PC || !Target) return;
        if (!HasEnoughMaterials(bot->PC, 40)) return;

        FVector BotLoc = bot->Pawn->K2_GetActorLocation();
        FVector TargetLoc = Target->K2_GetActorLocation();

        if (TargetLoc.Z > BotLoc.Z + 100.0f) {
            FVector Direction = (TargetLoc - BotLoc).GetNormalized();
            FVector StairLoc = BotLoc + (Direction * 150.0f) + FVector(0, 0, 150.0f);
            FRotator StairRot = UKismetMathLibrary::MakeRotFromXZ(Direction, FVector(0, 0, 1));

            TryBuild(bot->PC, EBuildType::Stair, StairLoc, StairRot);

            FVector WallLoc = StairLoc + (Direction * 100.0f);
            FRotator WallRot = UKismetMathLibrary::MakeRotFromXZ(-Direction, FVector(0, 0, 1));
            TryBuild(bot->PC, EBuildType::Wall, WallLoc, WallRot);

            FVector ConeLoc = StairLoc + FVector(0, 0, 100.0f);
            TryBuild(bot->PC, EBuildType::Cone, ConeLoc, StairRot);
        }
    }

    inline bool ShouldBuildForHighGround(PlayerBot* bot, AActor* Target) {
        if (!bot || !bot->Pawn || !Target) return false;
        if (!HasEnoughMaterials(bot->PC, 30)) return false;

        FVector BotLoc = bot->Pawn->K2_GetActorLocation();
        FVector TargetLoc = Target->K2_GetActorLocation();

        float HeightDiff = TargetLoc.Z - BotLoc.Z;
        float Distance = FVector::Distance(FVector(BotLoc.X, BotLoc.Y, 0), FVector(TargetLoc.X, TargetLoc.Y, 0));

        return HeightDiff > 150.0f && Distance < 1500.0f;
    }

    inline bool ShouldBoxUp(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return false;
        if (!HasEnoughMaterials(bot->PC, 40)) return false;

        return bot->bIsStressed || bot->Pawn->GetHealth() <= 50;
    }

    inline void UpdateBuildState(PlayerBot* bot) {
        if (!bot || !bot->PC || !bot->Pawn) return;

        static std::map<PlayerBot*, EBotBuildState> BuildStates;
        static std::map<PlayerBot*, float> BuildCooldowns;

        float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());

        if (BuildCooldowns.find(bot) == BuildCooldowns.end()) {
            BuildCooldowns[bot] = 0.0f;
        }

        if (CurrentTime < BuildCooldowns[bot]) return;

        if (!Globals::bBotBuildingEnabled) return;

        if (bot->BotState != EBotState::LookingForPlayers) {
            BuildStates[bot] = EBotBuildState::Idle;
            return;
        }

        if (!bot->NearestPlayerActor) {
            BuildStates[bot] = EBotBuildState::Idle;
            return;
        }

        float Distance = bot->Pawn->GetDistanceTo(bot->NearestPlayerActor);
        if (Distance > 2000.0f) {
            BuildStates[bot] = EBotBuildState::Idle;
            return;
        }

        EBotBuildState CurrentState = BuildStates[bot];

        switch (CurrentState) {
        case EBotBuildState::Idle:
            if (ShouldBoxUp(bot)) {
                BuildTurtle(bot);
                BuildStates[bot] = EBotBuildState::Turtling;
                BuildCooldowns[bot] = CurrentTime + 3.0f;
            }
            else if (ShouldBuildForHighGround(bot, bot->NearestPlayerActor)) {
                BuildHighGroundRetake(bot, bot->NearestPlayerActor);
                BuildStates[bot] = EBotBuildState::HighGroundRetake;
                BuildCooldowns[bot] = CurrentTime + 2.0f;
            }
            else if (Distance < 1000.0f && UKismetMathLibrary::RandomBoolWithWeight(0.3f)) {
                Build90s(bot, 2);
                BuildStates[bot] = EBotBuildState::Building90s;
                BuildCooldowns[bot] = CurrentTime + 2.5f;
            }
            else if (Distance < 800.0f && UKismetMathLibrary::RandomBoolWithWeight(0.4f)) {
                BuildBoxFight(bot, bot->Pawn->K2_GetActorLocation());
                BuildStates[bot] = EBotBuildState::BoxFighting;
                BuildCooldowns[bot] = CurrentTime + 3.0f;
            }
            break;

        case EBotBuildState::Building90s:
        case EBotBuildState::HighGroundRetake:
            if (UKismetMathLibrary::RandomBoolWithWeight(0.5f)) {
                BuildBoxFight(bot, bot->Pawn->K2_GetActorLocation());
                BuildStates[bot] = EBotBuildState::BoxFighting;
            }
            else {
                BuildStates[bot] = EBotBuildState::Idle;
            }
            BuildCooldowns[bot] = CurrentTime + 3.0f;
            break;

        case EBotBuildState::BoxFighting:
        case EBotBuildState::Turtling:
            BuildStates[bot] = EBotBuildState::Idle;
            BuildCooldowns[bot] = CurrentTime + 2.0f;
            break;
        }
    }
}

// Include PlayerBots.h after declarations to avoid circular dependencies
#include "PlayerBots.h"
