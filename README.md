# BranchTalk Forge

BranchTalk Forge는 Windows와 macOS를 대상으로 하는 C++20 협업 프로그램이다. 저장소는
데스크톱 클라이언트, 자체 호스팅 서버, 두 실행 파일이 함께 사용하는 core 라이브러리를
분리한다.

## 저장소 구조

```text
project/
├─ CMakeLists.txt
├─ CMakePresets.json
├─ vcpkg.json
├─ cmake/
│  └─ BranchTalkTargets.cmake
├─ apps/
│  ├─ client/
│  │  └─ main.cpp
│  ├─ desktop/
│  │  ├─ AppButton.qml
│  │  ├─ AppPanel.qml
│  │  ├─ CMakeLists.txt
│  │  ├─ app_view_model.cpp
│  │  ├─ app_view_model.hpp
│  │  ├─ main.cpp
│  │  ├─ Main.qml
│  │  ├─ Theme.qml
│  │  ├─ window_state_store.cpp
│  │  └─ window_state_store.hpp
│  └─ server/
│     ├─ include/
│     │  └─ branchtalk/server/
│     │     └─ server_app.hpp
│     ├─ src/
│     │  └─ server_app.cpp
│     └─ main.cpp
├─ libs/
│  └─ core/
│     ├─ include/
│     │  └─ branchtalk/
│     │     └─ core/
│     │        ├─ application_config.hpp
│     │        ├─ dependency_versions.hpp
│     │        ├─ error.hpp
│     │        ├─ logging.hpp
│     │        ├─ result.hpp
│     │        └─ version.hpp
│     └─ src/
│        ├─ application_config.cpp
│        ├─ dependency_versions.cpp
│        ├─ logging.cpp
│        └─ version.cpp
└─ tests/
   ├─ CMakeLists.txt
   ├─ cmake/
   │  ├─ verify_boundaries.cmake
   │  ├─ verify_desktop_disabled.cmake
   │  ├─ verify_logging.cmake
   │  ├─ verify_presets.cmake
   │  ├─ verify_server.cmake
   │  ├─ verify_theme_tokens.cmake
   │  └─ verify_view_model_boundary.cmake
   ├─ core/
   │  ├─ application_config_smoke.cpp
   │  ├─ dependency_versions_smoke.cpp
   │  ├─ logging_smoke.cpp
   │  ├─ public_api_smoke.cpp
   │  └─ result_smoke.cpp
   ├─ desktop/
   │  ├─ app_view_model_smoke.cpp
   │  └─ window_state_store_smoke.cpp
   ├─ server/
   │  └─ server_app_smoke.cpp
   └─ fixtures/
      └─ config/
         └─ *.json
```

## 코드 경계

| 영역 | 네임스페이스 | 책임 | 허용 의존성 |
|---|---|---|---|
| `apps/client` | `branchtalk::client` | 데스크톱 클라이언트 진입점 | C++ 표준 라이브러리, `libs/core` |
| `apps/desktop` | `branchtalk::desktop` | 선택적 Qt Quick 데스크톱 진입점, 화면 상태, 창 상태, QML 리소스 | `libs/core`, Qt Quick Controls |
| `apps/server` | `branchtalk::server` | 자체 호스팅 서버 진입점과 수명주기 | C++ 표준 라이브러리, `BranchTalk::server_app`, `libs/core` |
| `libs/core` | `branchtalk::core` | 클라이언트와 서버가 공유하는 제품 기본 계약 | C++ 표준 라이브러리, nlohmann-json, spdlog |

의존 방향은 다음 규칙을 따른다.

```text
apps/client ─┐
             ├─> libs/core ─┬─> C++ 표준 라이브러리
apps/server ─> server_app ──┘
                            ├─> nlohmann-json
                            └─> spdlog

apps/desktop ─┬─> libs/core
              ├─> Qt Quick Controls
              └─> Qt Core(QSettings)
```

- `client`와 `server`는 서로의 헤더나 구현을 참조하지 않는다.
- `core`는 어떤 실행 파일에도 의존하지 않는다.
- 실행 환경에만 필요한 코드는 해당 `apps` 디렉터리에 둔다.
- 두 실행 파일이 공유해야 하는 안정적인 계약만 `core`로 이동한다.

`branchtalk_architecture.boundaries` test는 이 방향을 build graph와 source include에서 함께
검사한다. client는 `BranchTalk::core`에만 직접 link하고 `branchtalk/server` header를
include하지 않는다. server 실행 파일은 `BranchTalk::server_app`에 직접 link하고,
server app target이 `BranchTalk::core` 사용 요구사항을 공개한다.

## CMake target

| target | 종류 | 입력 | 의존성 |
|---|---|---|---|
| `branchtalk_core` | 정적 라이브러리 | `libs/core/src/*.cpp` | C++ 표준 라이브러리, nlohmann-json, spdlog |
| `branchtalk_client` | 실행 파일 | `apps/client/main.cpp` | `BranchTalk::core` |
| `branchtalk_desktop_window_state` | 정적 라이브러리 | `window_state_store.cpp` | `Qt6::Core` |
| `branchtalk_desktop` | 선택적 Qt Quick 실행 파일 | `apps/desktop/main.cpp`, `AppViewModel`, QML module | `BranchTalk::core`, 창 상태 라이브러리, `Qt6::Quick`, `Qt6::QuickControls2` |
| `branchtalk_server_app` | 정적 라이브러리 | `apps/server/src/server_app.cpp` | `BranchTalk::core` |
| `branchtalk_server` | 실행 파일 | `apps/server/main.cpp` | `BranchTalk::server_app` |

`BranchTalk::core`는 `branchtalk_core`의 별칭이다. client는 core의 공개 include 경로와
C++20 요구사항을 직접 전달받고, server는 같은 요구사항을 `BranchTalk::server_app`의 공개
의존성을 통해 전달받는다.

`cmake/BranchTalkTargets.cmake`의 `branchtalk_configure_cpp_target()`은 모든 C++ target에
C++20, 표준 확장 비활성화, compiler별 경고를 같은 규칙으로 적용한다.
`branchtalk_add_test_executable()`은 smoke executable의 source·link library 선언 뒤 같은
target 기본값을 적용해 test target 설정의 반복을 줄인다. 제품 target과 test target 모두
전역 compiler flag를 수정하지 않는다.

`branchtalk_desktop`은 기본 빌드에서 비활성화된다. 이 경로에서는 Qt package를 찾지 않으므로
Qt가 설치되지 않은 환경에서도 core, client, server와 관련 테스트를 그대로 빌드할 수 있다.

Qt 6.5 이상의 Quick·Quick Controls 개발 패키지가 설치된 환경에서는 configure 때 target을
활성화한다.

```sh
cmake --preset debug -DBRANCHTALK_BUILD_DESKTOP=ON -DCMAKE_PREFIX_PATH=/path/to/Qt
cmake --build --preset debug --target branchtalk_desktop
ctest --preset debug -R branchtalk_desktop
```

`qt_add_qml_module()`은 `Main.qml`, `Theme.qml`, `AppButton.qml`, `AppPanel.qml`과 C++
`AppViewModel` type을 `BranchTalk` QML module에 포함한다. 데스크톱 실행 파일은 이 module의
`Main` type을 읽어 기본 800×600, 최소 640×480인 `ApplicationWindow`를 연다. smoke test는
같은 리소스를 offscreen platform에서 읽고 root object가 만들어지는지 확인한 뒤 종료한다.

## 테마와 디자인 토큰

`Theme` QML singleton은 밝은·어두운 모드의 창·패널·텍스트·강조 색과 공통 간격,
타이포그래피 크기·굵기, radius·control 크기를 한 곳에서 제공한다. `darkMode`가 바뀌면
`Main`, `AppPanel`, `AppButton`의 property binding이 같은 singleton의 새 값으로 다시
평가되므로 창을 다시 만들지 않고 화면이 갱신된다.

`AppPanel`은 배경·테두리·radius와 조정 가능한 content padding을 Theme token에서 읽고,
default property alias로 받은 content를 내부 content item에 배치한다. `AppButton`은
padding·높이·글꼴과 normal·hovered·pressed·disabled 색을 같은 token에서 읽으며, 좁은 열에는
최소 너비와 좌우 padding을 줄이는 compact 표현을 제공한다. 메인 창은 두 공통 component를
사용하며 직접 색상 literal이나 spacing 값을 소유하지 않는다.

`branchtalk_desktop.theme` test는 QML의 theme 전환 함수를 호출한 직후 창·패널 색과 버튼
문구가 함께 달라지는지 검사한다. `branchtalk_desktop.theme_tokens` test는 Theme singleton의
필수 token과 공통 component 사용을 확인하고, Theme 밖 QML에 색상 literal이나 주요 spacing
literal이 들어오면 실패한다.

## 3단 메인 레이아웃

`Main.qml`의 가로 `SplitView`는 워크스페이스 열, 채널 열, 대화 영역을 순서대로 배치한다.
각 영역은 `AppPanel`을 사용하며 워크스페이스 96, 채널 200, 대화 320의 최소 너비와 분할 핸들
너비를 `Theme` token에서 읽는다. 기본 너비에서는 세 영역이 모두 보이고 사용자가 분할 핸들로
영역 크기를 조정할 수 있다.

창 너비가 760보다 작아지면 대화 영역을 접고 채널 열이 남은 너비를 채운다. 워크스페이스와
채널 동작은 계속 표시되어 최소 창 너비에서도 핵심 탐색을 사용할 수 있다. 다시 넓어지면 대화
영역과 기존 분할 크기가 복원된다. 현재 표시 내용은 QML 배열로 만든 워크스페이스·채널·메시지
임시 모델에서 가져온다.

`branchtalk_desktop.layout` test는 창 너비를 1000→700→1000으로 바꾸며 세 영역의 최소 너비,
좁은 창에서의 대화 영역 접힘, 핵심 동작의 표시·활성 상태, 다시 넓혔을 때 분할 크기 복원을
offscreen platform에서 검사한다.

## 화면 전환과 AppViewModel

`AppViewModel`은 로그인, 메인, 설정을 `Screen` enum으로 정의하고 읽기 전용
`currentScreen` property를 화면 전환의 단일 상태 원천으로 제공한다. 초기 상태는 로그인이고,
`showLogin()`, `showMain()`, `showSettings()` 명령만 내부 상태를 바꾼다. 같은 화면을 다시
요청하면 notification을 반복하지 않는다.

데스크톱 진입점은 `AppViewModel` instance를 만들고 `Main`의 required property로 주입한다.
QML은 instance를 만들거나 현재 화면을 별도로 저장하지 않는다. 로그인·메인·설정 view의
`visible`은 `currentScreen` enum에 binding되고 button handler는 세 전환 명령 중 하나만
호출한다. 화면 전환 명령에는 자격 증명 검사, 통신, 저장소 접근이 포함되지 않는다.

```text
QML button command
  → AppViewModel currentScreen 변경
  → currentScreenChanged
  → 세 view의 visible binding 재평가
```

`branchtalk_desktop.app_view_model` test는 초기 상태와 세 명령, 동일 상태의 notification 억제를
검사한다. `branchtalk_desktop.navigation` test는 로그인→메인→설정→로그인 전환 때 하나의
view만 보이는지 확인한다. `branchtalk_desktop.view_model_boundary` test는 QML이
`currentScreen`을 자체 property로 선언하지 않고 주입된 상태와 명령만 사용하며, QML source에
network·database API가 들어오지 않는지 검사한다.

## 데스크톱 창 상태

`WindowStateStore`는 `QSettings`의 `window/x`, `window/y`, `window/width`, `window/height`
값을 창 상태로 묶어 저장한다. 앱 시작 시 네 값을 모두 정수로 읽을 수 있고 최소 크기를
충족하며 현재 화면과 겹칠 때만 QML root object의 초기 속성으로 전달한다. 값이 없거나 형식이
잘못됐거나 화면 밖에 있으면 초기 속성을 전달하지 않아 `Main.qml`의 기본 크기와 플랫폼 기본
위치를 그대로 사용한다.

정상 종료 직전에는 root object의 현재 위치와 크기를 저장한다. 창 상태 smoke test는 임시 INI
파일에 저장한 값을 새 `QSettings` 객체에서 다시 읽어 재실행 흐름을 확인하고, 숫자가 아닌 값,
최소 크기보다 작은 값, 화면 밖 위치가 복원되지 않는지 검사한다.

## 공통 오류 계약

`Error`는 호출자가 분기할 수 있는 `ErrorCode`와 진단용 `message`를 분리해 보관한다.
메시지는 화면 문구가 아니라 실패 원인을 전달하는 core 정보이며, 표시할 문구로 바꾸는 책임은
core 밖에 둔다.

`Result<T>`는 성공값 `T` 또는 `Error` 중 하나만 저장한다. `has_value()`와 명시적 bool
변환으로 상태를 확인하고, `value_if()`와 `error_if()`로 현재 상태에 맞는 데이터에
접근한다. 두 접근 함수는 반대 상태에서 예외를 던지는 대신 null pointer를 반환한다.

## 구조화 로그

client와 server는 core의 `initialize_logging()`을 사용해 같은 `[level] [category] message`
형식으로 stdout에 기록한다. `LoggingSettings::level`을 바꾸면 네 범주에 적용되는 최소 로그
수준이 함께 변경된다.

| 범주 | 용도 |
|---|---|
| `client` | 데스크톱 클라이언트 수명주기와 동작 |
| `server` | 자체 호스팅 서버 수명주기와 동작 |
| `database` | 데이터 저장소 접근 |
| `network` | 연결과 데이터 송수신 |

민감한 값은 일반 메시지로 넘기지 않고 `SensitiveValue`로 감싼 뒤 `write_sensitive()`에
전달한다. wrapper는 원문을 보관하거나 formatter에 넘기지 않고 `<redacted>`만 노출한다.

## 애플리케이션 설정

`ApplicationConfig`는 client와 server가 공유하는 서버 주소, 로그 수준, 데이터 경로를 한
객체로 묶는다. 기본값은 `127.0.0.1:8080`, `info`, `data`이며 설정 파일 없이 생성해도 바로
사용할 수 있다. 두 실행 파일은 이 기본 설정의 로그 수준으로 공통 로깅을 초기화한다.

`load_application_config()`는 JSON 객체의 `server_address`, `log_level`, `data_path`를
기본값에 선택적으로 덮어쓴다. 로그 수준은 `trace`, `debug`, `info`, `warning`, `error`,
`critical`, `off` 중 하나여야 한다. 필드의 형식이나 값이 잘못되거나 지원하지 않는 필드가
있으면 예외 대신 실패한 `Result<ApplicationConfig>`에 원인을 담아 반환한다.

## 서버 수명주기

`ServerApp`은 검증이 끝난 `ApplicationConfig`와 종료 여부를 확인하는 함수 pointer를
생성자에서 받는다. 설정의 로그 수준으로 공통 logging을 초기화하고 `server started`를
기록한 뒤 종료 요청이 들어올 때까지 대기한다. 요청을 확인하면 `server stopped`를 기록하고
정상 종료한다.

서버 실행 파일은 설정 경로 하나를 선택적으로 받는다. 경로가 없으면 기본 설정을 주입하고,
경로가 있으면 `load_application_config()`의 성공값만 `ServerApp`에 전달한다. 잘못된 설정은
`server startup error` 로그와 실패 종료 코드로 반환하며 수명주기를 시작하지 않는다.

`SIGINT`와 `SIGTERM` handler는 logging이나 할당을 수행하지 않고 lock-free
`std::atomic_flag`에 종료 요청만 기록한다. `ServerApp`은 주입된 조회 함수로 이 flag를
확인하므로 signal 처리와 제품 수명주기 로직이 분리된다. `--smoke-test`는 짧은 대기 뒤
process 내부에서 `SIGTERM`을 발생시켜 같은 handler와 정상 종료 경로를 검증한다.

## vcpkg 의존성

`vcpkg.json`의 manifest mode로 직접 의존성을 관리한다. builtin registry baseline은
`e03dc9b29710050cd1018bc5674688108658d327`로 고정하고, override에 패키지 버전을
명시한다.

| 패키지 | 고정 버전 | core에서 사용하는 목적 |
|---|---|---|
| `nlohmann-json` | `3.12.0#2` | 의존성 버전 정보를 JSON 문자열로 직렬화 |
| `spdlog` | `1.17.0` | 공통 형식과 범주를 가진 구조화 로그를 출력 |

`dependency_versions_json()`은 실제로 연결된 두 라이브러리의 버전 매크로를 읽어 JSON
문자열을 만든다. core 의존성 smoke test는 이 문자열이 manifest에 고정한 API 버전과
일치하는지 확인한다.

## CMake Preset

Windows와 macOS에서 같은 preset 이름과 명령 흐름을 사용한다. CMake가 각 운영체제의 기본
생성기를 선택하며, configure 결과는 구성별 디렉터리에 분리된다.

preset은 `VCPKG_ROOT` 환경 변수가 가리키는 vcpkg toolchain을 사용한다. vcpkg 설치
디렉터리를 지정한 뒤 configure하면 manifest의 패키지가 `build/vcpkg_installed`에
설치된다.

```powershell
$env:VCPKG_ROOT = 'C:\path\to\vcpkg'
```

```sh
export VCPKG_ROOT=/path/to/vcpkg
```

| preset | 구성 | 빌드 출력 폴더 |
|---|---|---|
| `debug` | Debug | `build/debug` |
| `release` | Release | `build/release` |

사용 가능한 configure preset은 다음 명령으로 확인한다.

```sh
cmake --list-presets
```

debug 구성의 공통 configure·build·test 흐름은 다음과 같다.

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

기존 configure cache를 사용하지 않고 기준선을 다시 확인할 때는 configure에 `--fresh`를
추가한다.

```sh
cmake --preset debug --fresh
cmake --build --preset debug
ctest --preset debug
```

release 구성은 세 명령의 preset 이름을 `release`로 바꿔 실행한다.

## 컴파일러 경고

경고 옵션은 `CMAKE_CXX_FLAGS` 같은 전역 변수에 추가하지 않고 각 target에만 적용한다.

- MSVC: `/W4`, `/permissive-`
- Apple Clang: `-Wall`, `-Wextra`, `-Wpedantic`

테스트는 client 시작 출력과 server의 시작·종료 signal 수명주기, 잘못된 설정의 오류 반환을
확인한다. desktop test는 QML 창 생성, theme binding의 즉시 갱신, token 중앙화와 창 상태의
저장·복원 및 잘못된 값 처리를 검사한다.
server app smoke는 설정과 종료 조건 주입을 직접 검사한다. preset 계약 테스트는
debug·release configure preset과 공통 target 기본값을 확인한다. architecture test는
client·server·server app의 link 방향과 client include 경계를 검사한다. core public API
smoke는 모든 공개 계약을 하나의 consumer target에서 include·link해 기반 통합 상태를
확인한다. core 의존성 테스트는 실제 링크된 JSON·로그 라이브러리의 API 버전이 manifest의
고정값과 일치하는지 확인한다. 로그 테스트는 네 범주의 출력, 설정에 따른 로그 수준 변경,
민감값 redaction을 확인한다.
