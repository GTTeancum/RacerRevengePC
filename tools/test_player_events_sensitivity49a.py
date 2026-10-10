#!/usr/bin/env python3
"""Candidate-only 49A semantic controls with frozen original-word references.

A passing unchanged baseline is mandatory. Each control changes an external copy
of the production translation unit, never production, tests, or the instruction
oracle. Only exit 1 with an explicitly expected native assertion is detection;
compiler failures, crashes, timeouts, and unrelated assertions are failures.
Every source/test/oracle input is hashed before and after each subprocess.
"""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PRODUCTION = "src/game/player_events49a.cpp"
TEST = "tests/native/player_events49a.cpp"
SELF = "tools/test_player_events_sensitivity49a.py"
COMMON = ["control_filter48b", "control_shaping48v", "control_binding48w",
          "player_input48x", "device_held48y", "event_dispatch48z"]
FLAGS = ["-std=c++17", "-O2", "-fno-fast-math", "-frounding-math",
         "-ffp-contract=off", "-I", str(ROOT / "src/game")]
FAILURE = re.compile(r"^player_events49a: FAIL (.+)$", re.MULTILINE)
MANAGER = ["manager concrete48Y original mismatch",
           "manager category selection/nonzero normalization",
           "absent/unknown manager source queried",
           "manager callback forwarded arguments"]
PLAYER = ["49A valid differential fixture rejected",
          "49A differential router-only consumption mismatch",
          "49A three ordered manager queries mismatch",
          "49A differential callback count mismatch",
          "49A differential full final player/environment/event/record state mismatch",
          "49A differential callback kind/order mismatch",
          "49A differential source query arguments mismatch",
          "49A differential effect/router arguments mismatch",
          "49A differential callback result fixture mismatch",
          "49A differential complete callback pre-state mismatch",
          "49A differential complete callback post-state mismatch"]
ADAPTER = ["49A press without player accepted",
           "49A press adapter result/sequence mismatch",
           "49A release adapter incorrectly queried/actioned",
           "49A release adapter touched player state",
           "49A release adapter routed press"]


def controls():
    """All anchors are exact and unique; effects remain explicit fixture calls."""
    items = []

    def add(name, area, replacements, expected):
        items.append({"name": name, "area": area, "replacements": replacements,
                      "expected_assertions": expected})

    def one(name, area, old, new, expected=PLAYER):
        add(name, area, [(old, new)], expected)

    one("manager_device_uses_bank_zero", "direct selected device index",
        "const auto* bank=sources.devices[sourceIndex];",
        "const auto* bank=sources.devices[0];", MANAGER)
    one("manager_absent_device_matches", "absent device successful miss",
        "if(!bank)return {ManagerHeldStatus49A::Complete,false};",
        "if(!bank)return {ManagerHeldStatus49A::Complete,true};",
        ["49A absent device fabricated held match"])
    one("manager_unknown_category_matches", "unknown category successful miss",
        "if(category>1u)return {ManagerHeldStatus49A::Complete,false};",
        "if(category>1u)return {ManagerHeldStatus49A::Complete,true};", MANAGER)
    one("manager_reverse_context_auxiliary", "category0 context and category1 auxiliary",
        "category==0u?sources.currentContext:sources.auxiliary",
        "category==0u?sources.auxiliary:sources.currentContext", MANAGER)
    one("manager_absent_context_matches", "null category0 source",
        "if(!source.present)return {ManagerHeldStatus49A::Complete,false};",
        "if(!source.present)return {ManagerHeldStatus49A::Complete,category==0u};", MANAGER)
    one("manager_absent_auxiliary_matches", "null category1 source",
        "if(!source.present)return {ManagerHeldStatus49A::Complete,false};",
        "if(!source.present)return {ManagerHeldStatus49A::Complete,category==1u};", MANAGER)
    one("manager_normalizes_only_one", "normalize every nonzero source return",
        "result.originalReturn!=0u", "result.originalReturn==1u", MANAGER)
    one("manager_source_receives_wrong_code", "source code forwarding",
        "source.query(source.context,code,mask)",
        "source.query(source.context,code^1,mask)", MANAGER)
    one("manager_source_receives_wrong_mask", "source mask forwarding",
        "source.query(source.context,code,mask)",
        "source.query(source.context,code,mask^1u)", MANAGER)
    one("manager_device_receives_wrong_code", "device code forwarding",
        "queryDeviceHeld48Y(*bank,code,mask,context,matched,nullptr)",
        "queryDeviceHeld48Y(*bank,code^1,mask,context,matched,nullptr)", MANAGER)
    one("manager_device_receives_wildcard_mask", "device mask forwarding",
        "queryDeviceHeld48Y(*bank,code,mask,context,matched,nullptr)",
        "queryDeviceHeld48Y(*bank,code,0x3fu,context,matched,nullptr)", MANAGER)
    one("manager_invents_scratch_output", "manager passes null scratch record",
        "queryDeviceHeld48Y(*bank,code,mask,context,matched,nullptr)",
        "queryDeviceHeld48Y(*bank,code,mask,context,matched,"
        "const_cast<DeviceButtonRecord48Y*>(&bank->records[0]))",
        ["manager held query wrote bank without scratch output"])
    for name, old, new in [
            ("context_presence", "sources.currentContext.present,sources.rootFlags228", "false,sources.rootFlags228"),
            ("root_flags", "sources.currentContext.present,sources.rootFlags228", "sources.currentContext.present,0u"),
            ("context518", "sources.context518,sources.context1898", "0u,sources.context1898"),
            ("context1898", "sources.context518,sources.context1898", "sources.context518,0u")]:
        one("manager_drops_" + name, "48Y gate receives live " + name, old, new, MANAGER)

    one("player_missing_effect_noop", "missing helper cannot become successful no-op",
        "if(!bindings.effects.apply){result.status=PlayerEventStatus49A::MissingEffectBinding;return false;}",
        "if(!bindings.effects.apply)return true;",
        ["49A missing effect binding dependency boundary"])
    one("player_ignores_unavailable_effect", "failed synchronous effect stops dependent work",
        "if(!bindings.effects.apply(bindings.effects.context,{kind,scalar,integer}))",
        "if(!bindings.effects.apply(bindings.effects.context,{kind,scalar,integer})&&false)",
        ["49A unavailable effect continued or consumed"])

    first = "query(env.actionRows[player.indexFFC].action6AC,2u,0x3fu,matched)"
    for name, replacement, area in [
            ("action_row_zero", "query(env.actionRows[0].action6AC,2u,0x3fu,matched)", "player action table index"),
            ("event_selects_action", "query(static_cast<std::int32_t>(event.code),2u,0x3fu,matched)", "held query independent of event code"),
            ("first_query_wrong_category", "query(env.actionRows[player.indexFFC].action6AC,1u,0x3fu,matched)", "first query device category2"),
            ("first_query_wrong_mask", "query(env.actionRows[player.indexFFC].action6AC,2u,0x80u,matched)", "first query mask3F")]:
        one("player_" + name, area, first, replacement)
    one("player_device_source_zero", "first query selected player source",
        "queryManagerHeld49A(env.held,code,category,mask,player.source5C)",
        "queryManagerHeld49A(env.held,code,category,mask,0u)")
    add("player_reverses_later_query_codes", "ordered1A then12 held queries", [
        ("query(0x1a,0u,0x80u,matched)", "query(0x12,0u,0x80u,matched)/* switched second */"),
        ("query(0x12,0u,0x80u,matched))return result;", "query(0x1a,0u,0x80u,matched))return result;")], PLAYER)
    for number, code in (("second", "0x1a"), ("third", "0x12")):
        one("player_" + number + "_query_wrong_category", number + " query context category0",
            "query(" + code + ",0u,0x80u,matched)", "query(" + code + ",1u,0x80u,matched)")
        one("player_" + number + "_query_wrong_mask", number + " query mask80",
            "query(" + code + ",0u,0x80u,matched)", "query(" + code + ",0u,0x3fu,matched)")

    gate = "if(matched&&!(env.flags29&4u)&&(env.gate58&2u)&&!(env.gate58&1u))"
    for name, replacement, area in [
            ("first_ignores_held", "if(!(env.flags29&4u)&&(env.gate58&2u)&&!(env.gate58&1u))", "first held result gates effect"),
            ("first_ignores_bit2", "if(matched&&(env.gate58&2u)&&!(env.gate58&1u))", "flags29 bit2 inhibits first action"),
            ("first_ignores_gate_bit1", "if(matched&&!(env.flags29&4u)&&!(env.gate58&1u))", "gate58 bit1 enables first action"),
            ("first_ignores_gate_bit0", "if(matched&&!(env.flags29&4u)&&(env.gate58&2u))", "gate58 bit0 inhibits first action")]:
        one("player_" + name, area, gate, replacement)
    one("player_zero_triggers_first_action", "strict negative1098 predicate",
        "if(player.value1098<0.0f)", "if(player.value1098<=0.0f)")
    call2255 = "if(!effect(PlayerEffectKind49A::Function2255B0))return result;"
    stores2255 = "player.value1094=2.5f;player.value1090=-1.0f;"
    one("player_stores_before2255B0", "stores occur after synchronous2255B0",
        call2255 + "\n            " + stores2255,
        stores2255 + "\n            " + call2255)
    one("player_wrong1094_constant", "binary32 constant2.5 store", "player.value1094=2.5f;", "player.value1094=2.0f;")
    one("player_wrong1090_constant", "binary32 constant-1 store", "player.value1090=-1.0f;", "player.value1090=1.0f;")
    one("player_first_action_consumes", "first action never consumes tail",
        stores2255, stores2255 + "result.handled=true;return result;")
    one("player_second_ignores_held", "second held result gates virtual call",
        "if(matched&&player.field40==0u)", "if(player.field40==0u)")
    one("player_second_ignores_field40", "field40 zero gate",
        "if(matched&&player.field40==0u)", "if(matched)")
    one("player_lazy_initializes_nonzero_flag", "only zero lazyFlag initializes",
        "if(env.lazyFlag85F4==0u)", "if(env.lazyFlag85F4!=1u)")
    one("player_lazy_wrong_word", "lazy default word2",
        "env.lazyWord4340A0=2u;", "env.lazyWord4340A0=1u;")
    one("player_lazy_omits_flag_store", "lazy flag set to1",
        "env.lazyFlag85F4=1u;", "/* omitted lazy flag store */")
    for value, old, new in [
            ("zero", "==0u?PlayerEffectKind49A::Virtual94", "==0u?PlayerEffectKind49A::Virtual90"),
            ("one", "==1u?PlayerEffectKind49A::Virtual90", "==1u?PlayerEffectKind49A::Virtual8C"),
            ("two", "==2u?PlayerEffectKind49A::Virtual8C", "==2u?PlayerEffectKind49A::Virtual88"),
            ("default", ":PlayerEffectKind49A::Virtual88;", ":PlayerEffectKind49A::Virtual94;")]:
        one("player_wrong_virtual_" + value, "lazy switch " + value + " virtual slot", old, new)
    one("player_second_action_consumes", "virtual action never consumes tail",
        "if(!effect(kind))return result;", "if(!effect(kind))return result;result.handled=true;return result;")
    one("player_third_ignores_held", "third held result gates tuning action",
        "if(matched&&player.source5C<=1u)", "if(player.source5C<=1u)")
    one("player_third_rejects_source_one", "third action admits source1",
        "if(matched&&player.source5C<=1u)", "if(matched&&player.source5C<1u)")
    one("player_third_admits_source_two", "third action excludes source2",
        "if(matched&&player.source5C<=1u)", "if(matched&&player.source5C<=2u)")
    for field, expression in (("field40", "player.field40==0u"),
                              ("source5C", "player.source5C<=1u"),
                              ("selection1125F", "env.selection1125F&15u")):
        owner = "env" if field == "selection1125F" else "player"
        add("player_snapshots_" + field, "live " + field + " after earlier callbacks", [
            ("bool matched=false;\n    if(!query(",
             "const auto initialValue=" + owner + "." + field + ";\n    bool matched=false;\n    if(!query("),
            (expression, expression.replace(owner + "." + field, "initialValue"))], PLAYER)
    one("player_full_selection_byte", "selection1125F low nibble only",
        "env.selection1125F&15u", "env.selection1125F")
    one("player_wrong_subtraction_constant", "exact binary32 3C23D70A subtraction operand",
        "std::uint32_t deltaBits=0x3c23d70au;", "std::uint32_t deltaBits=0x3c23d70bu;")
    one("player_adds_delta", "binary32 subtraction direction",
        "const float adjusted=left-right;", "const float adjusted=left+right;")
    one("player_CB4_receives_adjusted", "CB4 preserves authored float",
        "player.valueCB4=row.value102F0;", "player.valueCB4=adjusted;")
    one("player_CB0_receives_unadjusted", "CB0 receives exact adjusted float",
        "player.valueCB0=adjusted;", "player.valueCB0=row.value102F0;")
    one("player_CD0_no_increment", "CD0 row word plus1",
        "player.countCD0=row.word10388+1u;", "player.countCD0=row.word10388;")
    one("player_CD0_saturates", "CD0 increments modulo2^32",
        "player.countCD0=row.word10388+1u;",
        "player.countCD0=row.word10388==0xffffffffu?0xffffffffu:row.word10388+1u;")
    call170 = "if(!effect(PlayerEffectKind49A::Function22B170,player.valueCB0))return result;"
    call370 = "if(!effect(PlayerEffectKind49A::Function22B370,player.valueCB8))return result;"
    add("player_snapshots_CB8", "22B370 reads live CB8 after synchronous22B170", [
        (call170, "const auto initialCB8=player.valueCB8;" + call170),
        (call370, "if(!effect(PlayerEffectKind49A::Function22B370,initialCB8))return result;")], PLAYER)
    one("player_wrong22B170_argument", "22B170 scalar CB0", call170,
        "if(!effect(PlayerEffectKind49A::Function22B170,player.valueCB4))return result;")
    one("player_wrong22B370_argument", "22B370 scalar live CB8", call370,
        "if(!effect(PlayerEffectKind49A::Function22B370,player.valueCB0))return result;")
    one("player_wrong22BA60_argument", "22BA60 integer argument12",
        "Function22BA60,0.0f,12u", "Function22BA60,0.0f,11u")
    call570 = "if(!effect(PlayerEffectKind49A::Function22B570))return result;"
    call760 = "if(!effect(PlayerEffectKind49A::Function22B760))return result;"
    callA60 = "if(!effect(PlayerEffectKind49A::Function22BA60,0.0f,12u))return result;"
    one("player_reverses22B170_22B370", "ordered scalar helper effects",
        call170 + "\n        " + call370, call370 + "\n        " + call170)
    one("player_reverses22B570_22B760", "ordered nonscalar helper effects",
        call570 + "\n        " + call760, call760 + "\n        " + call570)
    for name, call in (("2255B0", call2255), ("virtual", "if(!effect(kind))return result;"),
                       ("22B170", call170), ("22B370", call370), ("22B570", call570),
                       ("22B760", call760), ("22BA60", callA60)):
        one("player_omits_" + name, "mandatory synchronous " + name + " effect", call, "/* omitted effect */")
    one("player_third_action_consumes", "tuning actions never consume tail",
        callA60, callA60 + "result.handled=true;return result;")

    tail = "result.childRoute=routeChildInputEvent48Z(bindings.children,true,event,record,"
    one("player_forwards_as_release", "tail forwards through press router", tail,
        "result.childRoute=routeChildInputEvent48Z(bindings.children,false,event,record,")
    one("player_forwards_event_copy", "exact event pointer forwarded to tail", tail,
        "auto copiedEvent=event;result.childRoute=routeChildInputEvent48Z(bindings.children,true,copiedEvent,record,")
    one("player_forwards_record_copy", "exact record pointer forwarded to tail", tail,
        "auto copiedRecord=record;result.childRoute=routeChildInputEvent48Z(bindings.children,true,event,copiedRecord,")
    one("player_invents_handled", "return final child router result",
        "result.handled=result.childRoute.handled;return result;", "result.handled=true;return result;")
    one("player_drops_handled", "preserve consumed child router result",
        "result.handled=result.childRoute.handled;return result;", "result.handled=false;return result;")
    one("adapter_reverses_press_release", "explicit press versus release adapter",
        "if(press) {", "if(!press) {", ADAPTER)
    one("adapter_release_uses_press_router", "release adapter unchanged release router",
        "routeChildInputEvent48Z(b.children,false,event,record,b.childCallback,b.childContext,b.childVisitBudget)",
        "routeChildInputEvent48Z(b.children,true,event,record,b.childCallback,b.childContext,b.childVisitBudget)", ADAPTER)
    one("adapter_inverts_handled", "adapter exposes final handler/router result",
        "binding.lastResult.handled?1u:0u", "binding.lastResult.handled?0u:1u", ADAPTER)
    return items


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def tracked_inputs():
    names = [PRODUCTION, "src/game/player_events49a.h", TEST, SELF,
             "evidence/playerEvent49A/recovery-manifest.json",
             "evidence/playerEvent49A/original-disassembly.txt"]
    names += ["src/game/" + name + suffix for name in COMMON for suffix in (".cpp", ".h")]
    names += [str(path.relative_to(ROOT)) for pattern in ("*49a*", "*48z.h", "*48y.h")
              for path in sorted((ROOT / "tools/gameplay").glob(pattern)) if path.is_file()]
    # Preserve every local transitive include used by unchanged source/test files.
    pending = list(names)
    seen = set(names)
    while pending:
        name = pending.pop()
        path = ROOT / name
        for include in re.findall(r'^\s*#\s*include\s*"([^"\n]+)"', path.read_text(), re.MULTILINE):
            included = (path.parent / include).resolve()
            if not included.is_file():
                included = (ROOT / "src/game" / include).resolve()
            if included.is_file() and ROOT in included.parents:
                relative = str(included.relative_to(ROOT))
                if relative not in seen:
                    seen.add(relative)
                    pending.append(relative)
    return sorted(seen)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--expected-test-sha256", required=True,
                        help="SHA256 supplied after the test/oracle author freezes the test")
    args = parser.parse_args()
    out = args.output_dir.resolve()
    if out == ROOT or ROOT in out.parents:
        parser.error("Output must be outside source tree")
    out.mkdir(exist_ok=True, parents=True)
    if any(out.iterdir()):
        parser.error("Output must be empty")
    hashes = {name: digest(ROOT / name) for name in tracked_inputs()}
    if hashes[TEST] != args.expected_test_sha256:
        parser.error("Test differs from finalized expected SHA256")
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
        except subprocess.TimeoutExpired as failure:
            def decode(value):
                return value.decode(errors="replace") if isinstance(value, bytes) else (value or "")
            result = {"returncode": None, "stdout": decode(failure.stdout),
                      "stderr": decode(failure.stderr), "timed_out": True}
        (out / (name + ".log")).write_text(result["stdout"] + result["stderr"])
        assert_stable()
        return result

    source = (ROOT / PRODUCTION).read_text()
    mutations = controls()
    rows, baseline, error = [], {}, None
    try:
        # Confirm every expected label is still an actual assertion in the frozen test.
        labels = set(re.findall(r'"([^"\n]+)"\);', (ROOT / TEST).read_text()))
        for mutation in mutations:
            candidate = source
            for old, new in mutation["replacements"]:
                if candidate.count(old) != 1:
                    raise ValueError("Nonunique mutation anchor: " + mutation["name"] + ": " + old)
                candidate = candidate.replace(old, new)
            if not all(label in labels for label in mutation["expected_assertions"]):
                raise ValueError("Expected assertion no longer exists: " + mutation["name"])
            (out / (mutation["name"] + ".cpp")).write_text(candidate)
        run_logged(["g++", "--version"], "compiler-version", 30)
        objects = []
        for name in [TEST] + ["src/game/" + name + ".cpp" for name in COMMON]:
            obj = out / (Path(name).stem + ("-test" if name == TEST else "") + ".o")
            build = run_logged(["g++"] + FLAGS + ["-c", str(ROOT / name), "-o", str(obj)], obj.stem + "-build", 180)
            if build["returncode"] != 0:
                raise RuntimeError("Unchanged baseline input did not compile: " + name)
            objects.append(str(obj))
        binary = out / "baseline"
        build = run_logged(["g++"] + FLAGS + objects + [str(ROOT / PRODUCTION), "-o", str(binary)], "baseline-build", 180)
        baseline["compile_returncode"] = build["returncode"]
        if build["returncode"] != 0:
            raise RuntimeError("Unchanged baseline did not compile")
        baseline.update(run_logged([str(binary)], "baseline-test", 120))
        if baseline["returncode"] != 0 or not baseline["stdout"].startswith("player_events49a: "):
            raise RuntimeError("Unchanged baseline did not pass")
        print("baseline: passed", flush=True)
        for mutation in mutations:
            name = mutation["name"]
            candidate, binary = out / (name + ".cpp"), out / name
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
                           detected=result["returncode"] == 1 and not result["timed_out"]
                           and assertion in mutation["expected_assertions"])
            rows.append(row)
            (out / "progress.json").write_text(json.dumps({"baseline": baseline, "controls": rows}, indent=2) + "\n")
            print(name + ": " + ("detected (" + row["assertion"] + ")" if row["detected"] else "FAILED"), flush=True)
    except (RuntimeError, OSError, ValueError) as failure:
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
              "undetected_controls": [row["name"] for row in rows if not row["detected"]],
              "unexecuted_controls": [mutation["name"] for mutation in mutations[len(rows):]],
              "controls": rows}
    (out / "sensitivity.json").write_text(json.dumps(result, indent=2) + "\n")
    print(result["status"] + ": " + str(result["detected_controls"]) + "/" + str(len(mutations))
          + " expected assertion failures; inputs " + ("unchanged" if stable else "CHANGED"), flush=True)
    if error:
        print(error, flush=True)
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
