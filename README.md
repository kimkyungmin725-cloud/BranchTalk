# BranchTalk Forge

BrachTalk Forge는 Windows와 macOS를 대상으로 하는 C++20 협업 프로그램이다. 저장소는
데스크톱 클라이언트, 자체 호스팅 서버, 두 실행 파일이 함께 사용하는 core 라이브러리를 
분리한다.

## 저장소 구조

```text
project/
├─ CMakeLists.txt
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

## 로컬 빌드와 테스트

```sh
cmake -S . -B build
cmake --build build --config Debug --target branchtalk_client branchtalk_server
ctest --test-dir build -C Debug --output-on-failure
```
 
-최상위 CMake 프로젝트와 실제 target 구성은 다음 개발 단계에서 추가한다.
테스트는 두 실행 파일을 각각 실행해 `core`의 버전 계약에 접근하고 정상 종료하는지 확인한다.
