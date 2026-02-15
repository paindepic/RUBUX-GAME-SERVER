#pragma once

namespace Globals {
    bool bIsProdServer = false;

    bool bCreativeEnabled = false;
    bool bSTWEnabled = false;
    bool bEventEnabled = false;

    bool bBossesEnabled = true;
    bool bBotsEnabled = true;

    bool bUseLegacyAI_MANG = true; // Keep this on true bro, I am NOT doing this!

    // OGSM Chapter 2 Season 2 - New AI Features
    bool bBotBuildingEnabled = true;       // Bot building (90s, box fighting, ramp rush)
    bool bBotVehicleEnabled = true;        // Bot vehicle AI (Choppa, boats)
    bool bVaultSystemEnabled = true;       // Boss keycards and vault looting
    bool bQuestSystemEnabled = true;       // Player daily/weekly quest system
    bool bStrategicDroppingEnabled = true; // Strategic landing on roof chests

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
