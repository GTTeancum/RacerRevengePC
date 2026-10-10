#!/usr/bin/env python3
"""Stage the source-inspected 50DU runtime closure; never build or launch it.

Python 3.8+, standard library only. Use --help for plan, stage and verify modes.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import stat
import struct
import sys
import zipfile

ROOT = Path(__file__).resolve().parents[1]
SCHEMA = "RR_PLAYABLE50FF_V1"
MANIFEST = "PLAYABLE50FF-MANIFEST.json"
CORPUS = "evidence/staticType2Producer49X/original-word/native-fixtures.json"
VISUAL = "evidence/visual49G/"
POD = "PODS/POD01/POD01.RES"
TA = "TRACKS/TA.RES"
READERS = (
    "tests/native/terrain_interactive_drive50du.cpp",
    "tests/native/authored_runtime_ground50cp_fixture.h",
    "tests/native/pod_materials50ed_fixture.h",
    "tests/native/track_materials50fa_fixture.h",
    "tests/native/sky50gw_fixture.h",
    "tests/native/sky50gw_index.inc",
    "tests/native/source_texture50fc_fixture.h",
    "tests/native/source_texture50fc_index.inc",
    "tests/native/pod_cables50fd_fixture.h",
    "tests/native/source_cable_texture50fc_fixture.h",
    "evidence/frameBridge49W/frame/frame_fixture.h",
    "evidence/chariotAnimation49T/flow/flow_fixture.h",
    "evidence/chariotAnimation49T/ik/source/ik_fixture.h",
    "evidence/vehicleVisual49R/source/visual_fixture.h",
    "platform/linux/native/window_host50dq.cpp",
    "platform/linux/native/offscreen_gl_context50dc.cpp",
    "src/assets/native_asset48b.cpp",
)
FIXED = (
    CORPUS,
    "work/native/project/disc/PODS/POD01/POD01.RES",
    "work/native/project/disc/TRACKS/TA.RES",
    "work/impactAuthoring49Y/Pod01.phy",
    "work/impactAuthoring49Y/ta.tun",
    "evidence/authoredInitialization49N/authoring-source/Pod01.phy",
    "evidence/frameBridge49W/shape/podboxf.col",
    VISUAL + "asset-selection.json",
    VISUAL + "export-placed-normal/export-report.json",
    VISUAL + "export-placed-normal/ta-world.rrmesh",
)
GEOMETRY = (
    "48-englt1.psg", "43-engrt1.psg", "52-chariot.psg",
    "49-englt1.vut", "44-engrt1.vut", "53-chariot.vut",
    "50-englt1.mot", "45-engrt1.mot", "54-01_brklt.mot",
    "55-01_brkrt.mot", "56-01_forwd.mot", "57-01_back.mot",
)
EFFECTS = (
    "podLeftEngineDust.emt", "podRightEngineDust.emt",
    "podLeftEngineSmoke.emt", "podRightEngineSmoke.emt",
    "podSparks.emt", "podFire.emt",
)
# These exact retained files include the original notices, without rewriting.
# No SDL, EGL, GL, zlib, or C++ runtime binary is bundled.
NOTICES = (
    "third_party/native-json/LICENSE.MIT",
    "third_party/native-zlib/README",
    "third_party/native-zlib/zlib.h",
)


def need(condition, message):
    if not condition:
        raise ValueError(message)


def digest(path):
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def relative(value):
    p = PurePosixPath(value)
    need(value and "\\" not in value and not p.is_absolute() and
         str(p) == value and all(x not in (".", "..") for x in p.parts),
         "Unsafe relative path: " + value)
    # A content-addressed hash can coincidentally contain the text "49c".
    # That is not a checkpoint name; still reject excluded named path parts.
    need(not any(x.lower() == "ee.bin" or
                 (not re.fullmatch(r"[0-9a-f]{64}(?:-mip\d+)?(?:\.[a-z0-9]+)?", x) and
                  ("49c" in x.lower() or "48u" in x.lower())) for x in p.parts),
         "Excluded input: " + value)
    return p


def source(root, rel):
    parts = relative(rel).parts
    p = root
    for part in parts:
        p = p / part
        need(not p.is_symlink(), "Symlink input rejected: " + rel)
    need(p.is_file(), "Missing regular input: " + rel)
    need(p.stat().st_size > 0, "Empty input: " + rel)
    return p


def json_bytes(value):
    return (json.dumps(value, sort_keys=True, indent=2, ensure_ascii=True) + "\n").encode()


def read_json(root, rel):
    return json.loads(source(root, rel).read_text(encoding="utf-8"))


def collect(root):
    """Follow only the current cold readers' actual file-opening branches."""
    entries = {}

    def add(rel, why, expected_hash=None, expected_bytes=None, role="runtime"):
        p = source(root, rel)
        size, sha = p.stat().st_size, digest(p)
        need(expected_hash is None or sha == expected_hash, "Hash mismatch: " + rel)
        need(expected_bytes is None or size == expected_bytes, "Size mismatch: " + rel)
        if rel not in entries:
            entries[rel] = dict(path=rel, bytes=size, sha256=sha, role=role, reasons=[])
        if why not in entries[rel]["reasons"]:
            entries[rel]["reasons"].append(why)

    for rel in FIXED:
        add(rel, "50DU direct cold input or cold ground/frame fixture")
    # Actual authored TA sky tiles, decoded cold from skies/ta.cub. Runtime
    # reads only these pinned RRTEX files, never an external codec.
    sky_index = source(root, "tests/native/sky50gw_index.inc").read_text()
    sky_tiles = re.findall(r'"(face[0-4]-tile[0-9]{2}\.rrtex)",\s*"([0-9a-f]{64})"', sky_index)
    need(len(sky_tiles) == 48 and len({name for name, _ in sky_tiles}) == 48,
         "Expected48 unique authenticated sky tiles")
    for name, sha in sky_tiles:
        add("evidence/sky50GW/" + name, "50GW authored CUB tile image", sha, 128*128*4+16)
    add("evidence/sky50GW/manifest.json", "50GW original CUB tile provenance", role="evidence")
    for name in GEOMETRY:
        add("evidence/authoredGeometry49O/asset-reader/fixtures/" + name,
            "49T Assets constructor: exact PSG/VUT/MOT input")
    for name in ("sin-table.bin", "cos-table.bin", "acos-table.bin"):
        add("evidence/authoredGeometry49O/motion/trig/" + name,
            "49T cold machine constructor: exact retained trigonometric table")
    for name in EFFECTS:
        add("evidence/effectAuthoring50BF/members/" + name,
            "50DU eight effect owners share six original EMT files")
    cable_reader = source(root, "tests/native/pod_cables50fd_fixture.h").read_text()
    cable_pin = re.search(r'sha256\(input\)\s*==\s*"([0-9a-f]{64})"', cable_reader)
    need(cable_pin is not None, "Missing cable reader Pod01 archive identity pin")
    add("work/native/project/disc/PODS/POD01/POD01.RES",
        "50FD cable material/texture source; 50FC cable decoder uses bytes in memory only",
        cable_pin.group(1))

    selection = read_json(root, VISUAL + "asset-selection.json")
    report = read_json(root, VISUAL + "export-placed-normal/export-report.json")
    need(selection["schema"] == "RR_VISUAL_SELECTION49G_V1" and
         selection["status"] == "complete", "Wrong visual selection schema/status")
    need(report["schema"] == "RR_AUTHORED_MESH49G_V1" and report["success"] is True,
         "Wrong placed export schema/status")
    # Match the adapter's pins without rewriting the original selection/report.
    adapter = source(root, "tests/native/track_materials50fa_fixture.h").read_text()
    for variable, rel in (("selectionSha", VISUAL + "asset-selection.json"),
                          ("exportSha", VISUAL + "export-placed-normal/export-report.json"),
                          ("archiveSha", "work/native/project/disc/TRACKS/TA.RES")):
        match = re.search(r'\b' + variable + r'\s*=\s*"([0-9a-f]{64})"', adapter)
        need(match is not None, "Missing reader identity pin: " + variable)
        add(rel, "50FA reader identity pin", match.group(1))

    objects = {x["path"]: x for x in selection["objects"]}
    need(len(objects) == len(selection["objects"]), "Duplicate visual object path")

    def selected(path, why):
        relative(path)
        need(path.startswith("cacheobjects/"), "Not a visual cache object: " + path)
        o = objects[path]
        need(o["extracted"] is True, "Required visual object not retained: " + path)
        add(VISUAL + path, why, o["sha256"], o["bytes"])

    def record(key, why):
        need(re.fullmatch("[0-9a-f]{64}", key) is not None, "Invalid visual content key")
        path = "cacheobjects/" + key + ".json"
        selected(path, why)
        result = read_json(root, VISUAL + path)
        need(result["key"] == key, "Visual record key mismatch")
        return result

    scenes = {(x["container"], x["key"]): x for x in selection["scene_requests"]}
    wanted = {(TA, x["key"]) for x in report["world_instances"]}
    wanted.update((x["container"], x["key"]) for x in selection["scene_requests"]
                  if x["container"] == POD)
    need(len(wanted) == 231, "Expected 228 TA scenes and 3 Pod scenes")
    materials = {(x["container"], x["name"]): x for x in selection["material_resolution"]}
    textures = {(x["container"], x["name"]): x for x in selection["texture_resolution"]}
    unresolved_names = set()
    used_textures = set()
    for container, key in sorted(wanted):
        row = scenes[container, key]
        r = record(key, "50ED/50FA scene provenance record")
        selected(row["rrscene"], "50DU/50FA actual Pod or placed TA source geometry")
        need(r == row["native"], "Scene native record differs from selection")
        for name in r["materials"]:
            material = materials[container, name]
            need(material["status"] == "resolved", "Unresolved required material: " + name)
            mr = record(material["key"], "50ED/50FA authored material record")
            passes = [x for x in mr["passes"] if x["pass"] == 1]
            need(len(passes) == 1, "Missing/duplicate authored pass 1")
            maps = [x for x in passes[0]["textures"] if x["slot"] == 1]
            need(len(maps) == 1, "Missing/duplicate authored slot 1")
            texture = textures[container, maps[0]["name"]]
            if texture["status"] != "resolved":
                need(container == TA, "Unresolved selected Pod texture")
                unresolved_names.add(texture["name"])
                continue
            tk = texture["key"]
            tr = record(tk, "50ED/50FA chosen texture provenance record")
            need(tr == texture["native"], "Texture native record differs from selection")
            choices = [m for m in tr["mips"] if objects.get(
                "cacheobjects/" + tk + "-mip" + str(m["index"]) + ".rrtex", {}).get("extracted") is True]
            need(choices, "No retained mip for selected texture")
            mip = max(choices, key=lambda m: m["width"] * m["height"])
            path = "cacheobjects/" + tk + "-mip" + str(mip["index"]) + ".rrtex"
            selected(path, "50ED/50FA largest-retained mip, authored pass 1 / slot 1")
            need(entries[VISUAL + path]["sha256"] == mip["sha256"], "Mip digest differs")
            used_textures.add((container, tk, mip["index"]))

    index = source(root, "tests/native/source_texture50fc_index.inc").read_text()
    decoded = re.findall(r'\{"([^"]+)", "([0-9a-f]{64})", "([^"]+\.rrtex)", '
                         r'"([0-9a-f]{64})", (\d+)u, (\d+)u, (\d+)u, (\d+)u\}', index)
    need(len(decoded) == 8, "Expected eight exact source-decoded 50FC textures")
    covered = set()
    for member, srcsha, filename, sha, srcbytes, width, height, mip in decoded:
        need(PurePosixPath(filename).name == filename, "Unsafe source texture basename")
        covered.add(PurePosixPath(member).stem.lower())
        add("evidence/textures50FC/" + filename,
            "50FC pinned decoded " + member + " (original member SHA-256 " + srcsha + ")",
            sha, int(width) * int(height) * 4 + 16)
    need(unresolved_names == covered, "50FC closure no longer covers exact unresolved TA texture set")
    for rel in NOTICES:
        add(rel, "Unmodified retained upstream license/attribution notice", role="notice")
    for rel in ("evidence/textures50FC/manifest.json", "evidence/textures50FC/source-proof.json"):
        add(rel, "Preserved source texture conversion provenance", role="provenance")
    reader_hashes = [dict(path=rel, bytes=source(root, rel).stat().st_size,
                          sha256=digest(source(root, rel))) for rel in READERS]
    return dict(schema=SCHEMA, package_kind="source-inspected-runtime-closure",
                corpus=CORPUS, entries=[entries[x] for x in sorted(entries)],
                reader_sources=reader_hashes,
                totals=dict(files=len(entries), bytes=sum(x["bytes"] for x in entries.values()),
                            scenes=len(wanted), retained_textures=len(used_textures),
                            source_decoded_textures=len(decoded), runtime_decoded_cable_textures=1),
                verification=dict(asset_hashes="verified against current retained inputs and available source pins",
                                  runtime="not run by packager",
                                  native_window="not verified by packager",
                                  frame_rate="not measured by packager",
                                  base_archive_required=False,
                                  system_libraries_bundled=False))


LAUNCHER = '''#!/usr/bin/env python3
"""Verify this package, change to its root, then execute its supplied binary."""
import hashlib, json, os, pathlib, sys
root = pathlib.Path(__file__).resolve().parent
m = json.loads((root / "PLAYABLE50FF-MANIFEST.json").read_text())
for e in m["entries"]:
    rel = pathlib.PurePosixPath(e["path"])
    if rel.is_absolute() or str(rel) != e["path"] or ".." in rel.parts or "\\\\" in e["path"]:
        raise SystemExit("Unsafe package path: " + e["path"])
    p = root
    for part in rel.parts:
        p = p / part
        if p.is_symlink(): raise SystemExit("Package symlink rejected: " + e["path"])
    if not p.is_file():
        raise SystemExit("Missing/unsafe package file: " + e["path"])
    h = hashlib.sha256()
    with p.open("rb") as f:
        for b in iter(lambda: f.read(1024 * 1024), b""): h.update(b)
    if p.stat().st_size != e["bytes"] or h.hexdigest() != e["sha256"]:
        raise SystemExit("Package file changed: " + e["path"])
binary = root / m["executable"]["path"]
if m["executable"]["path"] not in {e["path"] for e in m["entries"] if e["role"] == "executable"}:
    raise SystemExit("Executable is absent from the verified package inventory")
if "--verify-only" in sys.argv[1:]:
    if sys.argv[1:] != ["--verify-only"]: raise SystemExit("Use --verify-only by itself")
    print("Package file sizes and SHA-256 hashes verified; no executable was run.")
else:
    if os.name != "nt" and not os.access(binary, os.X_OK):
        raise SystemExit("Executable permission missing. Run: chmod +x " + str(binary))
    os.chdir(root)
    os.environ["PWD"] = str(root)
    arguments = []
    for argument in sys.argv[1:]:
        if argument == "--autodrive": os.environ["RR_AUTODRIVE"] = "1"
        elif argument == "--trace-phases": os.environ["RR_TRACE_PHASES"] = "1"
        else: arguments.append(argument)
    os.execv(str(binary), [str(binary), m["corpus"]] + arguments)
'''


def identify_executable(path):
    need(path.is_file() and not path.is_symlink(), "Executable must be an existing regular file")
    with path.open("rb") as f:
        head = f.read(4096)
    if head.startswith(b"\x7fELF"):
        need(len(head) >= 20 and head[4] in (1, 2) and head[5] in (1, 2), "Invalid ELF header")
        machine = struct.unpack(("<" if head[5] == 1 else ">") + "H", head[18:20])[0]
        return dict(format="ELF", bits=32 if head[4] == 1 else 64,
                    machine=machine, platform_compatibility="not established by file header")
    if head.startswith(b"MZ") and len(head) >= 64:
        offset = struct.unpack("<I", head[60:64])[0]
        with path.open("rb") as f:
            f.seek(offset); pe = f.read(6)
        need(len(pe) == 6 and pe[:4] == b"PE\0\0", "Invalid PE header")
        return dict(format="PE", machine=struct.unpack("<H", pe[4:6])[0],
                    platform_compatibility="not established by file header")
    raise ValueError("Executable is neither an ELF nor PE binary; refusing an accidental source/script input")


def verify_screenshot(path):
    """Accept supplied baseline JPEGs as exact bytes, without re-encoding."""
    relative(path.name)
    need(path.suffix.lower() in (".jpg", ".jpeg"), "Screenshot must have a .jpg or .jpeg extension")
    for p in (path, *path.parents):
        need(not p.is_symlink(), "Screenshot symlink rejected: " + str(path))
    need(path.is_file() and stat.S_ISREG(path.stat().st_mode),
         "Screenshot must be an existing regular file")
    need(4 <= path.stat().st_size <= 32 * 1024 * 1024, "Screenshot JPEG size is outside bounds")
    with path.open("rb") as f:
        beginning = f.read(3)
        f.seek(-2, os.SEEK_END)
        ending = f.read(2)
    need(beginning == b"\xff\xd8\xff" and ending == b"\xff\xd9",
         "Screenshot JPEG start/end markers missing: " + str(path))
    return path


def verify(stage):
    manifest = read_json(stage, MANIFEST)
    need(manifest["schema"] == SCHEMA, "Unknown package manifest schema")
    paths = set()
    for e in manifest["entries"]:
        need(e["path"] not in paths, "Duplicate manifest path")
        paths.add(e["path"])
        p = source(stage, e["path"])
        need(p.stat().st_size == e["bytes"] and digest(p) == e["sha256"],
             "Package verification failed: " + e["path"])
    actual = {p.relative_to(stage).as_posix() for p in stage.rglob("*") if p.is_file()}
    need(actual == paths | {MANIFEST}, "Unexpected/unlisted package files")
    return manifest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--plan", type=Path, help="Write closure inventory only; no executable required")
    parser.add_argument("--stage", type=Path, help="Create a NEW runtime directory; requires --executable")
    parser.add_argument("--executable", type=Path, help="Current completed binary, never launched by this tool")
    parser.add_argument("--evidence", type=Path, action="append", default=[],
                        help="Copy an exact build/validation receipt; repeatable; no directory copying")
    parser.add_argument("--screenshot", type=Path, action="append", default=[],
                        help="Copy a supplied baseline .jpg/.jpeg to validation/screenshots; repeatable")
    parser.add_argument("--zip", type=Path, help="Also write a NEW deterministic ZIP from --stage")
    parser.add_argument("--verify", type=Path, help="Verify existing package only; no executable is run")
    args = parser.parse_args()
    try:
        need(sum(bool(x) for x in (args.plan, args.stage, args.verify)) == 1,
             "Choose exactly one of --plan, --stage, --verify")
        need(not args.zip or args.stage, "--zip requires --stage")
        need(not args.executable or args.stage, "--executable requires --stage")
        need(not args.evidence or args.stage, "--evidence requires --stage")
        need(not args.screenshot or args.stage, "--screenshot requires --stage")
        if args.verify:
            m = verify(args.verify.resolve())
            print(json.dumps(dict(verified_files=len(m["entries"]), launched=False)))
            return
        root = args.root.resolve()
        m = collect(root)
        if args.plan:
            with args.plan.open("xb") as f: f.write(json_bytes(m))
            print(json.dumps(m["totals"]))
            return
        need(args.executable is not None, "Staging requires the completed --executable")
        executable = args.executable.absolute()
        relative(executable.name)
        info = identify_executable(executable)
        destination = args.stage.absolute()
        need(not destination.exists(), "Refusing to overwrite stage: " + str(destination))
        need(not args.zip or not args.zip.exists(), "Refusing to overwrite ZIP")
        additions = [(source(root, "docs/playable-build50ff.md"), "README50FF.md", "instructions"),
                     (source(root, "tools/package_playable50ff.py"), "tools/package_playable50ff.py", "packager"),
                     (executable, "bin/" + executable.name, "executable")]
        for evidence in args.evidence:
            p = evidence.absolute()
            relative(p.name)
            need(p.is_file() and not p.is_symlink(), "Evidence must be a regular file")
            need(p.suffix.lower() in (".json", ".txt", ".log", ".md"), "Evidence must be a text receipt")
            additions.append((p, "validation/" + p.name, "validation-receipt"))
        for screenshot in args.screenshot:
            p = verify_screenshot(screenshot.absolute())
            additions.append((p, "validation/screenshots/" + p.name, "baseline-screenshot"))
        used = {e["path"] for e in m["entries"]}
        for p, rel, role in additions:
            need(rel not in used, "Duplicate package destination: " + rel)
            used.add(rel)
            m["entries"].append(dict(path=rel, bytes=p.stat().st_size, sha256=digest(p),
                                     role=role, reasons=["Exact supplied " + role]))
        m["executable"] = dict(path="bin/" + executable.name, **info)
        # No timestamps or absolute workspace paths enter the generated manifest.
        destination.mkdir(parents=True)
        for e in m["entries"][:len(m["entries"]) - len(additions)]:
            out = destination / e["path"]; out.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source(root, e["path"]), out); out.chmod(0o644)
        for p, rel, role in additions:
            out = destination / rel; out.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(p, out); out.chmod(0o755 if role == "executable" else 0o644)
        launch = destination / "run50ff.py"
        launch.write_bytes(LAUNCHER.encode()); launch.chmod(0o755)
        m["entries"].append(dict(path="run50ff.py", bytes=launch.stat().st_size,
                                 sha256=digest(launch), role="launcher",
                                 reasons=["Working-directory-safe launcher with integrity verification"]))
        m["entries"].sort(key=lambda e: e["path"])
        m["totals"].update(files=len(m["entries"]), bytes=sum(e["bytes"] for e in m["entries"]))
        (destination / MANIFEST).write_bytes(json_bytes(m))
        verify(destination)
        receipt = dict(stage=str(destination), totals=m["totals"], launched=False,
                       manifest_sha256=digest(destination / MANIFEST))
        if args.zip:
            with zipfile.ZipFile(args.zip, "x", zipfile.ZIP_DEFLATED, compresslevel=6) as z:
                for p in sorted(destination.rglob("*")):
                    if not p.is_file(): continue
                    rel = p.relative_to(destination).as_posix()
                    zi = zipfile.ZipInfo("RacerRevenge50FF/" + rel, (1980, 1, 1, 0, 0, 0))
                    zi.create_system = 3
                    zi.external_attr = (stat.S_IFREG | (0o755 if rel in ("run50ff.py", m["executable"]["path"]) else 0o644)) << 16
                    zi.compress_type = zipfile.ZIP_DEFLATED
                    z.writestr(zi, p.read_bytes(), compresslevel=6)
            with zipfile.ZipFile(args.zip) as z:
                need(z.testzip() is None, "ZIP CRC verification failed")
                for e in m["entries"]:
                    b = z.read("RacerRevenge50FF/" + e["path"])
                    need(len(b) == e["bytes"] and hashlib.sha256(b).hexdigest() == e["sha256"],
                         "ZIP member hash verification failed")
            receipt["zip"] = dict(path=str(args.zip), bytes=args.zip.stat().st_size,
                                  sha256=digest(args.zip))
        print(json.dumps(receipt, indent=2))
    except (ValueError, OSError, KeyError, TypeError, json.JSONDecodeError) as e:
        parser.exit(1, "package_playable50ff: " + str(e) + "\n")


if __name__ == "__main__":
    main()
