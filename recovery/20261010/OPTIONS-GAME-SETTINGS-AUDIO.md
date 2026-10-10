# Game Options event2: frozen source-only overlay

Files and exact preimages are under this directory. Active repository is untouched. No builds, tests, syntax checks, project parsers, PCM decoder execution, or playback were run.

Original UIEVENTS.INI (790 bytes, SHA256 ed897d482c72b158f209c20baa6c4e8270a10cdbb9031315cd143a8666557115) explicitly maps Event2 to SFX/UI/advance.vag, IsStereo=0, IsLoop=0. Page4 source defaults branch1F5220 calls reset1F6840 and sync1F6450, then sound2 at1F5248, flush1F5260.

ADVANCE.VAG original: 2864 bytes, SHA256 e49eedb1fdfdc1c1ecf13d2d34f0357a5be77835f5293b295f4811050cd077ec. Retained zero header states payload2816 bytes and rate22050; original terminal flag1 is at offset2833 in block175. Native decoder includes that block then terminates, yielding4900 mono frames. This result was derived from raw header/block flags and reading the decoder source, not executing a decoder.

New playGameOptionsEffect allows only source page4 IDs1,2,3,4,5,7. Existing Main and menu allowlists remain unchanged. ADVANCE owns a separate original-event restart voice. requiredVoices becomes7; central host storage already follows that constant. No settings-volume application or sound-mode readiness is implied.
