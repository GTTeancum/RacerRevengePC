from pathlib import Path
import hashlib,json,zipfile
ROOT=Path(__file__).resolve().parents[2];OUT=ROOT/'work/local50A/impact-aftermath'
def sha(b):return hashlib.sha256(b).hexdigest()
def main():
    paths=[ROOT/'src/game/impact_aftermath50a.h',ROOT/'src/game/impact_aftermath50a.cpp',ROOT/'tests/native/impact_aftermath50a.cpp']
    paths+=list((ROOT/'tools/gameplay').glob('*50a*'))
    paths += [ROOT/'tools/gameplay'/p for p in ['query_oracle48h.py','query_extract48h.py','spring_fixture48j.py']]
    manifest={p.relative_to(ROOT).as_posix():dict(bytes=p.stat().st_size,sha256=sha(p.read_bytes())) for p in paths}
    (OUT/'source-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    paths+=list(p for p in OUT.rglob('*') if p.is_file())
    archive=ROOT/'work/local50A/ImpactAftermath-Stage150A-SourceEvidence.zip'
    assert not archive.exists(),'Never overwrite an existing evidence archive'
    members={p.relative_to(ROOT).as_posix():p.read_bytes() for p in paths}
    with zipfile.ZipFile(archive,'x',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for name,data in members.items():z.writestr(name,data)
    with zipfile.ZipFile(archive) as z:
        assert set(z.namelist())==set(members)
        for name,data in members.items():assert sha(z.read(name))==sha(data)
        assert z.testzip() is None
    report=dict(archive=str(archive),bytes=archive.stat().st_size,sha256=sha(archive.read_bytes()),members=len(members),member_readback_sha256={n:sha(d) for n,d in members.items()})
    (ROOT/'work/local50A/ImpactAftermath-Stage150A-Archive.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({k:v for k,v in report.items() if k!='member_readback_sha256'}))
if __name__=='__main__':main()

