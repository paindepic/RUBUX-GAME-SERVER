#pragma once
#include "framework.h"
#include "Quests.h"
#include <ctime>

enum class EQuestType : uint8 {
    Eliminations,
    OpenChests,
    SurviveStormPhases,
    TravelDistance,
    EliminateBoss,
    OpenVault,
    UseVehicle,
    PlaceTopTen,
    PlaceTopOne,
    WeaponEliminations,
    Max
};

enum class EQuestRarity : uint8 {
    Daily,
    Weekly
};

struct FPlayerQuest {
    FString QuestId;
    FString QuestName;
    EQuestType Type;
    int32 CurrentProgress;
    int32 TargetProgress;
    int32 XPReward;
    bool bCompleted;
    EQuestRarity Rarity;
    time_t ExpirationTime;
    std::string WeaponType;

    FPlayerQuest() :
        QuestId(L""),
        QuestName(L""),
        Type(EQuestType::Eliminations),
        CurrentProgress(0),
        TargetProgress(0),
        XPReward(0),
        bCompleted(false),
        Rarity(EQuestRarity::Daily),
        ExpirationTime(0),
        WeaponType("")
    {}
};

struct FQuestNotification {
    FString Title;
    FString Description;
    int32 XPReward;
    float ShowTime;
};

class FQuestManager {
public:
    std::vector<FPlayerQuest> DailyQuests;
    std::vector<FPlayerQuest> WeeklyQuests;
    std::map<AFortPlayerControllerAthena*, std::vector<FQuestNotification>> PendingNotifications;
    std::map<AFortPlayerControllerAthena*, float> DistanceTraveled;
    std::map<AFortPlayerControllerAthena*, int32> StormPhasesSurvived;
    time_t LastDailyReset;
    time_t LastWeeklyReset;

    static FQuestManager& Get() {
        static FQuestManager Instance;
        return Instance;
    }

    void Initialize() {
        time_t Now = time(nullptr);
        LastDailyReset = Now;
        LastWeeklyReset = Now;
    }

    bool ShouldResetDaily() {
        time_t Now = time(nullptr);
        struct tm LastReset = *gmtime(&LastDailyReset);
        struct tm Current = *gmtime(&Now);

        return LastReset.tm_mday != Current.tm_mday;
    }

    bool ShouldResetWeekly() {
        time_t Now = time(nullptr);
        struct tm LastReset = *gmtime(&LastWeeklyReset);
        struct tm Current = *gmtime(&Now);

        int DaysDiff = (Current.tm_yday + (Current.tm_year * 365)) - (LastReset.tm_yday + (LastReset.tm_year * 365));
        return DaysDiff >= 7;
    }

    FString GenerateQuestId(EQuestRarity Rarity, int Index) {
        time_t Now = time(nullptr);
        std::string Prefix = (Rarity == EQuestRarity::Daily) ? "Daily" : "Weekly";
        return FString(std::to_string(Now) + "_" + Prefix + "_" + std::to_string(Index));
    }

    FPlayerQuest CreateRandomQuest(EQuestRarity Rarity, int Index) {
        FPlayerQuest Quest;
        Quest.QuestId = GenerateQuestId(Rarity, Index);
        Quest.Rarity = Rarity;
        Quest.CurrentProgress = 0;
        Quest.bCompleted = false;

        time_t Now = time(nullptr);
        Quest.ExpirationTime = Now + ((Rarity == EQuestRarity::Daily) ? 86400 : 604800);

        int32 TypeCount = (int32)EQuestType::Max;
        EQuestType RandomType = (EQuestType)(rand() % TypeCount);
        Quest.Type = RandomType;

        switch (RandomType) {
        case EQuestType::Eliminations:
            Quest.QuestName = (Rarity == EQuestRarity::Daily) ? L"Eliminer des adversaires" : L"Eliminer de nombreux adversaires";
            Quest.TargetProgress = (Rarity == EQuestRarity::Daily) ? 3 : 15;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 1500 : 5000;
            break;

        case EQuestType::OpenChests:
            Quest.QuestName = (Rarity == EQuestRarity::Daily) ? L"Ouvrir des coffres" : L"Ouvrir de nombreux coffres";
            Quest.TargetProgress = (Rarity == EQuestRarity::Daily) ? 5 : 25;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 1200 : 4000;
            break;

        case EQuestType::SurviveStormPhases:
            Quest.QuestName = L"Survivre aux phases de la tempete";
            Quest.TargetProgress = (Rarity == EQuestRarity::Daily) ? 3 : 10;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 1000 : 3500;
            break;

        case EQuestType::TravelDistance:
            Quest.QuestName = L"Parcourir une distance";
            Quest.TargetProgress = (Rarity == EQuestRarity::Daily) ? 1000 : 5000;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 800 : 3000;
            break;

        case EQuestType::EliminateBoss:
            Quest.QuestName = L"Eliminer un boss";
            Quest.TargetProgress = 1;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 2000 : 8000;
            break;

        case EQuestType::OpenVault:
            Quest.QuestName = L"Ouvrir un coffre-fort";
            Quest.TargetProgress = 1;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 2500 : 10000;
            break;

        case EQuestType::UseVehicle:
            Quest.QuestName = L"Utiliser des vehicules";
            Quest.TargetProgress = (Rarity == EQuestRarity::Daily) ? 2 : 10;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 1000 : 3500;
            break;

        case EQuestType::PlaceTopTen:
            Quest.QuestName = L"Terminer dans le top 10";
            Quest.TargetProgress = (Rarity == EQuestRarity::Daily) ? 1 : 5;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 1500 : 6000;
            break;

        case EQuestType::PlaceTopOne:
            Quest.QuestName = L"Gagner une partie";
            Quest.TargetProgress = (Rarity == EQuestRarity::Daily) ? 1 : 3;
            Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 3000 : 15000;
            break;

        case EQuestType::WeaponEliminations:
            {
                std::vector<std::string> WeaponTypes = { "Shotgun", "Assault", "SMG", "Sniper", "Pistol" };
                Quest.WeaponType = WeaponTypes[rand() % WeaponTypes.size()];
                std::wstring WWeaponType(Quest.WeaponType.begin(), Quest.WeaponType.end());
                Quest.QuestName = L"Eliminations avec " + WWeaponType;
                Quest.TargetProgress = (Rarity == EQuestRarity::Daily) ? 2 : 10;
                Quest.XPReward = (Rarity == EQuestRarity::Daily) ? 1500 : 5000;
            }
            break;
        }

        return Quest;
    }

    void GenerateDailyQuests() {
        DailyQuests.clear();
        for (int i = 0; i < 3; i++) {
            DailyQuests.push_back(CreateRandomQuest(EQuestRarity::Daily, i));
        }
        LastDailyReset = time(nullptr);
    }

    void GenerateWeeklyQuests() {
        WeeklyQuests.clear();
        for (int i = 0; i < 7; i++) {
            WeeklyQuests.push_back(CreateRandomQuest(EQuestRarity::Weekly, i));
        }
        LastWeeklyReset = time(nullptr);
    }

    void SendQuestNotification(AFortPlayerControllerAthena* PC, FPlayerQuest& Quest) {
        if (!PC) return;

        FQuestNotification Notif;
        Notif.Title = (Quest.Rarity == EQuestRarity::Daily) ? L"Quete quotidienne completee!" : L"Quete hebdomadaire completee!";
        Notif.Description = Quest.QuestName + L" +" + FString(std::to_string(Quest.XPReward)) + L" XP";
        Notif.XPReward = Quest.XPReward;
        Notif.ShowTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());

        PendingNotifications[PC].push_back(Notif);

        static UFortAccoladeItemDefinition* QuestCompleteAccolade = StaticLoadObject<UFortAccoladeItemDefinition>("/Game/Athena/Items/Accolades/AccoladeId_014_QuestComplete.AccoladeId_014_QuestComplete");
        if (QuestCompleteAccolade) {
            Quests::GiveAccolade(PC, QuestCompleteAccolade);
        }
    }

    void GrantXPForQuest(AFortPlayerControllerAthena* PC, int32 XPAmount) {
        if (!PC || !PC->XPComponent) return;

        FXPEventInfo EventInfo{};
        EventInfo.EventXpValue = XPAmount;
        EventInfo.Priority = 1;
        EventInfo.TotalXpEarnedInMatch = PC->XPComponent->TotalXpEarned + XPAmount;

        PC->XPComponent->ChallengeXp += XPAmount;
        PC->XPComponent->TotalXpEarned += XPAmount;
        PC->XPComponent->OnXPEvent(EventInfo);
    }

    void CompleteQuest(AFortPlayerControllerAthena* PC, FPlayerQuest& Quest) {
        if (Quest.bCompleted) return;

        Quest.bCompleted = true;
        GrantXPForQuest(PC, Quest.XPReward);
        SendQuestNotification(PC, Quest);
    }

    void UpdateQuestProgress(AFortPlayerControllerAthena* PC, EQuestType Type, int32 Count = 1, const std::string& WeaponType = "") {
        if (!PC) return;

        for (auto& Quest : DailyQuests) {
            if (Quest.bCompleted) continue;
            if (Quest.Type == Type) {
                if (Type == EQuestType::WeaponEliminations && Quest.WeaponType != WeaponType) continue;
                Quest.CurrentProgress += Count;
                if (Quest.CurrentProgress >= Quest.TargetProgress) {
                    CompleteQuest(PC, Quest);
                }
            }
        }

        for (auto& Quest : WeeklyQuests) {
            if (Quest.bCompleted) continue;
            if (Quest.Type == Type) {
                if (Type == EQuestType::WeaponEliminations && Quest.WeaponType != WeaponType) continue;
                Quest.CurrentProgress += Count;
                if (Quest.CurrentProgress >= Quest.TargetProgress) {
                    CompleteQuest(PC, Quest);
                }
            }
        }
    }

    void OnPlayerElimination(AFortPlayerControllerAthena* PC, AFortPlayerStateAthena* Victim, const std::string& WeaponName) {
        if (!PC) return;
        UpdateQuestProgress(PC, EQuestType::Eliminations, 1);

        std::string LowerWeapon = WeaponName;
        std::transform(LowerWeapon.begin(), LowerWeapon.end(), LowerWeapon.begin(), ::tolower);

        if (LowerWeapon.contains("shotgun")) UpdateQuestProgress(PC, EQuestType::WeaponEliminations, 1, "Shotgun");
        else if (LowerWeapon.contains("assault") || LowerWeapon.contains("ar")) UpdateQuestProgress(PC, EQuestType::WeaponEliminations, 1, "Assault");
        else if (LowerWeapon.contains("smg") || LowerWeapon.contains("pistol")) UpdateQuestProgress(PC, EQuestType::WeaponEliminations, 1, "SMG");
        else if (LowerWeapon.contains("sniper")) UpdateQuestProgress(PC, EQuestType::WeaponEliminations, 1, "Sniper");
    }

    void OnChestOpened(AFortPlayerControllerAthena* PC) {
        if (!PC) return;
        UpdateQuestProgress(PC, EQuestType::OpenChests, 1);
    }

    void OnStormPhaseAdvanced() {
        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        if (!GameMode) return;

        for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++) {
            AFortPlayerControllerAthena* PC = GameMode->AlivePlayers[i];
            if (PC) {
                StormPhasesSurvived[PC]++;
                UpdateQuestProgress(PC, EQuestType::SurviveStormPhases, 1);
            }
        }
    }

    void OnDistanceTraveled(AFortPlayerControllerAthena* PC, float Distance) {
        if (!PC) return;
        DistanceTraveled[PC] += Distance;
        int32 Meters = (int32)(DistanceTraveled[PC] / 100.0f);
        if (Meters > 0) {
            UpdateQuestProgress(PC, EQuestType::TravelDistance, Meters);
            DistanceTraveled[PC] = 0.0f;
        }
    }

    void OnBossKilled(AFortPlayerControllerAthena* PC) {
        if (!PC) return;
        UpdateQuestProgress(PC, EQuestType::EliminateBoss, 1);
    }

    void OnVaultOpened(AFortPlayerControllerAthena* PC) {
        if (!PC) return;
        UpdateQuestProgress(PC, EQuestType::OpenVault, 1);
    }

    void OnVehicleUsed(AFortPlayerControllerAthena* PC) {
        if (!PC) return;
        UpdateQuestProgress(PC, EQuestType::UseVehicle, 1);
    }

    void OnMatchEnded(AFortPlayerControllerAthena* PC, int32 Place) {
        if (!PC) return;
        if (Place <= 10) UpdateQuestProgress(PC, EQuestType::PlaceTopTen, 1);
        if (Place == 1) UpdateQuestProgress(PC, EQuestType::PlaceTopOne, 1);
    }

    void TickNotifications(AFortPlayerControllerAthena* PC) {
        if (!PC) return;

        float CurrentTime = UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld());

        auto& Notifs = PendingNotifications[PC];
        for (auto it = Notifs.begin(); it != Notifs.end();) {
            if (CurrentTime - it->ShowTime > 5.0f) {
                it = Notifs.erase(it);
            }
            else {
                ++it;
            }
        }
    }

    void ShowQuestNotification(AFortPlayerControllerAthena* PC, FQuestNotification& Notif) {
        if (!PC) return;

        FText TitleText;
        TitleText.TextData = (FTextData*)Notif.Title.c_str();

        FText DescText;
        DescText.TextData = (FTextData*)Notif.Description.c_str();

        // Send to client via RPC
        // This would typically use a Client RPC to show UI
        // For now, we use the accolade system which already has UI
    }
};

namespace PlayerQuests {
    inline void InitializeQuestSystem() {
        if (!Globals::bQuestSystemEnabled) return;
        FQuestManager::Get().Initialize();
        FQuestManager::Get().GenerateDailyQuests();
        FQuestManager::Get().GenerateWeeklyQuests();
    }

    inline void Tick() {
        if (!Globals::bQuestSystemEnabled) return;

        if (FQuestManager::Get().ShouldResetDaily()) {
            FQuestManager::Get().GenerateDailyQuests();
        }

        if (FQuestManager::Get().ShouldResetWeekly()) {
            FQuestManager::Get().GenerateWeeklyQuests();
        }

        auto GameMode = (AFortGameModeAthena*)UWorld::GetWorld()->AuthorityGameMode;
        if (GameMode) {
            for (size_t i = 0; i < GameMode->AlivePlayers.Num(); i++) {
                FQuestManager::Get().TickNotifications(GameMode->AlivePlayers[i]);
            }
        }
    }
}
