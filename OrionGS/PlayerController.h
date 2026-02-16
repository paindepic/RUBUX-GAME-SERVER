#pragma once
#include "Utils.h"

using namespace std;

namespace PlayerController
{
    extern void (*ClientOnPawnDiedOG)(APlayerController* PlayerController, FFortPlayerDeathReport);
    extern void (*GetPlayerViewPointOG)(APlayerController* PlayerController, FVector outLocation, FRotator outRotation);
    extern void (*ServerReadyToStartMatchOG)(AFortPlayerController* PlayerController);
    extern __int64 (*OnDamageServerOG)(ABuildingSMActor* Actor, float Damage, FGameplayTagContainer DamageTags, FVector Momentum, FHitResult HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle EffectContext);
    extern void (*ServerLoadingScreenDroppedOG)(AFortPlayerController* PlayerController);
    extern void (*ServerSetInAircraftOG)(AFortPlayerStateAthena* PlayerState, bool bInAircraft);

    void ServerAcknowledgePossession(APlayerController* PlayerController, APawn* Pawn);
    bool AllPlayersConnected(AFortGameModeAthena* GameMode);
    void GetPlayerViewPoint(APlayerController* PlayerController, FVector& outLocation, FRotator& outRotation);
    void ClientOnPawnDied(AFortPlayerControllerAthena* PlayerController, FFortPlayerDeathReport);
    void ServerReadyToStartMatch(AFortPlayerController* PlayerController);
    void ServerAttemptAircraftJump(UFortControllerComponent_Aircraft* Comp, FRotator ClientRot);
    void ServerLoadingScreenDropped(AFortPlayerController* PlayerController);
    void ServerPlayEmoteItemHook(AFortPlayerController* PlayerController, UFortItemDefinition* EmoteAsset, float EmoteRandomNumber);
    void ServerExecuteInventoryItem(AFortPlayerController* PC, FGuid Guid);
    void ServerCheat(AFortPlayerControllerAthena* PC, FString msg);
    void ServerCreateBuildingActor(AFortPlayerControllerAthena* PC, FCreateBuildingActorData Data);
    __int64 OnDamageServer(ABuildingSMActor* Actor, float Damage, FGameplayTagContainer DamageTags, FVector Momentum, FHitResult HitInfo, AFortPlayerControllerAthena* InstigatedBy, AActor* DamageCauser, FGameplayEffectContextHandle EffectContext);
    void ServerEditBuildingActor(AFortPlayerControllerAthena* PC, ABuildingSMActor* BuildingActorToEdit, TSubclassOf<ABuildingSMActor> NewBuildingClass, uint8 RotationIterations, bool bMirrored);
    void ServerEndEditingBuildingActor(AFortPlayerControllerAthena* PC, ABuildingSMActor* BuildingActorToEdit);
    void ServerBeginEditingBuildingActor(AFortPlayerControllerAthena* PC, ABuildingSMActor* BuildingActorToEdit);
    void ServerAttemptInventoryDrop(AFortPlayerControllerAthena* PC, FGuid ItemGuid, int Count, bool bTrash);
    void ServerRepairBuildingActorHook(AFortPlayerController* PC, ABuildingSMActor* Actor);
    void ServerReturnToMainMenu(AFortPlayerControllerAthena* PC);
    void ServerReviveFromDBNO(AFortPlayerPawnAthena* Pawn, AFortPlayerControllerAthena* Instigator);
    void ServerSetInAircraft(AFortPlayerStateAthena* PlayerState, bool bInAircraft);

    namespace Abilities
    {
        extern FGameplayAbilitySpecHandle(*GiveAbilityOG)(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle* OutHandle, FGameplayAbilitySpec NewSpec);
        extern bool (*InternalTryActivateAbility)(UAbilitySystemComponent* AbilitySystemComp, FGameplayAbilitySpecHandle AbilityToActivate, FPredictionKey InPredictionKey, UGameplayAbility** OutInstancedAbility, void* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData);

        FGameplayAbilitySpecHandle GiveAbility(UAbilitySystemComponent* AbilitySystemComponent, UClass* AbilityClass, UObject* SourceObject = nullptr, bool RemoveAfterActivation = false);
        FGameplayAbilitySpec* FindAbilityFromSpecHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle& Handle);
        FGameplayAbilitySpec* FindAbilityFromClass(UAbilitySystemComponent* AbilitySystemComponent, UClass* Class);
        void InternalServerTryActivateAbilityHook(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle, bool InputPressed, FPredictionKey& PredictionKey, FGameplayEventData* TriggerEventData);
    }
}