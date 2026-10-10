from pathlib import Path
import json,hashlib,re
root=Path(__file__).resolve().parents[1]
def js(path):return json.loads((root/path).read_text(encoding='utf-8-sig'))
images=js('evidence/memory47N-20260930-155547-464/images.json')['images']
runs=js('evidence/47N/startup-runs.json');records=[]
for run in runs:
    variant=run['variant'];folder=Path(run['run'])
    assert js(f'evidence/47N/{variant}-startup-state.json')['saved_state_equivalent']
    observed=json.loads((folder/'host-memory.json').read_text(encoding='utf-8-sig'))
    samples=observed['samples'];assert samples
    log=(folder/'native.log').read_text()
    match=re.search(r'\[rr:compact-registry\] pages=(\d+) directory_bytes=(\d+) replacement_bytes=(\d+)',log);assert match
    image=next(i for i in images if Path(i['path']).name==f'racer_{variant}47n_fixed_probe.exe')
    baseline=images[0 if variant=='registry' else 1]
    records.append(dict(variant=variant,run=run['run'],binary_sha256=image['sha256'],image_bytes=image['image_bytes'],baseline_image_bytes=baseline['image_bytes'],image_reduction_bytes=baseline['image_bytes']-image['image_bytes'],
        dense_registry_absent_in_link_map=True,registry_replacement_pages=int(match[1]),registry_directory_bytes=int(match[2]),registry_replacement_bytes=int(match[3]),
        observed_samples=len(samples),maximum_sampled_private_bytes=max(s['private_bytes'] for s in samples),maximum_sampled_working_set_bytes=max(s['working_set_bytes'] for s in samples),os_peak_working_set_bytes=max(s['os_peak_working_set_bytes'] for s in samples),
        startup_state_equal=True))
report=dict(variants=records,stock_xbox_total_budget_bytes=64*1024*1024,stock_xbox_fit_proven=False,
    budget_blocker='Each current Windows image exceeds 64 MiB before adding guest RAM, renderer/device heaps, textures, framebuffers and platform allocations. It cannot be used wholesale as the final Xbox runtime.',
    x86_registry_immutable_bytes=js('build/registry47D/report.json')['x86_compact_total_bytes'],
    caveats=['Image size is linked virtual image extent, not measured residency or Xbox executable size.','Process samples include the Windows PCSX2 renderer, dependencies and allocator behavior; they are not Xbox unified-memory measurements.','No full-race or speed acceptance follows from startup comparison.','Leaf function span audit is only 21,792 bytes; its shared executor/wrapper overhead exceeds this span in the current diagnostic implementation. No native instruction code was removed.'])
(root/'evidence/47N/memory-report.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report,indent=2))
