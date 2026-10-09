#!/usr/bin/env python3
"""Diagnostic-only native RRMesh48B rendering with supplied native49F matrices.

Default operation is a non-drawing plan. Rendering requires its exact SHA256
acknowledgement. No asset extraction, positioning, auto-fit, material, texture,
light, visibility, portal, animation, or gameplay policy is implemented here.
Only installed Python, NumPy, Pillow, libEGL and libGL are used. No downloads.
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


MAX_MESH_BYTES = 64 * 1024 * 1024 + 16
MAX_VERTICES = 1_000_000
MAX_INDICES = 3_000_000
MAX_DRAWS = 8
MAX_PIXELS = 640 * 480
ADDRESS_SPACE_BYTES = 2 * 1024 * 1024 * 1024
WALL_SECONDS = 90
CPU_SECONDS = 60
SOFTWARE_ENV = {
    "LIBGL_ALWAYS_SOFTWARE": "true", "GALLIUM_DRIVER": "llvmpipe",
    "MESA_SHADER_CACHE_DISABLE": "true", "LP_NUM_THREADS": "1",
    "OMP_NUM_THREADS": "1", "OPENBLAS_NUM_THREADS": "1",
    "MKL_NUM_THREADS": "1", "NUMEXPR_NUM_THREADS": "1",
    "PYTHONDONTWRITEBYTECODE": "1",
}


def sha256(data):
    return hashlib.sha256(data).hexdigest()


def json_bytes(value):
    return json.dumps(value, sort_keys=True, separators=(",", ":"),
                      allow_nan=False).encode("utf-8")


def read_bounded(path, maximum):
    with path.open("rb") as handle:
        blob = handle.read(maximum + 1)
    if len(blob) > maximum:
        raise ValueError(f"Input exceeds {maximum} bytes: {path}")
    return blob


def write_json_new(path, value):
    with path.open("x", encoding="utf-8") as handle:
        json.dump(value, handle, indent=2, sort_keys=True, allow_nan=False)
        handle.write("\n")


def positive_integer(value, name, maximum):
    if type(value) is not int or not 0 < value <= maximum:
        raise ValueError(f"{name} must be an integer from 1 through {maximum}")
    return value


def load_mesh(path, np):
    raw = read_bounded(path, MAX_MESH_BYTES)
    if len(raw) < 16 or raw[:8] != b"RRMESH01":
        raise ValueError(f"RRMesh48B magic/header mismatch: {path}")
    vertices, indices = struct.unpack_from("<II", raw, 8)
    positive_integer(vertices, "vertex count", MAX_VERTICES)
    positive_integer(indices, "index count", MAX_INDICES)
    if indices % 3 or len(raw) != 16 + vertices * 24 + indices * 4:
        raise ValueError(f"RRMesh48B exact file length/triangle count mismatch: {path}")
    dtype = np.dtype([("position", "<f4", (3,)), ("uv", "<f4", (2,)),
                      ("color", "u1", (4,))])
    assert dtype.itemsize == 24
    vertex_data = np.frombuffer(raw, dtype=dtype, count=vertices, offset=16)
    index_data = np.frombuffer(raw, dtype="<u4", count=indices, offset=16 + vertices * 24)
    if (not np.isfinite(vertex_data["position"]).all()
            or not np.isfinite(vertex_data["uv"]).all()):
        raise ValueError(f"RRMesh48B non-finite position or UV: {path}")
    if int(index_data.max()) >= vertices:
        raise ValueError(f"RRMesh48B index outside validated vertex array: {path}")
    return {"raw": raw, "vertex_data": vertex_data, "indices": index_data,
            "summary": {"path": str(path), "sha256": sha256(raw), "file_bytes": len(raw),
                        "vertex_count": vertices, "index_count": indices,
                        "triangle_count": indices // 3,
                        "bounds_min": vertex_data["position"].min(axis=0).astype(float).tolist(),
                        "bounds_max": vertex_data["position"].max(axis=0).astype(float).tolist()}}


def f32(value):
    result = struct.unpack("<f", struct.pack("<f", value))[0]
    if not math.isfinite(result):
        raise ValueError("CPU control calculation exceeded finite float32 range")
    return result


def f32_bits(values):
    return [struct.unpack("<I", struct.pack("<f", float(value)))[0] for value in values]


def row_times_matrix(point, matrix):
    """Separate nearest binary32 product/add steps; not a target FPU claim."""
    result = []
    for col in range(4):
        value = f32(float(point[0]) * float(matrix[col]))
        for row in range(1, 4):
            product = f32(float(point[row]) * float(matrix[row * 4 + col]))
            value = f32(value + product)
        result.append(value)
    return result


def load_camera(path, np):
    raw = read_bounded(path, 128 * 1024)
    value = json.loads(raw)
    if value.get("schema") != "neutral49g.camera.v1":
        raise ValueError("Expected neutral49g.camera.v1 schema")
    camera = {"source": value, "sha256": sha256(raw), "path": str(path)}
    for name in ("view", "projection", "combined"):
        words = value.get(name + "_f32_bits")
        if not isinstance(words, list) or len(words) != 16 or any(
                type(word) is not int or not 0 <= word <= 0xFFFFFFFF for word in words):
            raise ValueError(f"{name}_f32_bits must contain exactly 16 uint32 values")
        matrix = np.asarray(words, dtype="<u4").view("<f4")
        if not np.isfinite(matrix).all():
            raise ValueError(f"Non-finite {name} matrix")
        camera[name] = matrix
    # Schema v1 fixes these conventions. Optional redundant metadata must agree.
    for name, expected in (("matrix_layout", "row_major"),
                           ("vector_convention", "row_vector"),
                           ("clip_depth", "zero_to_one")):
        if name in value and value[name] != expected:
            raise ValueError(f"Camera {name} must be {expected!r}")
    recomposed = []
    for row in range(4):
        recomposed.extend(row_times_matrix(camera["view"][row * 4:row * 4 + 4],
                                           camera["projection"]))
    if f32_bits(recomposed) != value["combined_f32_bits"]:
        raise ValueError("Supplied combined bits differ from separately rounded view*projection")
    return camera


def cpu_control(label, point, combined, np):
    point = [f32(float(component)) for component in point]
    native = row_times_matrix(point, combined)
    # GL_FALSE interpretation: contiguous native rows are GLSL columns. This
    # independent indexing check must agree with row multiplication exactly.
    gl_matrix = combined.reshape((4, 4), order="F")
    column = []
    for row in range(4):
        total = f32(float(gl_matrix[row, 0]) * point[0])
        for col in range(1, 4):
            total = f32(total + f32(float(gl_matrix[row, col]) * point[col]))
        column.append(total)
    if f32_bits(native) != f32_bits(column):
        raise ValueError("CPU row/column upload control failed")
    x, y, z, w = native
    return {"label": label, "world_xyzw": point, "clip_xyzw": native,
            "clip_f32_bits": f32_bits(native), "upload_transpose_control_passed": True,
            "inside_gl_zero_to_one_clip": -w <= x <= w and -w <= y <= w and 0 <= z <= w,
            "ndc_xyz": [x / w, y / w, z / w] if w else None}


def load_plan(recipe_path, cpu, np):
    raw = read_bounded(recipe_path, 128 * 1024)
    recipe = json.loads(raw)
    if recipe.get("schema") != "neutral49g.recipe.v1":
        raise ValueError("Expected neutral49g.recipe.v1 schema")
    unknown = set(recipe) - {"schema", "width", "height", "camera_json", "draws",
                            "wire_overlay", "description"}
    if unknown:
        raise ValueError(f"Unknown recipe keys: {sorted(unknown)}")
    width = positive_integer(recipe.get("width", 640), "width", 640)
    height = positive_integer(recipe.get("height", 480), "height", 480)
    if width * height > MAX_PIXELS:
        raise ValueError("Framebuffer pixel budget exceeded")
    wire = recipe.get("wire_overlay", False)
    if type(wire) is not bool:
        raise ValueError("wire_overlay must be boolean")
    if not isinstance(recipe.get("camera_json"), str) or not recipe["camera_json"]:
        raise ValueError("camera_json path is required")
    resolve = lambda name: (recipe_path.parent / name).resolve(strict=True)
    camera = load_camera(resolve(recipe["camera_json"]), np)
    entries = recipe.get("draws")
    if not isinstance(entries, list) or not 1 <= len(entries) <= MAX_DRAWS:
        raise ValueError(f"Recipe must contain 1 through {MAX_DRAWS} draw batches")
    draws, summaries, labels = [], [], set()
    controls = [cpu_control("homogeneous basis " + str(index), point,
                            camera["combined"], np) for index, point in enumerate(
                                ((0, 0, 0, 1), (1, 0, 0, 1),
                                 (0, 1, 0, 1), (0, 0, 1, 1)))]
    for entry in entries:
        if not isinstance(entry, dict) or set(entry) != {"label", "mesh_path",
                                                        "color_rgba", "provenance"}:
            raise ValueError("Each draw requires only label, mesh_path, color_rgba, provenance")
        if any(not isinstance(entry[key], str) or not entry[key] for key in
               ("label", "mesh_path", "provenance")) or entry["label"] in labels:
            raise ValueError("Draw strings must be nonempty and labels unique")
        labels.add(entry["label"])
        color = entry["color_rgba"]
        if not isinstance(color, list) or len(color) != 4 or any(
                type(item) not in (int, float) or not math.isfinite(item)
                or not 0 <= item <= 1 for item in color) or color[3] != 1:
            raise ValueError("Uniform color requires four finite [0,1] channels with alpha=1")
        draw = load_mesh(resolve(entry["mesh_path"]), np)
        draw.update({"label": entry["label"], "color_rgba": color})
        summary = dict(draw["summary"], label=entry["label"], color_rgba=color,
                       provenance=entry["provenance"],
                       placement="Already supplied in mesh; no renderer model transform")
        summaries.append(summary)
        draws.append(draw)
        positions = draw["vertex_data"]["position"]
        for index in sorted({0, len(positions) // 2, len(positions) - 1}):
            controls.append(cpu_control(entry["label"] + " vertex " + str(index),
                                        [*positions[index], 1.0], camera["combined"], np))
    triangles = sum(item["triangle_count"] for item in summaries)
    mesh_bytes = sum(item["file_bytes"] for item in summaries)
    gpu_bytes = mesh_bytes - 16 * len(draws)
    plan = {
        "schema": "neutral49g.plan.v1", "diagnostic_only": True,
        "recipe_path": str(recipe_path), "recipe_sha256": sha256(raw),
        "renderer_script_sha256": sha256(Path(__file__).read_bytes()),
        "vertex_shader_sha256": sha256(VERTEX_SHADER),
        "fragment_shader_sha256": sha256(FRAGMENT_SHADER),
        "camera": {"path": camera["path"], "sha256": camera["sha256"],
                   "view_f32_bits": camera["source"]["view_f32_bits"],
                   "projection_f32_bits": camera["source"]["projection_f32_bits"],
                   "combined_f32_bits": camera["source"]["combined_f32_bits"],
                   "separate_float32_view_projection_control_passed": True,
                   "matrix_layout": "row_major", "vector_convention": "row_vector",
                   "clip_depth": "zero_to_one"},
        "draws": summaries, "wire_overlay": wire,
        "solid_draw_calls": len(draws), "wire_draw_calls": len(draws) if wire else 0,
        "total_draw_calls": len(draws) * (2 if wire else 1),
        "unique_input_triangles": triangles,
        "submitted_triangles": triangles * (2 if wire else 1),
        "framebuffer": {"width": width, "height": height, "color": "RGBA8",
                        "depth": "DEPTH_COMPONENT24", "samples": 1,
                        "clear_rgba8": [18, 22, 30, 255],
                        "estimated_color_depth_storage_bytes": width * height * 8,
                        "single_readback_bytes": width * height * 4,
                        "png_count": 2 if wire else 1},
        "budget": {"cpu_affinity": [cpu], "LP_NUM_THREADS": 1,
                   "mesh_input_bytes": mesh_bytes, "gl_buffer_bytes": gpu_bytes,
                   "address_space_limit_bytes": ADDRESS_SPACE_BYTES,
                   "cpu_time_limit_seconds": CPU_SECONDS,
                   "wall_time_limit_seconds": WALL_SECONDS,
                   "driver_and_python_overhead": "Additional host diagnostic memory; actual RSS recorded"},
        "cpu_clip_controls": controls,
        "profile": {"unlit": True, "depth_test": "LESS", "textures": False,
                    "lights": False, "blending": False, "culling": False,
                    "authored_materials": False, "visibility_or_portal_policy": False,
                    "auto_fit_or_reposition": False, "all_supplied_triangles": True},
    }
    plan["plan_sha256"] = sha256(json_bytes(plan))
    return draws, camera, plan


class EGLContext:
    """Small ctypes binding; every pointer has an explicit native signature."""

    def __init__(self):
        self.egl = C.CDLL("libEGL.so.1")
        self.gl_library = C.CDLL("libGL.so.1")
        self.display = self.surface = self.context = None
        self.egl_function("eglGetProcAddress", C.c_void_p, C.c_char_p)
        self.egl_function("eglGetError", C.c_uint)
        self.egl_function("eglInitialize", C.c_uint, C.c_void_p,
                          C.POINTER(C.c_int), C.POINTER(C.c_int))
        self.egl_function("eglBindAPI", C.c_uint, C.c_uint)
        self.egl_function("eglChooseConfig", C.c_uint, C.c_void_p,
                          C.POINTER(C.c_int), C.POINTER(C.c_void_p),
                          C.c_int, C.POINTER(C.c_int))
        self.egl_function("eglCreatePbufferSurface", C.c_void_p, C.c_void_p,
                          C.c_void_p, C.POINTER(C.c_int))
        self.egl_function("eglCreateContext", C.c_void_p, C.c_void_p,
                          C.c_void_p, C.c_void_p, C.POINTER(C.c_int))
        self.egl_function("eglMakeCurrent", C.c_uint, C.c_void_p,
                          C.c_void_p, C.c_void_p, C.c_void_p)
        self.egl_function("eglDestroyContext", C.c_uint, C.c_void_p, C.c_void_p)
        self.egl_function("eglDestroySurface", C.c_uint, C.c_void_p, C.c_void_p)
        self.egl_function("eglTerminate", C.c_uint, C.c_void_p)
        self.egl_function("eglQueryString", C.c_char_p, C.c_void_p, C.c_int)
        address = self.egl.eglGetProcAddress(b"eglGetPlatformDisplayEXT")
        if not address:
            raise RuntimeError("EGL_EXT_platform_base is unavailable")
        platform_display = C.CFUNCTYPE(C.c_void_p, C.c_uint, C.c_void_p,
                                      C.POINTER(C.c_int))(address)
        self.display = platform_display(0x31DD, None, None)  # SURFACELESS_MESA
        major, minor = C.c_int(), C.c_int()
        self.check(self.egl.eglInitialize(self.display, C.byref(major),
                                          C.byref(minor)), "eglInitialize")
        self.egl_version = [major.value, minor.value]
        self.check(self.egl.eglBindAPI(0x30A2), "eglBindAPI(OPENGL)")
        attrs = (C.c_int * 15)(0x3033, 1, 0x3040, 8,  # PBUFFER, OPENGL
                              0x3024, 8, 0x3023, 8, 0x3022, 8,
                              0x3021, 8, 0x3025, 24, 0x3038)
        config, count = C.c_void_p(), C.c_int()
        self.check(self.egl.eglChooseConfig(self.display, attrs,
                                           C.byref(config), 1, C.byref(count)),
                   "eglChooseConfig")
        if count.value != 1:
            raise RuntimeError("No EGL pbuffer/OpenGL RGBA8/depth24 configuration")
        dimensions = (C.c_int * 5)(0x3057, 1, 0x3056, 1, 0x3038)
        self.surface = self.egl.eglCreatePbufferSurface(self.display, config,
                                                       dimensions)
        self.check(self.surface, "eglCreatePbufferSurface(1x1)")
        context_attrs = (C.c_int * 7)(0x3098, 4, 0x30FB, 5,
                                     0x30FD, 2, 0x3038)  # 4.5 compatibility
        self.context = self.egl.eglCreateContext(self.display, config, None,
                                                context_attrs)
        self.check(self.context, "eglCreateContext(4.5 compatibility)")
        self.check(self.egl.eglMakeCurrent(self.display, self.surface,
                                          self.surface, self.context),
                   "eglMakeCurrent")
        self.bind_gl()

    def egl_function(self, name, result, *arguments):
        function = getattr(self.egl, name)
        function.restype, function.argtypes = result, arguments
        return function

    def check(self, result, operation):
        if not result:
            raise RuntimeError(f"{operation}: EGL error 0x{self.egl.eglGetError():04x}")

    def gl_function(self, name, result, *arguments):
        address = self.egl.eglGetProcAddress(name.encode("ascii"))
        if not address:
            raise RuntimeError(f"Required GL function unavailable: {name}")
        function = C.CFUNCTYPE(result, *arguments)(address)
        setattr(self, name, function)

    def bind_gl(self):
        u, i, f, d, p = C.c_uint, C.c_int, C.c_float, C.c_double, C.c_void_p
        ip, up = C.POINTER(i), C.POINTER(u)
        signatures = {
            "glGetString": (C.c_char_p, u), "glGetError": (u,),
            "glGetIntegerv": (None, u, ip), "glEnable": (None, u),
            "glDisable": (None, u), "glClipControl": (None, u, u),
            "glViewport": (None, i, i, i, i), "glClearColor": (None, f, f, f, f),
            "glClearDepth": (None, d), "glClear": (None, u),
            "glDepthFunc": (None, u), "glDepthMask": (None, C.c_ubyte),
            "glColorMask": (None, C.c_ubyte, C.c_ubyte, C.c_ubyte, C.c_ubyte),
            "glPolygonMode": (None, u, u), "glPolygonOffset": (None, f, f),
            "glLineWidth": (None, f), "glFinish": (None,),
            "glPixelStorei": (None, u, i), "glReadPixels": (None, i, i, i, i, u, u, p),
            "glGenFramebuffers": (None, i, up), "glBindFramebuffer": (None, u, u),
            "glCheckFramebufferStatus": (u, u),
            "glDeleteFramebuffers": (None, i, up),
            "glGenRenderbuffers": (None, i, up), "glBindRenderbuffer": (None, u, u),
            "glRenderbufferStorage": (None, u, u, i, i),
            "glFramebufferRenderbuffer": (None, u, u, u, u),
            "glDeleteRenderbuffers": (None, i, up),
            "glDrawBuffer": (None, u), "glReadBuffer": (None, u),
            "glCreateShader": (u, u),
            "glShaderSource": (None, u, i, C.POINTER(C.c_char_p), ip),
            "glCompileShader": (None, u), "glGetShaderiv": (None, u, u, ip),
            "glGetShaderInfoLog": (None, u, i, ip, p),
            "glDeleteShader": (None, u), "glCreateProgram": (u,),
            "glAttachShader": (None, u, u), "glLinkProgram": (None, u),
            "glGetProgramiv": (None, u, u, ip),
            "glGetProgramInfoLog": (None, u, i, ip, p),
            "glUseProgram": (None, u), "glDeleteProgram": (None, u),
            "glGetUniformLocation": (i, u, C.c_char_p),
            "glUniformMatrix4fv": (None, i, i, C.c_ubyte, C.POINTER(f)),
            "glGetUniformfv": (None, u, i, C.POINTER(f)),
            "glUniform4f": (None, i, f, f, f, f),
            "glGenBuffers": (None, i, up), "glBindBuffer": (None, u, u),
            "glBufferData": (None, u, C.c_ssize_t, p, u),
            "glDeleteBuffers": (None, i, up),
            "glGenVertexArrays": (None, i, up), "glBindVertexArray": (None, u),
            "glDeleteVertexArrays": (None, i, up),
            "glEnableVertexAttribArray": (None, u),
            "glVertexAttribPointer": (None, u, i, u, C.c_ubyte, i, p),
            "glDrawElements": (None, u, i, u, p),
        }
        for name, signature in signatures.items():
            self.gl_function(name, *signature)

    def errors(self, operation):
        errors = []
        for _ in range(32):
            error = self.glGetError()
            if error == 0:
                break
            errors.append(f"0x{error:04x}")
        if errors:
            raise RuntimeError(f"{operation}: GL errors {errors}")

    def integer(self, enum):
        value = C.c_int()
        self.glGetIntegerv(enum, C.byref(value))
        return value.value

    def info(self):
        def string(enum):
            value = self.glGetString(enum)
            return value.decode("utf-8", errors="replace") if value else None
        return {"egl_version": self.egl_version, "gl_vendor": string(0x1F00),
                "gl_renderer": string(0x1F01), "gl_version": string(0x1F02),
                "glsl_version": string(0x8B8C),
                "max_renderbuffer_size": self.integer(0x84E8),
                "max_texture_size": self.integer(0x0D33)}

    def close(self):
        if self.display:
            self.egl.eglMakeCurrent(self.display, None, None, None)
            if self.context:
                self.egl.eglDestroyContext(self.display, self.context)
            if self.surface:
                self.egl.eglDestroySurface(self.display, self.surface)
            self.egl.eglTerminate(self.display)
        self.display = self.surface = self.context = None


VERTEX_SHADER = b"""#version 450 core
layout(location=0) in vec3 position;
uniform mat4 suppliedCombined;
void main() { gl_Position = suppliedCombined * vec4(position, 1.0); }
"""
FRAGMENT_SHADER = b"""#version 450 core
uniform vec4 solidColor;
layout(location=0) out vec4 color;
void main() { color = solidColor; }
"""


def shader_program(gl):
    shaders = []
    program = 0
    try:
        for kind, source in ((0x8B31, VERTEX_SHADER), (0x8B30, FRAGMENT_SHADER)):
            shader = gl.glCreateShader(kind)
            shaders.append(shader)
            if not shader:
                raise RuntimeError("glCreateShader failed")
            text = C.c_char_p(source)
            length = C.c_int(len(source))
            gl.glShaderSource(shader, 1, C.byref(text), C.byref(length))
            gl.glCompileShader(shader)
            ok = C.c_int()
            gl.glGetShaderiv(shader, 0x8B81, C.byref(ok))
            if not ok.value:
                log = C.create_string_buffer(8192)
                gl.glGetShaderInfoLog(shader, len(log), None, log)
                raise RuntimeError(f"Shader compilation failed: {log.value!r}")
        program = gl.glCreateProgram()
        if not program:
            raise RuntimeError("glCreateProgram failed")
        for shader in shaders:
            gl.glAttachShader(program, shader)
        gl.glLinkProgram(program)
        ok = C.c_int()
        gl.glGetProgramiv(program, 0x8B82, C.byref(ok))
        if not ok.value:
            log = C.create_string_buffer(8192)
            gl.glGetProgramInfoLog(program, len(log), None, log)
            raise RuntimeError(f"Shader link failed: {log.value!r}")
        gl.errors("shader compilation/link")
        return program
    except Exception:
        if program:
            gl.glDeleteProgram(program)
        raise
    finally:
        for shader in shaders:
            gl.glDeleteShader(shader)


def render(draws, camera, plan, output, np):
    """Caller must validate the acknowledged plan before reaching this function."""
    from PIL import Image, PngImagePlugin

    start = time.monotonic()
    gl = None
    program = 0
    framebuffer = C.c_uint()
    renderbuffers = (C.c_uint * 2)()
    objects = []
    checks = []
    files = []
    actual_draws = 0
    actual_triangles = 0
    width, height = plan["framebuffer"]["width"], plan["framebuffer"]["height"]

    def check(label):
        gl.errors(label)
        checks.append(label)

    def save_image(suffix):
        nonlocal files
        gl.glPixelStorei(0x0D05, 1)  # PACK_ALIGNMENT
        pixels = np.empty((height, width, 4), dtype=np.uint8)
        gl.glReadPixels(0, 0, width, height, 0x1908, 0x1401,
                        C.c_void_p(pixels.ctypes.data))
        check("bounded RGBA8 readback" + suffix)
        # GL lower-left -> conventional PNG upper-left; no geometry change.
        pixels = np.ascontiguousarray(pixels[::-1])
        path = output.parent / (output.name + suffix + ".png")
        metadata = PngImagePlugin.PngInfo()
        metadata.add_text("Title", "Diagnostic neutral native geometry, not gameplay")
        metadata.add_text("Description", "Supplied native49F camera; uniform unlit "
                          "colors; no materials, textures, lights, culling, portal "
                          "or visibility policy. " + ("Wire overlay companion."
                          if suffix else "Solid geometry pass."))
        metadata.add_text("Plan SHA256", plan["plan_sha256"])
        with path.open("xb") as handle:
            Image.fromarray(pixels).save(handle, format="PNG", pnginfo=metadata)
        raw = read_bounded(path, MAX_PIXELS * 8 + 65536)
        files.append({"path": str(path), "bytes": len(raw), "sha256": sha256(raw),
                      "kind": "wire_overlay_diagnostic" if suffix else "solid_diagnostic"})
        background = np.asarray(plan["framebuffer"]["clear_rgba8"], dtype=np.uint8)
        ys, xs = np.nonzero(np.any(pixels != background, axis=2))
        return {"non_background_pixels": int(len(xs)),
                "non_background_bbox_inclusive":
                    [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())]
                    if len(xs) else None}

    try:
        gl = EGLContext()
        info = gl.info()
        check("context information")
        if "llvmpipe" not in (info["gl_renderer"] or "").lower():
            raise RuntimeError("Expected explicitly forced llvmpipe software renderer")
        if max(width, height) > info["max_renderbuffer_size"]:
            raise ValueError("Planned framebuffer exceeds actual GL renderbuffer limit")
        gl.glGenFramebuffers(1, C.byref(framebuffer))
        gl.glBindFramebuffer(0x8D40, framebuffer.value)
        gl.glGenRenderbuffers(2, renderbuffers)
        for index, internal, attachment in ((0, 0x8058, 0x8CE0),
                                             (1, 0x81A6, 0x8D00)):
            gl.glBindRenderbuffer(0x8D41, renderbuffers[index])
            gl.glRenderbufferStorage(0x8D41, internal, width, height)
            gl.glFramebufferRenderbuffer(0x8D40, attachment, 0x8D41,
                                          renderbuffers[index])
        gl.glDrawBuffer(0x8CE0)
        gl.glReadBuffer(0x8CE0)
        status = gl.glCheckFramebufferStatus(0x8D40)
        if status != 0x8CD5:
            raise RuntimeError(f"Framebuffer incomplete: 0x{status:04x}")
        check("RGBA8/depth24 framebuffer complete")
        gl.glClipControl(0x8CA1, 0x935F)  # LOWER_LEFT, ZERO_TO_ONE
        if gl.integer(0x935C) != 0x8CA1 or gl.integer(0x935D) != 0x935F:
            raise RuntimeError("GL clip origin/depth mode did not match supplied camera")
        gl.glViewport(0, 0, width, height)
        for flag in (0x0B44, 0x0BE2, 0x0B50, 0x0DE1, 0x0BC0, 0x0BD0,
                     0x809D, 0x8DB9, 0x0C11, 0x864F, 0x8C89):
            # Culling, blend, legacy light/texture/alpha, dither, multisample,
            # sRGB conversion, scissor, depth clamp, rasterizer discard.
            gl.glDisable(flag)
        gl.glEnable(0x0B71)  # DEPTH_TEST
        gl.glDepthFunc(0x0201)  # LESS
        gl.glDepthMask(1)
        gl.glColorMask(1, 1, 1, 1)
        gl.glPolygonMode(0x0408, 0x1B02)  # FRONT_AND_BACK, FILL
        gl.glClearColor(*(value / 255.0 for value in plan["framebuffer"]["clear_rgba8"]))
        gl.glClearDepth(1.0)
        gl.glClear(0x00004000 | 0x00000100)
        check("explicit neutral state and Z0..1 clip control")
        program = shader_program(gl)
        gl.glUseProgram(program)
        matrix_location = gl.glGetUniformLocation(program, b"suppliedCombined")
        color_location = gl.glGetUniformLocation(program, b"solidColor")
        if matrix_location < 0 or color_location < 0:
            raise RuntimeError("Required shader uniform was not active")
        combined = camera["combined"]
        # Input flat rows of native M become columns of GLSL transpose(M).
        # Thus GLSL transpose(M) * column(p) equals native row(p) * M.
        # No numerical transpose, recomposition, remapping or fitting occurs.
        gl.glUniformMatrix4fv(matrix_location, 1, 0,
                              combined.ctypes.data_as(C.POINTER(C.c_float)))
        returned = np.empty(16, dtype=np.float32)
        gl.glGetUniformfv(program, matrix_location,
                          returned.ctypes.data_as(C.POINTER(C.c_float)))
        if not np.array_equal(returned.view(np.uint32), combined.view(np.uint32)):
            raise RuntimeError("Matrix uniform round-trip changed supplied float32 bits")
        check("matrix uniform exact-bit round-trip")
        for draw in draws:
            vao, buffers = C.c_uint(), (C.c_uint * 2)()
            objects.append((vao, buffers))
            gl.glGenVertexArrays(1, C.byref(vao))
            gl.glBindVertexArray(vao.value)
            gl.glGenBuffers(2, buffers)
            gl.glBindBuffer(0x8892, buffers[0])
            gl.glBufferData(0x8892, draw["vertex_data"].nbytes,
                            C.c_void_p(draw["vertex_data"].ctypes.data), 0x88E4)
            gl.glBindBuffer(0x8893, buffers[1])
            gl.glBufferData(0x8893, draw["indices"].nbytes,
                            C.c_void_p(draw["indices"].ctypes.data), 0x88E4)
            gl.glVertexAttribPointer(0, 3, 0x1406, 0, 24, None)
            gl.glEnableVertexAttribArray(0)
            check("validated vertex/index upload " + draw["label"])
        prepared = time.monotonic()
        for draw, (vao, _) in zip(draws, objects):
            gl.glBindVertexArray(vao.value)
            gl.glUniform4f(color_location, *draw["color_rgba"])
            gl.glDrawElements(0x0004, len(draw["indices"]), 0x1405, None)
            actual_draws += 1
            actual_triangles += len(draw["indices"]) // 3
            check("solid triangle draw " + draw["label"])
        gl.glFinish()
        check("solid draw finish")
        solid_end = time.monotonic()
        coverage = {"solid": save_image("")}
        if plan["wire_overlay"]:
            gl.glPolygonMode(0x0408, 0x1B01)  # LINE
            gl.glEnable(0x2A02)  # POLYGON_OFFSET_LINE
            gl.glPolygonOffset(-1.0, -1.0)
            gl.glDepthFunc(0x0203)  # LEQUAL, explicit diagnostic overlay only
            gl.glDepthMask(0)
            gl.glLineWidth(1.0)
            gl.glUniform4f(color_location, 1.0, 1.0, 1.0, 1.0)
            for draw, (vao, _) in zip(draws, objects):
                gl.glBindVertexArray(vao.value)
                gl.glDrawElements(0x0004, len(draw["indices"]), 0x1405, None)
                actual_draws += 1
                actual_triangles += len(draw["indices"]) // 3
                check("diagnostic wire overlay draw " + draw["label"])
            gl.glFinish()
            check("wire draw finish")
            coverage["wire_overlay"] = save_image("-wire")
        if actual_draws != plan["total_draw_calls"] or actual_triangles != plan["submitted_triangles"]:
            raise RuntimeError("Actual draw accounting differs from acknowledged plan")
        return {"status": "rendered_diagnostic_only", "renderer": info,
                "actual_draw_calls": actual_draws,
                "actual_submitted_triangles": actual_triangles,
                "uniform_bits_round_trip": True,
                "matrix_convention": "native row-major row-vector M uploaded GL_FALSE "
                    "becomes GLSL transpose(M), then transpose(M) * column(position,1)",
                "clip_control": "GL_LOWER_LEFT / GL_ZERO_TO_ONE",
                "gl_error_checks_passed": checks, "images": files, "coverage": coverage,
                "timing_seconds": {"context_shader_buffer_setup": prepared - start,
                                   "solid_draws_and_finish": solid_end - prepared,
                                   "render_function_including_png": time.monotonic() - start},
                "interpretation": "Host diagnostic elapsed times only. No gameplay FPS, "
                    "real-time performance, Xbox performance or target-memory claim."}
    finally:
        if gl:
            gl.glUseProgram(0)
            gl.glBindVertexArray(0)
            for vao, buffers in objects:
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


def output_paths(prefix, wire):
    suffixes = [".png", ".json"] + (["-wire.png"] if wire else [])
    return [prefix.parent / (prefix.name + suffix) for suffix in suffixes]


def self_test(np):
    """CPU-only, valid allocated arrays, no EGL or GL calls and no source assets."""
    import tempfile

    checks = []
    identity = [1.0 if row == col else 0.0 for row in range(4) for col in range(4)]
    matrix = np.asarray([1, 2, 3, 4, 5, 6, 7, 8,
                         9, 10, 11, 12, 13, 14, 15, 16], dtype=np.float32)
    control = cpu_control("non-symmetric transpose control", [1, 2, 3, 1], matrix, np)
    assert control["clip_xyzw"] == [51.0, 58.0, 65.0, 72.0]
    checks.append("non-symmetric row-vector versus GL column upload indexing")
    with tempfile.TemporaryDirectory(prefix="neutral49g-cpu-") as directory:
        root = Path(directory)
        vertices = b"".join(struct.pack("<5f4B", *point, 0, 0, 255, 255, 255, 255)
                            for point in ((0, 0, 0.5), (1, 0, 0.5), (0, 1, 0.5)))
        mesh_raw = b"RRMESH01" + struct.pack("<II", 3, 3) + vertices + struct.pack("<3I", 0, 1, 2)
        mesh_path = root / "synthetic.rrmesh"
        mesh_path.write_bytes(mesh_raw)
        mesh = load_mesh(mesh_path, np)
        assert mesh["summary"]["triangle_count"] == 1
        assert mesh["vertex_data"].nbytes == 72 and mesh["indices"].nbytes == 12
        checks.append("exact RRMesh48B 24-byte layout and triangle accounting")
        mutations = [("truncated", mesh_raw[:-1]),
                     ("extra_byte", mesh_raw + b"\0"),
                     ("out_of_range_index", mesh_raw[:-4] + struct.pack("<I", 3)),
                     ("nonfinite_position", mesh_raw[:16] + struct.pack("<I", 0x7F800000) + mesh_raw[20:])]
        for label, raw in mutations:
            mesh_path.write_bytes(raw)
            try:
                load_mesh(mesh_path, np)
            except ValueError:
                checks.append("reject " + label)
            else:
                raise AssertionError("Parser accepted " + label)
        mesh_path.write_bytes(mesh_raw)
        camera_path = root / "camera.json"
        camera_value = {"schema": "neutral49g.camera.v1",
                        **{name + "_f32_bits": f32_bits(identity)
                           for name in ("view", "projection", "combined")}}
        camera_path.write_bytes(json_bytes(camera_value))
        recipe_path = root / "recipe.json"
        recipe_path.write_bytes(json_bytes({
            "schema": "neutral49g.recipe.v1", "width": 640, "height": 480,
            "camera_json": "camera.json", "wire_overlay": False,
            "draws": [{"label": "synthetic", "mesh_path": "synthetic.rrmesh",
                       "color_rgba": [1, 0, 0, 1], "provenance": "CPU-only synthetic test"}]}))
        _, _, plan1 = load_plan(recipe_path, min(os.sched_getaffinity(0)), np)
        _, _, plan2 = load_plan(recipe_path, min(os.sched_getaffinity(0)), np)
        assert plan1 == plan2 and plan1["total_draw_calls"] == 1
        assert plan1["submitted_triangles"] == 1
        checks.append("deterministic non-drawing plan and fingerprint")
        camera_value["combined_f32_bits"][0] = 0
        camera_path.write_bytes(json_bytes(camera_value))
        try:
            load_camera(camera_path, np)
        except ValueError:
            checks.append("reject inconsistent camera combined matrix")
        else:
            raise AssertionError("Parser accepted inconsistent camera matrices")
    return {"status": "passed", "tests": checks, "draw_calls": 0,
            "egl_or_gl_context_created": False, "source_assets_read": False}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--recipe", type=Path)
    operation = parser.add_mutually_exclusive_group()
    operation.add_argument("--plan", action="store_true", help="Validate/print plan; never initialize GL (default)")
    operation.add_argument("--render", action="store_true", help="Render the exact acknowledged plan in a bounded child")
    operation.add_argument("--self-test", action="store_true", help="CPU-only parser/matrix controls; no GL calls")
    parser.add_argument("--plan-output", type=Path, help="Write new JSON plan, without overwriting")
    parser.add_argument("--output-prefix", type=Path, help="New PNG/report basename for rendering")
    parser.add_argument("--draw-ack", help="Exact plan_sha256 from a previous non-drawing --plan")
    parser.add_argument("--cpu", type=int, help="One CPU in current allowed affinity set; default is lowest")
    parser.add_argument("--_worker", action="store_true", help=argparse.SUPPRESS)
    args = parser.parse_args()
    os.environ.update(SOFTWARE_ENV)
    allowed = os.sched_getaffinity(0)
    cpu = min(allowed) if args.cpu is None else args.cpu
    if cpu not in allowed:
        parser.error("--cpu must be in the current allowed CPU affinity set")
    if args._worker:
        if not args.render:
            parser.error("Internal worker is only valid for an acknowledged render")
        os.sched_setaffinity(0, {cpu})
        resource.setrlimit(resource.RLIMIT_AS, (ADDRESS_SPACE_BYTES, ADDRESS_SPACE_BYTES))
        resource.setrlimit(resource.RLIMIT_CPU, (CPU_SECONDS, CPU_SECONDS + 1))
        file_limit = MAX_PIXELS * 8 + 65536
        resource.setrlimit(resource.RLIMIT_FSIZE, (file_limit, file_limit))
    import numpy as np

    if sys.byteorder != "little":
        raise RuntimeError("This diagnostic binding requires little-endian host float32/index arrays")
    if args.self_test:
        print(json.dumps(self_test(np), indent=2))
        return 0
    if args.recipe is None:
        parser.error("--recipe is required unless --self-test is selected")
    began = time.monotonic()
    recipe_path = args.recipe.resolve(strict=True)
    draws, camera, plan = load_plan(recipe_path, cpu, np)
    if not args.render:
        if args.draw_ack or args.output_prefix:
            parser.error("--draw-ack and --output-prefix require --render")
        if args.plan_output:
            write_json_new(args.plan_output, plan)
        print(json.dumps(plan, indent=2, sort_keys=True, allow_nan=False))
        return 0
    if args.plan_output:
        parser.error("Use --plan-output with a non-drawing plan only")
    if args.draw_ack != plan["plan_sha256"]:
        parser.error("Rendering blocked: --draw-ack must match the current non-drawing plan_sha256")
    if args.output_prefix is None:
        parser.error("--output-prefix is required for rendering")
    output = args.output_prefix.resolve()
    if not output.parent.is_dir():
        parser.error("Output parent directory must already exist")
    if any(path.exists() for path in output_paths(output, plan["wire_overlay"])):
        parser.error("Rendering will not overwrite an existing PNG/report")
    if not args._worker:
        # Parent waits only. All Mesa work executes in the one-CPU bounded child.
        command = [sys.executable, "-B", str(Path(__file__).resolve()),
                   "--recipe", str(recipe_path), "--render", "--draw-ack", args.draw_ack,
                   "--output-prefix", str(output), "--cpu", str(cpu), "--_worker"]
        del draws, camera
        try:
            result = subprocess.run(command, env=dict(os.environ), timeout=WALL_SECONDS,
                                    check=False)
        except subprocess.TimeoutExpired:
            print("Renderer child stopped at the 90-second wall-time ceiling", file=sys.stderr)
            return 124
        return result.returncode
    try:
        result = render(draws, camera, plan, output, np)
    except Exception as error:
        result = {"status": "failed", "error": f"{type(error).__name__}: {error}",
                  "interpretation": "No successful visual result claimed. Partial PNGs may exist."}
    usage = resource.getrusage(resource.RUSAGE_SELF)
    result.update({"schema": "neutral49g.render-report.v1", "plan": plan,
                   "runtime": {"pid": os.getpid(), "python": sys.version,
                               "numpy": np.__version__,
                               "cpu_affinity": sorted(os.sched_getaffinity(0)),
                               "software_environment": SOFTWARE_ENV,
                               "maximum_resident_set_bytes": usage.ru_maxrss * 1024,
                               "user_cpu_seconds": usage.ru_utime,
                               "system_cpu_seconds": usage.ru_stime,
                               "validation_and_render_wall_seconds": time.monotonic() - began}})
    report_path = output.parent / (output.name + ".json")
    write_json_new(report_path, result)
    print(json.dumps({"status": result["status"], "report": str(report_path),
                      "plan_sha256": plan["plan_sha256"],
                      "images": result.get("images", []),
                      "runtime": result["runtime"]}, indent=2, sort_keys=True))
    return 0 if result["status"] == "rendered_diagnostic_only" else 1


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, AssertionError) as error:
        print(f"neutral49g: {type(error).__name__}: {error}", file=sys.stderr)
        sys.exit(1)
