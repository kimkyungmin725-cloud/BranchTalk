# BranchTalk Forge

BranchTalk Forge is a C++20 collaboration application for Windows and macOS. The
repository contains a Qt Quick desktop app, a self-hosted server, and a small
core library shared by the executables.

## Building and testing

The desktop app is optional so the core, command-line client, and server can be
built on machines without Qt.

For day-to-day desktop work, configure one build directory with the desktop and
contract tests enabled:

```powershell
cmake -S . -B build/local `
  -DBRANCHTALK_BUILD_DESKTOP=ON `
  -DBUILD_TESTING=ON `
  -DBRANCHTALK_ENABLE_CONTRACT_TESTS=ON `
  -DCMAKE_PREFIX_PATH=C:/path/to/Qt
cmake --build build/local --config Debug --parallel 4
```

Run the short test groups while working:

```powershell
ctest --test-dir build/local -C Debug -L fast --output-on-failure -j 4
ctest --test-dir build/local -C Debug -L integration --output-on-failure -j 4
```

Run the full local check before finishing a larger change:

```powershell
python tools/stage_check.py --build-dir build/local --config Debug
```

The full check validates the repository text policy, builds the default CMake
graph, then runs the `fast`, `integration`, and `contract` labels. It stops
before the build if a label is missing or a text file contains non-ASCII
characters. More details are in [TESTING.md](TESTING.md).

## Repository layout

```text
BranchTalk/
|-- CMakeLists.txt
|-- CMakePresets.json
|-- vcpkg.json
|-- cmake/
|   `-- BranchTalkTargets.cmake
|-- apps/
|   |-- client/
|   |   `-- main.cpp
|   |-- desktop/
|   |   |-- CMakeLists.txt
|   |   |-- main.cpp
|   |   |-- Main.qml
|   |   |-- MessageComposer.qml
|   |   |-- MessageDelegate.qml
|   |   |-- message_composer_state.cpp
|   |   |-- message_composer_state.hpp
|   |   |-- message_list_model.cpp
|   |   |-- message_list_model.hpp
|   |   |-- Theme.qml
|   |   |-- AppButton.qml
|   |   |-- AppPanel.qml
|   |   |-- app_view_model.cpp
|   |   |-- channel_list_model.cpp
|   |   |-- workspace_list_model.cpp
|   |   `-- window_state_store.cpp
|   `-- server/
|       |-- include/branchtalk/server/server_app.hpp
|       |-- src/server_app.cpp
|       `-- main.cpp
|-- libs/
|   `-- core/
|       |-- include/branchtalk/core/
|       `-- src/
|-- tests/
|   |-- CMakeLists.txt
|   |-- cmake/
|   |-- core/
|   |-- desktop/
|   |-- server/
|   `-- fixtures/
`-- tools/
    |-- check_text_policy.py
    `-- stage_check.py
```

## Code boundaries

| Area | Namespace | Responsibility | Direct dependencies |
|---|---|---|---|
| `apps/client` | `branchtalk::client` | Command-line client entry point | C++ standard library, `BranchTalk::core` |
| `apps/desktop` | `branchtalk::desktop` | Qt Quick UI, navigation, list models, and window state | `BranchTalk::core`, Qt Quick Controls |
| `apps/server` | `branchtalk::server` | Server entry point and application lifetime | `BranchTalk::server_app` |
| `libs/core` | `branchtalk::core` | Shared result, configuration, logging, and version contracts | C++ standard library, nlohmann-json, spdlog |

Dependencies point inward toward the shared core:

```text
apps/client  -> libs/core
apps/desktop -> libs/core + Qt
apps/server  -> server_app -> libs/core
libs/core    -> nlohmann-json + spdlog
```

The client and server do not include each other's headers. The core library does
not depend on an executable. Platform-specific code stays under `apps` unless
both executables need the same stable contract.

`branchtalk_architecture.boundaries` checks these link and include rules.

## CMake targets

| Target | Type | Main inputs | Dependencies |
|---|---|---|---|
| `branchtalk_core` | Static library | `libs/core/src/*.cpp` | nlohmann-json, spdlog |
| `branchtalk_client` | Executable | `apps/client/main.cpp` | `BranchTalk::core` |
| `branchtalk_server_app` | Static library | `apps/server/src/server_app.cpp` | `BranchTalk::core` |
| `branchtalk_server` | Executable | `apps/server/main.cpp` | `BranchTalk::server_app` |
| `branchtalk_desktop_window_state` | Static library | `window_state_store.cpp` | `Qt6::Core` |
| `branchtalk_desktop` | Optional Qt Quick executable | Desktop C++ sources and QML module | Core, window state, Qt Quick |

`BranchTalk::core` is an alias for `branchtalk_core`.

`cmake/BranchTalkTargets.cmake` applies C++20, disables compiler extensions,
and sets compiler warnings for each C++ target. It also provides the helper used
to define the small test executables. The project does not modify global CMake
compiler flags.

The desktop target is disabled by default. Enable it with Qt 6.5 or newer:

```sh
cmake --preset debug \
  -DBRANCHTALK_BUILD_DESKTOP=ON \
  -DCMAKE_PREFIX_PATH=/path/to/Qt
cmake --build --preset debug --target branchtalk_desktop
ctest --preset debug -R branchtalk_desktop
```

## Desktop application

`qt_add_qml_module()` packages `Main.qml`, `Theme.qml`, `AppButton.qml`,
`AppPanel.qml`, `MessageComposer.qml`, and `MessageDelegate.qml`, plus the
registered desktop state and model types, in the `BranchTalk` QML module. The
application opens an 800 by
600 window with a minimum size of 640 by 480.

### Theme and layout

`Theme.qml` owns the light and dark colors, spacing, typography, radii, and
control sizes used by the other QML files. `AppPanel` and `AppButton` read those
values instead of keeping their own color and spacing constants.

The desktop selects the Basic control style before loading QML so the custom
control content and backgrounds render consistently on Windows and macOS.

`AppPanel` accepts an explicit content size for standalone cards. Split-view
panels use their assigned size instead of deriving it from anchored children,
which avoids recursive implicit-size bindings.

The main view uses a horizontal `SplitView` with workspace, channel, and
conversation columns. The conversation column is hidden below the 760 pixel
breakpoint and returns when the window becomes wider.

The `theme` and `layout` scenarios in `branchtalk_desktop.ui_flow` exercise the
QML behavior with the offscreen platform. The contract tests check that theme
tokens and model boundaries remain in place.

### Navigation

`AppViewModel` owns the current screen. It exposes `showLogin()`, `showMain()`,
and `showSettings()` and does not emit a change notification when the requested
screen is already active. QML binds each view to `currentScreen` and does not
keep another copy of the navigation state.

```text
button handler
  -> AppViewModel command
  -> currentScreenChanged
  -> QML visibility bindings update
```

`branchtalk_desktop.app_view_model` tests the state transitions. The
`navigation` scenario in `branchtalk_desktop.ui_flow` checks the visible QML
view. `branchtalk_desktop.view_model_boundary` keeps networking and database
access out of QML.

### Workspace and channel models

`WorkspaceListModel` and `ChannelListModel` are `QAbstractListModel`
implementations. Both expose stable IDs so selection does not depend on a row
number that can change after insertion, removal, or filtering.

The workspace model also exposes loading and empty states. The channel model
handles search, selection, unread counts, mute state, and text or voice channel
type. It updates only the affected roles instead of resetting the full model.

The C++ model tests cover roles and notifications. The `workspace-sidebar` and
`channel-sidebar` scenarios in `branchtalk_desktop.ui_flow` cover the QML
delegates. The matching contract tests prevent temporary QML arrays from
replacing the injected models.

### Message list

`MessageListModel` exposes stable message IDs, author identity, display time,
body text, delivery status, and grouping state as QML roles. The desktop entry
point owns the model and injects it into `Main`; QML does not keep a second
message array.

The conversation panel uses a `ListView` with item reuse instead of a
`Repeater`. Only delegates near the visible area stay instantiated, so a long
conversation does not create one QML object for every message.

The UI flow moves each target row into the viewport before inspecting its
delegate. A null `itemAtIndex()` result for an offscreen row does not mean the
model row is missing; it means `ListView` has not instantiated that delegate.

Updating a message with the same stable ID emits `dataChanged` only for roles
whose values changed. An author change also updates the next row's grouping
role when necessary. The model does not reset, which preserves the row count,
scroll position, and visible delegate instances.

`MessageDelegate` shows the author and timestamp on the first message in a
group, then renders the body and the `Pending`, `Sent`, or `Failed` state. The
model test covers validation, role values, grouping, and targeted updates. The
UI flow scrolls 240 rows and verifies that an in-place update keeps the same
visible item. The contract test prevents a temporary QML array or model reset
from replacing this behavior.

### Message composer and IME handling

`MessageComposerState` owns the draft text, UTF-16 character count, and whether
the draft can be sent. Submission is ignored while an input method is
composing or when the draft contains only whitespace. A successful submission
emits the message once and clears the draft.

`MessageComposer` binds a multiline `TextArea` to that state. Return and keypad
Enter submit a completed draft, while Shift+Enter inserts a newline. During
Korean IME composition, Enter remains with the input method and does not send a
partial message. The behavior is the same on Windows and macOS.

The state test covers whitespace, Unicode character counts, duplicate change
notifications, composition, and reset after submission. The consolidated QML
flow sends key and preedit events through the text area on the offscreen
platform.

### Window state

`WindowStateStore` saves the window position and size through `QSettings`. A
saved state is restored only when all values are valid, the size is above the
minimum, and the window overlaps an available screen. Invalid or off-screen
values fall back to the defaults in `Main.qml`.

## Core contracts

### Errors and results

`Error` stores an `ErrorCode` for program flow and a diagnostic message for
logs. UI text is chosen outside the core library.

`Result<T>` contains either a value or an `Error`. Callers can use
`has_value()` or the explicit boolean conversion, then access the active side
with `value_if()` or `error_if()`. The accessors return a null pointer when the
other side is active.

### Logging

The client and server call `initialize_logging()` and write to stdout using this
format:

```text
[level] [category] message
```

The current categories are `client`, `server`, `database`, and `network`.
`LoggingSettings::level` applies the same minimum level to all four.

Sensitive strings must be wrapped in `SensitiveValue` and passed to
`write_sensitive()`. The logger emits `<redacted>` and never forwards the
original string to the formatter.

### Application configuration

`ApplicationConfig` contains the server address, log level, and data path. The
defaults are `127.0.0.1:8080`, `info`, and `data`.

`load_application_config()` reads these optional JSON fields:

- `server_address`
- `log_level`
- `data_path`

Unknown fields, invalid types, and unsupported log levels return a failed
`Result<ApplicationConfig>` instead of throwing across the application
boundary.

### Server lifetime

`ServerApp` receives a validated configuration and a function that reports
whether shutdown was requested. It initializes logging, writes the start and
stop messages, and keeps signal handling separate from the application loop.

The executable accepts an optional configuration path. A bad configuration
returns a failure exit code before the server lifetime starts.

The `SIGINT` and `SIGTERM` handlers only set a lock-free `std::atomic_flag`.
They do not allocate or write logs. `--smoke-test` raises `SIGTERM` inside the
process and follows the same shutdown path used in normal operation.

## Dependencies

`vcpkg.json` uses manifest mode with a pinned registry baseline. The direct
dependencies are:

| Package | Pinned version | Use |
|---|---|---|
| `nlohmann-json` | `3.12.0#2` | JSON configuration and dependency version output |
| `spdlog` | `1.17.0` | Structured logging |

`dependency_versions_json()` reads the API version macros from the libraries
that were actually linked. The dependency smoke test compares them with the
versions pinned in the manifest.

## CMake presets

The `debug` and `release` presets use the vcpkg toolchain found through
`VCPKG_ROOT`. Packages are installed under `build/vcpkg_installed`.

PowerShell:

```powershell
$env:VCPKG_ROOT = 'C:\path\to\vcpkg'
```

POSIX shell:

```sh
export VCPKG_ROOT=/path/to/vcpkg
```

| Preset | Configuration | Build directory |
|---|---|---|
| `debug` | Debug | `build/debug` |
| `release` | Release | `build/release` |

Common commands:

```sh
cmake --list-presets
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Use `--fresh` when the configure cache must be discarded:

```sh
cmake --preset debug --fresh
cmake --build --preset debug
ctest --preset debug
```

## Compiler warnings

Warnings are set per target:

- MSVC: `/W4`, `/permissive-`
- Apple Clang: `-Wall`, `-Wextra`, `-Wpedantic`

No warning flags are added through `CMAKE_CXX_FLAGS`.
