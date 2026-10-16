# Reachable native original UI navigation audit

Base0b31d18, source inspection only. No input simulation, application run,
build, test, syntax/sanitizer check or decoder execution was performed.

## Bounded wiring correction

The actual SDL physical keypad Enter edge88 was omitted from the original
frontend confirm dispatch although ordinary Enter40 and Space44 already reached
original input4. HostSessionMenu50IF already recognizes88 independently. The
single driver correction sends either physical Enter edge to the same existing
original input4. This is explicit native keyboard translation, not a claim that
the original executable consumes SDL key codes.

The original frontend still receives controller0 through Session.input;
coordinator.input performs the existing page-specific selection, Accept,
profile/audio proposal and destination admission. Simultaneous confirm keys
produce one event through the same OR condition. Focus/minimize/event-budget,
pause and originalUiActive gates remain unchanged. No event is synthesized and
no game-controller or driving path is altered. No assets, defaults, page state,
back-navigation action or rendering are substituted.

## Existing back and Accept joins

- Options1F9640 Back stores actual remembered slot6E4 then proposes Main4/1
  at1F99A4. OptionsSession converts only the exact destination-owner-blocked
  proposal; Frontend admits the real Main owner and deferred transition.
- Controls pages use the existing controls controller plan. Accept/cancel
  proposals preserve the original local/profile distinction and return to
  Options1; source audio readiness remains required where enabled.
- Game Options4 Cancel proposes Options1 and remembered group6E8 without
  committing pending settings. Accept requires the concrete sound-mode owner
  and actual frontend mixer before committing source settings. Muted-only
  startup therefore does not certify that Accept branch.

No additional concrete back-navigation defect was identified in this pass.

## Remaining reachable limits

- Options slot2/page3 is prepared but explicitly unadmitted. The real menu
  controller/hotplug mapping and original missing-controller popup owner are
  missing. Passing a boolean secondControllerPresent must not bypass this.
- Options slot4 requests screen11/page1 (mode5), and slots5/6 request Options
  pages5/6. These destination owners remain unavailable. Existing rejection
  keeps prospective audio/profile writes uncommitted.
- Actual frontend input currently maps physical keyboard edges to controller0.
  Game-controller menu navigation is not joined. The race-only Start bridge
  explicitly excludes originalUiActive and controls the host pause owner.
- Race Pause remains HostSessionMenu50IF: its source declares host-only
  navigation, and the driver renders that host overlay. There is no original
  pause-page session/resource/transition owner in this active route. Pause,
  Resume, Restart and Return Original Main cannot be labeled an original pause
  UI merely because they correctly suspend and retire the original race.
- Return Original Main and restart paths already clear the explicit application
  pause word before whole-world departure, including F5/stdin restart that
  bypasses menu.poll. Focus-loss suppression does not write that source word.

These larger gaps require authentic source owners/assets and corresponding
transition/input lifecycle work. This patch does not admit them or replace
original pages with host menus. Root owns centralized validation.
