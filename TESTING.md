# Local Testing

The normal test loop runs behavior checks that are useful during implementation.
Source layout and CMake contract checks stay registered but run only when requested.

## Configure once

Keep one incremental build directory with the desktop and contract tests enabled.

PowerShell:

```powershell
cmake -S . -B build/local `
  -DBRANCHTALK_BUILD_DESKTOP=ON `
  -DBUILD_TESTING=ON `
  -DBRANCHTALK_ENABLE_CONTRACT_TESTS=ON
cmake --build build/local --config Debug --parallel 4
ctest --test-dir build/local -C Debug -L fast --output-on-failure -j 4
ctest --test-dir build/local -C Debug -L integration --output-on-failure -j 4
```

macOS or another single-configuration generator:

```sh
cmake -S . -B build/local \
  -DBRANCHTALK_BUILD_DESKTOP=ON \
  -DBUILD_TESTING=ON \
  -DBRANCHTALK_ENABLE_CONTRACT_TESTS=ON
cmake --build build/local --parallel 4
ctest --test-dir build/local -L fast --output-on-failure -j 4
ctest --test-dir build/local -L integration --output-on-failure -j 4
```

The `fast` label covers value types, models, and other checks that do not need an
external process. The `integration` label covers server execution, log files,
QML loading, and user flows that cross component boundaries.

Contract tests are registered in the same build directory but are not included
in the `fast` or `integration` labels.

## Contract tests

Run contract tests after changing CMake targets, directory boundaries, or QML
source structure:

```powershell
ctest --test-dir build/local -C Debug -L contract --output-on-failure -j 4
```

Keep the contract tests in the build. Selecting them by label avoids CMake and
vcpkg reconfiguration before and after every run.

## Stage check

Run one complete local check before finishing a stage:

```powershell
python tools/stage_check.py --build-dir build/local --config Debug
```

```sh
python3 tools/stage_check.py --build-dir build/local --config Debug
```

The command checks repository paths and text files for non-ASCII characters,
builds the default graph, and runs `fast`, `integration`, and `contract`. It
reports an error before the build when a label has no registered tests.
Configure with `BRANCHTALK_ENABLE_CONTRACT_TESTS=ON` before using it.

The default worker count is 1 because parallel Visual Studio and Qt QML builds
have been unreliable in this project. Use `--jobs 2` or a higher value only
after confirming that the local toolchain handles it consistently.

## UI regression flow

`branchtalk_desktop.ui_flow` runs these scenarios in one application process.
Each scenario receives a new QML engine and fresh model objects.

- Navigation and ViewModel bindings
- Immediate theme updates
- Wide and compact layouts
- Workspace list updates
- Channel search, selection, and collapse

Use the existing diagnostic arguments when one scenario needs to be isolated:

```powershell
build\local\Debug\branchtalk_desktop.exe --navigation-test
build\local\Debug\branchtalk_desktop.exe --theme-test
build\local\Debug\branchtalk_desktop.exe --layout-test
build\local\Debug\branchtalk_desktop.exe --workspace-model-test
build\local\Debug\branchtalk_desktop.exe --channel-model-test
```

On macOS or with a single-configuration generator, run the same arguments with
`build/local/branchtalk_desktop`.

## Adding tests

- Add a CTest entry only when an existing suite and label cannot isolate the
  boundary.
- Add scenarios to the existing executable for other behavior in the same layer.
- Prefer public behavior and state transitions over source-string checks.
- Keep security, database migration, permission, idempotency, and recovery
  coverage.
- Run packaging and performance checks only in the stages that need them.
- Run the full regression once before a stage is finished. Use the relevant
  label while implementing.
