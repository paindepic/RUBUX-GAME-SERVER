#pragma once

namespace Globals {
    bool bIsProdServer = false;

    bool bCreativeEnabled = false;
    bool bSTWEnabled = false;
    bool bEventEnabled = false;

    bool bBossesEnabled = true;
    bool bBotsEnabled = true;

    bool bUseLegacyAI_MANG = true; // Keep this on true bro, I am NOT doing this!

    // Ultra-Intelligent Bot Feature Flags
    bool bAdvancedAIEnabled = true;          // Master switch for enhanced AI features
    bool bStrategicLandingEnabled = true;    // Smart POI-based landing with size preference
    bool bBuildFightsEnabled = true;         // Enable 90s, box fights, ramp rush, turtling
    bool bWeaponSwitchingEnabled = true;     // Smart weapon selection based on range
    bool bVehicleSystemEnabled = true;       // Enable Choppa/boat detection and usage
    bool bFarmingSystemEnabled = true;       // Intelligent resource farming
    bool bBossVaultSystemEnabled = true;     // Boss combat, keycard looting, vault opening
    bool bQuestSystemEnabled = true;         // Daily quests for players with XP rewards

    bool LateGame = false;
    bool Automatics = false;
    bool BattleLab = false;
    bool Blitz = false;
    bool StormKing = false;
    bool Arsenal = false;
    bool TeamRumble = false;
    bool SolidGold = false;
    bool UnVaulted = false;
    bool Siphon = false;
    bool Arena = false;

    int MaxBotsToSpawn = 100;
    int MinPlayersForEarlyStart = 90;
}
