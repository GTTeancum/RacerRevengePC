#!/usr/bin/env python3
"""Candidate-only 48Z semantic controls; original-word oracles stay unchanged.

Build a passing baseline, then mutate only an external copy of the production
translation unit. A control is detected only by a normal exit-1 test assertion
with an expected label: compilation failures, crashes and timeouts never count.
Hash every source/test/oracle input before and after every run to reject drift.
"""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PRODUCTION = "src/game/event_dispatch48z.cpp"
TEST = "tests/native/event_dispatch48z.cpp"
COMMON = ["control_filter48b", "control_shaping48v", "control_binding48w",
          "player_input48x", "device_held48y"]
FLAGS = ["-std=c++17", "-O2", "-fno-fast-math", "-frounding-math",
         "-ffp-contract=off", "-I", str(ROOT / "src/game")]
FAILURE = re.compile(r"^event_dispatch48z after [0-9]+ checks "
                     r"\([0-9]+ managers, [0-9]+ routers\): (.+)$", re.MULTILINE)
MANAGER = ["valid manager fixture rejected", "manager completed-event count mismatch",
           "manager callback count/identity mismatch", "manager live queue/context mismatch",
           "manager full event storage mismatch", "manager context bank mismatch",
           "manager auxiliary bank mismatch", "manager device bank mismatch",
           "callback original site/order mismatch", "callback selected identity mismatch",
           "callback exact pointer identity mismatch", "callback event snapshots mismatch",
           "callback record snapshots mismatch"]
ROUTER = ["router result differential", "router visit count differential",
          "router callback count/identity mismatch", "router parent mutation differential",
          "router live-node mutation differential", "router event/record state differential",
          "callback original site/order mismatch", "callback selected identity mismatch",
          "callback event snapshots mismatch", "callback record snapshots mismatch"]


def controls():
    """Each replacement anchor must occur once, including multi-edit controls."""
    items = []

    def add(name, area, replacements, expected):
        items.append({"name": name, "area": area, "replacements": replacements,
                      "expected_assertions": expected})

    def one(name, area, old, new, expected):
        add(name, area, [(old, new)], expected)

    one("manager_exclusive_release_press", "live release-to-press",
        "if(event.transition==0u&&!invoke(callbacks.release))return result;",
        "if(event.transition==0u){if(!invoke(callbacks.release))return result;} else",
        MANAGER)
    one("manager_press_on_any_nonzero", "transition identity",
        "if(event.transition==1u&&!invoke(callbacks.press))return result;",
        "if(event.transition!=0u&&!invoke(callbacks.press))return result;", MANAGER)
    for field in ("code", "deviceId"):
        add("manager_cache_" + field, "live event " + field, [
            ("const auto category=event.category;",
             "const auto category=event.category;const auto initialField=event." + field + ";"),
            ("auto* record=resolve(category,event,context,sources);",
             "auto lookupEvent=event;lookupEvent." + field + "=initialField;"
             "auto* record=resolve(category,lookupEvent,context,sources);")], MANAGER)
    one("manager_reselect_category_after_release", "category selected once",
        "auto* record=resolve(category,event,context,sources);",
        "auto* record=resolve(event.category,event,context,sources);", MANAGER)
    add("manager_snapshot_count", "live event count", [
        ("auto* const storage=q.events;const auto capacity=q.capacity;",
         "auto* const storage=q.events;const auto capacity=q.capacity;const auto initialCount=q.count;"),
        ("while(q.count>0&&result.eventsCompleted<static_cast<std::uint32_t>(q.count))",
         "while(initialCount>0&&result.eventsCompleted<static_cast<std::uint32_t>(initialCount))")], MANAGER)
    one("manager_cancel_inflight_press_after_shrink", "in-flight count semantics",
        "if(event.transition==1u&&!invoke(callbacks.press))return result;",
        "if(q.count>0&&event.transition==1u&&!invoke(callbacks.press))return result;", MANAGER)
    for field in ("present", "mask1898"):
        add("manager_snapshot_context_" + field, "live context " + field, [
            ("auto* const storage=q.events;const auto capacity=q.capacity;",
             "auto* const storage=q.events;const auto capacity=q.capacity;const auto initialContext=context;"),
            ("event.contextMask=context.present?context.mask1898:0u;",
             "event.contextMask=" + ("initialContext.present?context.mask1898:0u;" if field == "present"
                                     else "context.present?initialContext.mask1898:0u;"))], MANAGER)
    one("manager_retag_between_release_and_press", "one live tag per event",
        "if(event.transition==1u&&!invoke(callbacks.press))return result;",
        "if(event.transition==1u)event.contextMask=context.present?context.mask1898:0u;\n"
        "        if(event.transition==1u&&!invoke(callbacks.press))return result;", MANAGER)
    one("manager_wrong_absent_context_tag", "absent context zero tag",
        "event.contextMask=context.present?context.mask1898:0u;",
        "event.contextMask=context.present?context.mask1898:0x3fu;", MANAGER)
    one("manager_stop_on_consumed_return", "manager ignores callback return",
        "return true; // originalReturn is deliberately ignored by the manager.",
        "return outcome.originalReturn==0u; // Incorrect consuming manager.", MANAGER)
    one("manager_context_uses_auxiliary_bank", "category 0 record bank",
        "span=context.records;", "span=sources.auxiliary;", MANAGER)
    one("manager_auxiliary_uses_context_bank", "category 1 record bank",
        "else if(selectedCategory==1)span=sources.auxiliary;",
        "else if(selectedCategory==1)span=context.records;", MANAGER)
    one("manager_device_uses_bank_zero", "category 2 device bank",
        "span=sources.devices[event.deviceId];", "span=sources.devices[0];", MANAGER)
    add("manager_cache_release_record", "live record re-resolution", [
        ("const auto category=event.category;",
         "const auto category=event.category;auto* const initialRecord=resolve(category,event,context,sources);"),
        ("auto* record=resolve(category,event,context,sources);", "auto* record=initialRecord;")], MANAGER)
    add("manager_snapshot_context_record_binding", "live context record binding", [
        ("const auto category=event.category;", "const auto category=event.category;const auto initialContext=context;"),
        ("auto* record=resolve(category,event,context,sources);",
         "auto* record=resolve(category,event,initialContext,sources);")],
        MANAGER + ["press cached release's record binding"])
    add("manager_snapshot_source_record_bindings", "live auxiliary/device record bindings", [
        ("const auto category=event.category;", "const auto category=event.category;const auto initialSources=sources;"),
        ("auto* record=resolve(category,event,context,sources);",
         "auto* record=resolve(category,event,context,initialSources);")],
        MANAGER + ["press cached release's record binding", "late release-to-press boundary mismatch"])

    gate = "if(!(router.flags24&(press?0x400u:0x200u)))return result;"
    one("router_release_uses_press_gate", "parent release 0x200 gate", gate,
        "if(!(router.flags24&0x400u))return result;", ROUTER)
    one("router_press_uses_release_gate", "parent press 0x400 gate", gate,
        "if(!(router.flags24&0x200u))return result;", ROUTER)
    one("router_ignores_parent_gate", "parent gates before traversal", gate,
        "if(false)return result;", ROUTER)
    one("router_rechecks_parent_after_callback", "parent gate loaded once",
        "child=storage[child].next; // Deliberately after callback, never a snapshot.",
        "if(!(router.flags24&(press?0x400u:0x200u)))return result;\n"
        "        child=storage[child].next;", ROUTER)
    add("router_reloads_first_child", "first child loaded once", [
        ("auto child=router.firstChild;", "auto child=router.firstChild;const auto initialFirst=child;"),
        ("child=storage[child].next; // Deliberately after callback, never a snapshot.",
         "child=router.firstChild!=initialFirst?router.firstChild:storage[child].next;")], ROUTER)
    for name, predicate, area in [
            ("router_ignores_bit0", "!(flags&8u)", "child bit0 enable"),
            ("router_ignores_bit3", "(flags&1u)", "child bit3 exclusion"),
            ("router_invents_bit2_exclusion", "(flags&1u)&&!(flags&12u)", "child bit2 must not exclude")]:
        one(name, area, "if((flags&1u)&&!(flags&8u))", "if(" + predicate + ")", ROUTER)
    add("router_snapshots_child_flags", "live future-child flags", [
        ("auto* const storage=router.children;const auto count=router.childCount;",
         "auto* const storage=router.children;const auto count=router.childCount;"
         "EventChild48Z initialNodes[32]{};if(storage&&count<=32)for(unsigned i=0;i<count;++i)initialNodes[i]=storage[i];"),
        ("const auto flags=storage[child].flags29;",
         "const auto flags=child<32?initialNodes[child].flags29:storage[child].flags29;")], ROUTER)
    add("router_snapshots_sibling_link", "live sibling links", [
        ("const auto flags=storage[child].flags29;",
         "const auto flags=storage[child].flags29;const auto initialNext=storage[child].next;"),
        ("child=storage[child].next; // Deliberately after callback, never a snapshot.",
         "child=initialNext;")], ROUTER)
    one("router_ignores_consumed_return", "first consumed result stops traversal",
        "if(outcome.originalReturn!=0u){result.handled=true;return result;}",
        "if(false){result.handled=true;return result;}", ROUTER)
    one("router_consumes_only_exact_one", "any nonzero return consumes",
        "if(outcome.originalReturn!=0u){result.handled=true;return result;}",
        "if(outcome.originalReturn==1u){result.handled=true;return result;}", ROUTER)
    one("router_checks_storage_after_consumption", "consumption before further storage access",
        "if(outcome.originalReturn!=0u){result.handled=true;return result;}",
        "if(router.children!=storage||router.childCount!=count){result.status=EventRouteStatus48Z::StorageChanged;return result;}\n"
        "            if(outcome.originalReturn!=0u){result.handled=true;return result;}",
        ["handled child incorrectly examined next storage"])

    bit3 = "if(app.rootFlags228&8u){app.lastRoute.handled=true;return {true,1};}"
    bit2 = "if(press&&(app.rootFlags228&4u)){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}"
    post = "if(press&&!app.lastRoute.handled&&(app.rootFlags228&4u)){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}"
    one("bridge_ignores_root_bit3", "root bit3 consumes", bit3,
        "if(false){app.lastRoute.handled=true;return {true,1};}",
        ["root bit3 bridge consumption failed"])
    one("bridge_bit3_returns_unhandled", "root bit3 handled return", bit3,
        "if(app.rootFlags228&8u){app.lastRoute.handled=true;return {true,0};}",
        ["root bit3 bridge consumption failed"])
    one("bridge_ignores_initial_press_bit2", "normal press bit2 exclusion", bit2,
        "if(false){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}",
        ["special press mode accepted"])
    one("bridge_applies_press_bit2_to_release", "release allows root bit2", bit2,
        "if(app.rootFlags228&4u){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}",
        ["normal category2 bridge routing failed"])
    one("bridge_bit2_before_bit3", "root bit3 precedes special mode", bit3 + "\n    " + bit2,
        bit2 + "\n    " + bit3, ["root bit3 bridge consumption failed"])
    one("bridge_ignores_post_unhandled_bit2", "post-unhandled live bit2", post,
        "if(false){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}",
        ["live post-routing special mode silently skipped"])
    add("bridge_snapshots_post_unhandled_bit2", "post-unhandled live root flags", [
        ("auto& app=*static_cast<JoystickApplicationBridge48Z*>(opaque);",
         "auto& app=*static_cast<JoystickApplicationBridge48Z*>(opaque);const auto initialFlags=app.rootFlags228;"),
        (post, "if(press&&!app.lastRoute.handled&&(initialFlags&4u)){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}")],
        ["live post-routing special mode silently skipped"])
    one("bridge_special_fallback_after_handled", "handled press skips special fallback", post,
        "if(press&&(app.rootFlags228&4u)){app.lastRoute.status=EventRouteStatus48Z::UnsupportedMode;return {false,0};}",
        ["handled child incorrectly entered special fallback"])
    one("bridge_release_uses_press_route", "release wrapper route identity",
        "return bridge(p,false,e,r);", "return bridge(p,true,e,r);",
        ["bridge child exact binding mismatch"])
    one("bridge_press_uses_release_route", "press wrapper route identity",
        "return bridge(p,true,e,r);", "return bridge(p,false,e,r);",
        ["bridge child exact binding mismatch"])

    append_words = "q.events[count+i]={e.buttonCode,e.kind,e.deviceId,e.held,e.contextMask};"
    one("append_overwrites_existing_events", "append at current count", append_words,
        "q.events[i]={e.buttonCode,e.kind,e.deviceId,e.held,e.contextMask};",
        ["append replaced existing events"])
    one("append_reverses_event_order", "ordered transition append",
        "const auto& e=batch.transitions[i];", "const auto& e=batch.transitions[batch.transitionCount-1-i];",
        ["48Y appended words/order mismatch"])
    for field, value in (("kind", "1u"), ("deviceId", "0u"), ("held", "2u"), ("contextMask", "0u")):
        one("append_changes_" + field, "preserve appended " + field, append_words,
            append_words.replace("e." + field, value), ["48Y appended words/order mismatch"])
    one("append_count_replaces_instead_of_adds", "append count includes prior queue",
        "q.count=static_cast<std::int32_t>(count+batch.transitionCount);return true;",
        "q.count=static_cast<std::int32_t>(batch.transitionCount);return true;",
        ["48Y ordered append rejected"])
    one("append_rejects_exact_capacity", "exact capacity is valid",
        "batch.transitionCount>q.capacity-count", "batch.transitionCount>=q.capacity-count",
        ["empty append required storage", "exact append boundary rejected"])
    one("append_ignores_capacity", "bounded append capacity",
        "batch.transitionCount>q.capacity-count", "false",
        ["invalid bounded append accepted"])
    one("reset_keeps_nonzero_count", "reset only queue count",
        "void resetInputEventCount48Z(InputEventQueue48Z& q) noexcept {q.count=0;}",
        "void resetInputEventCount48Z(InputEventQueue48Z& q) noexcept {q.count=1;}",
        ["reset changed queue ownership/payload"])
    one("reset_changes_event_payload", "reset preserves queued payload",
        "void resetInputEventCount48Z(InputEventQueue48Z& q) noexcept {q.count=0;}",
        "void resetInputEventCount48Z(InputEventQueue48Z& q) noexcept {if(q.count>0)q.events[0].code=0;q.count=0;}",
        ["reset changed queue ownership/payload"])
    return items


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    args = parser.parse_args()
    out = args.output_dir.resolve()
    if out == ROOT or ROOT in out.parents:
        parser.error("Output must be outside source tree")
    out.mkdir(exist_ok=True, parents=True)
    if any(out.iterdir()):
        parser.error("Output must be empty")
    tracked = [PRODUCTION, "src/game/event_dispatch48z.h", TEST,
               "tools/test_event_dispatch_sensitivity48z.py"]
    tracked += ["src/game/" + name + suffix for name in COMMON for suffix in (".cpp", ".h")]
    tracked += [str(path.relative_to(ROOT)) for pattern in ("*48z.h", "*48y.h")
                for path in sorted((ROOT / "tools/gameplay").glob(pattern))]
    hashes = {name: digest(ROOT / name) for name in sorted(set(tracked))}
    (out / "input-hashes-before.json").write_text(json.dumps(hashes, indent=2) + "\n")

    def assert_stable():
        changed = [name for name, before in hashes.items() if digest(ROOT / name) != before]
        if changed:
            raise RuntimeError("Source/test/oracle inputs changed: " + ", ".join(changed))

    def run_logged(command, name, timeout):
        (out / (name + "-command.json")).write_text(json.dumps(command, indent=2) + "\n")
        try:
            process = subprocess.run(command, capture_output=True, text=True, timeout=timeout)
            result = {"returncode": process.returncode, "stdout": process.stdout,
                      "stderr": process.stderr, "timed_out": False}
        except subprocess.TimeoutExpired as error:
            def decode(value):
                return value.decode(errors="replace") if isinstance(value, bytes) else (value or "")
            result = {"returncode": None, "stdout": decode(error.stdout),
                      "stderr": decode(error.stderr), "timed_out": True}
        (out / (name + ".log")).write_text(result["stdout"] + result["stderr"])
        assert_stable()
        return result

    source = (ROOT / PRODUCTION).read_text()
    mutations = controls()
    assertion_labels = set(re.findall(r'"([^"\n]+)"\);', (ROOT / TEST).read_text()))
    for mutation in mutations:
        candidate = source
        for old, new in mutation["replacements"]:
            if candidate.count(old) != 1:
                raise ValueError("Nonunique mutation anchor: " + mutation["name"] + ": " + old)
            candidate = candidate.replace(old, new)
        if not all(label in assertion_labels for label in mutation["expected_assertions"]):
            raise ValueError("Expected assertion no longer exists: " + mutation["name"])
        (out / (mutation["name"] + ".cpp")).write_text(candidate)

    rows = []
    baseline = {}
    error = None
    try:
        objects = []
        for name in [TEST] + ["src/game/" + name + ".cpp" for name in COMMON]:
            obj = out / (Path(name).stem + ("-test" if name == TEST else "") + ".o")
            build = run_logged(["g++"] + FLAGS + ["-c", str(ROOT / name), "-o", str(obj)], obj.stem + "-build", 180)
            if build["returncode"] != 0:
                raise RuntimeError("Unchanged baseline input did not compile: " + name)
            objects.append(str(obj))
        baseline_binary = out / "baseline"
        build = run_logged(["g++"] + FLAGS + objects + [str(ROOT / PRODUCTION), "-o", str(baseline_binary)], "baseline-build", 180)
        baseline["compile_returncode"] = build["returncode"]
        if build["returncode"] != 0:
            raise RuntimeError("Unchanged baseline did not compile")
        baseline.update(run_logged([str(baseline_binary)], "baseline-test", 120))
        if baseline["returncode"] != 0 or not baseline["stdout"].startswith("event_dispatch48z: "):
            raise RuntimeError("Unchanged baseline did not pass")
        print("baseline: passed", flush=True)
        for mutation in mutations:
            name = mutation["name"]
            candidate = out / (name + ".cpp")
            binary = out / name
            build = run_logged(["g++"] + FLAGS + objects + [str(candidate), "-o", str(binary)], name + "-build", 180)
            row = {key: value for key, value in mutation.items() if key != "replacements"}
            row.update(candidate_sha256=digest(candidate), compile_returncode=build["returncode"],
                       compile_timed_out=build["timed_out"], detected=False)
            if build["returncode"] == 0:
                result = run_logged([str(binary)], name + "-test", 120)
                match = FAILURE.search(result["stderr"])
                assertion = match.group(1) if match else None
                row.update(returncode=result["returncode"], timed_out=result["timed_out"],
                           assertion=assertion, output=result["stdout"] + result["stderr"],
                           detected=result["returncode"] == 1 and assertion in mutation["expected_assertions"])
            rows.append(row)
            print(name + ": " + ("detected (" + row["assertion"] + ")" if row["detected"] else "FAILED"), flush=True)
    except (RuntimeError, OSError) as failure:
        error = str(failure)
    after = {name: digest(ROOT / name) for name in hashes}
    (out / "input-hashes-after.json").write_text(json.dumps(after, indent=2) + "\n")
    stable = hashes == after
    passed = not error and stable and len(rows) == len(mutations) and all(row["detected"] for row in rows)
    result = {"status": "passed" if passed else "failed", "error": error,
              "production_sha256": hashes[PRODUCTION], "test_sha256": hashes[TEST],
              "inputs_unchanged": stable, "input_sha256": hashes,
              "baseline": baseline, "expected_controls": len(mutations),
              "detected_controls": sum(row["detected"] for row in rows),
              "surviving_mutations": [row["name"] for row in rows if row.get("returncode") == 0],
              "controls": rows}
    (out / "sensitivity.json").write_text(json.dumps(result, indent=2) + "\n")
    print(result["status"] + ": " + str(result["detected_controls"]) + "/" + str(len(mutations)) + " expected assertion failures; inputs " + ("unchanged" if stable else "CHANGED"), flush=True)
    if error:
        print(error, flush=True)
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
