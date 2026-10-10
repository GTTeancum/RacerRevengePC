# Authenticated Options back event5

Source-only, unbuilt and unexecuted. Exact preimages included.

The original 790-byte UIEVENTS.INI (SHA256 ed897d482c72b158f209c20baa6c4e8270a10cdbb9031315cd143a8666557115) explicitly defines Event5 FileName=SFX\UI\back.vag, IsStereo=0, IsLoop=0. Options controller source requests event5 for Back. This does not infer a mapping from neighboring names.

BACK.VAG:11568 bytes, SHA256 3306a9ef40ab752a26a645f28f96ab61d2a7bb91b7a83c989496bd22e7a24b18. Static source-header read: payload11520 at+0C, sample rate22050 at+10. Only nonzero block flag is1 at block offset11536;719 blocks through the terminal block yield20132 mono frames. No PCM decoder was run. Existing authenticated native decoder is used only by the staged runtime.

Main playMenuEffect allowlist remains1,3,4,7. New playOptionsEffect allowlist1,3,4,5 shares the real effect voices; BACK occupies added effect voice5, raising requiredVoices to6. Host must size its dedicated mixer via requiredVoices, not a hardcoded5. Shared Main fade/music code unchanged. Cold prepare additionally authenticates the BACK source.
