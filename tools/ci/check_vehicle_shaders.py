"""Check the race shaders linked into a Windows game build with NVIDIA Cg and Khronos glslang."""
import argparse
import os
from pathlib import Path
import re
import subprocess


def main():
    root = Path(__file__).resolve().parents[2]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--game", required=True, type=Path)
    parser.add_argument("--cgc", required=True, type=Path, help="NVIDIA Cg 3.1 standalone compiler")
    parser.add_argument("--glslang", required=True, type=Path, help="Khronos glslang executable")
    parser.add_argument("--output", type=Path, default=root / "work/scratch/vehicle-shader-check")
    args = parser.parse_args()

    cpp = (root / "platform/linux/native/vehicle_shader_gl50cz.cpp").read_text(encoding="utf-8")
    binary = args.game.read_bytes()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, TEMP=str(output), TMP=str(output))
    cgc, glslang = args.cgc.resolve(), args.glslang.resolve()

    def checked_source(stage):
        function = f"vehicleGl{stage}Source50CZ"
        match = re.search(rf'{function}\(\) noexcept\s*\{{\s*return R"GLSL\((.*?)\)GLSL";', cpp, re.S)
        if not match:
            raise ValueError(f"Missing GLSL source in {function}")
        shader = match.group(1)
        for linebreak in ("\n", "\r\n"):
            if shader.replace("\n", linebreak).encode() + b"\0" in binary:
                return shader
        raise ValueError(f"{args.game.name} does not contain the checked-out {stage.lower()} shader")

    vertex = output / "vehicle.vert"
    vertex.write_text(checked_source("Vertex"), encoding="utf-8")
    fragment = checked_source("Fragment")

    def compile_nvidia(source, profile):
        assembly = source.with_suffix(source.suffix + ".asm")
        subprocess.run([cgc, "-oglsl", "-profile", profile, "-o", assembly, source],
                       cwd=output, env=env, check=True)

    compile_nvidia(vertex, "gp5vp")
    version, body = fragment.split("\n", 1)
    for variant, defines in (("base", ""), ("coverage", "#define RR_COVERAGE_PAYLOAD 1\n")):
        path = output / f"vehicle-{variant}.frag"
        path.write_text(f"{version}\n{defines}{body}", encoding="utf-8")
        compile_nvidia(path, "gp5fp")
        subprocess.run([glslang, "-l", vertex, path], cwd=output, env=env, check=True)


if __name__ == "__main__":
    main()
