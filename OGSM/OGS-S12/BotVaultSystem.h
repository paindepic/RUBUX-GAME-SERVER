#pragma once
#include "framework.h"
#include "Looting.h"

enum class EVaultState : uint8 {
    None,
    SeekingBoss,
    FightingBoss,
    LootingKeycard,
    SeekingVault,
    OpeningVault,
    LootingVault
};

enum class EBossType : uint8 {
    None,
    TNTina,
    Deadpool,
    Skye,
    Brutus,
    Midas,
    Ocean
};

struct FBossInfo {
    EBossType Type;
    std::string Name;
    FVector Location;
    std::string KeycardPath;
    std::string WeaponPath;
    std::string POIName;
};

struct FVaultInfo {
    FVector Location;
    EBossType BossType;
    bool bOpened;
};

namespace BotVaultSystem {
    inline std::vector<FBossInfo> GetChapter2Season2Bosses() {
        return {
            { EBossType::TNTina, "TNTina", FVector(-77143, -80634, 69), 
              "/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_Rig.AGID_Athena_Keycard_Rig",
              "/Game/Athena/Items/Weapons/Boss/WID_Boss_Tina.WID_Boss_Tina",
              "The Rig" },
            { EBossType::Deadpool, "Deadpool", FVector(113312, 113547, -1837),
              "/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_Yacht.AGID_Athena_Keycard_Yacht",
              "/Game/Athena/Items/Weapons/Boss/WID_Boss_Hos_MG.WID_Boss_Hos_MG",
              "The Yacht" },
            { EBossType::Skye, "Skye", FVector(106902, -84901, -1834),
              "/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_Shark.AGID_Athena_Keycard_Shark",
              "/Game/Athena/Items/Weapons/Boss/WID_Boss_Skye_AR.WID_Boss_Skye_AR",
              "The Shark" },
            { EBossType::Brutus, "Brutus", FVector(-19544, 105594, 69),
              "/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_Grotto.AGID_Athena_Keycard_Grotto",
              "/Game/Athena/Items/Weapons/Boss/WID_Boss_Brutus_MG.WID_Boss_Brutus_MG",
              "The Grotto" },
            { EBossType::Midas, "Midas", FVector(6364, 4866, 69),
              "/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_Agency.AGID_Athena_Keycard_Agency",
              "/Game/Athena/Items/Weapons/Boss/WID_Boss_Midas_DR.WID_Boss_Midas_DR",
              "The Agency" },
            { EBossType::Ocean, "Ocean", FVector(-92971, 78709, 69),
              "/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_Fortilla.AGID_Athena_Keycard_Fortilla",
              "/Game/Athena/Items/Weapons/Boss/WID_Boss_Adventure_GH.WID_Boss_Adventure_GH",
              "The Fortilla" }
        };
    }

    inline bool IsBoss(AActor* Actor) {
        if (!Actor) return false;

        std::string Name = Actor->GetName();
        return Name.contains("Boss") || Name.contains("MANG") || Name.contains("POI");
    }

    inline bool HasKeycard(PlayerBot* bot, EBossType Type) {
        if (!bot || !bot->PC || !bot->PC->Inventory) return false;

        auto Bosses = GetChapter2Season2Bosses();
        std::string KeycardPath;
        for (auto& Boss : Bosses) {
            if (Boss.Type == Type) {
                KeycardPath = Boss.KeycardPath;
                break;
            }
        }

        if (KeycardPath.empty()) return false;

        for (size_t i = 0; i < bot->PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = bot->PC->Inventory->Inventory.ReplicatedEntries[i];
            if (Entry.ItemDefinition) {
                std::string ItemName = Entry.ItemDefinition->GetPathName();
                if (ItemName == KeycardPath) return true;
            }
        }
        return false;
    }

    inline bool HasAnyKeycard(PlayerBot* bot) {
        if (!bot || !bot->PC || !bot->PC->Inventory) return false;

        for (size_t i = 0; i < bot->PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = bot->PC->Inventory->Inventory.ReplicatedEntries[i];
            if (Entry.ItemDefinition) {
                std::string ItemName = Entry.ItemDefinition->GetName();
                if (ItemName.contains("Keycard")) return true;
            }
        }
        return false;
    }

    inline EBossType GetKeycardType(PlayerBot* bot) {
        if (!bot || !bot->PC || !bot->PC->Inventory) return EBossType::None;

        auto Bosses = GetChapter2Season2Bosses();
        for (size_t i = 0; i < bot->PC->Inventory->Inventory.ReplicatedEntries.Num(); i++) {
            auto& Entry = bot->PC->Inventory->Inventory.ReplicatedEntries[i];
            if (Entry.ItemDefinition) {
                std::string ItemName = Entry.ItemDefinition->GetPathName();
                for (auto& Boss : Bosses) {
                    if (ItemName == Boss.KeycardPath) return Boss.Type;
                }
            }
        }
        return EBossType::None;
    }

    inline FVector FindNearestBossLocation(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return FVector();

        FVector BotLoc = bot->Pawn->K2_GetActorLocation();
        auto Bosses = GetChapter2Season2Bosses();

        FVector NearestLoc;
        float ClosestDist = FLT_MAX;

        for (auto& Boss : Bosses) {
            float Dist = FVector::Distance(BotLoc, Boss.Location);
            if (Dist < ClosestDist) {
                ClosestDist = Dist;
                NearestLoc = Boss.Location;
            }
        }

        return NearestLoc;
    }

    inline FVector GetVaultLocationForBoss(EBossType Type) {
        auto Bosses = GetChapter2Season2Bosses();
        for (auto& Boss : Bosses) {
            if (Boss.Type == Type) {
                return Boss.Location + FVector(200.0f, 0, -100.0f);
            }
        }
        return FVector();
    }

    inline AActor* FindVaultAtLocation(FVector Location, float MaxDistance = 500.0f) {
        TArray<AActor*> Vaults;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), ABuildingContainer::StaticClass(), &Vaults);

        AActor* FoundVault = nullptr;
        float ClosestDist = MaxDistance;

        for (size_t i = 0; i < Vaults.Num(); i++) {
            AActor* Vault = Vaults[i];
            if (!Vault) continue;

            std::string Name = Vault->GetName();
            if (!Name.contains("Vault") && !Name.contains("Safe")) continue;

            float Dist = FVector::Distance(Vault->K2_GetActorLocation(), Location);
            if (Dist < ClosestDist) {
                ClosestDist = Dist;
                FoundVault = Vault;
            }
        }

        Vaults.Free();
        return FoundVault;
    }

    inline void LootMythicWeapon(PlayerBot* bot, EBossType BossType) {
        if (!bot || !bot->PC) return;

        auto Bosses = GetChapter2Season2Bosses();
        std::string WeaponPath;
        for (auto& Boss : Bosses) {
            if (Boss.Type == BossType) {
                WeaponPath = Boss.WeaponPath;
                break;
            }
        }

        if (WeaponPath.empty()) return;

        UFortItemDefinition* WeaponDef = StaticLoadObject<UFortItemDefinition>(WeaponPath);
        if (WeaponDef) {
            bot->GiveItemBot(WeaponDef, 1, Looting::GetClipSize(WeaponDef));
        }
    }

    inline void OpenVault(PlayerBot* bot, AActor* Vault) {
        if (!bot || !bot->Pawn || !bot->PC || !Vault) return;

        FVector VaultLoc = Vault->K2_GetActorLocation();
        FVector BotLoc = bot->Pawn->K2_GetActorLocation();
        float Dist = FVector::Distance(BotLoc, VaultLoc);

        if (Dist > 300.0f) {
            bot->PC->MoveToActor(Vault, 100.0f, true, false, true, nullptr, true);
            return;
        }

        bot->PC->StopMovement();

        FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(BotLoc, VaultLoc);
        bot->PC->SetControlRotation(LookRot);
        bot->PC->K2_SetActorRotation(LookRot, true);

        float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
        if (bot->TimeToNextAction == 0.0f) {
            bot->TimeToNextAction = CurrentTime + 1.5f;
            bot->Pawn->bStartedInteractSearch = true;
            bot->Pawn->OnRep_StartedInteractSearch();
        }
        else if (CurrentTime >= bot->TimeToNextAction) {
            Looting::SpawnLoot((ABuildingContainer*)Vault);
            Vault->bHidden = true;

            bot->PickupAllItemsInRange(600.0f);

            bot->Pawn->bStartedInteractSearch = false;
            bot->Pawn->OnRep_StartedInteractSearch();
            bot->TimeToNextAction = 0.0f;

            EBossType KeyType = GetKeycardType(bot);
            LootMythicWeapon(bot, KeyType);
        }
    }

    inline void PickupKeycard(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return;

        static auto PickupClass = AFortPickupAthena::StaticClass();
        TArray<AActor*> Pickups;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), PickupClass, &Pickups);

        for (size_t i = 0; i < Pickups.Num(); i++) {
            AFortPickupAthena* Pickup = (AFortPickupAthena*)Pickups[i];
            if (!Pickup || Pickup->bHidden) continue;

            if (Pickup->PrimaryPickupItemEntry.ItemDefinition) {
                std::string ItemName = Pickup->PrimaryPickupItemEntry.ItemDefinition->GetName();
                if (ItemName.contains("Keycard")) {
                    float Dist = Pickup->GetDistanceTo(bot->Pawn);
                    if (Dist < 400.0f) {
                        bot->Pickup(Pickup);
                    }
                    else if (Dist < 1500.0f) {
                        bot->PC->MoveToActor(Pickup, 50.0f, true, false, true, nullptr, true);
                    }
                }
            }
        }

        Pickups.Free();
    }

    inline void UpdateVaultState(PlayerBot* bot) {
        if (!bot || !bot->Pawn || !bot->PC) return;
        if (!Globals::bVaultSystemEnabled) return;

        static std::map<PlayerBot*, EVaultState> VaultStates;
        static std::map<PlayerBot*, float> VaultCooldowns;

        if (VaultStates.find(bot) == VaultStates.end()) {
            VaultStates[bot] = EVaultState::None;
        }

        float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());
        if (VaultCooldowns.find(bot) == VaultCooldowns.end()) {
            VaultCooldowns[bot] = 0.0f;
        }

        if (CurrentTime < VaultCooldowns[bot]) return;

        EVaultState CurrentState = VaultStates[bot];

        switch (CurrentState) {
        case EVaultState::None:
            if (UKismetMathLibrary::RandomBoolWithWeight(0.1f) && bot->BotState == EBotState::Looting) {
                FVector BossLoc = FindNearestBossLocation(bot);
                if (!BossLoc.IsZero()) {
                    bot->TargetDropZone = BossLoc;
                    VaultStates[bot] = EVaultState::SeekingBoss;
                    VaultCooldowns[bot] = CurrentTime + 5.0f;
                }
            }
            break;

        case EVaultState::SeekingBoss:
            if (HasAnyKeycard(bot)) {
                VaultStates[bot] = EVaultState::LootingKeycard;
                VaultCooldowns[bot] = CurrentTime + 2.0f;
            }
            else {
                PickupKeycard(bot);
            }
            break;

        case EVaultState::LootingKeycard:
            {
                EBossType KeyType = GetKeycardType(bot);
                if (KeyType != EBossType::None) {
                    FVector VaultLoc = GetVaultLocationForBoss(KeyType);
                    if (!VaultLoc.IsZero()) {
                        bot->TargetDropZone = VaultLoc;
                        VaultStates[bot] = EVaultState::SeekingVault;
                        VaultCooldowns[bot] = CurrentTime + 5.0f;
                    }
                    else {
                        VaultStates[bot] = EVaultState::None;
                    }
                }
                else {
                    VaultStates[bot] = EVaultState::None;
                }
            }
            break;

        case EVaultState::SeekingVault:
            {
                EBossType KeyType = GetKeycardType(bot);
                FVector VaultLoc = GetVaultLocationForBoss(KeyType);
                AActor* Vault = FindVaultAtLocation(VaultLoc);

                if (Vault) {
                    float Dist = FVector::Distance(bot->Pawn->K2_GetActorLocation(), VaultLoc);
                    if (Dist < 500.0f) {
                        VaultStates[bot] = EVaultState::OpeningVault;
                        VaultCooldowns[bot] = CurrentTime + 3.0f;
                    }
                }
                else {
                    VaultStates[bot] = EVaultState::None;
                }
            }
            break;

        case EVaultState::OpeningVault:
            {
                EBossType KeyType = GetKeycardType(bot);
                FVector VaultLoc = GetVaultLocationForBoss(KeyType);
                AActor* Vault = FindVaultAtLocation(VaultLoc);

                if (Vault) {
                    OpenVault(bot, Vault);
                    VaultStates[bot] = EVaultState::LootingVault;
                    VaultCooldowns[bot] = CurrentTime + 5.0f;
                }
                else {
                    VaultStates[bot] = EVaultState::None;
                }
            }
            break;

        case EVaultState::LootingVault:
            VaultStates[bot] = EVaultState::None;
            VaultCooldowns[bot] = CurrentTime + 10.0f;
            break;
        }
    }

    inline void OnBossKilled(AActor* Boss, AController* Killer) {
        if (!Boss || !Killer) return;
        if (!Globals::bVaultSystemEnabled) return;

        std::string BossName = Boss->GetName();
        auto Bosses = GetChapter2Season2Bosses();

        FBossInfo KilledBoss;
        KilledBoss.Type = EBossType::None;

        for (auto& BossInfo : Bosses) {
            if (BossName.contains(BossInfo.Name) || BossName.contains(BossInfo.POIName)) {
                KilledBoss = BossInfo;
                break;
            }
        }

        if (KilledBoss.Type == EBossType::None) return;

        UFortItemDefinition* KeycardDef = StaticLoadObject<UFortItemDefinition>(KilledBoss.KeycardPath);
        if (KeycardDef) {
            FVector DropLoc = Boss->K2_GetActorLocation();
            SpawnPickup(KeycardDef, 1, 0, DropLoc, EFortPickupSourceTypeFlag::Other, EFortPickupSpawnSource::PlayerElimination);
        }

        if (Killer->IsA(AFortPlayerControllerAthena::StaticClass())) {
            AFortPlayerControllerAthena* PC = (AFortPlayerControllerAthena*)Killer;
            Quests::GiveAccolade(PC, StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_078_ElimBoss.AccoladeId_078_ElimBoss"));
        }
    }
}
