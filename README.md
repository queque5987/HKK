
- [구현한 기능](#)
   
	- [1-1. 환경(Foliage) 피드백 시스템 구현](#환경(Foliage)-피드백-시스템-구현)

- [수정한 기능](#)

	- [2-1. 리슨서버에서의 UI 중복 생성 이슈](#리슨서버에서의-UI-중복-생성-이슈)

	- [2-2. 장비 장착 시스템 구현 중 싱크 이슈](#장비-장착-시스템-구현-중-싱크-이슈)


# 구현한 기능

-----
# 환경(Foliage) 피드백 시스템 구현
-----

## 아이디어

0. 수풀 사이를 비집고 들어갈 때 수풀의 반응을 구현하고자함.
1. Wind Directional Source를 override하여 특정 액터와 충돌 중인 부분에 Vector를 더하는 방식으로 구현하면 될 것 같음
2. Perlin Noise를 활용하여 구현하고자 하였으나, 3차원 공간에서 성능 저하 이슈가 있다는 점을 발견
3. RenderTarget을 사용하여 충돌 중인 좌표를 gpu를 사용하여 계산, 적용하기로 결정

## 구현

### Foliage Interact Material

![FoliageTest_Trial1](https://github.com/user-attachments/assets/8c8bc5a6-3fd9-4ca1-aac3-f0e1f03958ed)

![FoliageTest_Material1](https://github.com/user-attachments/assets/6bdd571f-adb7-413c-b87a-5872256f1486)

기존 마켓플레이스 Foliage의 머티리얼에 WPO를 더하여 구현

RenderTarget을 4등분하여 각각 XY, XZ, YZ 세 좌표면을 저장하여 3차원 좌표 내의 3차원 벡터를 각 세 평면에 나누어 저장하였습니다.

RenderTarget 중 R 값에는 중심으로부터 임의의 위치까지 향하는 방향 벡터에서 Theta를 산출하여 저장하였습니다.

Center Position을 파라미터로 받아 Texture Coordinate에 뺀 후, Normalize하여 산출하였고, Atan2를 사용하여 Theta 값으로 치환하여 저장하였습니다. 

G 값에는 중점으로부터의 거리를 저장하였습니다.

파라미터로 입력 받은 Center Position과 Radius를 활용하여 Radial Gradient Exponential으로 특정 좌표로부터 원형 그라데이션을 그렸습니다.

### Foliage Interact Volume

```c++
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoliageInteractVolume.generated.h"

class UBoxComponent;
class UMaterialInstance;
class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;

UCLASS()
class HKK_API AFoliageInteractVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AFoliageInteractVolume();

protected:
	/*Box Should Be 1:1:1 Size For Now*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> FoliageInteractionBound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstance> MI_FoliageInteractRenderTargetGenerator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstanceDynamic> MID_FoliageInteractRenderTargetGenerator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextureRenderTarget2D> RT_FoliageInteractImpact;

	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	bool IsInside(const FVector& Location);
	void SetImpulse(const FVector& ImpactLocation, float Radius);
};
```

Foliage의 충돌을 연산 할 Volume과 해당 Volume 안에서 연산을 수행할 컴포넌트를 구현하였습니다.

> FoliageInteractionBound : 충돌을 계산할 바운더리
> 
> MI_FoliageInteractRenderTargetGenerator : 충돌을 계산하는 머티리얼 포인터
> 
> MID_FoliageInteractRenderTargetGenerator : 위 머티리얼의 다이나믹 인스턴스
> 
> RT_FoliageInteractImpact : 충돌 정보를 저장할 렌더타깃의 포인터
> 
> bool IsInside(const FVector& Location) : 바운더리 내에 존재하는지 확인하는 함수


```C++
void AFoliageInteractVolume::SetImpulse(const FVector& ImpactLocation, float Radius)
{
	if (RT_FoliageInteractImpact == nullptr || MID_FoliageInteractRenderTargetGenerator == nullptr) return;
	FBox Bounds = FoliageInteractionBound->Bounds.GetBox();
	FVector RelativeLocation = ImpactLocation;
	RelativeLocation.X = (RelativeLocation.X - Bounds.Min.X) / Bounds.GetSize().X;
	RelativeLocation.Y = (RelativeLocation.Y - Bounds.Min.Y) / Bounds.GetSize().Y;
	RelativeLocation.Z = (RelativeLocation.Z - Bounds.Min.Z) / Bounds.GetSize().Z;

	UE_LOG(LogTemp, Log, TEXT("Scaled Location : %s"), *RelativeLocation.ToString());
	FLinearColor Color{ (float)RelativeLocation.X, (float)RelativeLocation.Y, (float)RelativeLocation.Z, 0.f };

	MID_FoliageInteractRenderTargetGenerator->SetScalarParameterValue(TEXT("Radius"), Radius / FoliageInteractionBound->Bounds.GetBox().GetSize().X);
	MID_FoliageInteractRenderTargetGenerator->SetVectorParameterValue(TEXT("CenterPosition"), Color);
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_FoliageInteractImpact);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT_FoliageInteractImpact, MID_FoliageInteractRenderTargetGenerator);
}
```

> void SetImpulse(const FVector& ImpactLocation, float Radius) : 충돌을 지정하는 함수
>
> 파라미터를 설정 후 DrawMaterialToRenderTarget을 사용하여 RT_FoliageInteractImpact에 저장

저장된 RenderTarget은 Foliage의 머티리얼에서 WorldPositionOffset에 더하여 구현.

## 1차 개선점

### 1. Direction 관련 버그 발견

RenderTarget의 포맷은 채널당 16비트(RTF_RGBA16f)를 사용하였으나 Material을 RenderTarget으로 변환하는 과정에서 음수가 소실되는 현상을 발견.

Normalize시 기존 -1 ~ 1 사이의 범위를 0 ~ 1 범위로 매핑하여 해결하였음.

### 2. 확장성을 고려한 분산

Theta로 변환하여 저장할 경우 RGBA 중 하나의 채널만을 사용하여 경제적이나, 여러개의 충돌한 액터의 벡터를 합산하는 데 불필요한 연산이 요구됨

따라서 XY 좌표면의 경우 방향벡터의 X, Y 값을 각각 저장하는 방식으로 변경하여, 여러개의 RenderTarget을 병합하기 용이하도록 하였음.

![FoliageTest_Trial2](https://github.com/user-attachments/assets/1c8d6ffc-026e-4ed8-a99f-3f459b64c92b)

## 2차 개선점

### 1. 여러개의 Impulse 병합

여러개의 Impulse가 들어왔을 경우도 고려하였습니다. 각 Impulse가 Volume에 들어오거나 나갈 때마다

Materail Instance Dynamic을 생성 / 소멸시킨다면 오버헤드가 클 것이라 판단하였습니다.

따라서 Material에 충돌이 일어나고 있는 부분을 여러개의 (현재 기준 4개) 파라미터로 대입할 수 있게하였고,

각각의 충돌을 합쳐 ( R, G에 저장한 방향벡터의 경우 단순 Add 노드로 구현 가능, B의 경우 최대값으로 구현) 병렬로 처리할 수 있도록 하였습니다.

```C++
TQueue<TScriptInterface<IICharacterMovement>> tempQueue;
for (int8 i = 0; i < 4; i++)
{
	//if (InteractingSourceComponentQueue.IsEmpty()) continue;
	TScriptInterface<IICharacterMovement> SourceComponent;
	InteractingSourceComponentQueue.Dequeue(SourceComponent);
	//if (SourceComponent.GetObject() == nullptr || !IsInside(SourceComponent->_GetLocation())) continue;  <<  SetImpulse 내부에서 널포인터일 경우 i번째 파라미터를 초기화하기 때문에 널포인터 체크 하지 않음
	SetImpulse(SourceComponent, i);
	if (SourceComponent.GetObject() == nullptr) continue;
	if (IsInside(SourceComponent->_GetLocation()))
	{
		tempQueue.Enqueue(SourceComponent); // Inside Volume : Push Again
	}
	else
	{
		InteractingSourceComponentSet.Remove(SourceComponent);	// Outside Volume : Remove From Set
	}
}
for (int8 i = 0; i < 4; i++)
{
	if (tempQueue.IsEmpty()) break;
	TScriptInterface<IICharacterMovement> SourceComponent;
	tempQueue.Dequeue(SourceComponent);
	if (SourceComponent.GetObject() == nullptr) continue;
	InteractingSourceComponentQueue.Enqueue(SourceComponent);
}
if (RT_FoliageInteractImpact != nullptr && MID_FoliageInteractRenderTargetGenerator != nullptr)
{
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT_FoliageInteractImpact, MID_FoliageInteractRenderTargetGenerator);
}
```

기존의 충돌한 액터의 컴포넌트(UFoliageInteractSourceComponent)의 Tick에서 RenderTarget을 업데이트 하는 방식에서

지정한 만큼의(위 코드 기준 4개) 액터를 한 Tick에서 처리하는 FIFO 방식으로 구현하였습니다.

이렇게 한다면 지정한 만큼의 파라미터를 동시에 처리하여 합칠 수 있으며, 하드웨어 성능에 따라 병렬적으로 처리할 액터의 수를 제한할 수 있습니다.

### 2. Trail 남기기

이전의 RenderTarget을 초기화하는 부분을 제거하고, 현재 Tick에서 계산한 RenderTarget과 병합하는 노드를 추가하였습니다.

매 연산마다 이전 RenderTarget의 B(Impulse 벡터의 크기에 해당)를 일정 수준 감소시키고, 현재의 RenderTarget과 병합하여 구현하였습니다.

R, G에 각각 벡터의 (X, Y), (X, Z), (Y, Z) 축에 해당하는 값을 저장해두었기 때문에 R, G 값은 더한 뒤, Normalize하였고, B의 경우 최대 값을 갖도록 구현하였습니다.

![FoliageTest_Trail3_Trail](https://github.com/user-attachments/assets/74690545-3ce6-4a5a-9f1e-21cc43846d33)

> Trail(이전 프레임의 연산된 RenderTarget 병합) 적용한 모습

![FoliageTest_Trial3_Stress](https://github.com/user-attachments/assets/aebb0e00-d0be-4cdd-8488-5b054d55e5ec)

> 디버깅용 액터를 소환하여 테스트하는 모습

![FoliageTest_Trial3_StressWithin](https://github.com/user-attachments/assets/a831e494-3cdf-429d-b879-10cc8d78724c)

> Lifespan 약 10초의 액터를 대략 400개 유지하며 실시한 테스트
> 한 프레임당 4개씩 연산하여 성능 상 오버헤드는 크지 않으나, 액터의 속도가 빨라질수록 부자연스럽게 연결됨

## 차후 개선점

1. 성능에 따른 병렬 처리 개수 제한 설정

2. 캐릭터의 Bone마다 UFoliageInteractSourceComponent를 부착하여 디테일한 상호작용

3. 거리에 따라 액터에 부착된 Component의 개수를 조절하여 최적화

-----
# 리슨서버에서의 UI 중복 생성 이슈
-----

1. 피격 시, 피격당한 캐릭터의 PlayerState 내부 CurrHP의 체력 감소
2. CurrHP 감소하며 On_Rep 호출
3. On_Rep에서는 델리게이트(FOnUpdateStatFloat) 브로드캐스트
4. 위 델리게이트에 바인드되어 있는 위젯(Widget_HUD)의 멤버 함수 (OnUpdatePlayerStatFloat) 호출
5. 해당 함수에서 HPBar 갱신

타 클라이언트에서의 UI는 체력이 감소하면서 체력바도 같이 성공적으로 갱신됨
다만, Server의 주체 (리슨 서버)에서는 감소하지 않는 상황 발생

<img width="1394" height="862" alt="image" src="https://github.com/user-attachments/assets/4a0f6227-cd51-4c7b-8bd4-0f4089712d4b" />

1. 객체 생성 순서 불일치로 인한 Race Condition?
   
   => 기존 플레이어 컨트롤러의 BeginPlay 단계에서 델리게이트 바인딩을 시도하는 로직에 문제가 있다고 판단.
   
   => OnRep_PlayerState 함수를 오버라이딩하여 해당 단계에서 바인딩 시도.
   
   => 디버그용 텍스트를 출력하여 각 클라이언트에서 모든 플레이어의 HP를 확인하였으나, 체력 변수는 정상적으로 Replicate되고 있음.
   
   => 해결은 실패했으나, BeginPlay 단계에서 PlayerState가 형성되지 않는 문제를 발견함.

```C++
void AHKKPlayerController::LoadingRace()
{
	if (!WidgetControllerSetup)
	{
		if (WidgetComponent != nullptr) WidgetControllerSetup = WidgetComponent->SetController(this);
	}
	if (!WidgetHUDBind)
	{
		if (IsLocalController() && WidgetComponent != nullptr)
		{
			WidgetHUDBind = WidgetHUDBind = WidgetComponent->Bind_HUD(this);
		}
	}
	if (WidgetControllerSetup && WidgetHUDBind) return;
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AHKKPlayerController::LoadingRace);
}
```
   
2. 재귀적인 Initialize을 통한 안정성 확보
   
   => PlayerState에 국한된 문제였으나 향후 볼륨이 커진 후 초기화 단계에서의 Race Condition은 피할 수 없다고 판단.
   
   => 순차적으로 초기화를 진행하고, 초기화 결과를 확인하여 초기화 되지 않은 부분은 다음 Tick에서 재시도하는 로직으로 변경.
   
   => 모든 초기화가 정상적으로 진행됐음을 확신할 수 있게됨.
   
   => 해결은 여전히 실패했으나, 디버깅을 시도한 결과 모든 델리게이트는 정상적으로 바인드 되어 있고 심지어 바인드된 함수도 정상 호출됨.
   
   => 위젯의 기본 체력바를 절반으로 설정, 바인드 이후 체력바를 최대한으로 설정한 뒤 에디터에서 게임 실행 도중
   
   모든 캐릭터 스폰 이후 리슨 서버의 클라이언트에서 체력바가 초기화(절반)되는 현상 발견.

```C++
void AHKKPlayerController::LoadingRace()
{
	if (!WidgetControllerSetup)
	{
		if (WidgetComponent != nullptr && IsLocalController()) WidgetControllerSetup = WidgetComponent->SetController(this);
	}
	if (!WidgetHUDBind)
	{
		if (IsLocalController() && WidgetComponent != nullptr)
		{
			WidgetHUDBind = WidgetHUDBind = WidgetComponent->Bind_HUD(this);
		}
	}
	if (WidgetControllerSetup && WidgetHUDBind) return;
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AHKKPlayerController::LoadingRace);
}
```

3. 리슨서버에서의 UI 중복 생성
   
   => HUD 위젯이 중복으로 생성되는 것을 확신함.
   
   => 기존 문제라 판단하였던 Bind_HUD 함수가 아닌 SetController를 재검토.
   
   => SetController는 ControllerWidgetComponent에서 위젯을 뷰포트에 소환하는 기능이 포함되어 있었음.
   
   => 모든 클라이언트의 컨트롤러를 보유하는 리슨 서버 환경에서 SetController 함수가 중복되어 호출되고 있었음.
   
   => 조건에 IsLocalController를 추가하여 허무하게 해결.

해결 방법 : 클라이언트로서 소유하고 있는 Controller인 경우에만 UI를 생성하도록 조건 추가.

조건 추가를 통해 해결 가능한 단순한 문제였으나 멀리 돌아가고 있었고,

오히려 그 과정에서 덕분에 코드를 돌아보며 아키텍쳐를 개선하여 가독성을 높일 수 있었고,

의존성을 최소화하여 전체적인 구조를 객체지향적으로 리팩토링 할 수 있었습니다.

-----
# 장비 장착 시스템 구현 중 싱크 이슈
-----

1. 마우스 스크롤을 통해 장비 스위칭 가능
2. HKKPlayerController에서 MouseScrolled 함수를 통해 OnKeyTriggered 델리게이트 브로드캐스트
3. PlayerState에서 현재 장비창에 등록되어 있는 아이템 배열(TArray<FItemConfig>)을 순회하여 장비 가능한 아이템을 검색
4. Server_SetCuurentEquipSlotIndex를 호출하여 CurrentEquipSlotIndex의 값을 해당 아이템의 인덱스로 변경
5. OnRep_CurrentEquipSlotIndex를 통해 OnEquipmentSlotSwitched 델리게이트를 통해 아이템 정보(FItemConfig)를 브로드캐스트
6. HUD, Inventory 위젯 등에서 바인드된 함수를 통해 위젯 애니메이션 재생 및 텍스쳐 교체
7. PlayerCharacter에서 바인드된 함수 OnEquipmentSlotSwitched를 통해 CombatComponent의 Server_SpawnAndAttachWeapon함수 호출
8. Server상에서 무기 액터를 스폰 시 WorldSubSystem을 상속한 UObjectPoolSystem을 통해 오브젝트 풀링 사용
9. 스폰된 무기 액터의 포인터를 EquipActor에 저장한 후, OnRep_ItemEquip 함수 호출
10. PlayerCharacter의 AttachItem을 통해 각 클라이언트에 Replicated된 무기 액터를 부착시킴

세 개의 델리게이트를 통해 변수를 변경하고 OnRep함수를 통해 애니메이션을 실행, UI의 텍스쳐를 변경하도록 하여 안정적이나 반응속도가 매우 떨어지는 문제 발견

1. 지나치게 안정적인 로직
   
	==> 서버에서 아이템 배열을 순회하고, 액터를 소환하고, 부착하는 동안

	==>클라이언트는 서버의 응답을 기다리고, 완료될 경우 애니메이션을 재생하는 형태로 구현되었음

	==> OnKeyTriggered 델리게이트를 통해 키가 입력되었을 경우 변경할 무기가 있는지 정도만 확인 후 애니메이션을 실행시키는 방향으로 개선

	==> 무기가 있는지 여부를 확인하기 위해선 장비 슬롯에 장착 시 Replicate시킬 필요가 있음
