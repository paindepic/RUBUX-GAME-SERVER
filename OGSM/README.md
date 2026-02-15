# OGSM (OGS Chapter 2 Season 2 Mod)

Amélioration complète des bots AI pour Fortnite Chapter 2 Season 2.

## 🎮 Nouvelles Fonctionnalités

### 🤖 Système de Construction des Bots (BotBuilding.h)
- **Build Fights** - Les bots construisent pendant les combats
- **90s** - Technique de construction en escalier optimisée
- **Box Fighting** - Les bots construisent des boîtes défensives
- **Ramp Rush** - Construction de rampes pour pousser l'ennemi
- **High Ground Retake** - Reprise de hauteur tactique
- **Turtling** - Construction défensive quand stressés

### 🚁 Système de Véhicules (BotDriving.h)
- **Conduite de Choppa** - Les bots pilotent des hélicoptères
- **Combat Aérien** - Tir depuis le Choppa
- **Conduite de Bateaux** - Navigation sur l'eau
- **Recherche de véhicules** - Les bots trouvent et utilisent les véhicules

### 🏛️ Système de Vault (BotVaultSystem.h)
- **Boss Ch2S2** - TNTina, Deadpool, Skye, Brutus, Midas, Ocean
- **Cartes d'accès** - Loot des cartes après avoir tué un boss
- **Coffres-forts** - Ouverture avec les cartes d'accès
- **Armes Mythiques** - Loot des armes de boss (Midas Drum Gun, etc.)

### 📋 Système de Quêtes (PlayerQuests.h)
- **Quêtes Quotidiennes** - 3 quêtes journalières avec récompenses XP
- **Quêtes Hebdomadaires** - 7 quêtes hebdomadaires
- **Types de Quêtes**:
  - Éliminations
  - Ouvrir des coffres
  - Survivre aux phases de tempête
  - Parcourir des distances
  - Éliminer des boss
  - Ouvrir des coffres-forts
  - Utiliser des véhicules
  - Terminer dans le top 10/top 1
  - Éliminations avec armes spécifiques
- **Notifications** - Alertes de complétion de quêtes
- **Récompenses XP** - Distribution automatique d'XP

### 🪂 Plongée Stratégique (Enhanced AIDropZone)
- **Plongée 90s** - Vitesse de descente optimisée (~60 m/s)
- **Déploiement du Planeur** - À 300m d'altitude
- **Coffres de Toit** - Atterrissage stratégique sur les toits
- **Ciblage de Boss POI** - 40% de chance de cibler les POIs avec boss

### ⚙️ Flags de Configuration (Globals.h)
```cpp
bool bBotBuildingEnabled = true;       // Construction des bots
bool bBotVehicleEnabled = true;        // Véhicules des bots
bool bVaultSystemEnabled = true;       // Système de vault
bool bQuestSystemEnabled = true;       // Système de quêtes
bool bStrategicDroppingEnabled = true; // Plongée stratégique
```

## 🛠️ Compilation

Utilisez Visual Studio 2022 avec les outils de build v143.
Ouvrez `OGSM.sln` et compilez en Release x64.

## 📝 Crédits

Basé sur OGS 12.41 - Made with love by ObsessedTech
Modifications OGSM Chapter 2 Season 2
