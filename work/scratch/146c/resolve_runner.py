import re
p=r'D:\Programming\GitHub\RacerRevengePC\.claude\worktrees\agent-a2601093eb9a4eea9\tools\capture\run_windows_takeover.py'
t=open(p,encoding='utf-8',newline='').read()
blocks=list(re.finditer(r'<<<<<<< HEAD\r?\n(.*?)=======\r?\n(.*?)>>>>>>> worktree-agent-a061eb9837b6bc01f\r?\n',t,re.S))
assert len(blocks)==4
out=[];last=0
for i,m in enumerate(blocks):
    ours,theirs=m.group(1),m.group(2)
    if i==0:
        new=theirs+ours
    elif i==1:  # their rider args, minus their placeholder --addon-course; then ours
        k=theirs.index("    parser.add_argument('--addon-course'")
        new=theirs[:k]+ours
    elif i==2:  # their rider checks, minus their addon-course placeholder; then ours
        k=theirs.index("    if args.addon_course is not None:")
        new=theirs[:k]+ours
    else:
        k=theirs.index("    if args.addon_course is not None:")
        new=theirs[:k]+ours
    out.append(t[last:m.start()]+new);last=m.end()
out.append(t[last:])
t=''.join(out)
assert '<<<<<<<' not in t and '>>>>>>>' not in t
open(p,'w',encoding='utf-8',newline='').write(t)
print('ok')
