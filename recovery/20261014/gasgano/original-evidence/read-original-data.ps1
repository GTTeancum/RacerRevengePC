# Static data inspection only. No extracted assets, project modules or converters.
param([switch]$Supplemental)
$ErrorActionPreference = 'Stop'
$disc = 'D:\Programming\GitHub\RacerRevengePC\work\native\project\disc'
function Hash-Bytes([byte[]]$data) { [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData($data)).ToLowerInvariant() }
function U32([byte[]]$data, [int]$at) { [BitConverter]::ToUInt32($data,$at) }
function U16([byte[]]$data, [int]$at) { [BitConverter]::ToUInt16($data,$at) }
function Read-Original([string]$name, [int]$limit) {
    $path = Join-Path $disc $name
    if ((Get-Item -LiteralPath $path).Length -gt $limit) { throw 'Input bound' }
    return ,[IO.File]::ReadAllBytes($path)
}
function Read-Archive([string]$name, [string]$pattern) {
    $raw = Read-Original $name 6291456
    $table = U32 $raw 16
    if ((U32 $raw 0) -ne 3 -or (U32 $raw 8) -ne 832 -or (U32 $raw 12) -ne 0 -or $table % 2 -ne 0 -or $table -gt 65536) { throw 'RES header' }
    $at = 20 + $table
    $count = U32 $raw $at
    $extent = U32 $raw ($at+4)
    if ($count -gt 2000 -or $extent -gt 262144) { throw 'Index bound' }
    $at += 8
    $end = $at + $extent
    $selected = [Collections.Generic.List[object]]::new()
    for ($i=0; $i -lt $count; $i++) {
        $indexOffset=$at
        $n=U32 $raw $at
        $at+=4
        if ($n -lt 1 -or $n -gt 1024 -or $at+$n+8 -gt $end) { throw 'Name extent' }
        $member=[Text.Encoding]::ASCII.GetString($raw,$at,$n)
        $at+=$n
        $logical=U32 $raw $at
        $size=U32 $raw ($at+4)
        $at+=8
        if ($member.Replace('\','/').ToLowerInvariant() -match $pattern) {
            $selected.Add([ordered]@{name=$member; index_file_offset=$indexOffset; logical_offset=$logical; bytes=$size})
        }
    }
    if ($at -ne $end) { throw 'Index extent' }
    $blocks=[Collections.Generic.List[object]]::new()
    for ($i=0; $i -lt $table/2-1; $i++) {
        $size=U16 $raw (20+2*$i)
        if (!$size -or $at+$size -ge $raw.Length -or $raw[$at+$size] -ne 255) { throw 'Block extent' }
        $blocks.Add(@($at,$size))
        $at+=$size+1
    }
    if ($at -ne $raw.Length -or (U16 $raw (18+$table)) -ne 0) { throw 'RES suffix' }
    foreach ($row in $selected) {
        $lo=[int]$row.logical_offset; $size=[int]$row.bytes
        if ($size -lt 1 -or $size -gt 262144 -or $lo+$size -gt $blocks.Count*24576) { throw 'Member extent' }
        $first=[int][Math]::Floor($lo/24576); $last=[int][Math]::Floor(($lo+$size-1)/24576)
        $combined=[IO.MemoryStream]::new()
        $proof=[Collections.Generic.List[object]]::new()
        for ($i=$first; $i -le $last; $i++) {
            $offset=[int]$blocks[$i][0]; $compressed=[int]$blocks[$i][1]
            $input=[IO.MemoryStream]::new($raw,$offset,$compressed,$false)
            $z=[IO.Compression.ZLibStream]::new($input,[IO.Compression.CompressionMode]::Decompress)
            $chunk=[byte[]]::new(24577); $got=0
            while ($got -lt $chunk.Length) { $n=$z.Read($chunk,$got,$chunk.Length-$got); if (!$n) { break }; $got+=$n }
            $z.Dispose(); $input.Dispose()
            if ($got -ne 24576) { throw 'Inflated block extent' }
            $combined.Write($chunk,0,24576)
            $proof.Add([ordered]@{index=$i; file_offset=$offset; bytes=$compressed})
        }
        $joined=$combined.ToArray(); $combined.Dispose()
        $data=[byte[]]::new($size); [Array]::Copy($joined,($lo%24576),$data,0,$size)
        $row['sha256']=Hash-Bytes $data; $row['compressed_blocks']=$proof.ToArray()
        $member=$row.name.Replace('\','/').ToLowerInvariant()
        if ($member -match '\.(mcf|phy|psm|ini)$') { $row['authored_text']=[Text.Encoding]::ASCII.GetString($data) }
        if ($member.EndsWith('/widgets.dat')) {
            $row['relevant_sections']=@(([Text.Encoding]::ASCII.GetString($data) -split '(?=\[Widget \d+\])') | Where-Object { $_ -match '(?i)(07c\.mcf|07\.psg|07_bkg)' })
        }
        if ($member.EndsWith('.pst')) {
            $kind=U32 $data 4; $mips=U32 $data 8
            if ((U32 $data 0) -ne 1 -or $kind -gt 2 -or $mips -lt 1 -or $mips -gt 7) { throw 'PST layout' }
            $cursor=12; if ($kind -eq 0) { $cursor+=64 }; if ($kind -eq 1) { $cursor+=1024 }
            $levels=[Collections.Generic.List[object]]::new()
            for ($j=0; $j -lt $mips; $j++) {
                $header=$cursor; $height=U32 $data $cursor; $width=U32 $data ($cursor+4); $cursor+=16
                if ($kind -eq 2) { $n=U32 $data $cursor; $cursor+=4 } else { $n=$width*$height; if ($kind -eq 0) { $n/=2 } }
                if ($cursor+$n -gt $size) { throw 'PST payload extent' }
                $payload=[byte[]]::new($n); [Array]::Copy($data,$cursor,$payload,0,$n)
                $levels.Add([ordered]@{level=$j; header_offset=$header; data_offset=$cursor; bytes=$n; width=$width; height=$height; sha256=(Hash-Bytes $payload)})
                $cursor+=$n
            }
            $row['pst']=[ordered]@{version=1; kind=$kind; mips=$levels.ToArray(); end_offset=$cursor}
        }
    }
    return [ordered]@{path=$name; bytes=$raw.Length; sha256=(Hash-Bytes $raw); members=$selected.ToArray()}
}
if ($Supplemental) {
    $extra=@(
        (Read-Archive 'UI/UIBKG.RES' '^ui/textures/07_bkg\.pst$'),
        (Read-Archive 'UI/UIRIDER/UIRIDER.RES' '^ui/textures/(specmap1|podstat|podstat2|notch)\.pst$')
    )
    $clips=foreach ($name in @('AUDIO/SPEECH/UI/GASSEL01.VAG','AUDIO/SPEECH/UI/GASSEL02.VAG','AUDIO/SFX/RACE/GEN2_NGN.VAG','AUDIO/SFX/RACE/AS_BEAM.VAG')) {
        if (Test-Path -LiteralPath (Join-Path $disc $name) -PathType Leaf) {
            $raw=Read-Original $name 16777216
            [ordered]@{path=$name; present=$true; bytes=$raw.Length; sha256=(Hash-Bytes $raw)}
        } else { [ordered]@{path=$name; present=$false} }
    }
    $result=[ordered]@{archives=$extra; exact_authored_audio_paths=@($clips)}
    $result | ConvertTo-Json -Depth 15 | Set-Content -LiteralPath (Join-Path $PSScriptRoot 'supplemental-data.json') -Encoding utf8
    $clips | ConvertTo-Json
    return
}
$elf=Read-Original 'SLUS_202.68' 2972720
if ($elf.Length -ne 2972720 -or (U32 $elf 0) -ne 1179403647) { throw 'ELF identity' }
$phoff=U32 $elf 28; $phsize=U16 $elf 42; $phcount=U16 $elf 44
$segments=[Collections.Generic.List[object]]::new()
for ($i=0; $i -lt $phcount; $i++) {
    $at=$phoff+$i*$phsize
    if ((U32 $elf $at) -eq 1) { $segments.Add([ordered]@{offset=(U32 $elf ($at+4)); virtual=(U32 $elf ($at+8)); file_bytes=(U32 $elf ($at+16))}) }
}
$words=[Collections.Generic.List[object]]::new()
for ($pc=0x1e7c40; $pc -lt 0x1e7e58; $pc+=4) {
    $s=$segments | Where-Object { $pc -ge $_.virtual -and $pc -lt $_.virtual+$_.file_bytes } | Select-Object -First 1
    $at=$s.offset+$pc-$s.virtual
    $words.Add([ordered]@{pc=('{0:x8}' -f $pc); file_offset=$at; word=('{0:x8}' -f (U32 $elf $at))})
}
$archives=@(
    (Read-Archive 'UI/UIRIDER/UIRIDER.RES' '^ui/(textures/07(c?_[a-z0-9]+)\.(pst|psm)|geometry/07\.psg|anim/07c(_[a-z0-9]+)?\.(mcf|psg|vut|mot)|uirider/widgets\.dat)$'),
    (Read-Archive 'UI/UIPODTEX.RES' '^ui/textures/07_[123]\.pst$'),
    (Read-Archive 'UI/PODPHYS.RES' '^pods/pod07/pod07\.phy$'),
    (Read-Archive 'PODS/POD07/POD07.RES' '^(pods/pod07/pod07\.phy|audio/speech/pod07/[^/]+\.ini)$')
)
$audio=foreach ($name in @('AUDIO/UIACCEPT.INI','AUDIO/SPEECH/DAX/INRACE/POD07ANN.INI')) {
    $raw=Read-Original $name 65536
    $row=[ordered]@{path=$name; bytes=$raw.Length; sha256=(Hash-Bytes $raw)}
    if ($name -eq 'AUDIO/UIACCEPT.INI') { $row['relevant_sections']=@(([Text.Encoding]::ASCII.GetString($raw) -split '(?=\[Event\d+\])') | Where-Object { $_ -match 'gassel' }) }
    $row
}
$result=[ordered]@{elf=[ordered]@{path='SLUS_202.68'; bytes=$elf.Length; sha256=(Hash-Bytes $elf); load_segments=$segments.ToArray(); constructor_words=$words.ToArray()}; archives=$archives; audio=@($audio)}
$result | ConvertTo-Json -Depth 15 | Set-Content -LiteralPath (Join-Path $PSScriptRoot 'original-data.json') -Encoding utf8
[pscustomobject]@{path=$result.elf.path; bytes=$result.elf.bytes; sha256=$result.elf.sha256} | ConvertTo-Json
$archives | ForEach-Object { [ordered]@{path=$_.path; bytes=$_.bytes; sha256=$_.sha256; selected_members=$_.members.Count} } | ConvertTo-Json
$audio | ConvertTo-Json -Depth 5
