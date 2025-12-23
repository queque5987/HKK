쪼아요 쪼아요 리슨서버 쪼아요

리슨서버에서의 UI 중복 생성

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
