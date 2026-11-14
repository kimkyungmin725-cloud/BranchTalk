# BranchTalk Accelerated Track Rules

## Baseline and stages

- This directory starts from the stage 16 baseline, which includes the channel
  sidebar.
- Read the next goal from
  `D:/Projects/chatroom/course-workspace/accelerated/sessions/<NNN>/session.md`.
- Implement stages 17 through 28 in order. Do not pull later work into the
  current stage.
- Use the matching `materials/student-material.md` and
  `materials/teacher-script.md` as the learning references.

## Code and Git

- Change product code and local tests only in this repository.
- Do not add a remote to this local Git repository.
- Do not commit build output.
- Write source comments, UI text, and repository documentation in English.
- Use ASCII text in repository files. Avoid smart punctuation, decorative
  symbols, and non-ASCII separators.
- A commit should contain one change that can be explained and reverted as a
  unit, together with its tests.
- Do not force a whole stage into one commit, and do not split commits by file count.
- Read `git diff --cached` before each commit. The subject must describe the
  main staged change.
- Write commit subjects in English and use ASCII characters only.
- Use a specific action in the subject and do not end it with a period. Avoid
  generic subjects such as `work`, `cleanup`, `update`, `integration`, or
  `finish`.
- Do not put `session`, a stage number, an internal tag, or another tracking ID
  in the subject.

## Tests

- While implementing, repeat only the relevant `fast` or `integration` label.
- Before finishing a stage, run
  `python tools/stage_check.py --build-dir <build-dir> --config <config>`.
- Do not reduce coverage for security, permissions, migrations, idempotency, or
  crash recovery.
- Do not add a new test executable or source-string check for every small feature.
- When CMake or QML structure changes, run the `contract` checks as well.
- Do not add remote CI configuration.

## Materials and state

- Manage accelerated materials and stage state only under
  `D:/Projects/chatroom/course-workspace/accelerated/`.
- Do not change the original course materials or tags.
- Check stage state with
  `python D:/Projects/chatroom/tools/acceleratedctl.py status <NNN>`.
- Materials remain drafts until the code is captured. Review them against the
  captured code and test results before closing them.
- After the material review, run
  `python D:/Projects/chatroom/tools/acceleratedctl.py close-materials <NNN>`.
