# Native UI destination texture publication

Source-only, untested. Added after the 17:22 UTC centralized Main observation.

`VehicleWindowGl50DS::bindOriginalUiTextures` allows the real frontend owner to
publish a different authenticated immutable CPU/GPU bank between submissions.
It requires an initialized original-UI renderer on its creating thread and
current context, no active frame, and matching texture counts and every source
identity. The actual GPU publication token is retained. Future draws still
must match that exact bank/token/context; a destination cannot impersonate
Main's textures by reusing local indices.

Atlas alpha maxima are first collected in bounded temporary storage. A rejected
candidate leaves the previous bank, alpha bounds and committed image unchanged.
The operation performs no upload, draw, swap, frame publication, source UI
transition, race update, allocation or GL completion barrier. Caller owns the
new bank lifetime and must retain prior GPU owners until submitted work has
completed (or keep them through renderer close). Existing explicit completion
and close contracts are unchanged.

This is a host resource mechanism only. No destination is admitted by calling
it. Main-to-Options/Rider routing still requires complete original screen,
scene, lifecycle, audio and application ownership. No runtime caller is enabled
in this change; existing Main capture uses the original cold bank.
