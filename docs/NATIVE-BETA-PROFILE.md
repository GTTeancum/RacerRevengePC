# v101 native profile and Options usage

Implementation draft for the integrated beta candidate. The code described below has been written; this document does not certify a build or runtime smoke check.

## What is retained

The beta keeps two separate native user-data files:

- `progress-v1.bin`: the native profile, including career progress, earned/unspent credits, supported unlock/selection state, and Hall/name records handled by the profile owner.
- `settings-v1.bin`: accepted Game Options and control presets, including Auto-save, sound mode, the four volume levels, vibration, split-screen preference, and the remembered settings group.

These are versioned native beta files, not original PlayStation 2 memory-card images. The beta does not write over an imported original-card file. Saving a split-screen preference does not create a supported two-player race mode.

The Linux implementation chooses its user-data directory in this order:

1. `RR_NATIVE_SAVE_DIR`, when set to a nonempty path.
2. An absolute `XDG_DATA_HOME`, followed by `racer-revenge`.
3. `HOME`, followed by `.local/share/racer-revenge`.

An explicitly empty `RR_NATIVE_SAVE_DIR` disables selection of a save directory. If no usable directory is available, the application retains its current session but cannot claim persistence. The current file helper is POSIX-based; this draft does not claim Windows save support.

## Game Options and Controls

Use the normal Main → Options route.

- Game Options: Up/Down selects a setting; Left/Right changes it. R restores the local defaults, Enter accepts, and Escape cancels.
- Controls: use the existing controller/preset page and its normal Accept or Cancel action.
- Accept saves the committed preferences to `settings-v1.bin`. Defaults alone, unaccepted changes, Cancel and Back do not write the settings file.
- Accepted settings are restored at the next application startup, before the shared sound-mode/audio owner is initialized.

The career Auto-save switch does not suppress an explicit settings Accept. Turning Auto-save off is itself a preference that must survive restarting the application.

## Manual native profile Save and Load

Open Main → Options → Memory Card. This existing page is the entry point, but the native window title explicitly identifies the beta PROFILE file rather than claiming an original memory-card operation.

- Tab, or the mapped Circle input: save the current native profile.
- R, or the mapped Square input: request a load.
- Press R/Square a second time to confirm replacing unsaved in-memory profile changes with the saved profile.
- Escape, or the mapped Triangle input, cancels a pending load confirmation without also leaving the page. Otherwise it follows the normal Back route.

The window title reports the requested action and the resulting success or failure. Manual profile Load does not reload `settings-v1.bin`; preferences remain independently governed by their own Accept and startup restore path.

Load is transactional: a missing, malformed or unreadable file must not partially install a profile. A successful load changes the profile generation so subsequent menus reconstruct their observations from the restored profile rather than reuse stale career snapshots.

## Auto-save and unsaved progress

The live committed Game Options Auto-save bit controls automatic native profile saves at the integrated progression/selection boundaries. A manual Save remains available when Auto-save is off.

Disabling Auto-save or encountering a disk failure does not erase earned in-memory progress. The native backend allows ordinary career navigation while retaining the real dirty state; it does not falsely clear that state to claim a successful save. Save manually before quitting if Auto-save is off. If saving fails, the unsaved changes may be lost when the process exits.

## Failures and file safety

Settings and profile warnings share a nonmodal native window-title status. They do not replace the original menu artwork or introduce a modal dialog into animation/physics processing.

- Missing file: a new profile/preferences session may start without an imported save.
- Corrupt or unreadable file: keep current session data intact and report the failure; do not silently overwrite the rejected file.
- Locked file: another process may own the corresponding save lock. Do not run simultaneous writers against the same user-data directory.
- Write failure: do not claim that the current session was saved. The atomic-write helper preserves the previous destination when failure occurs before replacement.
- Save unconfirmed: replacement occurred, but final directory durability could not be confirmed. This is not the same as either a confirmed durable save or a definitely unchanged old file.

For a normal writable settings session, accepting again can retry a failed write. If the settings store was disabled at startup by corruption, access failure or lock contention, correct the cause and restart before attempting restoration again. Preserve a copy of any damaged file before repairing or removing it.

## Resuming a career

The resumed Tournament → Course path uses the selected career's real progression to choose the next original course and tier. The implemented continuation range covers progress values 0 through 12. Nonzero credits, kills and prior results no longer incorrectly require a clean first-course profile.

Course information uses retained remaining prize, actual previous place and the recovered course-specific kill-bonus percentages. A prior unsuccessful attempt can display its actual numeric best place rather than a false never-raced status.

Only the first course's PAR value is currently authenticated. For other courses, the beta deliberately omits the unknown PAR marker and highlight instead of guessing a number. Completed-career presentation, full original medal/trophy branch parity and unrelated Garage purchase behavior are not certified by this work.

## Vibration

The race impact-feedback path now consumes the committed Game Options vibration bit instead of always using an enabled default. This feeds the existing actual controller-feedback/SDL rumble path. It does not invent keyboard vibration or claim feedback from an absent, disconnected or unsupported controller. Physical rumble still requires a compatible connected gamepad and a qualifying original gameplay feedback event; changing the option alone does not play a test vibration.
