# BranchTalk Forge

BrachTalk Forge는 Windows와 macOS를 대상으로 하는 C++20 협업 프로그램이다. 저장소는
데스크톱 클라이언트, 자체 호스팅 서버, 두 실행 파일이 함께 사용하는 core 라이브러리를 
분리한다.

## 저장소 구조

```text
project/
├─ CMakeLists.txt
├─ CMakePresets.json
├─ vcpkg.json
├─ app/
│  ├─ client/
│  │  └─ main.cpp
│  └─ server/
│     └─ main.cpp
├─ libs/
│  └─ core/
│     ├─ include/
│     │  └─ branchtalk/
│     │     └─ core/
│     │        ├─ dependency_versions.hpp
│     │        ├─ error.hpp
│     │        ├─ logging.hpp
│     │        ├─ result.hpp
│     |        └─ version.hpp
│     └─ src/
│        ├─ dependency_versions.cpp
│        ├─ logging.cpp
│        └─ version.cpp
└─ tests/
   ├─ cmake/
   │  ├─ verify_logging.cmake
   │  └─ verify_presets.cmake
   └─ core/
      ├─ dependency_versoins_smoke.cpp
      ├─ logging_smoke.cpp
      └─ result_smoke.cpp
```

## 코드 경계

| 영역 | 네임스페이스 | 책임 | 허용 의존성 |
|---|---|---|---|
| `apps/client` | `branchtalk::client` | 데스크톱 클라이언트 진입점 | C++ 표준 라이브러리, `libs/core` |
| `apps/server` | `branchtalk::server` | 자체 호스팅 서버 진입점 | C++ 표준 라이브러리, `libs/core` |
| `libs/core` | `branchtalk::core` | 클라이언트와 서버가 공유하는 제품 기본 계약 | C++ 표준 라이브러리, nlohmann-json, spdlog |

의존 방향은 다음 규칙을 따른다.

```text
apps/client ─┐
             ├─> libs/core ─┬─> C++ 표준 라이브러리
apps/server ─┘              ├─> nlohmann-json
                            └─> spdlog
```

- `client`와 `server`는 서로의 헤더나 구현을 참조하지 않는다.
- `core`는 어떤 실행 파일에도 의존하지 않는다.
- 실행 환경에만 필요한 코드는 해당 `apps` 디렉터리에 둔다.
- 두 실행 파일이 공유해야 하는 안정적인 계약만 `core`로 이동한다.

## CMake target

| target | 종류 | 입력 | 의존성 |
|---|---|---|---|
| `branchtalk_core` | 정적 라이브러리 | `libs/core/src/*.cpp` | C++ 표준 라이브러리, nlohmann-json, spdlog |
| `branchtalk_client` | 실행 파일 | `apps/client/main.cpp` | `BranchTalk::core` |
| `branchtalk_server` | 실행 파일 | `apps/server/main.cpp` | `BranchTalk::core` |

`BranchTalk::core`는 `branchtalk_core`의 별칭이다. 클라이언트와 서버는 공개 include 경로와 C++20 사용 요구사항을 이 target을 통해 전달받는다.

## 공통 오류 계약

`#Error`는 호출자가 분기할 수 있는 `ErrorCode`와 진당용 `message`를 분리해 보관한다. 
메시지는 화면 문구가 아니라 실패 원인을 전달하는 core 정보이며, 표시할 문구로 바꾸는 책임은 
core 밖에 둔다.

`Result<T>`는 성공값, `T` 또는 `Error` 중 하나만 저장한다. `has_value()`와 명시적 bool
변환으로 상태를 확인하고, `value_if()`와 `error_if()`로 현재 상태에 맞는 데이터에
접근한다. 두 접근 함수는 반대 상태에서 예외를 던지는 대신 null pointer를 반환한다.

## 구조화 로그

client와 server는 core의 `initialize_logging()`을 사용해 같은 `[level] [category] message`
형식으로 stdout에 기록한다. `loggingSettings::level`을 바꾸면 네 범주에 적용되는 최소 로그
 수준이 함께 변경된다

| 범주 | 용도 |
|---|---|
| `client` | 데스크톱 클라이언트 수명주기와 동작|
| `server` | 자체 호스팅 서버 수명주기와 동작 |
| `database` | 데이터 저장소 접근 |
| `network` | 연결과 데이터 송수신 |

민감한 값은 일반 메시지로 넘기지 않고 `SensitiveValue`로 감싼 뒤 `wirte_sensitive()`에
전달한다. wrapper는 원문을 보관하거나 formatter에 넘기지 않고 `<reducted>`만 노출한다.

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
$env:VCPKG_ROOT = 'c:\path\to\vcpkg'
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

release 구성은 세 명령의 preset 이름을 `release`로 바꿔 실행한다.

## 컴파일러 경고

경고 옵션은 `CMAKE_CXX_FLAGS` 같은 전역 변수에 추가하지 않고 각 target에만 적용한다. preset 
계약 테스트는 debug·release configure preset이 노출되는지와 경고 설정이 target 범위에 
머무르는지를 확인한다. core 의존성 테스트는 실제 링크된 JSON·로그 라이브러리의 API
버전이 manifest의 고정값과 일치하는지 확인한다.
