import json, sys
from collections import Counter
base = r'D:\Programming\GitHub\RacerRevengePC\work\scratch\racer-arcade\export\a2r\pods\POD%s\manifest.json'
for nn in sys.argv[1:]:
    m = json.load(open(base % nn)); c = m['checks']
    print('== POD%s %s' % (nn, m['arcade']['code']), 'RES', c['res']['bytes'], 'members', c['res']['members'], '/', c['res']['target_members'], 'equal', c['members_equal_target'], 'reread', c['reread_identical'])
    print('   failures', {k: v['error'][:70] for k, v in c['decoder_failures'].items()})
    print('   src', dict(Counter(x['source'] for x in m['members'])), 'facts', c['facts'])
    print('   size', c['size_ft'])
    print('   nodes', m['chariot_nodes'], m['cable_source'], m['notes'])
    for x in m['members']:
        if x['name'].lower().endswith('.pst') and x['source'] != 'D': print('   ', x['name'], x['flags'])
        if x['name'].lower().endswith('.psg') and x['source'] != 'D':
            d = c['decoders'][x['name']]; print('    ', x['name'], d.get('normal_alignment'), d.get('nodes'))
