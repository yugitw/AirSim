#include "ComputerVisionPawn.h"
#include "Engine/World.h"
#include "ManualPoseController.h"


AComputerVisionPawn::AComputerVisionPawn()
{
    static ConstructorHelpers::FClassFinder<APIPCamera> pip_camera_class(TEXT("Blueprint'/AirSim/Blueprints/BP_PIPCamera'"));
    pip_camera_class_ = pip_camera_class.Succeeded() ? pip_camera_class.Class : nullptr;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    camera_front_center_base_ = CreateDefaultSubobject<USceneComponent>(TEXT("camera_front_center_base_"));
    camera_front_center_base_->SetRelativeLocation(FVector(0, 0, 0)); //front
    camera_front_center_base_->SetupAttachment(RootComponent);
}

void AComputerVisionPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation,
    FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
    pawn_events_.getCollisionSignal().emit(MyComp, Other, OtherComp, bSelfMoved, HitLocation,
        HitNormal, NormalImpulse, Hit);
}

void AComputerVisionPawn::initializeForBeginPlay()
{
    //put camera little bit above vehicle
    FTransform camera_transform(FVector::ZeroVector);
    FActorSpawnParameters camera_spawn_params;
    camera_spawn_params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    camera_spawn_params.Name = "camera_front";
    camera_front_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, camera_transform, camera_spawn_params);
    camera_front_->AttachToComponent(camera_front_center_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = "camera_left";
    camera_left_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, 
        FTransform(FRotator(0, -90, 0), FVector::ZeroVector), camera_spawn_params);
    camera_left_->AttachToComponent(camera_front_center_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = "camera_right";
    camera_right_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, 
        FTransform(FRotator(0, 90, 0), FVector::ZeroVector), camera_spawn_params);
    camera_right_->AttachToComponent(camera_front_center_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = "camera_up";
    camera_up_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, 
        FTransform(FRotator(90.f, 0, 0), FVector::ZeroVector), camera_spawn_params);
    camera_up_->AttachToComponent(camera_front_center_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = "camera_down";
    camera_down_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, 
        FTransform(FRotator(-90.f, 0, 0), FVector::ZeroVector), camera_spawn_params);
    camera_down_->AttachToComponent(camera_front_center_base_, FAttachmentTransformRules::KeepRelativeTransform);

    camera_spawn_params.Name = "camera_back";
    camera_back_ = this->GetWorld()->SpawnActor<APIPCamera>(pip_camera_class_, 
        FTransform(FRotator(0, -180.f, 0), FVector::ZeroVector), camera_spawn_params);
    camera_back_->AttachToComponent(camera_front_center_base_, FAttachmentTransformRules::KeepRelativeTransform);

    manual_pose_controller_ = NewObject<UManualPoseController>(this, "ComputerVision_ManualPoseController");
    manual_pose_controller_->initializeForPlay();
    manual_pose_controller_->setActor(this);
}

const common_utils::UniqueValueMap<std::string, APIPCamera*> AComputerVisionPawn::getCameras() const
{
    common_utils::UniqueValueMap<std::string, APIPCamera*> cameras;
    cameras.insert_or_assign("front", camera_front_);
    cameras.insert_or_assign("left", camera_left_);
    cameras.insert_or_assign("right", camera_right_);
    cameras.insert_or_assign("up", camera_up_);
    cameras.insert_or_assign("down", camera_down_);
    cameras.insert_or_assign("back", camera_back_);


    cameras.insert_or_assign("0", camera_front_);
    cameras.insert_or_assign("1", camera_left_);
    cameras.insert_or_assign("2", camera_right_);
    cameras.insert_or_assign("3", camera_up_);
    cameras.insert_or_assign("4", camera_down_);
    cameras.insert_or_assign("5", camera_back_);

    cameras.insert_or_assign("fpv", camera_front_);
    cameras.insert_or_assign("", camera_front_);

    return cameras;
}

void AComputerVisionPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    camera_front_ = nullptr;
    camera_left_ = nullptr;
    camera_right_ = nullptr;
    camera_up_ = nullptr;
    camera_down_ = nullptr;
    camera_back_ = nullptr;

    camera_front_center_base_ = nullptr;

    manual_pose_controller_ = nullptr;
}

void AComputerVisionPawn::Tick(float Delta)
{
    Super::Tick(Delta);
    pawn_events_.getPawnTickSignal().emit(Delta);

    //update ground level
    if (manual_pose_controller_->getActor() == this) {
        manual_pose_controller_->updateActorPose(Delta);
    }
}

void AComputerVisionPawn::BeginPlay()
{
    Super::BeginPlay();
}



