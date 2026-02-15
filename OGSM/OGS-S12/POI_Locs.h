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

// OGSM - Chapter 2 Season 2 Detailed POI System

// Grandes villes avec Boss + Vault
struct FBossPOI {
    FString Name;
    FVector Location;
    FVector VaultLocation;
    UFortItemDefinition* Keycard;
    UFortItemDefinition* MythicWeapon;
    int32 MaxBots;
};

inline std::vector<FBossPOI> BossPOIs = {
    {L"The Agency", {6364, 4866, 100}, {6400, 4900, 50},
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheAgency"),
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Boss_Midas"), 10},
    {L"The Rig", {-77143, -80634, 100}, {-77100, -80600, 50},
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheRig"),
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Boss_TNTina"), 8},
    {L"The Yacht", {113312, 113547, -1837}, {113350, 113580, -1900},
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheYacht"),
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Boss_Deadpool"), 8},
    {L"The Shark", {106902, -84901, -1834}, {106950, -84850, -1900},
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheShark"),
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Boss_Skye"), 10},
    {L"The Grotto", {-19544, 105594, 69}, {-19500, 105650, 20},
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheGrotto"),
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Boss_Brutus"), 8},
    {L"The Fortilla", {50000, -50000, 100}, {50050, -49950, 50},
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Gameplay/Keycards/AGID_Athena_Keycard_TheFortilla"),
     StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Boss_Ocean"), 6}
};

// Moyennes villes (style Pleasant Park) - Toits avec coffres
struct FMediumPOI {
    FString Name;
    FVector Center;
    float Radius;
    int32 MaxBots;
    bool bHasRoofChests;
};

inline std::vector<FMediumPOI> MediumPOIs = {
    {L"Pleasant Park", {64240, -16323, 69}, 3000.0f, 6, true},
    {L"Sweaty Sands", {31257, -77599, 69}, 2500.0f, 5, true},
    {L"Retail Row", {-39867, 91323, 69}, 2800.0f, 5, true},
    {L"Misty Meadows", {-88282, 23038, 69}, 2500.0f, 4, true},
    {L"Lazy Lake", {-43937, 52030, 69}, 2200.0f, 4, true},
    {L"Holly Hedges", {-13311, -81033, 69}, 2000.0f, 3, true},
    {L"Risky Reels", {30808, 10669, 69}, 1800.0f, 3, false},
    {L"Frenzy Farm", {30536, 42295, 69}, 2500.0f, 4, false},
    {L"Craggy Cliffs", {96764, 29166, -2226}, 2400.0f, 4, true},
    {L"Dirty Docks", {17963, 112955, 69}, 2600.0f, 4, false},
    {L"Steamy Stacks", {76334, 90345, -1450}, 2700.0f, 5, false}
};

// Petites zones (Little Android style)
struct FSmallPOI {
    FString Name;
    FVector Location;
    int32 MaxBots;
};

inline std::vector<FSmallPOI> SmallPOIs = {
    {L"Little Android", {5000, 5000, 100}, 2},
    {L"Camp Cod", {-40000, -40000, 100}, 2},
    {L"Stack Shack", {-30000, 30000, 100}, 1},
    {L"Shanty Town", {20000, 10000, 100}, 2},
    {L"Gas Station", {10000, 20000, 100}, 1},
    {L"Weeping Woods", {-28997, -31787, 69}, 2},
    {L"Slurpy Swamp", {-64881, -46495, 69}, 2},
    {L"Catty Corner", {-68550, 80804, 69}, 2}
};

// Toits avec coffres pour atterrissage précis (Pleasant Park style)
struct FRoofChestLocation {
    FVector Location;
    bool bIsAvailable;
};

inline std::vector<FRoofChestLocation> RoofChestLocations = {
    {{20100, -20100, 350}, true},
    {{20250, -19950, 350}, true},
    {{19900, -20250, 350}, true},
    {{20500, -19800, 350}, true},
    {{19800, -20500, 350}, true},
    {{20300, -20000, 350}, true},
    {{20050, -20150, 350}, true},
    {{20150, -19850, 350}, true},
    {{19850, -19950, 350}, true},
    {{20400, -20200, 350}, true},
    // Sweaty Sands roofs
    {{31000, -77800, 350}, true},
    {{31200, -77500, 350}, true},
    {{31500, -77700, 350}, true},
    {{31150, -77450, 350}, true},
    {{31300, -77900, 350}, true},
    // Retail Row roofs
    {{-40000, 91500, 350}, true},
    {{-39800, 91200, 350}, true},
    {{-40150, 91450, 350}, true},
    {{-39700, 91600, 350}, true},
    {{-39950, 91350, 350}, true},
    // Misty Meadows roofs
    {{-88500, 22800, 350}, true},
    {{-88300, 23100, 350}, true},
    {{-88650, 22950, 350}, true},
    {{-88200, 22700, 350}, true},
    {{-88400, 23200, 350}, true},
    // Lazy Lake roofs
    {{-44100, 52200, 350}, true},
    {{-43900, 51900, 350}, true},
    {{-44250, 52150, 350}, true},
    {{-43800, 52300, 350}, true},
    {{-44000, 51800, 350}, true}
};