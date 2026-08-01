# BranchTalk Forge

BranchTalk Forge는 Windows와 macos를 대상으로 하는 c++20 협업 프로그렘이다. 저장소는 데스크톱 클라이언트, 자체 호스팅 서버, 두 실행 파일이 함께 사용하는 core 라이브러리를 분리한다.

## 저장소 구조


```
project/
├ app/
| ├ client/
| | ├ main.cpp
| └ server/
|   └ main.cpp/
└ libs/
  └ core/
   ├ include/
   | └ branchtalk/
   |   └ core/
   |     └ version.hpp
   └src/
    └version.cpp
```

##코드 경계

| 영역 | 네임스페이스 | 책임 | 허용 의존성 |
|---|---|---|---|
| 'apps/client' | 'branchtalk::client' | 데스크톱 클라이언트 진입점 | C++ 표준 라이브러리, 'libs/core' |
| 'apps/server' | 'branchtalk::server' | 자체 호스팅 서버 진입점 | C++ 표준 라이브러리, 'libs/core' |
| 'libs/core' | 'branchtalk::core' | 크랄이언트와 서버가 공유하는 제품 기본 계약 | C++ 표준 라이브러리 |

의존 방법은 다음 규칙을 따른다.

```text
apps/client ─┐
             ├─> libs/core ─> C++ 표준 라이브러리
apps/server ─┘
```

- 'client' 와 'server'는 서로의 헤더나 구현을 참조하지 않는다.
- 'core;는 어떤 실행 파일에도 의존하지 않는다.
- 실행 환경에만 피요한 코드는 해당 'apps' 디렉터리에 둔다.
- 두 실행 파일이 공유해야 하는 안정적인 계약만 'core'로 이동한다.

## 현재 기준선

- 제품 비전은 'branchtalk::score::kVersion'에 정의한다.
- 현제 버전은 '0.1.0'이다.
- 이 단계의 실행은 파일은 코드 경계와 C++20 구문을 확인하기 위한 최소 진입장벽이다.
- 빌드 시스템, Qt 화면, 네트워크 기능은 아직 포함하지 않는다.

최상위 CMake 프로젝트와 실제 target 구성은 다음 개발 단계에서 추가한다.

