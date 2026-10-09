#!/usr/bin/env python3
"""Bounded first-pass/slot-one texture diagnostic over unchanged 49G meshes.

Default operation validates a non-drawing plan. A render requires its exact hash.
This is a labeled external diagnostic, not a recovered original material renderer.
Only installed NumPy, Pillow, EGL and GL are used. No asset conversion or installs.
"""
from __future__ import annotations

import argparse
import ctypes as C
import hashlib
import json
import math
import os
from pathlib import Path
import resource
import struct
import subprocess
import sys
import time

import render_neutral49g as base


MAX_MANIFEST_BYTES = 2 * 1024 * 1024
MAX_TEXTURES = 26
MAX_TEXTURE_DIMENSION = 256
MAX_TEXTURE_RGBA_BYTES = 3_940_352
MAX_DRAWS = 512
FALLBACKS = {"TA": [0.57, 0.63, 0.68, 1.0],
             "POD01": [0.97, 0.58, 0.15, 1.0]}
AUTHORED_HASHES = {
    "TA": "4b38721b18705c459da282fe7736e3f3d45f3c7a0c21daeca033ca96833415d7",
    "POD01": "378d2984f9acb8f25e9d58f25c3c519d85293ec284b2fa12e8f28f4fa540471f",
    "camera": "fe38b3609f916bee428986debe90f343bcc97d8fcfa917e8156e04214bc2f42b",
}
VERTEX_SHADER = b"""#version 450 core
layout(location=0) in vec3 position;
layout(location=1) in vec2 nativeUV;
layout(location=2) in vec4 nativeColor;
uniform mat4 suppliedCombined;
out vec2 uv;
out vec3 vertexRGB;
void main() {
    gl_Position = suppliedCombined * vec4(position, 1.0);
    uv = nativeUV;
    vertexRGB = nativeColor.rgb;
}
"""
FRAGMENT_SHADER = b"""#version 450 core
in vec2 uv;
in vec3 vertexRGB;
uniform sampler2D nativeImage;
uniform int useTexture;
uniform vec4 fallbackColor;
layout(location=0) out vec4 color;
void main() {
    if (useTexture != 0)
        color = vec4(texture(nativeImage, uv).rgb * vertexRGB, 1.0);
    else
        color = vec4(fallbackColor.rgb, 1.0);
}
"""


def demand(condition, message):
    if not condition:
        raise ValueError(message)


def integer(value, label, maximum, minimum=0):
    demand(type(value) is int and minimum <= value <= maximum,
           f"{label} must be an integer in [{minimum}, {maximum}]")
    return value


def digest_match(raw, expected, label):
    demand(isinstance(expected, str) and len(expected) == 64
           and all(c in "0123456789abcdef" for c in expected), f"Invalid {label} SHA256")
    demand(base.sha256(raw) == expected, f"{label} SHA256 mismatch")


def relative_input(root, value, label):
    demand(isinstance(value, str) and value and not Path(value).is_absolute(),
           f"{label} must be a nonempty relative path")
    return (root / value).resolve(strict=True)


def load_texture(path, row, np):
    raw = base.read_bounded(path, 16 + MAX_TEXTURE_DIMENSION ** 2 * 4)
    demand(len(raw) >= 16 and raw[:8] == b"RRTEX001", "Expected RRTEX001 header")
    width, height = struct.unpack_from("<II", raw, 8)
    integer(width, "texture width", MAX_TEXTURE_DIMENSION, 1)
    integer(height, "texture height", MAX_TEXTURE_DIMENSION, 1)
    size = width * height * 4
    demand(len(raw) == 16 + size, "RRTexture01 exact RGBA extent mismatch")
    integer(row["width"], "declared texture width", MAX_TEXTURE_DIMENSION, 1)
    integer(row["height"], "declared texture height", MAX_TEXTURE_DIMENSION, 1)
    integer(row["rgba_bytes"], "declared texture RGBA bytes", MAX_TEXTURE_DIMENSION ** 2 * 4, 1)
    demand((row["width"], row["height"], row["rgba_bytes"]) == (width, height, size),
           "Texture dimensions/payload differ from binding metadata")
    digest_match(raw, row["sha256"], "texture")
    integer(row["mip_index"], "original mip index", 16)
    # This is a view into the original byte string: no channel/row manipulation.
    rgba = np.frombuffer(raw, dtype=np.uint8, count=size, offset=16).reshape(height, width, 4)
    return {"raw": raw, "rgba": rgba,
            "summary": {"id": row["id"], "path": str(path), "sha256": row["sha256"],
                        "original_mip_index": row["mip_index"], "gl_level": 0,
                        "width": width, "height": height, "rgba_bytes": size,
                        "file_bytes": len(raw)}}


def load_plan(recipe_path, cpu, disable_textures, np):
    recipe_raw = base.read_bounded(recipe_path, MAX_MANIFEST_BYTES)
    recipe = json.loads(recipe_raw)
    demand(isinstance(recipe, dict) and recipe.get("schema") == "textured49h.recipe.v1",
           "Expected textured49h.recipe.v1")
    demand(not set(recipe) - {"schema", "profile", "width", "height", "camera_json",
                              "bindings_json", "description", "pixel_probes"},
           "Unknown recipe key")
    profile = recipe.get("profile", "authored49h")
    demand(profile in ("authored49h", "positive_fixture49h"), "Unknown diagnostic profile")
    width = integer(recipe.get("width", 640), "width", 640, 1)
    height = integer(recipe.get("height", 480), "height", 480, 1)
    demand(width * height <= base.MAX_PIXELS, "Framebuffer pixel budget exceeded")
    camera_path = relative_input(recipe_path.parent, recipe["camera_json"], "camera_json")
    camera = base.load_camera(camera_path, np)
    bindings_path = relative_input(recipe_path.parent, recipe["bindings_json"], "bindings_json")
    bindings_raw = base.read_bounded(bindings_path, MAX_MANIFEST_BYTES)
    bindings = json.loads(bindings_raw)
    demand(isinstance(bindings, dict) and bindings.get("schema") == "textured49h.bindings.v1",
           "Expected textured49h.bindings.v1")
    demand(not set(bindings) - {"schema", "meshes", "textures", "spans", "summary", "provenance", "policy"},
           "Unknown binding-manifest top-level key")
    rows = bindings.get("meshes")
    demand(isinstance(rows, list) and len(rows) == 2, "Exactly two shared mesh sets required")
    demand([row.get("id") for row in rows] == ["TA", "POD01"], "Mesh order must be TA, POD01")
    meshes, mesh_by_id, mesh_summaries = [], {}, []
    controls = [base.cpu_control("homogeneous basis " + str(i), point, camera["combined"], np)
                for i, point in enumerate(((0, 0, 0, 1), (1, 0, 0, 1),
                                           (0, 1, 0, 1), (0, 0, 1, 1)))]
    for row in rows:
        name = row["id"]
        path = relative_input(bindings_path.parent, row["path"], "mesh path")
        mesh = base.load_mesh(path, np)
        digest_match(mesh["raw"], row["sha256"], "mesh " + name)
        summary = mesh["summary"]
        integer(row["vertices"], "declared mesh vertices", base.MAX_VERTICES, 1)
        integer(row["indices"], "declared mesh indices", base.MAX_INDICES, 1)
        integer(row["triangles"], "declared mesh triangles", base.MAX_INDICES // 3, 1)
        demand((row["vertices"], row["indices"], row["triangles"]) ==
               (summary["vertex_count"], summary["index_count"], summary["triangle_count"]),
               "Mesh counts differ from binding metadata")
        demand(row.get("fallback_rgba", FALLBACKS[name]) == FALLBACKS[name],
               "Mesh fallback must preserve exact 49G color")
        mesh["id"], mesh["fallback_rgba"] = name, FALLBACKS[name]
        meshes.append(mesh)
        mesh_by_id[name] = mesh
        mesh_summaries.append(dict(summary, id=name, fallback_rgba=FALLBACKS[name]))
        positions = mesh["vertex_data"]["position"]
        for i in sorted({0, len(positions) // 2, len(positions) - 1}):
            controls.append(base.cpu_control(name + " vertex " + str(i),
                                             [*positions[i], 1.0], camera["combined"], np))
    demand(len({m["summary"]["path"] for m in meshes}) == 2, "Mesh paths must be distinct")
    texture_rows = bindings.get("textures")
    demand(isinstance(texture_rows, list) and 1 <= len(texture_rows) <= MAX_TEXTURES,
           "Texture object count exceeds the 26-object bound")
    textures, texture_by_id = [], {}
    for row in texture_rows:
        name = row.get("id")
        demand(isinstance(name, str) and name and name not in texture_by_id,
               "Texture IDs must be unique nonempty strings")
        texture = load_texture(relative_input(bindings_path.parent, row["path"], "texture path"), row, np)
        textures.append(texture)
        texture_by_id[name] = texture
    demand(len({t["summary"]["path"] for t in textures}) == len(textures),
           "Each selected texture file must be uploaded only once")
    texture_bytes = sum(t["rgba"].nbytes for t in textures)
    demand(texture_bytes <= MAX_TEXTURE_RGBA_BYTES, "Texture payload budget exceeded")
    span_rows = bindings.get("spans")
    demand(isinstance(span_rows, list) and 1 <= len(span_rows) <= MAX_DRAWS,
           "Span draw count must be from 1 through 512")
    spans, used_textures = [], set()
    cursors = {key: [0, 0] for key in mesh_by_id}
    seen_pod = False
    textured_triangles = fallback_triangles = textured_spans = 0
    for order, row in enumerate(span_rows):
        integer(row.get("draw_order"), "draw_order", MAX_DRAWS - 1)
        demand(row.get("draw_order") == order, "Span order must be explicit and consecutive")
        name = row.get("mesh_id")
        demand(name in mesh_by_id, "Unknown span mesh")
        if name == "POD01":
            seen_pod = True
        demand(not seen_pod or name == "POD01", "All TA spans must precede Pod spans")
        mesh = mesh_by_id[name]
        index_start = integer(row["index_start"], "index_start", len(mesh["indices"]))
        count = integer(row["index_count"], "index_count", len(mesh["indices"]), 1)
        vertex_start = integer(row["vertex_start"], "vertex_start", len(mesh["vertex_data"]))
        vertices = integer(row["vertex_count"], "vertex_count", len(mesh["vertex_data"]), 1)
        demand(index_start % 3 == 0 and count % 3 == 0, "Spans must contain complete triangles")
        demand(index_start + count <= len(mesh["indices"])
               and vertex_start + vertices <= len(mesh["vertex_data"]), "Span exceeds its shared buffer")
        demand(cursors[name] == [index_start, vertex_start],
               "Ordered spans must partition each index/vertex buffer exactly once")
        selected = mesh["indices"][index_start:index_start + count]
        demand(int(selected.min()) >= vertex_start and int(selected.max()) < vertex_start + vertices,
               "Span indices escape the corresponding native surface vertex extent")
        cursors[name] = [index_start + count, vertex_start + vertices]
        integer(row.get("pass"), "material pass", 1, 1)
        integer(row.get("slot"), "texture slot", 1, 1)
        demand(row.get("fallback_rgba", FALLBACKS[name]) == FALLBACKS[name],
               "Span fallback differs from 49G per-mesh color")
        texture_id = row.get("texture_id")
        demand(texture_id is None or texture_id in texture_by_id, "Unknown span texture ID")
        expected_status = "textured" if texture_id is not None else "neutral_fallback"
        demand(row.get("status") == expected_status, "Binding status and texture ID disagree")
        if texture_id is None:
            demand(isinstance(row.get("fallback_reason"), str) and row["fallback_reason"],
                   "Every neutral binding requires its retained reason")
            fallback_triangles += count // 3
        else:
            used_textures.add(texture_id)
            textured_spans += 1
            textured_triangles += count // 3
        spans.append({"draw_order": order, "mesh_id": name,
                      "index_start": index_start, "index_count": count,
                      "index_byte_offset": index_start * 4, "vertex_start": vertex_start,
                      "vertex_count": vertices, "triangle_count": count // 3,
                      "index_span_sha256": base.sha256(selected.tobytes()),
                      "texture_id": texture_id, "status": expected_status,
                      "fallback_reason": row.get("fallback_reason"), "fallback_rgba": FALLBACKS[name],
                      "scene_key": row.get("scene_key"), "scene_member": row.get("scene_member"),
                      "instance": row.get("instance"), "material_index": row.get("material_index"),
                      "material": row.get("material"), "material_key": row.get("material_key"),
                      "pass": 1, "slot": 1, "program_unexecuted": row.get("program_unexecuted")})
    for mesh in meshes:
        demand(cursors[mesh["id"]] == [len(mesh["indices"]), len(mesh["vertex_data"])],
               "Span partition does not reach the exact end of both shared buffers")
    demand(used_textures == set(texture_by_id), "Texture list must contain exactly the active unique bindings")
    triangles = sum(len(m["indices"]) // 3 for m in meshes)
    mesh_bytes = sum(len(m["raw"]) for m in meshes)
    buffer_bytes = sum(m["vertex_data"].nbytes + m["indices"].nbytes for m in meshes)
    if profile == "authored49h":
        demand((width, height) == (640, 480), "Authored diagnostic must remain 640x480")
        demand(camera["sha256"] == AUTHORED_HASHES["camera"], "Authored 49G camera bytes changed")
        demand(all(m["summary"]["sha256"] == AUTHORED_HASHES[m["id"]] for m in meshes),
               "Authored 49G geometry/pose bytes changed")
        demand((len(spans), len(textures), texture_bytes, triangles, buffer_bytes) ==
               (360, 26, 3_940_352, 73_409, 3_663_732), "Authored resource totals changed")
        demand((textured_spans, textured_triangles, len(spans) - textured_spans, fallback_triangles) ==
               (286, 67_967, 74, 5_442), "Authored texture/fallback coverage changed")
        demand(all(s["mesh_id"] == "TA" for s in spans if s["texture_id"] is None),
               "Unexpected authored Pod fallback")
    probes = recipe.get("pixel_probes", [])
    demand(isinstance(probes, list) and len(probes) <= 64, "Too many pixel probes")
    demand(not probes or profile == "positive_fixture49h", "Pixel expectations belong to positive fixtures only")
    for probe in probes:
        demand(isinstance(probe, dict) and set(probe) == {"label", "x", "y", "rgba8", "tolerance"},
               "Invalid positive pixel probe")
        demand(isinstance(probe["label"], str) and probe["label"], "Probe label is required")
        integer(probe["x"], "probe x", width - 1)
        integer(probe["y"], "probe y", height - 1)
        integer(probe["tolerance"], "probe tolerance", 2)
        demand(isinstance(probe["rgba8"], list) and len(probe["rgba8"]) == 4, "Probe requires RGBA")
        for channel in probe["rgba8"]:
            integer(channel, "probe channel", 255)
    demand(not (disable_textures and probes), "Use the authored neutral control independently of texture probes")
    uploaded_texture_bytes = 0 if disable_textures else texture_bytes
    texture_file_bytes = sum(len(t["raw"]) for t in textures)
    pixels = width * height
    plan = {
        "schema": "textured49h.plan.v1", "diagnostic_only": True, "fixture_profile": profile,
        "recipe_path": str(recipe_path), "recipe_sha256": base.sha256(recipe_raw),
        "bindings_path": str(bindings_path), "bindings_sha256": base.sha256(bindings_raw),
        "renderer_script_sha256": base.sha256(Path(__file__).read_bytes()),
        "base49g_script_sha256": base.sha256(Path(base.__file__).read_bytes()),
        "vertex_shader_sha256": base.sha256(VERTEX_SHADER),
        "fragment_shader_sha256": base.sha256(FRAGMENT_SHADER),
        "camera": {"path": camera["path"], "sha256": camera["sha256"],
                   **{name + "_f32_bits": camera["source"][name + "_f32_bits"]
                      for name in ("view", "projection", "combined")},
                   "matrix_layout": "row_major", "vector_convention": "row_vector",
                   "clip_depth": "zero_to_one"},
        "meshes": mesh_summaries, "textures": [t["summary"] for t in textures], "spans": spans,
        "span_partition": "Two complete ordered, nonoverlapping index/vertex partitions; no base-vertex rebasing",
        "solid_draw_calls": len(spans), "wire_draw_calls": 0, "total_draw_calls": len(spans),
        "unique_input_triangles": triangles, "submitted_triangles": triangles,
        "binding_coverage": {"textured_spans": textured_spans, "textured_triangles": textured_triangles,
                             "fallback_spans": len(spans) - textured_spans, "fallback_triangles": fallback_triangles},
        "textures_enabled": not disable_textures,
        "planned_texture_uploads": 0 if disable_textures else len(textures),
        "planned_mesh_uploads": 2, "planned_gl_buffer_uploads": 4,
        "framebuffer": {"width": width, "height": height, "color": "RGBA8",
                        "depth": "DEPTH_COMPONENT24", "samples": 1, "clear_rgba8": [18, 22, 30, 255],
                        "estimated_color_depth_storage_bytes": pixels * 8,
                        "single_readback_bytes": pixels * 4, "readback_plus_row_flip_bytes": pixels * 8},
        "budget": {"cpu_affinity": [cpu], "LP_NUM_THREADS": 1,
                   "mesh_input_bytes": mesh_bytes, "texture_input_bytes": texture_file_bytes,
                   "texture_header_bytes": 16 * len(textures), "texture_rgba_input_bytes": texture_bytes,
                   "gl_buffer_bytes": buffer_bytes, "gl_texture_bytes": uploaded_texture_bytes,
                   "gl_geometry_and_texture_payload_bytes": buffer_bytes + uploaded_texture_bytes,
                   "input_plus_gl_payload_framebuffer_one_readback_bytes":
                       mesh_bytes + texture_file_bytes + buffer_bytes + uploaded_texture_bytes + pixels * 12,
                   "input_plus_gl_payload_framebuffer_readback_and_flip_bytes":
                       mesh_bytes + texture_file_bytes + buffer_bytes + uploaded_texture_bytes + pixels * 16,
                   "manifest_file_bytes_read": {"recipe": len(recipe_raw), "bindings": len(bindings_raw),
                                                "camera": camera_path.stat().st_size},
                   "address_space_limit_bytes": base.ADDRESS_SPACE_BYTES,
                   "cpu_time_limit_seconds": base.CPU_SECONDS, "wall_time_limit_seconds": base.WALL_SECONDS,
                   "exclusions": "Named byte counts are payload arithmetic. Parsed metadata, validation temporaries, PNG work, "
                                 "EGL pbuffer, shader/driver/Python allocations and alignment are additional; actual RSS recorded."},
        "profile": {"unlit": True, "depth_test": "LESS", "blending": False, "culling": False,
                    "alpha_test": False, "output_alpha": 1, "sRGB_decode_or_encode": False,
                    "texture": "Unmodified RRTEX001 RGBA8 bytes; largest original mip uploaded as GL level 0 only",
                    "uv": "Exported source UV unchanged; source row zero is T=0; no UV or texture-row flip",
                    "color": "texture RGB times perspective-interpolated vertex UNORM8 RGB; no brightness/alpha correction",
                    "sampler": {"min_filter": "LINEAR", "mag_filter": "LINEAR", "wrap_s": "REPEAT",
                                "wrap_t": "REPEAT", "base_level": 0, "max_level": 0, "min_lod": 0, "max_lod": 0},
                    "fallback": FALLBACKS, "original_material_programs": False, "extra_passes_or_maps": False,
                    "animation_lighting_or_visibility": False, "auto_fit_or_reposition": False},
        "cpu_clip_controls": controls, "pixel_probes": probes,
    }
    plan["plan_sha256"] = base.sha256(base.json_bytes(plan))
    return meshes, textures, camera, plan


class TexturedContext(base.EGLContext):
    def bind_gl(self):
        super().bind_gl()
        u, i, f, p = C.c_uint, C.c_int, C.c_float, C.c_void_p
        signatures = {
            "glActiveTexture": (None, u), "glGenTextures": (None, i, C.POINTER(u)),
            "glBindTexture": (None, u, u), "glDeleteTextures": (None, i, C.POINTER(u)),
            "glTexParameteri": (None, u, u, i), "glTexParameterf": (None, u, u, f),
            "glTexImage2D": (None, u, i, i, i, i, i, u, u, p),
            "glGetTexParameteriv": (None, u, u, C.POINTER(i)),
            "glGetTexParameterfv": (None, u, u, C.POINTER(f)),
            "glGetTexLevelParameteriv": (None, u, i, u, C.POINTER(i)),
            "glUniform1i": (None, i, i), "glGetUniformiv": (None, u, i, C.POINTER(i)),
        }
        for name, signature in signatures.items():
            self.gl_function(name, *signature)


def shader_program(gl):
    shaders, program = [], 0
    try:
        for kind, source in ((0x8B31, VERTEX_SHADER), (0x8B30, FRAGMENT_SHADER)):
            shader = gl.glCreateShader(kind)
            shaders.append(shader)
            demand(shader != 0, "glCreateShader failed")
            text, length = C.c_char_p(source), C.c_int(len(source))
            gl.glShaderSource(shader, 1, C.byref(text), C.byref(length))
            gl.glCompileShader(shader)
            ok = C.c_int()
            gl.glGetShaderiv(shader, 0x8B81, C.byref(ok))
            if not ok.value:
                log = C.create_string_buffer(8192)
                gl.glGetShaderInfoLog(shader, len(log), None, log)
                raise RuntimeError(f"49H shader compilation failed: {log.value!r}")
        program = gl.glCreateProgram()
        demand(program != 0, "glCreateProgram failed")
        for shader in shaders:
            gl.glAttachShader(program, shader)
        gl.glLinkProgram(program)
        ok = C.c_int()
        gl.glGetProgramiv(program, 0x8B82, C.byref(ok))
        if not ok.value:
            log = C.create_string_buffer(8192)
            gl.glGetProgramInfoLog(program, len(log), None, log)
            raise RuntimeError(f"49H shader link failed: {log.value!r}")
        gl.errors("49H shader compilation/link")
        return program
    except Exception:
        if program:
            gl.glDeleteProgram(program)
        raise
    finally:
        for shader in shaders:
            gl.glDeleteShader(shader)


def render(meshes, textures, camera, plan, output, np):
    from PIL import Image, PngImagePlugin

    start = time.monotonic()
    gl, program = None, 0
    framebuffer, renderbuffers = C.c_uint(), (C.c_uint * 2)()
    texture_names = (C.c_uint * len(textures))()
    objects, texture_ids, checks, texture_checks = {}, {}, [], []
    actual_draws = actual_triangles = texture_uploads = buffer_uploads = 0
    width, height = plan["framebuffer"]["width"], plan["framebuffer"]["height"]

    def check(label):
        gl.errors(label)
        checks.append(label)

    try:
        gl = TexturedContext()
        info = gl.info()
        demand("llvmpipe" in (info["gl_renderer"] or "").lower(), "Expected forced llvmpipe software renderer")
        demand(max(width, height) <= info["max_renderbuffer_size"], "Framebuffer exceeds actual GL limit")
        demand(all(max(t["rgba"].shape[:2]) <= info["max_texture_size"] for t in textures),
               "Texture exceeds actual GL limit")
        gl.glGenFramebuffers(1, C.byref(framebuffer))
        gl.glBindFramebuffer(0x8D40, framebuffer.value)
        gl.glGenRenderbuffers(2, renderbuffers)
        for index, internal, attachment in ((0, 0x8058, 0x8CE0), (1, 0x81A6, 0x8D00)):
            gl.glBindRenderbuffer(0x8D41, renderbuffers[index])
            gl.glRenderbufferStorage(0x8D41, internal, width, height)
            gl.glFramebufferRenderbuffer(0x8D40, attachment, 0x8D41, renderbuffers[index])
        gl.glDrawBuffer(0x8CE0)
        gl.glReadBuffer(0x8CE0)
        demand(gl.glCheckFramebufferStatus(0x8D40) == 0x8CD5, "Framebuffer is incomplete")
        check("RGBA8/depth24 framebuffer complete")
        gl.glClipControl(0x8CA1, 0x935F)
        demand(gl.integer(0x935C) == 0x8CA1 and gl.integer(0x935D) == 0x935F,
               "GL clip origin/depth differs from supplied 49G camera")
        gl.glViewport(0, 0, width, height)
        for flag in (0x0B44, 0x0BE2, 0x0B50, 0x0DE1, 0x0BC0, 0x0BD0,
                     0x809D, 0x8DB9, 0x0C11, 0x864F, 0x8C89):
            gl.glDisable(flag)
        gl.glEnable(0x0B71)
        gl.glDepthFunc(0x0201)
        gl.glDepthMask(1)
        gl.glColorMask(1, 1, 1, 1)
        gl.glPolygonMode(0x0408, 0x1B02)
        gl.glClearColor(*(v / 255.0 for v in plan["framebuffer"]["clear_rgba8"]))
        gl.glClearDepth(1.0)
        gl.glClear(0x00004000 | 0x00000100)
        program = shader_program(gl)
        gl.glUseProgram(program)
        locations = {name: gl.glGetUniformLocation(program, name.encode("ascii"))
                     for name in ("suppliedCombined", "nativeImage", "useTexture", "fallbackColor")}
        demand(all(v >= 0 for v in locations.values()), "Required 49H shader uniform is inactive")
        combined = camera["combined"]
        gl.glUniformMatrix4fv(locations["suppliedCombined"], 1, 0,
                              combined.ctypes.data_as(C.POINTER(C.c_float)))
        returned = np.empty(16, dtype=np.float32)
        gl.glGetUniformfv(program, locations["suppliedCombined"], returned.ctypes.data_as(C.POINTER(C.c_float)))
        demand(np.array_equal(returned.view(np.uint32), combined.view(np.uint32)),
               "Matrix uniform round trip changed supplied float32 bits")
        gl.glActiveTexture(0x84C0)
        gl.glUniform1i(locations["nativeImage"], 0)
        sampler_unit = C.c_int(-1)
        gl.glGetUniformiv(program, locations["nativeImage"], C.byref(sampler_unit))
        demand(sampler_unit.value == 0, "Texture sampler did not bind unit zero")
        check("exact matrix bits, explicit shader state and sampler unit")
        for mesh in meshes:
            vao, buffers = C.c_uint(), (C.c_uint * 2)()
            objects[mesh["id"]] = (vao, buffers)
            gl.glGenVertexArrays(1, C.byref(vao))
            gl.glBindVertexArray(vao.value)
            gl.glGenBuffers(2, buffers)
            for target, handle, array in ((0x8892, buffers[0], mesh["vertex_data"]),
                                           (0x8893, buffers[1], mesh["indices"])):
                gl.glBindBuffer(target, handle)
                gl.glBufferData(target, array.nbytes, C.c_void_p(array.ctypes.data), 0x88E4)
                buffer_uploads += 1
            for location, count, kind, normalized, offset in ((0, 3, 0x1406, 0, 0),
                                                             (1, 2, 0x1406, 0, 12),
                                                             (2, 4, 0x1401, 1, 20)):
                gl.glVertexAttribPointer(location, count, kind, normalized, 24, C.c_void_p(offset))
                gl.glEnableVertexAttribArray(location)
            check("one shared vertex/index upload " + mesh["id"])
        if plan["textures_enabled"]:
            gl.glGenTextures(len(textures), texture_names)
            gl.glBindBuffer(0x88EC, 0)  # PIXEL_UNPACK_BUFFER
            gl.glPixelStorei(0x0CF5, 1)  # UNPACK_ALIGNMENT
            gl.glPixelStorei(0x0CF2, 0)  # UNPACK_ROW_LENGTH
            parameters = ((0x2801, 0x2601), (0x2800, 0x2601), (0x2802, 0x2901),
                          (0x2803, 0x2901), (0x813C, 0), (0x813D, 0))
            for texture, handle in zip(textures, texture_names):
                summary, rgba = texture["summary"], texture["rgba"]
                demand(handle != 0, "Texture object creation returned zero")
                texture_ids[summary["id"]] = handle
                gl.glBindTexture(0x0DE1, handle)
                for name, value in parameters:
                    gl.glTexParameteri(0x0DE1, name, value)
                for name in (0x813A, 0x813B):
                    gl.glTexParameterf(0x0DE1, name, 0.0)
                gl.glTexImage2D(0x0DE1, 0, 0x8058, summary["width"], summary["height"],
                                0, 0x1908, 0x1401, C.c_void_p(rgba.ctypes.data))
                texture_uploads += 1
                for name, expected in parameters:
                    result = C.c_int()
                    gl.glGetTexParameteriv(0x0DE1, name, C.byref(result))
                    demand(result.value == expected, "Texture sampler integer state mismatch")
                for name in (0x813A, 0x813B):
                    result = C.c_float()
                    gl.glGetTexParameterfv(0x0DE1, name, C.byref(result))
                    demand(result.value == 0.0, "Texture LOD clamp differs from zero")
                for name, expected in ((0x1000, summary["width"]), (0x1001, summary["height"]), (0x1003, 0x8058)):
                    result = C.c_int()
                    gl.glGetTexLevelParameteriv(0x0DE1, 0, name, C.byref(result))
                    demand(result.value == expected, "Uploaded texture size/format mismatch")
                texture_checks.append({"id": summary["id"], "parameters_and_level_zero_verified": True})
                check("one unmodified largest-mip upload " + summary["id"])
        prepared = time.monotonic()
        for span in plan["spans"]:
            gl.glBindVertexArray(objects[span["mesh_id"]][0].value)
            use_texture = plan["textures_enabled"] and span["texture_id"] is not None
            gl.glUniform1i(locations["useTexture"], int(use_texture))
            gl.glUniform4f(locations["fallbackColor"], *span["fallback_rgba"])
            gl.glBindTexture(0x0DE1, texture_ids[span["texture_id"]] if use_texture else 0)
            gl.glDrawElements(0x0004, span["index_count"], 0x1405, C.c_void_p(span["index_byte_offset"]))
            actual_draws += 1
            actual_triangles += span["triangle_count"]
            check("ordered surface draw " + str(span["draw_order"]))
        gl.glFinish()
        check("solid draw finish")
        drawn = time.monotonic()
        demand((actual_draws, actual_triangles, texture_uploads, buffer_uploads) ==
               (plan["total_draw_calls"], plan["submitted_triangles"], plan["planned_texture_uploads"], 4),
               "Actual draw/upload accounting differs from acknowledged plan")
        gl.glPixelStorei(0x0D05, 1)
        pixels = np.empty((height, width, 4), dtype=np.uint8)
        gl.glReadPixels(0, 0, width, height, 0x1908, 0x1401, C.c_void_p(pixels.ctypes.data))
        check("bounded RGBA8 readback")
        # Only final framebuffer storage is reversed for PNG. Source UV/texture rows are unchanged.
        pixels = np.ascontiguousarray(pixels[::-1])
        probe_results = []
        for probe in plan["pixel_probes"]:
            actual = pixels[probe["y"], probe["x"]].astype(int).tolist()
            passed = all(abs(a - b) <= probe["tolerance"] for a, b in zip(actual, probe["rgba8"]))
            probe_results.append(dict(probe, actual_rgba8=actual, passed=passed))
            demand(passed, "Positive pixel control failed: " + probe["label"] + ": " + str(actual))
        demand(bool(np.all(pixels[:, :, 3] == 255)), "Diagnostic output alpha must remain opaque")
        path = output.parent / (output.name + ".png")
        metadata = PngImagePlugin.PngInfo()
        metadata.add_text("Title", "First-pass texture diagnostic, not original material shading or gameplay")
        metadata.add_text("Description", "Unchanged 49G camera/geometry; source UV and RGBA; linear repeat base level; "
                          "texture RGB times vertex UNORM8 RGB; opaque alpha. " +
                          ("Textures disabled: 49G neutral control." if not plan["textures_enabled"] else
                           "Unresolved bindings retain 49G per-mesh neutral colors."))
        metadata.add_text("Plan SHA256", plan["plan_sha256"])
        with path.open("xb") as handle:
            Image.fromarray(pixels).save(handle, format="PNG", pnginfo=metadata)
        png = base.read_bounded(path, base.MAX_PIXELS * 8 + 65536)
        background = np.asarray(plan["framebuffer"]["clear_rgba8"], dtype=np.uint8)
        ys, xs = np.nonzero(np.any(pixels != background, axis=2))
        return {"status": "rendered_diagnostic_only", "renderer": info,
                "actual_draw_calls": actual_draws, "actual_submitted_triangles": actual_triangles,
                "actual_texture_uploads": texture_uploads, "actual_gl_buffer_uploads": buffer_uploads,
                "shared_mesh_sets": len(objects), "uniform_bits_round_trip": True,
                "clip_control": "GL_LOWER_LEFT / GL_ZERO_TO_ONE", "texture_state_checks": texture_checks,
                "gl_error_checks_passed": checks, "positive_pixel_controls": probe_results,
                "opaque_output_alpha": True, "pixel_rgba8_sha256": base.sha256(pixels.tobytes()),
                "images": [{"path": str(path), "bytes": len(png), "sha256": base.sha256(png),
                            "kind": "textured_diagnostic" if plan["textures_enabled"] else "neutral_control"}],
                "coverage": {"non_background_pixels": len(xs), "non_background_bbox_inclusive":
                             [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None},
                "timing_seconds": {"context_shader_buffer_texture_setup": prepared - start,
                                   "solid_draws_and_finish": drawn - prepared,
                                   "render_function_including_png": time.monotonic() - start},
                "interpretation": "External host diagnostic only; no original material, native-platform renderer, "
                                  "gameplay FPS, realtime throughput or target-memory claim."}
    finally:
        if gl:
            gl.glUseProgram(0)
            gl.glBindVertexArray(0)
            gl.glBindTexture(0x0DE1, 0)
            gl.glDeleteTextures(len(textures), texture_names)
            for vao, buffers in objects.values():
                gl.glDeleteBuffers(2, buffers)
                if vao.value:
                    gl.glDeleteVertexArrays(1, C.byref(vao))
            if program:
                gl.glDeleteProgram(program)
            gl.glBindFramebuffer(0x8D40, 0)
            gl.glDeleteRenderbuffers(2, renderbuffers)
            if framebuffer.value:
                gl.glDeleteFramebuffers(1, C.byref(framebuffer))
            gl.close()


def linear_repeat_reference(rgba, uv, color):
    """Independent scalar reference: texel-center coordinates and modulo wrapping."""
    height, width = len(rgba), len(rgba[0])
    x, y = uv[0] * width - 0.5, uv[1] * height - 0.5
    ix, iy = math.floor(x), math.floor(y)
    fx, fy = x - ix, y - iy
    out = []
    for channel in range(3):
        low = rgba[iy % height][ix % width][channel] * (1 - fx) + rgba[iy % height][(ix + 1) % width][channel] * fx
        high = rgba[(iy + 1) % height][ix % width][channel] * (1 - fx) + rgba[(iy + 1) % height][(ix + 1) % width][channel] * fx
        out.append(round((low * (1 - fy) + high * fy) * color[channel] / 255.0))
    return out + [255]


def write_probe_fixture(root):
    """Only valid ordinary files; 12 draws, 24 triangles, 2 RGBA2x2 textures, 64x48."""
    root.mkdir(parents=False, exist_ok=False)
    pattern = [[[31, 73, 151, 0], [211, 47, 83, 63]],
               [[59, 193, 107, 127], [167, 101, 229, 255]]]
    solid = [[[23, 187, 91, 10], [23, 187, 91, 10]],
             [[23, 187, 91, 10], [23, 187, 91, 10]]]
    textures = []
    for name, rgba in (("asymmetric", pattern), ("solid", solid)):
        raw = b"RRTEX001" + struct.pack("<II", 2, 2) + bytes(c for row in rgba for pixel in row for c in pixel)
        (root / (name + ".rrtex")).write_bytes(raw)
        textures.append({"id": name, "path": name + ".rrtex", "sha256": base.sha256(raw),
                         "mip_index": 0, "width": 2, "height": 2, "rgba_bytes": 16})
    cases = [
        ("row0 col0 and ignored alpha", "asymmetric", (0.25, 0.25), (255, 255, 255, 0)),
        ("row0 col1 distinct UNORM channels", "asymmetric", (0.75, 0.25), (127, 63, 191, 64)),
        ("row1 col0 unflipped V", "asymmetric", (0.25, 0.75), (255, 255, 255, 255)),
        ("row1 col1 RGBA channel order", "asymmetric", (0.75, 0.75), (255, 255, 255, 255)),
        ("TA fallback ignores vertex RGB", None, (0.25, 0.25), (0, 0, 0, 0)),
        ("texture B after fallback", "solid", (0.25, 0.25), (255, 255, 255, 255)),
        ("texture A on mesh2 with signed wrap", "asymmetric", (-0.75, 1.25), (255, 255, 255, 255)),
        ("bilinear wrap across both seams", "asymmetric", (1.125, -0.125), (255, 255, 255, 255)),
        ("bilinear four-texel center", "asymmetric", (0.5, 0.5), (255, 255, 255, 255)),
        ("bilinear asymmetric fractions", "asymmetric", (0.375, 0.625), (255, 255, 255, 255)),
        ("texture B native RGB multiply opaque", "solid", (0.25, 0.25), (64, 128, 192, 0)),
        ("Pod fallback remains orange", None, (0.25, 0.25), (0, 0, 0, 0)),
    ]
    meshes, spans, probes = [], [], []
    for mesh_number, name in enumerate(("TA", "POD01")):
        vertices, indices = bytearray(), []
        for patch in range(mesh_number * 6, mesh_number * 6 + 6):
            label, texture_id, uv, color = cases[patch]
            column, row = patch % 4, patch // 4
            left, right, top, bottom = column * 16 + 2, column * 16 + 14, row * 16 + 2, row * 16 + 14
            vertex_start, index_start = len(vertices) // 24, len(indices)
            for x, y in ((left, top), (right, top), (right, bottom), (left, bottom)):
                vertices.extend(struct.pack("<5f4B", x / 64 * 2 - 1, 1 - y / 48 * 2, 0.5, *uv, *color))
            indices.extend(vertex_start + i for i in (0, 1, 2, 0, 2, 3))
            spans.append({"draw_order": patch, "mesh_id": name, "index_start": index_start, "index_count": 6,
                          "vertex_start": vertex_start, "vertex_count": 4, "pass": 1, "slot": 1,
                          "texture_id": texture_id, "status": "textured" if texture_id else "neutral_fallback",
                          "fallback_reason": None if texture_id else "valid fixture without first-pass texture",
                          "fallback_rgba": FALLBACKS[name], "material": label,
                          "program_unexecuted": "positive fixture, no material program"})
            expected = (linear_repeat_reference(pattern if texture_id == "asymmetric" else solid, uv, color)
                        if texture_id else [round(c * 255) for c in FALLBACKS[name]])
            probes.append({"label": label, "x": column * 16 + 8, "y": row * 16 + 8,
                           "rgba8": expected, "tolerance": 1})
        raw = b"RRMESH01" + struct.pack("<II", len(vertices) // 24, len(indices)) + vertices + struct.pack("<" + "I" * len(indices), *indices)
        filename = name.lower() + ".rrmesh"
        (root / filename).write_bytes(raw)
        meshes.append({"id": name, "path": filename, "sha256": base.sha256(raw), "vertices": len(vertices) // 24,
                       "indices": len(indices), "triangles": len(indices) // 3, "fallback_rgba": FALLBACKS[name]})
    identity = [1.0 if row == col else 0.0 for row in range(4) for col in range(4)]
    base.write_json_new(root / "camera.json", {"schema": "neutral49g.camera.v1",
        **{name + "_f32_bits": base.f32_bits(identity) for name in ("view", "projection", "combined")}})
    base.write_json_new(root / "bindings.json", {"schema": "textured49h.bindings.v1", "meshes": meshes,
                                                 "textures": textures, "spans": spans})
    base.write_json_new(root / "recipe.json", {"schema": "textured49h.recipe.v1", "profile": "positive_fixture49h",
        "width": 64, "height": 48, "camera_json": "camera.json", "bindings_json": "bindings.json", "pixel_probes": probes})
    return root / "recipe.json"


def self_test(np):
    import tempfile

    checks = []
    matrix = np.asarray([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16], dtype=np.float32)
    demand(base.cpu_control("nonsymmetric matrix", [1, 2, 3, 1], matrix, np)["clip_xyzw"] ==
           [51.0, 58.0, 65.0, 72.0], "Positive matrix indexing control failed")
    checks.append("positive nonsymmetric native-row versus GL-column indexing")
    pattern = [[[31, 73, 151, 0], [211, 47, 83, 63]], [[59, 193, 107, 127], [167, 101, 229, 255]]]
    demand(linear_repeat_reference(pattern, (0.25, 0.25), [255] * 4) == [31, 73, 151, 255], "Texel center control failed")
    demand(linear_repeat_reference(pattern, (-0.75, 1.25), [255] * 4) == [31, 73, 151, 255], "Signed wrap control failed")
    demand(linear_repeat_reference(pattern, (0.5, 0.5), [255] * 4) == [117, 104, 142, 255], "Four-texel linear control failed")
    checks.append("independent scalar texel-center, signed-wrap and bilinear arithmetic")
    with tempfile.TemporaryDirectory(prefix="texture49h-positive-cpu-") as directory:
        recipe = write_probe_fixture(Path(directory) / "fixture")
        cpu = min(os.sched_getaffinity(0))
        meshes, textures, _, plan = load_plan(recipe, cpu, False, np)
        demand(plan == load_plan(recipe, cpu, False, np)[3], "Plan/fingerprint is nondeterministic")
        demand((plan["total_draw_calls"], plan["submitted_triangles"], plan["planned_texture_uploads"]) == (12, 24, 2),
               "Positive fixture resource accounting failed")
        demand(plan["budget"]["gl_buffer_bytes"] == 1440 and plan["budget"]["gl_texture_bytes"] == 32,
               "Positive fixture upload-byte accounting failed")
        demand(all(np.shares_memory(t["rgba"], np.frombuffer(t["raw"], dtype=np.uint8)) for t in textures),
               "Texture payload must be a non-copying raw-byte view")
        demand(all(m["vertex_data"].dtype.itemsize == 24 for m in meshes), "Native vertex stride mismatch")
        checks += ["valid two-mesh/native-layout byte accounting", "12 ordered complete shared-buffer span partitions",
                   "two unique 2x2 raw RGBA views and upload accounting", "deterministic non-drawing plan and hashes",
                   "positive texture switching and missing-binding fixtures prepared"]
    return {"status": "passed", "tests": checks, "draw_calls": 0, "egl_or_gl_context_created": False,
            "source_assets_read": False, "invalid_input_or_fault_injection": False}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--recipe", type=Path)
    operation = parser.add_mutually_exclusive_group()
    operation.add_argument("--plan", action="store_true")
    operation.add_argument("--render", action="store_true")
    operation.add_argument("--self-test", action="store_true")
    operation.add_argument("--write-probe-fixture", type=Path)
    parser.add_argument("--disable-textures", action="store_true", help="49G neutral-color control using all ordered spans")
    parser.add_argument("--plan-output", type=Path)
    parser.add_argument("--output-prefix", type=Path)
    parser.add_argument("--draw-ack")
    parser.add_argument("--cpu", type=int)
    parser.add_argument("--_worker", action="store_true", help=argparse.SUPPRESS)
    args = parser.parse_args()
    os.environ.update(base.SOFTWARE_ENV)
    allowed = os.sched_getaffinity(0)
    cpu = min(allowed) if args.cpu is None else args.cpu
    demand(cpu in allowed, "Selected CPU must be in the current allowed affinity set")
    if args._worker:
        demand(args.render, "Internal worker is only valid for an acknowledged render")
        os.sched_setaffinity(0, {cpu})
        resource.setrlimit(resource.RLIMIT_AS, (base.ADDRESS_SPACE_BYTES, base.ADDRESS_SPACE_BYTES))
        resource.setrlimit(resource.RLIMIT_CPU, (base.CPU_SECONDS, base.CPU_SECONDS + 1))
        file_limit = base.MAX_PIXELS * 8 + 65536
        resource.setrlimit(resource.RLIMIT_FSIZE, (file_limit, file_limit))
    import numpy as np

    demand(sys.byteorder == "little", "Diagnostic requires little-endian native arrays")
    if args.self_test:
        print(json.dumps(self_test(np), indent=2))
        return 0
    if args.write_probe_fixture:
        path = write_probe_fixture(args.write_probe_fixture.resolve())
        print(json.dumps({"status": "positive_fixture_written", "recipe": str(path),
                          "planned_draws": 12, "triangles": 24, "texture_upload_bytes": 32,
                          "framebuffer": [64, 48], "draw_calls": 0}, indent=2))
        return 0
    demand(args.recipe is not None, "--recipe is required")
    began = time.monotonic()
    recipe_path = args.recipe.resolve(strict=True)
    meshes, textures, camera, plan = load_plan(recipe_path, cpu, args.disable_textures, np)
    if not args.render:
        demand(not args.draw_ack and not args.output_prefix, "--draw-ack/--output-prefix require --render")
        if args.plan_output:
            base.write_json_new(args.plan_output, plan)
        print(json.dumps(plan, indent=2, sort_keys=True, allow_nan=False))
        return 0
    demand(not args.plan_output, "--plan-output is only for non-drawing plans")
    demand(args.draw_ack == plan["plan_sha256"], "Render requires the exact current non-drawing plan SHA256")
    demand(args.output_prefix is not None, "--output-prefix is required for rendering")
    output = args.output_prefix.resolve()
    demand(output.parent.is_dir(), "Output parent must already exist")
    demand(not any(path.exists() for path in base.output_paths(output, False)), "Existing outputs will not be overwritten")
    if not args._worker:
        command = [sys.executable, "-B", str(Path(__file__).resolve()), "--recipe", str(recipe_path),
                   "--render", "--draw-ack", args.draw_ack, "--output-prefix", str(output),
                   "--cpu", str(cpu), "--_worker"]
        if args.disable_textures:
            command.append("--disable-textures")
        del meshes, textures, camera
        try:
            return subprocess.run(command, env=dict(os.environ), timeout=base.WALL_SECONDS, check=False).returncode
        except subprocess.TimeoutExpired:
            print("Renderer child stopped at the 90-second wall-time ceiling", file=sys.stderr)
            return 124
    try:
        result = render(meshes, textures, camera, plan, output, np)
    except Exception as error:
        result = {"status": "failed", "error": f"{type(error).__name__}: {error}",
                  "interpretation": "No successful visual result claimed; partial PNG may exist."}
    usage = resource.getrusage(resource.RUSAGE_SELF)
    result.update({"schema": "textured49h.render-report.v1", "plan": plan,
                   "runtime": {"pid": os.getpid(), "python": sys.version, "numpy": np.__version__,
                               "cpu_affinity": sorted(os.sched_getaffinity(0)), "software_environment": base.SOFTWARE_ENV,
                               "maximum_resident_set_bytes": usage.ru_maxrss * 1024,
                               "user_cpu_seconds": usage.ru_utime, "system_cpu_seconds": usage.ru_stime,
                               "validation_and_render_wall_seconds": time.monotonic() - began}})
    report_path = output.parent / (output.name + ".json")
    base.write_json_new(report_path, result)
    print(json.dumps({"status": result["status"], "report": str(report_path), "plan_sha256": plan["plan_sha256"],
                      "images": result.get("images", []), "runtime": result["runtime"]}, indent=2, sort_keys=True))
    return 0 if result["status"] == "rendered_diagnostic_only" else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, AssertionError, KeyError, TypeError) as error:
        print(f"textured49h: {type(error).__name__}: {error}", file=sys.stderr)
        sys.exit(1)
