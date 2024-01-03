// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "MyPlayer.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGlassesRemovedSignature);

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	EPS_Idle UMETA(DisplayName="Idle"),
	EPS_Action UMETA(DisplayName="Action")
};

UENUM(BlueprintType)
enum class EPlayerGadgets : uint8
{
	EPG_Melee UMETA(DisplayName="Melee"),
	EPG_Vaccum UMETA(DisplayName = "Vaccum"),
	EPG_Disguise UMETA(DisplayName = "Disguise"),
	EPG_NoiseMaker UMETA(DisplayName = "Noise Maker"),
	EPG_Decoy UMETA(DisplayName = "Decoy"),
	EPG_None UMETA(DisplayName = "None"),
};

class AVaccum;
class AThrowables;
class ABaseballBat;
class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class PROJECTCATCH_API AMyPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AMyPlayer();

	virtual void NotifyJumpApex() override;

	UFUNCTION(BlueprintCallable, meta=(DisplayName="On Throw Ball"))
	void ThrowBall();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Wearing Disguise"))
	bool GetWearingDisguise() const;
	
	FOnGlassesRemovedSignature OnGlassesRemoved;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Wheel Pressed"))
	void WheelPressed(const FInputActionValue& Value);
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Wheel Released"))
	void WheelReleased(const FInputActionValue& Value);
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Wheel Click"))
	void WheelClick();
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Player Gadget"))
	void SetPlayerGadget(EPlayerGadgets Gadgets, bool bForce = false);

	void Jumping();
	void HoldWalk(bool bWalk);
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void ActionPressed();
	void ActionReleased();
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> WalkAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> GadgetAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> GadgetWheelAction;

	UPROPERTY(BlueprintReadOnly, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	EPlayerGadgets CurrentGadget;
	UPROPERTY(BlueprintReadWrite, Category = State, meta = (AllowPrivateAccess = "true"))
	EPlayerStates CurrentState;
	UPROPERTY(BlueprintReadOnly, Category = Action, meta = (AllowPrivateAccess = "true"))
	bool bPressingActionButton;
	UPROPERTY(BlueprintReadWrite, Category = Action, meta = (AllowPrivateAccess = "true"))
	bool bUsingAction;
	UPROPERTY(BlueprintReadWrite, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	bool bSoft;
	UPROPERTY(BlueprintReadWrite, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	bool bShowingWidget;
	UPROPERTY(BlueprintReadOnly, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> GadgetActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AVaccum> VaccumClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AThrowables> NoiseMakerClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AThrowables> DecoyClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseballBat> BaseballBatClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMesh> DisguiseGadgetMesh;
	UPROPERTY(BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	FVector2D MouseDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true", DisplayName="Camera"))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true", DisplayName = "Spring Arm"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(BlueprintReadOnly, Category = Gadget, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> ActiveBall;
	
	float SneakSpeed;
	float DefaultSpeed;
};
