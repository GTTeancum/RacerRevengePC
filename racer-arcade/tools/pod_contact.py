# Contact sheet of the pod exporter previews: per kind, 3/4 view of LOD0 pod, LOD1 pod, cockpit+pilot,
# pilot motion (front). python -I tools/pod_contact.py out.png [CODE ...]
import os, sys
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import racer_assets as R
pv = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'export', 'pods', 'preview')
codes = sys.argv[2:] or R.CODES
cols = [('pod_lod0', 2), ('pod_lod1', 1), ('cockpit_pilot', 2), ('pilot_motion', 0)]
T = 220
out = Image.new('RGB', (60 + T * len(cols), T * len(codes)), (15, 15, 15)); d = ImageDraw.Draw(out)
for r, c in enumerate(codes):
    d.text((4, r * T + T // 2), c, fill=(255, 255, 0))
    for k, (name, tile) in enumerate(cols):
        p = os.path.join(pv, '%s_%s.png' % (c, name))
        if os.path.exists(p):
            im = Image.open(p).crop((tile * 360, 20, tile * 360 + 360, 380)).resize((T, T))
            out.paste(im, (60 + k * T, r * T))
out.save(sys.argv[1])
