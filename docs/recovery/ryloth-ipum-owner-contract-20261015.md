# RA crowd kind2 flags0: source contract and concrete bridge gap

Static-only review. No converter, project execution, tests, builds or syntax checks. No runtime candidate is justified yet. Raw crowd.atm and source-extents.json were read/extracted using bounded stdlib archive decompression only, with actual RA.RES SHA1e8e504383da884d1a8436402cbbc2a3ff7cdc9390d19d97b3398c3dd51f84d2 and actual member SHA76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031 verified. This is byte extraction, not image decoding.

## Exact source extents

All offsets below are absolute within214578-byte crowd.atm. Header is revision1/count6/storage0. Every frame is revision1/kind2/count5 with dimensions16,32,64,128,256. No palette exists. Each mip is16-byte dimensional header,4-byte IPUM payload length, then that exact payload including its16-byte ipum header. Detailed source header/payload bounds and compressed SHA256 for all30 mips are recorded in source-extents.json; only source data hashes were generated.

|Frame|PST offset|PST bytes|Loaded mip header|Loaded IPUM offset|Loaded IPUM bytes|
|---|---:|---:|---:|---:|---:|
|0|12|35304|11623|11643|23673|
|1|35316|35486|46945|46965|23837|
|2|70802|35949|82565|82585|24166|
|3|106751|36392|118726|118746|24397|
|4|143143|35949|154906|154926|24166|
|5|179092|35486|190721|190741|23837|

Every flags0 child loads source mip ordinal4 into native slot0,256x256, levelCount1. Frames4/5 have the same loaded compressed payloads as2/1, respectively. Distinct original child allocations/identities must survive that equality; byte deduplication cannot collapse the six-child array.

## Original ingestion and ownership

-10237C stores the real child;10239C with a2=0 calls24E880.24E8A8 preserves flags.24EADC..24EBC8 implements the already proven flags0 final-mip-only traversal. Skipped kind2 levels read their length at24ECAC and seek over that payload at24ECC4.
-24E960/24E96C obtain format and bit depth.29E7D4..D8 maps kind2 to1555;29E714..18 maps it to16bpp.
-24E99C branches directly to24EA40 for kind2, bypassing both palette allocations.24EA40..60 sets resourceF8 bit0 and initializes resourceFC policy with argument10 at257750. Existing indexed flagsF8 bit0=0 cannot be reused.
-24ED7C dispatches kind2 to24F148.24F158 reads the4-byte stored length;24F16C calls2507C0 with actual destination level and original dimensional header.
-2507D4 stores full compressed byte count in level+1C;2507E8 allocates that many bytes with alignment64 via305500;2507F0 publishes allocation to level+18 before failure check. It then records width/height and related descriptor fields. This is allocation and compressed ingestion, not an eager RGBA decode.
-24F198 reads the complete original payload into that allocation.24F1A0..C0 subtracts16 from level+1C, keeps original allocation base in level+00, advances level+18 past the16-byte ipum header. Thus source base/remaining codec span are separate fields. Do not label encoded bytes as indexed texels or already-decoded1555 words.
-24E370 destructor iterates five level bases at24E3A8..DC, frees nonnull base via305728 subject to actual F8 bit1 ownership condition. Palette children are separate204/208 owners and absent for kind2. For F8 bit0,24E438..40 invokes2544F0 with actual global443870, then257820 at24E448 and29C7A0 at24E454. Native renderer drain/retirement obligations must be real; this proof does not fabricate or emulate that global service.

Full words are retained here. Exact later original IPUM submission machinery beyond these ingestion/release calls is deliberately outside this bounded recovery; the existing50FC host decoder remains the proposed native image bridge.

## Existing bridge evidence and missing join

The unchanged tools/assets/decode_source_textures50fc.py already pins this EXACT crowd.atm SHA as an original TA member. Identical bytes are legitimate reusable source evidence, not permission to borrow a TA archive identity. Its parse_resource supports all six frames and all five mips. Its decode_ipu uses native FFmpeg first-frame YUV420P with unchanged scalar conversion, removing only final4-byte B1 sequence terminator. Source RGB16 dither/RGB5 expansion/TEXA255/AEM1 policy remains disclosed and hardware IDCT/framebuffer parity remains unverified.

Existing tests/native/source_texture50fc_index.inc pins only crowd-frame0-mip4.rrtex SHAfcfeb0c00938ab1bbae791a24d705c18989b4b222e9d492ab4593d67dff79e98. No complete six-frame compiled output catalog was found. The source_texture50fc_fixture callback publishes a host image-bank result; it cannot certify canonical texture resource construction.

OriginalUiIpumTextureCache50KG is additionally restricted to compiled Rider archive/member identities and whole-PST offset0. textureFor/provenanceFor explicitly reject nonzero offset. Passing extracted frames as fake archive members, modifying the authoritative source name, or using caller-supplied output pins would violate that owner contract. OriginalStandardTextureLevelView50LA currently has no kind2 compressed-storage discriminator or typed authenticated image lease. Canonical resolver still rejects kind2 and existing aggregate splitter rejects it. Therefore a resource implementation cannot yet use an available authentic complete bridge.

## Smallest future central conversion / owner design

1. Preserve unchanged50FC helper and verify its actual source SHA before import in an explicitly authorized future central conversion. Authenticate RA archive, member, all six PST ranges and all30 mip payloads from source-extents.json. Keep source0..5 identity even for repeated content.
2. Convert only six loaded ordinal4 payloads for the bounded flags0 route, while validating all skipped mip framing/hashes. Record native decoder binary/version, exact command, helper/generator hashes, full RA/ELF/member/frame/mip provenance, YUV420P/RGBA/RRTEX hashes from actual outputs, and hardwarePixelParityVerified=false. A future flags2 consumer would require separate full-mip output coverage. Use a fresh output root, no overwrites, and fail closed absent the generated catalog. Existing frame0 pin may be compared with real output; never invent or assume other output hashes.
3. Add a typed immutable cache lease keyed by authenticated mounted RA archive/member identity plus actual frame PST extent and source mip ordinal. Keep it separate from material/reference ownership. It should return only compiled/authenticated outputs, with exact raw-source equality against the same canonical resolver corpus. No arbitrary image callback or caller expected hashes.
4. Extend actual canonical aggregate parsing/child construction for kind2 with compressed source allocation/extent semantics above, real format1555/16bpp/F8 bit0, no palette, and distinct six child lifetime registrations. Only attach proven pixel leases; own compressed source and lease through resource/renderer retirement. Existing source flags0 ordering and partial-child retention remain authoritative.
5. Admit exact crowd PrelitCompressed pass through genuine existing DefaultMT/model-reference route and existing shared Application clock/prototype selection. This program/material/draw join is separate remaining work. Do not relax ScreenObject guard globally, register346 synthetic clones, or substitute a static frame0 image bank for the aggregate.

No output generation, canonical resource implementation, renderer claim or RA admission is included here. These exact source/typed-lease boundaries are the stopping point for this task.
