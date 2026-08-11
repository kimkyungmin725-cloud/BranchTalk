# BranchTalk Forge

BrachTalk Forge는 Windows와 macOS를 대상으로 하는 C++20 협업 프로그램이다. 저장소는
데스크톱 클라이언트, 자체 호스팅 서버, 두 실행 파일이 함께 사용하는 core 라이브러리를 
분리한다.

## 저장소 구조

```text
project/
├─ CMakeLists.txt
├─ CMakePresets.json
├─ app/
│  ├─ client/
│  │  └─ main.cpp
│  └─ server/
│     └─ main.cpp
└─ libs/
   └─ core/
      ├─ include/
      │  └─ branchtalk/
      │     └─ core/
      |        └─ version.hpp
      └─ src/
         └─ version.cpp
```

## 코드 경계

| 영역 | 네임스페이스 | 책임 | 허용 의존성 |
|---|---|---|---|
| `apps/client` | `branchtalk::client` | 데스크톱 클라이언트 진입점 | C++ 표준 라이브러리, `libs/core` |
| `apps/server` | `branchtalk::server` | 자체 호스팅 서버 진입점 | C++ 표준 라이브러리, `libs/core` |
| `libs/core` | `branchtalk::core` | 클라이언트와 서버가 공유하는 제품 기본 계약 | C++ 표준 라이브러리 |

의존 방향은 다음 규칙을 따른다.

```text
apps/client ─┐
             ├─> libs/core ─> C++ 표준 라이브러리
apps/server ─┘
```

- `client`와 `server`는 서로의 헤더나 구현을 참조하지 않는다.
- `core`는 어떤 실행 파일에도 의존하지 않는다.
- 실행 환경에만 필요한 코드는 해당 `apps` 디렉터리에 둔다.
- 두 실행 파일이 공유해야 하는 안정적인 계약만 `core`로 이동한다.

## CMake target

| target | 종류 | 입력 | 의존성 |
|---|---|---|---|
| `branchtalk_core` | 정적 라이브러리 | `libs/core/src/version.cpp` | C 표준 라이브러리 |
| `branchtalk_client` | 실행 파일 | `apps/client/main.cpp` | `BranchTalk::core` |
| `branchtalk_server` | 실행 파일 | `apps/server/main.cpp` | `BranchTalk::core` |

`BranchTalk::core`는 `branchtalk_core`의 별칭이다. 클라이언트와 서버는 공개 include 경로와 C++20 사용 요구사항을 이 target을 통해 전달받는다.

## CMake Preset

Windows와 macOS에서 같은 preset 이름과 명령 흐름을 사용한다. CMake가 각 운영체제의 기본 
생성기를 선택하며, configure 결과는 구성별 디렉터리에 분리된다.

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
머무르는지를 확인한다.
