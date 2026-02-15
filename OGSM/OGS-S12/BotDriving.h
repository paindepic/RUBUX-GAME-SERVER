#pragma once
#include "framework.h"

// Forward declaration
struct PlayerBot;

enum class EBotVehicleState : uint8 {
    NoVehicle,
    Entering,
    Driving,
    Exiting,
    Combat
};

enum class EVehicleType : uint8 {
    None,
    Choppa,
    Boat,
    Car,
    Truck
};

struct FVehicleInfo {
    AActor* Vehicle;
    EVehicleType Type;
    FVector Location;
    bool bOccupied;
};

namespace BotDriving {
    inline EVehicleType GetVehicleType(AActor* Vehicle);
    inline bool IsVehicle(AActor* Actor);
    inline AActor* FindNearestVehicle(PlayerBot* bot, float MaxDistance = 5000.0f);
    inline bool ShouldUseVehicle(PlayerBot* bot, FVector TargetLocation);
    inline bool EnterVehicle(PlayerBot* bot, AActor* Vehicle);
    inline void ExitVehicle(PlayerBot* bot);
    inline void DriveToLocation(PlayerBot* bot, FVector TargetLocation);
    inline void CombatFlyChoppa(PlayerBot* bot, AActor* Target);
    inline void UpdateVehicleState(PlayerBot* bot);
    inline void DriveBoat(PlayerBot* bot, FVector TargetLocation);

    inline EVehicleType GetVehicleType(AActor* Vehicle) {
        if (!Vehicle) return EVehicleType::None;

        std::string Name = Vehicle->GetName();
        if (Name.contains("Hoagie") || Name.contains("Choppa") || Name.contains("Helicopter")) {
            return EVehicleType::Choppa;
        }
        else if (Name.contains("Meatball") || Name.contains("Boat") || Name.contains("Motorboat")) {
            return EVehicleType::Boat;
        }
        else if (Name.contains("Valet") || Name.contains("SportsCar")) {
            return EVehicleType::Car;
        }
        else if (Name.contains("Truck") || Name.contains("BigRig")) {
            return EVehicleType::Truck;
        }
        return EVehicleType::None;
    }

    inline bool IsVehicle(AActor* Actor) {
        if (!Actor) return false;
        return Actor->IsA(AFortAthenaVehicle::StaticClass()) ||
            Actor->IsA(AFortPhysicsPawn::StaticClass()) ||
            GetVehicleType(Actor) != EVehicleType::None;
    }

    inline AActor* FindNearestVehicle(PlayerBot* bot, float MaxDistance = 5000.0f) {
        if (!bot || !bot->Pawn) return nullptr;

        TArray<AActor*> Vehicles;
        UGameplayStatics::GetDefaultObj()->GetAllActorsOfClass(UWorld::GetWorld(), AFortAthenaVehicle::StaticClass(), &Vehicles);

        AActor* NearestVehicle = nullptr;
        float ClosestDist = MaxDistance;

        for (size_t i = 0; i < Vehicles.Num(); i++) {
            AActor* Vehicle = Vehicles[i];
            if (!Vehicle) continue;

            EVehicleType Type = GetVehicleType(Vehicle);
            if (Type == EVehicleType::None) continue;

            float Dist = Vehicle->GetDistanceTo(bot->Pawn);
            if (Dist < ClosestDist) {
                AFortAthenaVehicle* FortVehicle = Cast<AFortAthenaVehicle>(Vehicle);
                if (FortVehicle && FortVehicle->GetDriver()) continue;

                ClosestDist = Dist;
                NearestVehicle = Vehicle;
            }
        }

        Vehicles.Free();
        return NearestVehicle;
    }

    inline bool ShouldUseVehicle(PlayerBot* bot, FVector TargetLocation) {
        if (!bot || !bot->Pawn) return false;
        if (!Globals::bBotVehicleEnabled) return false;

        float Distance = FVector::Distance(bot->Pawn->K2_GetActorLocation(), TargetLocation);
        return Distance > 3000.0f;
    }

    inline bool EnterVehicle(PlayerBot* bot, AActor* Vehicle) {
        if (!bot || !bot->Pawn || !bot->PC || !Vehicle) return false;

        AFortAthenaVehicle* FortVehicle = Cast<AFortAthenaVehicle>(Vehicle);
        if (!FortVehicle) return false;

        if (FortVehicle->GetDriver()) return false;

        float Dist = Vehicle->GetDistanceTo(bot->Pawn);
        if (Dist > 300.0f) {
            bot->PC->MoveToActor(Vehicle, 100.0f, true, false, true, nullptr, true);
            return false;
        }

        FVector VehicleLoc = Vehicle->K2_GetActorLocation();
        FVector BotLoc = bot->Pawn->K2_GetActorLocation();
        FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(BotLoc, VehicleLoc);

        bot->PC->SetControlRotation(LookRot);
        bot->PC->K2_SetActorRotation(LookRot, true);

        FortVehicle->SetDriver(bot->Pawn);

        return true;
    }

    inline void ExitVehicle(PlayerBot* bot) {
        if (!bot || !bot->Pawn) return;

        AFortPlayerPawnAthena* Pawn = bot->Pawn;
        if (Pawn->GetVehicle()) {
            Pawn->GetVehicle()->SetDriver(nullptr);
        }
    }

    inline void DriveToLocation(PlayerBot* bot, FVector TargetLocation) {
        if (!bot || !bot->Pawn || !bot->PC) return;

        AFortAthenaVehicle* Vehicle = bot->Pawn->GetVehicle();
        if (!Vehicle) {
            bot->CurrentVehicle = nullptr;
            bot->VehicleState = EBotVehicleState::NoVehicle;
            return;
        }

        bot->CurrentVehicle = Vehicle;
        bot->VehicleState = EBotVehicleState::Driving;

        FVector VehicleLoc = Vehicle->K2_GetActorLocation();
        FVector Direction = (TargetLocation - VehicleLoc).GetNormalized();
        float Distance = FVector::Distance(VehicleLoc, TargetLocation);

        if (Distance < 500.0f) {
            ExitVehicle(bot);
            bot->CurrentVehicle = nullptr;
            bot->VehicleState = EBotVehicleState::NoVehicle;
            return;
        }

        FRotator TargetRot = UKismetMathLibrary::MakeRotFromXZ(Direction, FVector(0, 0, 1));
        bot->PC->SetControlRotation(TargetRot);
        bot->PC->K2_SetActorRotation(TargetRot, true);

        bot->Pawn->AddMovementInput(Direction, 1.0f, true);

        if (UKismetMathLibrary::RandomBoolWithWeight(0.1f)) {
            bot->Pawn->Jump();
        }
    }

    inline void CombatFlyChoppa(PlayerBot* bot, AActor* Target) {
        if (!bot || !bot->Pawn || !bot->PC || !Target) return;

        AFortAthenaVehicle* Vehicle = bot->Pawn->GetVehicle();
        if (!Vehicle) return;

        EVehicleType Type = GetVehicleType((AActor*)Vehicle);
        if (Type != EVehicleType::Choppa) return;

        bot->VehicleState = EBotVehicleState::Combat;

        FVector VehicleLoc = Vehicle->K2_GetActorLocation();
        FVector TargetLoc = Target->K2_GetActorLocation();
        FVector Direction = (TargetLoc - VehicleLoc).GetNormalized();
        float Distance = FVector::Distance(VehicleLoc, TargetLoc);

        FVector CircleOffset = FVector(
            UKismetMathLibrary::Sin(UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld()) * 2.0f) * 800.0f,
            UKismetMathLibrary::Cos(UGameplayStatics::GetDefaultObj()->GetTimeSeconds(UWorld::GetWorld()) * 2.0f) * 800.0f,
            200.0f
        );

        FVector OrbitTarget = TargetLoc + CircleOffset;
        FVector FlyDirection = (OrbitTarget - VehicleLoc).GetNormalized();

        FRotator TargetRot = UKismetMathLibrary::MakeRotFromXZ(FlyDirection, FVector(0, 0, 1));
        bot->PC->SetControlRotation(TargetRot);

        bot->Pawn->AddMovementInput(FlyDirection, 0.8f, true);

        if (Distance < 1500.0f && bot->PC->LineOfSightTo(Target, VehicleLoc, true)) {
            bot->Pawn->PawnStartFire(0);
        }
        else {
            bot->Pawn->PawnStopFire(0);
        }
    }

    inline void UpdateVehicleState(PlayerBot* bot) {
        if (!bot || !bot->Pawn || !bot->PC) return;
        if (!Globals::bBotVehicleEnabled) return;

        auto GameState = (AFortGameStateAthena*)UWorld::GetWorld()->GameState;
        if (!GameState || !GameState->SafeZoneIndicator) return;

        FVector SafeZoneCenter = GameState->SafeZoneIndicator->NextCenter;
        FVector BotLoc = bot->Pawn->K2_GetActorLocation();

        float DistToSafeZone = FVector::Distance(FVector(BotLoc.X, BotLoc.Y, 0), FVector(SafeZoneCenter.X, SafeZoneCenter.Y, 0));

        if (bot->VehicleState == EBotVehicleState::NoVehicle) {
            if (DistToSafeZone > 2000.0f || (bot->BotState == EBotState::MovingToSafeZone && DistToSafeZone > 1500.0f)) {
                AActor* Vehicle = FindNearestVehicle(bot);
                if (Vehicle) {
                    if (EnterVehicle(bot, Vehicle)) {
                        bot->CurrentVehicle = Vehicle;
                        bot->VehicleState = EBotVehicleState::Driving;
                    }
                }
            }
        }
        else if (bot->VehicleState == EBotVehicleState::Driving) {
            if (bot->BotState == EBotState::MovingToSafeZone) {
                DriveToLocation(bot, SafeZoneCenter);
            }
            else if (bot->BotState == EBotState::LookingForPlayers && bot->NearestPlayerActor) {
                EVehicleType Type = GetVehicleType(bot->CurrentVehicle);
                if (Type == EVehicleType::Choppa) {
                    CombatFlyChoppa(bot, bot->NearestPlayerActor);
                }
                else {
                    DriveToLocation(bot, bot->NearestPlayerActor->K2_GetActorLocation());
                }
            }
        }
    }

    inline void DriveBoat(PlayerBot* bot, FVector TargetLocation) {
        if (!bot || !bot->Pawn || !bot->PC) return;

        AFortAthenaVehicle* Vehicle = bot->Pawn->GetVehicle();
        if (!Vehicle) return;

        EVehicleType Type = GetVehicleType((AActor*)Vehicle);
        if (Type != EVehicleType::Boat) return;

        FVector VehicleLoc = Vehicle->K2_GetActorLocation();
        FVector Direction = (TargetLocation - VehicleLoc).GetNormalized();
        float Distance = FVector::Distance(VehicleLoc, TargetLocation);

        if (Distance < 400.0f) {
            ExitVehicle(bot);
            return;
        }

        FRotator TargetRot = UKismetMathLibrary::MakeRotFromXZ(Direction, FVector(0, 0, 1));
        bot->PC->SetControlRotation(TargetRot);

        bot->Pawn->AddMovementInput(Direction, 1.0f, true);
    }
}

// Include PlayerBots.h after declarations to avoid circular dependencies
#include "PlayerBots.h"
