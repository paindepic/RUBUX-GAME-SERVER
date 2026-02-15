#pragma once
#include "framework.h"

// POI's, Landmarks, ect
// Ordered Left to Right, Top to Down.

// Some of these have Z Coords but i really dc abt them so ye

// Double Whitespace inbetween means missing locs

// Basic POI's for the basic bots!! i may implement additional behaviours that require different pois to these but these are the base ones
std::vector<FVector> DropZoneLocations = {
    { 106902, -84901, -1834 }, // The Shark (Complete)
    { 107834, -78584, -783 }, // Top of The Shark
    { 120009, -84032, -3370 }, // The Shark Prison
    { 112255, -91220, -3011 }, // The Shark Choppa Spawn
    { 99820, -82200, -3370 }, // The Shark Basketball Court

    //{ 119369, -56829, 853 }, // LightHouse (Complete)
    //{ 115738, -53198, -298 }, // LightHouse Island House
    //{ 108900, -69654, -3762 }, // LightHouse Island Boathouse

    //{ 84127, 14, 69 }, // Pretty House

    { 96764, 29166, -2226 }, // Craggy Cliffs
    { 104078, 26670, -1834 }, // Fishsticks restruant

    { 94032, 51513, 4693 }, // Shadow RadioTower thing idk

    { 76334, 90345, -1450 }, // Steamy Stacks
    { 81669, 90762, 469 }, // Kevolution Energy
    { 80258, 95433, 69 },
    { 86691, 102076, 69 },

    { 113312, 113547, -1837 }, // The Yacht (Complete)
    { 116567, 113665, -2602 }, // The Yacht
    { 109895, 113636, -2986 }, // The Yacht
    { 106233, 108428, -3762 }, // The Yacht Island

    //{ 56804, 55233, 69 }, // Farm Shop
    //{ 64096, 50928, 69 }, // Farm House

    { 64240, -16323, 69 }, // Pleasant Park
    { 60290, -16240, 69 }, // PP Shadow Henchmen Base

    //{ 71534, -63077, 69 }, // Crashed Plane Site

    { 31257, -77599, 69 }, // Sweaty Sands

    { 13637, -24219, 69 }, // Salty Springs

    { 30808, 10669, 69 }, // Risky Reels

    { 30536, 42295, 69 }, // Frenzy Farm

    { 30437, 69691, 69 }, // Shadow Henchmen Gas Station

    { 17963, 112955, 69 }, // Dirty Docks

    { 6364, 4866, 69 }, // Agency
    { 6426, 7100, 69 },
    { 6245, 1741, 69 },
    { 11188, 4863, 69 },
    { 657, 4334, 69 },
    { 4084, 7167, 69 },

    { -13311, -81033, 69 }, // Holly Hedges

    { -28997, -31787, 69 }, // Weeping Woods

    { -64881, -46495, 69 }, // Slurpy Swamp

    { -77143, -80634, 69 }, // The Rig
    { -73305, -90910, 69 },

    { -88282, 23038, 69 }, // Misty Meadows
    { -88329, 31859, 69 },

    { -68041, 29906, 69 }, // Misty henchmen house

    { -43937, 52030, 69 }, // Lazy Lake
    { -54818, 57400, 69 },

    { -68550, 80804, 69 }, // Catty Corner

    { -92971, 78709, 69 }, // Mountain henchmen base

    { -39867, 91323, 69 }, // Retail Row
    { -38750, 102176, 69 },

    { -19544, 105594, 69 }, // Grotto
    { -17383, 112993, 69 },
    { -22500, 112479, 69 },
};

// ============================================================================
// GRANDES VILLES AVEC BOSS + VAULT (Chapitre 2 Saison 2)
// ============================================================================
struct FBossPOI {
    FString Name;                    // Nom du POI
    FVector Location;                // Position centrale
    FVector VaultLocation;           // Position du vault
    FVector BossSpawnLocation;       // Position spawn boss
    FString BossName;                // Nom du boss (Midas, Brutus, etc.)
    FString KeycardPath;             // Chemin de la carte
    FString MythicWeaponPath;        // Chemin de l'arme mythique
    int32 MaxBots;                   // Max bots pour ce POI
    int32 PriorityLevel;             // Priorité (1-10)
};

inline std::vector<FBossPOI> BossPOIs = {
    {
        L"The Agency",
        {6364, 4866, 100},
        {6400, 4900, 50},
        {6380, 4880, 100},
        L"Midas",
        L"/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheAgency",
        L"/Game/Athena/Items/Weapons/WID_Boss_Midas",
        10, 10
    },
    {
        L"The Rig",
        {-77143, -80634, 100},
        {-77100, -80600, 50},
        {-77120, -80620, 100},
        L"TNTina",
        L"/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheRig",
        L"/Game/Athena/Items/Weapons/WID_Boss_TNTina",
        8, 9
    },
    {
        L"The Yacht",
        {113312, 113547, -1837},
        {113350, 113580, -1900},
        {113330, 113560, -1837},
        L"Deadpool",
        L"/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheYacht",
        L"/Game/Athena/Items/Weapons/WID_Boss_Deadpool",
        8, 8
    },
    {
        L"The Shark",
        {106902, -84901, -1834},
        {106950, -84850, -1900},
        {106920, -84880, -1834},
        L"Skye",
        L"/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheShark",
        L"/Game/Athena/Items/Weapons/WID_Boss_Skye",
        10, 9
    },
    {
        L"The Grotto",
        {-19544, 105594, 69},
        {-19500, 105650, 20},
        {-19530, 105620, 69},
        L"Brutus",
        L"/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheGrotto",
        L"/Game/Athena/Items/Weapons/WID_Boss_Brutus",
        8, 9
    },
    {
        L"The Fortilla",
        {50000, -50000, 100},
        {50050, -49950, 50},
        {50020, -49980, 100},
        L"Ocean",
        L"/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheFortilla",
        L"/Game/Athena/Items/Weapons/WID_Boss_Ocean",
        6, 7
    }
};

// ============================================================================
// MOYENNES VILLES (Style Pleasant Park) - Priorité toits avec coffres
// ============================================================================
struct FMediumPOI {
    FString Name;
    FVector Center;
    float Radius;
    int32 MaxBots;
    bool bHasRoofChests;
    TArray<FVector> RoofChestLocations;
    int32 PriorityLevel;
};

inline std::vector<FMediumPOI> MediumPOIs = {
    {
        L"Pleasant Park",
        {20000, -20000, 100},
        3000.0f,
        6,
        true,
        {{20100, -20100, 350}, {20250, -19950, 350}, {19900, -20250, 350}, {20500, -19800, 350}},
        8
    },
    {
        L"Sweaty Sands",
        {40000, -40000, 100},
        2500.0f,
        5,
        true,
        {{40100, -40100, 350}, {40250, -39950, 350}},
        7
    },
    {
        L"Retail Row",
        {30000, 30000, 100},
        2800.0f,
        5,
        true,
        {{30100, 30100, 350}, {30250, 29950, 350}},
        7
    },
    {
        L"Misty Meadows",
        {10000, 40000, 100},
        2500.0f,
        4,
        true,
        {{10100, 40100, 350}},
        6
    },
    {
        L"Lazy Lake",
        {25000, 25000, 100},
        2200.0f,
        4,
        true,
        {{25100, 25100, 350}},
        6
    },
    {
        L"Hollywood",
        {35000, -15000, 100},
        2000.0f,
        3,
        true,
        {{35100, -15100, 350}},
        5
    },
    {
        L"Risky Reels",
        {45000, 15000, 100},
        1800.0f,
        3,
        false,
        {},
        5
    },
    {
        L"Frenzy Farm",
        {15000, 15000, 100},
        2500.0f,
        4,
        false,
        {},
        6
    }
};

// ============================================================================
// PETITES ZONES (Style Little Android)
// ============================================================================
struct FSmallPOI {
    FString Name;
    FVector Location;
    int32 MaxBots;
    int32 PriorityLevel;
};

inline std::vector<FSmallPOI> SmallPOIs = {
    {L"Little Android", {5000, 5000, 100}, 2, 4},
    {L"Camp Cod", {-40000, -40000, 100}, 2, 4},
    {L"Stack Shack", {-30000, 30000, 100}, 1, 3},
    {L"Shanty Town", {20000, 10000, 100}, 2, 4},
    {L"Gas Station", {10000, 20000, 100}, 1, 3},
    {L"Rainbow Rentals", {-20000, -10000, 100}, 1, 3},
    {L"Beach Bus Stop", {30000, -30000, 100}, 1, 2}
};

// ============================================================================
// FONCTIONS UTILITAIRES POI
// ============================================================================
inline FBossPOI* GetBossPOIByName(const FString& Name) {
    for (auto& POI : BossPOIs) {
        if (POI.Name == Name) return &POI;
    }
    return nullptr;
}

inline FVector GetRandomDropZoneWithPriority(int32 BotIndex, int32 TotalBots) {
    // Distribution: 40% Boss POI, 35% Medium, 20% Small, 5% Random
    int32 Choice = rand() % 100;

    if (Choice < 40 && !BossPOIs.empty()) {
        return BossPOIs[rand() % BossPOIs.size()].Location;
    } else if (Choice < 75 && !MediumPOIs.empty()) {
        return MediumPOIs[rand() % MediumPOIs.size()].Center;
    } else if (Choice < 95 && !SmallPOIs.empty()) {
        return SmallPOIs[rand() % SmallPOIs.size()].Location;
    }

    // Fallback
    return DropZoneLocations.empty() ? FVector() : DropZoneLocations[rand() % DropZoneLocations.size()];
}

// Vérifier si dans la trajectoire du bus
inline bool IsLocationInBusPath(FVector Location) {
    auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
    AActor* Bus = GameState->GetAircraft(0);
    if (!Bus) return true;

    FVector BusLoc = Bus->K2_GetActorLocation();
    FVector BusForward = Bus->GetActorForwardVector();

    FVector ToLoc = Location - BusLoc;
    ToLoc.Z = 0;
    BusForward.Z = 0;

    return (ToLoc.X * BusForward.X + ToLoc.Y * BusForward.Y) > 0;
}

// Vérifier si dans la future safe zone
inline bool IsInFutureSafeZone(FVector Location, AFortSafeZoneIndicator* SafeZone) {
    if (!SafeZone) return true;
    return Location.DistanceTo(SafeZone->NextCenter) < SafeZone->NextRadius * 0.85f;
}

// Compter bots par POI
inline int32 CountBotsInPOI(FVector POICenter, float Radius) {
    int32 Count = 0;
    extern std::vector<class PlayerBot*> PlayerBotArray;
    for (auto Bot : PlayerBotArray) {
        if (Bot && !Bot->TargetDropZone.IsZero()) {
            if (Bot->TargetDropZone.DistanceTo(POICenter) < Radius) {
                Count++;
            }
        }
    }
    return Count;
}
