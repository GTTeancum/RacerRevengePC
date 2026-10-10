#!/usr/bin/env python3
"""Candidate-only compact authored-world 49B semantic sensitivity checks.

An unchanged baseline must pass first. Only an external copy of the new 49B
translation unit is mutated. Importer, original decoder, test, and authentic
RRWORLD1 inputs stay byte-identical throughout. A control counts only after
compiling and exiting 1 at an explicitly expected assertion. Build failures,
crashes, timeouts, unrelated assertions, and surviving controls are failures.
"""
import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PRODUCTION = "src/assets/world_index49b.cpp"
HEADER = "src/assets/world_index49b.h"
TEST = "tests/native/world_index49b.cpp"
SELF = "tools/test_world_index_sensitivity49b.py"
COMMON = ["src/assets/import_world48f.cpp", "src/assets/world_asset48f.cpp"]
FIXTURE_ROOT = "evidence/nativePc48K-build-20261003-174328-460"
WORLD_NAMES = ("ga", "gb", "gc", "ma", "mb", "ra", "rc", "sa", "sc", "ta", "tb", "tc")
FLAGS = ["-std=c++17", "-O2", "-fno-fast-math", "-frounding-math", "-ffp-contract=off", "-I", str(ROOT / "src/assets")]
FAILURE = re.compile(r"^FAIL world_index49b: (.+) checks=\d+$", re.MULTILINE)


def controls():
    items = []

    def one(name, area, old, new, *expected):
        items.append({"name": name, "area": area, "replacements": [(old, new)], "expected_assertions": list(expected)})

    exact = "binary64 bit identity"
    one("perturb_position_one_binary64_ulp", "exact stored position field copying",
        "std::memcpy(d.position.data(), s.position.data(), sizeof(d.position));",
        "std::memcpy(d.position.data(), s.position.data(), sizeof(d.position)); d.position[0] = std::nextafter(d.position[0], std::numeric_limits<double>::infinity());", exact)
    one("narrow_matrix_to_binary32", "normalized matrix preserves binary64 precision",
        "std::memcpy(d.matrix.data(), s.matrix.data(), sizeof(d.matrix));",
        "for (std::size_t k = 0; k < d.matrix.size(); ++k) d.matrix[k] = static_cast<float>(s.matrix[k]);", exact)
    one("normalize_negative_zero", "preserve signed zero in authored positions",
        "std::memcpy(d.position.data(), s.position.data(), sizeof(d.position));",
        "std::memcpy(d.position.data(), s.position.data(), sizeof(d.position)); for (double& x : d.position) if (x == 0) x = 0.0;", exact)
    for field, wrong in (("look", "up"), ("up", "look")):
        one("copy_" + field + "_from_" + wrong, "independent authored " + field + " vector",
            "s." + field + ".data(), sizeof(d." + field + ")", "s." + wrong + ".data(), sizeof(d." + field + ")", exact)
    one("transpose_matrix", "authored 4x4 matrix ordering",
        "std::memcpy(d.matrix.data(), s.matrix.data(), sizeof(d.matrix));",
        "for (unsigned row = 0; row < 4; ++row) for (unsigned col = 0; col < 4; ++col) d.matrix[row * 4 + col] = s.matrix[col * 4 + row];", exact)
    one("reverse_placement_order", "source placement array order", "const auto& s = source.instances[i]; auto& d = built.placements_[i];",
        "const auto& s = source.instances[r.placements - i - 1]; auto& d = built.placements_[i];", "lossless placement section identity")
    one("zero_based_placement_zone", "one-based authored placement zone", "d.zone = s.zone;", "d.zone = s.zone - 1;", "lossless placement section identity")
    one("zero_based_placement_ordinal", "one-based per-zone placement ordinal", "d.ordinal = s.ordinal;", "d.ordinal = s.ordinal - 1;", "lossless placement section identity")
    one("invert_lighting", "exact authored lighting bit", "d.use_lighting = s.use_lighting ? 1u : 0u;", "d.use_lighting = s.use_lighting ? 0u : 1u;", "placement identity lighting")

    one("resource_id_always_zero", "placement-to-interned-resource pair identity", "d.resource_pair = resourceIds[i];", "d.resource_pair = 0;", "resource pair first-appearance identity")
    one("disable_pair_interning", "repeated references reuse their first pair ID", "if (found != interned.end())", "if (false && found != interned.end())", "resource pair first-appearance identity", "interned resource counts and exact text extent")
    one("lexicographic_pair_order", "resource IDs follow first appearance rather than map order", "WorldIndexRequirements49B r;\n        r.zones = source.zones.size();", "std::sort(keys.begin(), keys.end());\n        WorldIndexRequirements49B r;\n        r.zones = source.zones.size();", "both resource names byte-exact")
    one("resource_pair_loses_second_member", "both opaque member strings retained independently", "const Key key{p.geometry_member, p.collision_member};", "const Key key{p.geometry_member, p.geometry_member};", "both resource names byte-exact")
    one("normalize_resource_slashes", "opaque resource text remains byte-exact", "// Everything that can allocate/fail has completed before either commit.", "for (char& c : built.text_) if (c == '\\\\') c = '/';\n        // Everything that can allocate/fail has completed before either commit.", "both resource names byte-exact")
    for field in ("geometry", "collision"):
        source = "first" if field == "geometry" else "second"
        one("truncate_" + field + "_text", field + " string_view exact extent", "d." + field + "_bytes = static_cast<std::uint32_t>(key." + source + ".size());", "d." + field + "_bytes = static_cast<std::uint32_t>(key." + source + ".size() - 1);", "both resource names byte-exact")
    one("second_text_uses_first_offset", "independent offsets into packed text", "d.collision_offset = offset;", "d.collision_offset = d.geometry_offset;", "both resource names byte-exact")

    one("zone_range_restarts_at_zero", "contiguous source-ordered placement ranges", "{s.zone, first, s.models, 0, 0}", "{s.zone, 0, s.models, 0, 0}", "zone identity and direct ordered span")
    one("zone_range_omits_last_placement", "complete zone placement count", "{s.zone, first, s.models, 0, 0}", "{s.zone, first, s.models ? s.models - 1 : 0, 0, 0}", "zone identity and direct ordered span")
    one("reverse_portal_records", "portal source order", "const auto& s = source.portals[i]; auto& d = built.portals_[i];", "const auto& s = source.portals[r.portals - i - 1]; auto& d = built.portals_[i];", "incident portal original order", "portal identity and ordered endpoints")
    one("zero_based_portal_ordinal", "authored portalN identity", "d.ordinal = static_cast<std::uint32_t>(i + 1);", "d.ordinal = static_cast<std::uint32_t>(i);", "portal identity and ordered endpoints")
    one("swap_portal_endpoints", "ordered endpoint identities", "d.zones = s.zones;", "d.zones = {s.zones[1], s.zones[0]};", "portal identity and ordered endpoints")
    one("rotate_portal_vertices", "four authored vertices preserve order and coordinates", "std::memcpy(d.vertices.data(), s.vertices.data(), sizeof(d.vertices));", "for (unsigned k = 0; k < 4; ++k) d.vertices[k] = s.vertices[(k + 1) % 4];", exact)
    one("perturb_portal_vertex_one_binary64_ulp", "exact stored portal coordinate copying", "std::memcpy(d.vertices.data(), s.vertices.data(), sizeof(d.vertices));", "std::memcpy(d.vertices.data(), s.vertices.data(), sizeof(d.vertices)); d.vertices[0][0] = std::nextafter(d.vertices[0][0], std::numeric_limits<double>::infinity());", exact)
    for endpoint in (0, 1):
        one("omit_incidence_endpoint_" + str(endpoint + 1), "every portal appears at endpoint " + str(endpoint + 1),
            "for (auto zone : built.portals_[i].zones) built.incidents_[cursor[zone - 1]++] = static_cast<std::uint32_t>(i);",
            "for (unsigned endpoint = 0; endpoint < 2; ++endpoint) if (endpoint != " + str(endpoint) + ") { auto zone = built.portals_[i].zones[endpoint]; built.incidents_[cursor[zone - 1]++] = static_cast<std::uint32_t>(i); }", "incident portal original order")
    one("reverse_incident_portal_order", "both endpoint lists retain source portal order", "for (std::size_t i = 0; i < r.portals; ++i)\n            for (auto zone : built.portals_[i].zones)", "for (std::size_t n = r.portals; n > 0; --n)\n            for (auto zone : built.portals_[n - 1].zones)")
    # The reverse traversal uses the corresponding source ID rather than inventing one.
    items[-1]["replacements"].append(("built.incidents_[cursor[zone - 1]++] = static_cast<std::uint32_t>(i);", "built.incidents_[cursor[zone - 1]++] = static_cast<std::uint32_t>(n - 1);"))
    items[-1]["expected_assertions"] = ["incident portal original order"]
    one("incident_ranges_restart_at_zero", "per-zone contiguous incident ranges", "z.first_incident_portal = first;", "z.first_incident_portal = 0;", "incident portal original order", "incident span counts")

    for name, old, new, label in (
        ("scene", "audit.scene_info_block = source.scene_info_block;", "audit.scene_info_block = source.scene_info_block + 1;", "scene source identity"),
        ("zone", "audit.zone_parameter_blocks[i] = s.parameter_block;", "audit.zone_parameter_blocks[i] = s.parameter_block + 1;", "zone source block identity"),
        ("portal", "audit.portal_parameter_blocks[i] = s.parameter_block;", "audit.portal_parameter_blocks[i] = s.parameter_block + 1;", "portal source block identity"),
        ("placement", "audit.placement_parameter_blocks[i] = s.parameter_block;", "audit.placement_parameter_blocks[i] = s.parameter_block + 1;", "placement source block and byte offset exact"),
        ("byte_offset", "static_cast<std::uint32_t>(s.source_block_offset)", "static_cast<std::uint32_t>(s.source_block_offset + 1)", "placement source block and byte offset exact")):
        one("wrong_provenance_" + name, "optional source identity: " + name, old, new, label)
    one("provenance_required_for_lighting", "core fields independent of optional provenance output", "d.use_lighting = s.use_lighting ? 1u : 0u;", "d.use_lighting = provenance && s.use_lighting ? 1u : 0u;", "placement identity lighting")
    one("retain_unrequested_provenance", "no hidden retained source-ID sidecar for core-only load", "// Everything that can allocate/fail has completed before either commit.", "if (!provenance) { static std::vector<std::uint32_t> retained; std::vector<std::uint32_t> extra(r.placements); retained.swap(extra); }\n        // Everything that can allocate/fail has completed before either commit.", "reported vector capacities equal actual retained requests")

    load_limit = "strictly smaller load limit fails atomically"
    build_limit = "strictly smaller build limit fails atomically"
    for field, cap in (("input_bytes", "std::size_t(2u * 1024u * 1024u)"), ("blocks", "8192u"), ("fields_per_block", "1024u"), ("zones", "256u"), ("portals", "4096u"), ("instances", "100000u"), ("instances_per_zone", "10000u")):
        one("ignore_limit_" + field, "caller-reduced " + field + " limit", "std::min(w." + field + ", " + cap + ")", cap, load_limit)
    one("ignore_load_asset_limit", "load-frontdoor byte bound", "need(input.size() <= std::min(limits.asset_bytes, max_world_asset_bytes48f)", "need(input.size() <= max_world_asset_bytes48f", load_limit)
    one("ignore_build_asset_limit", "typed-build serialized byte bound", "need(validated.size() <= std::min(limits.asset_bytes, max_world_asset_bytes48f)", "need(validated.size() <= max_world_asset_bytes48f", build_limit)
    for field, cap in (("resource_pairs", "100000u"), ("text_bytes", "maxTextBytes"), ("payload_bytes", "maxPayloadBytes")):
        one("ignore_limit_" + field, "caller-reduced " + field + " bound", "std::min(limits." + field + ", " + cap + ")", cap, load_limit)
    one("reject_equal_asset_limit", "limits admit exact equality", "need(input.size() <= std::min(limits.asset_bytes, max_world_asset_bytes48f)", "need(input.size() < std::min(limits.asset_bytes, max_world_asset_bytes48f)", "all limits inclusive exact equality")
    one("skip_typed_source_validation", "typed-build still validates against unchanged authoritative importer", "if (!encodeWorldAsset48F(source, validated, error)) return false;", "/* omitted complete source validation */", "typed invalid source rejected with both outputs atomic")
    one("clear_owner_before_validation", "failed frontdoor keeps old owned index", "error.clear();\n    try {\n        need(input.size()", "error.clear();\n    output = OwnedWorldIndex49B{};\n    try {\n        need(input.size()", "frontdoor malformed rejection both outputs atomic")
    one("clear_provenance_before_validation", "failed frontdoor keeps old optional provenance", "error.clear();\n    try {\n        need(input.size()", "error.clear();\n    if (provenance) *provenance = WorldIndexProvenance49B{};\n    try {\n        need(input.size()", "frontdoor malformed rejection both outputs atomic")

    for field, count in (("zones", "zones"), ("placements", "placements"), ("portals", "portals"), ("resource_pairs", "resource_pairs"), ("text", "text_bytes"), ("incident_portal_ids", "incident_portal_ids")):
        one("ignore_copy_capacity_" + field, "caller-owned " + field + " capacity checked before writing", "storage." + field + ".size < r." + count, "false", "copy expected failure")
    one("write_destination_before_copy_failure", "failed caller copy is byte-atomic", "error = WorldIndexCopyError49B::insufficient_storage; return false;", "storage.zones.data[0].zone ^= 1u; error = WorldIndexCopyError49B::insufficient_storage; return false;", "failed copy destination and view untouched")
    one("replace_view_before_copy_failure", "failed caller copy preserves output view", "error = WorldIndexCopyError49B::insufficient_storage; return false;", "output = WorldIndexView49B{}; error = WorldIndexCopyError49B::insufficient_storage; return false;", "failed copy destination and view untouched")
    one("accept_default_copy_source", "default view is not a valid empty world", "if (!sourceValid(source))", "if (false && !sourceValid(source))", "default source is failure, never empty success")
    one("accept_unrepresentable_copy_extent", "multiplication-overflowing capacity is rejected before writes", "error = WorldIndexCopyError49B::invalid_extent; return false;\n    }\n    const auto bytes", "out = {}; return true;\n    }\n    const auto bytes", "copy expected failure")
    one("accept_overlap_as_successful_noop", "overlap cannot be accepted as an in-place no-op", "error = WorldIndexCopyError49B::overlapping_storage; return false;\n            }", "return true;\n            }", "copy expected failure")
    one("ignore_destination_alignment", "typed destination alignment is checked before copying", "if (address % alignof(T) != 0)", "if (false && address % alignof(T) != 0)", "copy expected failure")
    for name, old in (("placements", "copySpan(source.placements(), storage.placements);"), ("resource_records", "copySpan(source.resourcePairs(), storage.resource_pairs);"), ("text", "copySpan(source.text(), storage.text);"), ("incidents", "copySpan(source.incidentPortalIds(), storage.incident_portal_ids);")):
        expected = {"placements": "lossless placement section identity", "resource_records": "both resource names byte-exact", "text": "both resource names byte-exact", "incidents": "incident portal original order"}[name]
        one("omit_caller_copy_" + name, "caller-owned copy includes " + name, old, "/* omitted copy */", expected)
    one("borrow_source_after_copy", "successful copy borrows only caller-owned storage", "output = WorldIndexAccess49B::view(storage, r); return true;", "output = source; return true;", "copied zone view owns exact caller extent")
    one("copy_view_exposes_spare_capacity", "borrowed output lengths use logical extents", "v.zones_ = {s.zones.data, r.zones};", "v.zones_ = {s.zones.data, s.zones.size};", "copied zone view owns exact caller extent")
    one("allocate_during_copy", "caller copy is allocation-free", "const auto r = source.requirements();\n    if (storage.zones.size", "std::vector<unsigned char> temporary(source.text().begin(), source.text().end());\n    if (temporary.size() != source.text().size) return false;\n    const auto r = source.requirements();\n    if (storage.zones.size", "failed copy no allocations", "core copy/query repeated loops allocate zero")

    one("zone_zero_aliases_first", "zone identity zero is invalid", "return number && number <= zones_.size ? zones_.data + (number - 1) : nullptr;", "return number <= zones_.size && zones_.size ? zones_.data + (number ? number - 1 : 0) : nullptr;", "invalid zone rejected")
    one("reject_last_valid_zone", "inclusive one-based zone lookup", "number <= zones_.size", "number < zones_.size", "zone lookup identity")
    one("accept_placement_ordinal_end", "placement ordinal one-past-end rejected", "ordinal <= z->placement_count", "ordinal <= z->placement_count + 1", "placement ordinal boundaries")
    one("accept_placement_id_end", "placement ID zero-based bound", "id < placements_.size", "id <= placements_.size", "placement ID boundaries")
    one("accept_portal_id_end", "portal ID zero-based bound", "id < portals_.size", "id <= portals_.size", "portal ID boundaries")
    one("invalid_resource_overwrites_output", "failed resource lookup preserves caller output", "if (id >= resources_.size) return false;", "if (id >= resources_.size) { output = {}; return false; }", "resource failure output atomicity")
    one("invalid_zone_returns_placements", "invalid zone yields empty placement span", ": WorldIndexSpan49B<const WorldPlacement49B>{};", ": placements_;", "invalid zone rejected")
    one("invalid_zone_returns_incidents", "invalid zone yields empty incident span", ": WorldIndexSpan49B<const std::uint32_t>{};", ": incidents_;", "invalid zone rejected")
    one("query_allocates_resource_text", "resource query is allocation-free", "const auto& r = resources_[id];", "std::vector<char> temporary(text_.begin(), text_.end()); if (temporary.size() != text_.size) return false;\n    const auto& r = resources_[id];", "old and compact measured queries no allocations")
    one("payload_omits_incidence_bytes", "requirements report every retained logical element", "r.text_bytes + r.incident_portal_ids * sizeof(std::uint32_t);", "r.text_bytes;", "typed element payload arithmetic")
    return items


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def tracked_inputs():
    names = {PRODUCTION, HEADER, TEST, SELF, *COMMON}
    pending = list(names)
    while pending:
        name = pending.pop()
        path = ROOT / name
        for include in re.findall(r'^\s*#\s*include\s*"([^"\n]+)"', path.read_text(), re.MULTILINE):
            included = (path.parent / include).resolve()
            if not included.is_file():
                included = (ROOT / "src/assets" / include).resolve()
            if included.is_file() and ROOT in included.parents:
                relative = str(included.relative_to(ROOT))
                if relative not in names:
                    names.add(relative)
                    pending.append(relative)
    for architecture in ("x64", "x86"):
        for name in WORLD_NAMES:
            names.add(FIXTURE_ROOT + "/" + architecture + "-world-fixtures/" + name + "-tracks_" + name + ".rrworld")
    # Authentic RRWORLD1 embeds the original SCB. The unchanged production
    # importer and parameter decoder are the independent projection reference.
    return sorted(names)



def fixture_reference_manifest():
    rows = []
    for name in WORLD_NAMES:
        paths = [FIXTURE_ROOT + "/" + arch + "-world-fixtures/" + name + "-tracks_" + name + ".rrworld" for arch in ("x64", "x86")]
        raw = (ROOT / paths[0]).read_bytes()
        if raw != (ROOT / paths[1]).read_bytes() or raw[:8] != b"RRWORLD1" or len(raw) < 48:
            raise RuntimeError("Authentic paired fixture mismatch: " + name)
        def word(at):
            return int.from_bytes(raw[at:at + 4], "little")
        extent = word(24)
        if word(8) != 1 or word(12) != len(raw) or not 15 <= extent <= len(raw) - 48:
            raise RuntimeError("Authentic fixture header mismatch: " + name)
        original = raw[48:48 + extent]
        if original[:7] != b"BINARY\0":
            raise RuntimeError("Embedded original parameter header mismatch: " + name)
        rows.append({"world": name, "paired_fixture_paths": paths, "rrworld_sha256": hashlib.sha256(raw).hexdigest(), "rrworld_bytes": len(raw), "embedded_original_offset": 48, "embedded_original_bytes": extent, "embedded_original_sha256": hashlib.sha256(original).hexdigest(), "zones": word(32), "portals": word(36), "placements": word(40)})
    totals = {key: sum(row[key] for row in rows) for key in ("rrworld_bytes", "embedded_original_bytes", "zones", "portals", "placements")}
    if totals != {"rrworld_bytes": 15945212, "embedded_original_bytes": 6366352, "zones": 186, "portals": 206, "placements": 15340}:
        raise RuntimeError("Authentic fixture totals differ from independent audit")
    return {"reference": "Unchanged original 48F decoder and RRWORLD1 parser, with original SCB bytes embedded in each cache file. x86 fixture identity is not a claim of executing a 32-bit binary.", "unique_worlds": len(rows), "totals": totals, "worlds": rows}


def excluded_equivalent_candidates():
    return [{"name": name, "reason": "Valid authored vectors are rounded to binary32 by unchanged import_world48f.cpp decimal(), then widened to binary64. A float roundtrip is therefore equivalent, including signed zero and binary32 subnormals. It is excluded from the sensitivity score; no invalid typed source was invented to force a failure.", "source": "src/assets/import_world48f.cpp:143-145"} for name in ("narrow_position_to_binary32", "narrow_portal_vertices")]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--expected-production-sha256", required=True)
    parser.add_argument("--expected-header-sha256", required=True)
    parser.add_argument("--expected-test-sha256", required=True)
    args = parser.parse_args()
    out = args.output_dir.resolve()
    if out == ROOT or ROOT in out.parents:
        parser.error("Output must be outside source tree")
    out.mkdir(exist_ok=True, parents=True)
    if any(out.iterdir()):
        parser.error("Output must be empty")
    hashes = {name: digest(ROOT / name) for name in tracked_inputs()}
    for name, expected in ((PRODUCTION, args.expected_production_sha256), (HEADER, args.expected_header_sha256), (TEST, args.expected_test_sha256)):
        if hashes[name] != expected:
            parser.error("Input differs from finalized expected SHA256: " + name)
    (out / "input-hashes-before.json").write_text(json.dumps(hashes, indent=2) + "\n")

    def assert_stable():
        changed = [name for name, before in hashes.items() if digest(ROOT / name) != before]
        if changed:
            raise RuntimeError("Source/test/reference/fixtures changed: " + ", ".join(changed))

    def run_logged(command, name, timeout):
        (out / (name + "-command.json")).write_text(json.dumps(command, indent=2) + "\n")
        assert_stable()
        try:
            process = subprocess.run(command, capture_output=True, text=True, timeout=timeout)
            result = {"returncode": process.returncode, "stdout": process.stdout, "stderr": process.stderr, "timed_out": False}
        except subprocess.TimeoutExpired as failure:
            def decode(value):
                return value.decode(errors="replace") if isinstance(value, bytes) else (value or "")
            result = {"returncode": None, "stdout": decode(failure.stdout), "stderr": decode(failure.stderr), "timed_out": True}
        (out / (name + ".log")).write_text(result["stdout"] + result["stderr"])
        assert_stable()
        return result

    source = (ROOT / PRODUCTION).read_text()
    mutations = controls()
    rows, baseline, error = [], {}, None
    (out / "control-definitions.json").write_text(json.dumps(mutations, indent=2) + "\n")
    try:
        (out / "fixture-reference-hashes.json").write_text(json.dumps(fixture_reference_manifest(), indent=2) + "\n")
        (out / "excluded-equivalent-candidates.json").write_text(json.dumps(excluded_equivalent_candidates(), indent=2) + "\n")
        labels = set(re.findall(r'"([^"\n]+)"\);', (ROOT / TEST).read_text()))
        for mutation in mutations:
            candidate = source
            for old, new in mutation["replacements"]:
                if candidate.count(old) != 1:
                    raise ValueError("Nonunique mutation anchor: " + mutation["name"] + ": " + old)
                candidate = candidate.replace(old, new)
            if not mutation["expected_assertions"] or not all(label in labels for label in mutation["expected_assertions"]):
                raise ValueError("Expected assertion absent from frozen test: " + mutation["name"])
            (out / (mutation["name"] + ".cpp")).write_text(candidate)
        run_logged(["g++", "--version"], "compiler-version", 30)
        objects = []
        for name in [TEST] + COMMON:
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
        baseline.update(run_logged([str(binary), str(ROOT)], "baseline-test", 180))
        if baseline["returncode"] != 0 or not re.search(r"^TOTAL worlds=12 .* status=PASS$", baseline["stdout"], re.MULTILINE):
            raise RuntimeError("Unchanged authentic-corpus baseline did not pass")
        print("baseline: passed", flush=True)
        for mutation in mutations:
            name = mutation["name"]
            candidate, binary = out / (name + ".cpp"), out / name
            build = run_logged(["g++"] + FLAGS + objects + [str(candidate), "-o", str(binary)], name + "-build", 180)
            row = {key: value for key, value in mutation.items() if key != "replacements"}
            row.update(candidate_sha256=digest(candidate), compile_returncode=build["returncode"], compile_timed_out=build["timed_out"], detected=False)
            if build["returncode"] == 0:
                result = run_logged([str(binary), str(ROOT)], name + "-test", 180)
                match = FAILURE.search(result["stderr"])
                assertion = match.group(1) if match else None
                row.update(returncode=result["returncode"], timed_out=result["timed_out"], assertion=assertion, output=result["stdout"] + result["stderr"], detected=result["returncode"] == 1 and not result["timed_out"] and assertion in mutation["expected_assertions"])
            rows.append(row)
            (out / "progress.json").write_text(json.dumps({"baseline": baseline, "controls": rows}, indent=2) + "\n")
            print(name + ": " + ("detected (" + row["assertion"] + ")" if row["detected"] else "FAILED"), flush=True)
    except (RuntimeError, OSError, ValueError) as failure:
        error = str(failure)
    after = {name: digest(ROOT / name) for name in hashes}
    (out / "input-hashes-after.json").write_text(json.dumps(after, indent=2) + "\n")
    stable = hashes == after
    passed = not error and stable and len(rows) == len(mutations) and all(row["detected"] for row in rows)
    result = {"status": "passed" if passed else "failed", "error": error, "production_sha256": hashes[PRODUCTION], "header_sha256": hashes[HEADER], "test_sha256": hashes[TEST], "inputs_unchanged": stable, "input_sha256": hashes, "baseline": baseline, "excluded_equivalent_candidates": excluded_equivalent_candidates(), "expected_controls": len(mutations), "detected_controls": sum(row["detected"] for row in rows), "surviving_mutations": [row["name"] for row in rows if row.get("returncode") == 0], "undetected_controls": [row["name"] for row in rows if not row["detected"]], "unexecuted_controls": [mutation["name"] for mutation in mutations[len(rows):]], "controls": rows}
    (out / "sensitivity.json").write_text(json.dumps(result, indent=2) + "\n")
    summary = ["49B candidate-only sensitivity", "Status: " + result["status"], "Unchanged baseline: " + ("PASS" if baseline.get("returncode") == 0 else "NOT PASSED"), "Expected assertion failures: " + str(result["detected_controls"]) + "/" + str(len(mutations)), "Inputs unchanged: " + str(stable), "Reference: unchanged 48F importer/decoder and identical x64/x86 authentic RRWORLD1 corpus; 12 worlds, 186 zones, 206 portals, 15,340 placements.", "Two equivalent float-roundtrip candidates were excluded; final position/portal controls perturb actual stored coordinates by one binary64 ULP.", "Only new 49B implementation copies are mutated. No runtime collision work, old-checkpoint changes, renderer or asset transfer."]
    if error:
        summary.append("Error: " + error)
    summary.extend(row["name"] + ": " + ("detected: " + row["assertion"] if row["detected"] else "FAILED") for row in rows)
    (out / "report.txt").write_text("\n".join(summary) + "\n")
    print(result["status"] + ": " + str(result["detected_controls"]) + "/" + str(len(mutations)) + " expected assertion failures; inputs " + ("unchanged" if stable else "CHANGED"), flush=True)
    if error:
        print(error, flush=True)
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
