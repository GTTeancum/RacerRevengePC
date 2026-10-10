# Particle camera continuation

This separately staged pair extends the frozen eight-file Main camera/movement candidate. It does not alter those files or the active repository.

`prepareOriginalUiMainParticleCamera50KG` consumes the existing prepared Main core camera and returns:

* `sourceScreen140`: original screen matrix, before any native API convention conversion;
* `combinedSourceScreen250`: core combined matrix multiplied by that screen matrix;
* `focalPixels1D8`: original focal-distance scalar used to scale particle size.

The matrices use original flat row-vector storage. For position (x,y,z,1), a component is x*M[column] + y*M[4+column] + z*M[8+column] + M[12+column], in source arithmetic order. The particle semantic packet's `sourceDepthColumn` therefore receives elements 2,6,10,14. Its `camera1D8` receives `focalPixels1D8`.

## Static source proof

* Base viewport setup 114920/114960 stores original unsigned viewport width at +1E8 (114988). Main's constructor passes 640x448.
* Core build 115350 writes view at +C0, projection at +80, and their product at +100. Existing `camera_view49f` exposes those numeric outputs.
* Focal scalar 11548C..1154FC computes half FOV, multiplies by exact radians-conversion bits 3C8EFA34, calls recovered tangent 13BCA0, then divides half unsigned viewport width by that tangent. This grouping must not be replaced by the core projection's half of a previously converted FOV.
* PSXCamera creation 236234..236310 sets bounds at 2048 plus/minus half width/height; screen matrix diagonal is half width, negative half height, and source depth-near minus depth-far. Screen translation is source X/Y center and source depth-far.
* Source depth-near is 1. Source depth-far is float bits 4AFFFFDE, or 8388591. These are distinct fields from the projection near/far defaults 1/100000.
* Ordinary update 236DD0 first rebuilds the core matrices, rewrites screen X/Y diagonal from viewport spans and zoom fields +328/+32C, both initialized to 1 for this Main camera. It calls guardband setup, then 236E44..236EBC multiplies combined+100 by screen+140 into +250 using the same original VU instruction pattern as the recovered core multiply.

The new code duplicates only the small private recovered tangent polynomial from the current native `camera_view49f.cpp` because that helper is not exported. It retains the same constants, branch behavior and individual rounding steps. No host `tan` substitution or algebraic shortcut is used.

## Bounded native contract

Only the prepared authenticated Main camera identity and original dimensions are admitted. Dynamic zoom, alternate viewport, live camera updates and debug pose overrides are not represented. Inputs/arithmetic results must be finite and FE_TONEAREST must be active. Failure leaves output unchanged. Strict no-fast-math/no-contraction compilation is required.

This pair does not perform perspective division, screen-origin conversion, particle corner expansion, frustum clipping, depth bucketing, visibility or GPU submission. Those remain explicit downstream particle/render-owner operations. No success result claims that a scene or particle frame ran.

Source inspection checked all offsets, matrix multiplication order, screen initialization/update fields, width conversion domain, tangent grouping and output-field mapping. No build, syntax checker, project parser, importer, tests or gameplay were run. Root owns integration and the authorized central check.

Relevant read-only byte evidence is in the parent `evidence` directory:

* `base-camera-viewport-114920-114AE0.txt`
* `base-camera-build-115350-1156F0.txt`
* `menu-camera-create-236180-236450.txt`
* `menu-camera-update-tail-236A20-236F00.txt`
* `camera-screen-setup-2371D0-237430.txt`
