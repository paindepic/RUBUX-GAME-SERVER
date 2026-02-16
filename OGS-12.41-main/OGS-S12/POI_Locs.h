#pragma once
#include "framework.h"

// POI's, Landmarks, ect
// Ordered Left to Right, Top to Down.

// POI Size Classification for Strategic Landing
enum class EPoiSize : uint8 {
    Large,   // Major named POIs (high loot, high competition)
    Medium,  // Medium POIs and landmarks (balanced loot, medium competition)
    Small    // Small landmarks and isolated areas (lower loot, safer)
};

// POI Location with metadata for strategic landing
struct FPOILocation {
    FVector Location;
    EPoiSize Size;
    const char* Name;
    bool bHasVault;
    bool bHasBoss;
    
    FPOILocation(FVector Loc, EPoiSize S, const char* N, bool Vault = false, bool Boss = false)
        : Location(Loc), Size(S), Name(N), bHasVault(Vault), bHasBoss(Boss) {}
};

// Categorized POI system for ultra-intelligent landing
static std::vector<FPOILocation> CategorizedPOIs = {
    // Large POIs - High loot density, high competition
    FPOILocation({ 106902, -84901, -1834 }, EPoiSize::Large, "The Shark", true, true),
    FPOILocation({ 96764, 29166, -2226 }, EPoiSize::Large, "Craggy Cliffs", false, false),
    FPOILocation({ 76334, 90345, -1450 }, EPoiSize::Large, "Steamy Stacks", false, false),
    FPOILocation({ 113312, 113547, -1837 }, EPoiSize::Large, "The Yacht", true, true),
    FPOILocation({ 64240, -16323, 69 }, EPoiSize::Large, "Pleasant Park", false, false),
    FPOILocation({ 31257, -77599, 69 }, EPoiSize::Large, "Sweaty Sands", false, false),
    FPOILocation({ 13637, -24219, 69 }, EPoiSize::Large, "Salty Springs", false, false),
    FPOILocation({ 30808, 10669, 69 }, EPoiSize::Large, "Risky Reels", false, false),
    FPOILocation({ 30536, 42295, 69 }, EPoiSize::Large, "Frenzy Farm", false, false),
    FPOILocation({ 17963, 112955, 69 }, EPoiSize::Large, "Dirty Docks", false, false),
    FPOILocation({ 6364, 4866, 69 }, EPoiSize::Large, "Agency", true, true),
    FPOILocation({ -13311, -81033, 69 }, EPoiSize::Large, "Holly Hedges", false, false),
    FPOILocation({ -28997, -31787, 69 }, EPoiSize::Large, "Weeping Woods", false, false),
    FPOILocation({ -64881, -46495, 69 }, EPoiSize::Large, "Slurpy Swamp", false, false),
    FPOILocation({ -77143, -80634, 69 }, EPoiSize::Large, "The Rig", true, true),
    FPOILocation({ -88282, 23038, 69 }, EPoiSize::Large, "Misty Meadows", false, false),
    FPOILocation({ -43937, 52030, 69 }, EPoiSize::Large, "Lazy Lake", false, false),
    FPOILocation({ -68550, 80804, 69 }, EPoiSize::Large, "Catty Corner", false, false),
    FPOILocation({ -39867, 91323, 69 }, EPoiSize::Large, "Retail Row", false, false),
    FPOILocation({ -19544, 105594, 69 }, EPoiSize::Large, "Grotto", true, true),
    
    // Medium POIs - Balanced loot and competition
    FPOILocation({ 107834, -78584, -783 }, EPoiSize::Medium, "Top of The Shark", false, false),
    FPOILocation({ 120009, -84032, -3370 }, EPoiSize::Medium, "The Shark Prison", false, false),
    FPOILocation({ 104078, 26670, -1834 }, EPoiSize::Medium, "Fishsticks Restaurant", false, false),
    FPOILocation({ 94032, 51513, 4693 }, EPoiSize::Medium, "Shadow RadioTower", false, false),
    FPOILocation({ 81669, 90762, 469 }, EPoiSize::Medium, "Kevolution Energy", false, false),
    FPOILocation({ 116567, 113665, -2602 }, EPoiSize::Medium, "The Yacht Deck", false, false),
    FPOILocation({ 60290, -16240, 69 }, EPoiSize::Medium, "PP Shadow Henchmen Base", false, false),
    FPOILocation({ 30437, 69691, 69 }, EPoiSize::Medium, "Shadow Henchmen Gas Station", false, false),
    FPOILocation({ 6426, 7100, 69 }, EPoiSize::Medium, "Agency Interior", false, false),
    FPOILocation({ -68041, 29906, 69 }, EPoiSize::Medium, "Misty Henchmen House", false, false),
    FPOILocation({ -54818, 57400, 69 }, EPoiSize::Medium, "Lazy Lake Houses", false, false),
    FPOILocation({ -92971, 78709, 69 }, EPoiSize::Medium, "Mountain Henchmen Base", false, false),
    FPOILocation({ -38750, 102176, 69 }, EPoiSize::Medium, "Retail Row Shops", false, false),
    FPOILocation({ -73305, -90910, 69 }, EPoiSize::Medium, "The Rig Platform", false, false),
    
    // Small POIs - Lower loot, safer landing spots
    FPOILocation({ 112255, -91220, -3011 }, EPoiSize::Small, "The Shark Choppa Spawn", false, false),
    FPOILocation({ 99820, -82200, -3370 }, EPoiSize::Small, "The Shark Basketball Court", false, false),
    FPOILocation({ 80258, 95433, 69 }, EPoiSize::Small, "Steamy Stacks Building", false, false),
    FPOILocation({ 86691, 102076, 69 }, EPoiSize::Small, "Steamy Stacks Warehouse", false, false),
    FPOILocation({ 109895, 113636, -2986 }, EPoiSize::Small, "The Yacht Lower Deck", false, false),
    FPOILocation({ 106233, 108428, -3762 }, EPoiSize::Small, "The Yacht Island", false, false),
    FPOILocation({ 11188, 4863, 69 }, EPoiSize::Small, "Agency East Building", false, false),
    FPOILocation({ 657, 4334, 69 }, EPoiSize::Small, "Agency West Building", false, false),
    FPOILocation({ 4084, 7167, 69 }, EPoiSize::Small, "Agency North Building", false, false),
    FPOILocation({ 6245, 1741, 69 }, EPoiSize::Small, "Agency South Building", false, false),
    FPOILocation({ -88329, 31859, 69 }, EPoiSize::Small, "Misty Meadows Houses", false, false),
    FPOILocation({ -17383, 112993, 69 }, EPoiSize::Small, "Grotto Entrance", false, false),
    FPOILocation({ -22500, 112479, 69 }, EPoiSize::Small, "Grotto Docks", false, false),
};

// Legacy drop zone locations (for backward compatibility)
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

// Strategic POI Selection Utilities for Ultra-Intelligent Bots
namespace POIUtils {
    inline FVector GetRandomPOIBySize(EPoiSize PreferredSize) {
        std::vector<FPOILocation> FilteredPOIs;
        for (const auto& POI : CategorizedPOIs) {
            if (POI.Size == PreferredSize) {
                FilteredPOIs.push_back(POI);
            }
        }
        
        if (FilteredPOIs.empty()) {
            return CategorizedPOIs[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, CategorizedPOIs.size() - 1)].Location;
        }
        
        return FilteredPOIs[UKismetMathLibrary::GetDefaultObj()->RandomIntegerInRange(0, FilteredPOIs.size() - 1)].Location;
    }
    
    inline FPOILocation GetNearestPOIWithBoss(FVector FromLocation) {
        FPOILocation* NearestBossPOI = nullptr;
        float MinDistance = FLT_MAX;
        auto Math = UKismetMathLibrary::GetDefaultObj();
        
        for (auto& POI : CategorizedPOIs) {
            if (POI.bHasBoss) {
                float Distance = Math->Vector_Distance(FromLocation, POI.Location);
                if (Distance < MinDistance) {
                    MinDistance = Distance;
                    NearestBossPOI = &POI;
                }
            }
        }
        
        if (NearestBossPOI) {
            return *NearestBossPOI;
        }
        
        return CategorizedPOIs[0];
    }
    
    inline FPOILocation GetNearestPOIWithVault(FVector FromLocation) {
        FPOILocation* NearestVaultPOI = nullptr;
        float MinDistance = FLT_MAX;
        auto Math = UKismetMathLibrary::GetDefaultObj();
        
        for (auto& POI : CategorizedPOIs) {
            if (POI.bHasVault) {
                float Distance = Math->Vector_Distance(FromLocation, POI.Location);
                if (Distance < MinDistance) {
                    MinDistance = Distance;
                    NearestVaultPOI = &POI;
                }
            }
        }
        
        if (NearestVaultPOI) {
            return *NearestVaultPOI;
        }
        
        return CategorizedPOIs[0];
    }
    
    inline FVector GetStrategicDropZone(FVector BusLocation, float BusPathAngle, bool bPreferHotDrop) {
        auto Math = UKismetMathLibrary::GetDefaultObj();
        
        EPoiSize PreferredSize = EPoiSize::Medium;
        if (bPreferHotDrop) {
            float Random = Math->RandomFloatInRange(0.0f, 1.0f);
            if (Random < 0.4f) PreferredSize = EPoiSize::Large;
            else if (Random < 0.7f) PreferredSize = EPoiSize::Medium;
            else PreferredSize = EPoiSize::Small;
        } else {
            float Random = Math->RandomFloatInRange(0.0f, 1.0f);
            if (Random < 0.2f) PreferredSize = EPoiSize::Large;
            else if (Random < 0.5f) PreferredSize = EPoiSize::Medium;
            else PreferredSize = EPoiSize::Small;
        }
        
        std::vector<FPOILocation> FilteredPOIs;
        for (const auto& POI : CategorizedPOIs) {
            if (POI.Size == PreferredSize) {
                float Distance = Math->Vector_Distance(BusLocation, POI.Location);
                if (Distance < 30000.0f) {
                    FilteredPOIs.push_back(POI);
                }
            }
        }
        
        if (FilteredPOIs.empty()) {
            return GetRandomPOIBySize(PreferredSize);
        }
        
        return FilteredPOIs[Math->RandomIntegerInRange(0, FilteredPOIs.size() - 1)].Location;
    }
}