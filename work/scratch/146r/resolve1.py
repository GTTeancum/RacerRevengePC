import re
from edlib import WT


def hunks(rel, choose):
    p = WT / rel
    t = open(p, encoding='utf-8', newline='').read()
    rx = re.compile(r'<<<<<<< ours\r?\n(.*?)=======\r?\n(.*?)>>>>>>> theirs\r?\n', re.S)
    i = [0]

    def f(m):
        r = choose(i[0], m.group(1), m.group(2)); i[0] += 1; return r
    t = rx.sub(f, t)
    assert '<<<<<<<' not in t and '>>>>>>>' not in t, rel
    open(p, 'w', encoding='utf-8', newline='').write(t)
    print(rel, 'hunks', i[0])


# image bank header: both fields (ours first, then theirs incl. the limits struct).
hunks('src/assets/original_ui_image_bank50kg.h', lambda i, o, t: o + t)


def bank(i, o, t):
    if i == 0:
        return 'struct Located {OriginalUiMemberView50KG view;std::size_t globalOrdinal=0;bool loose=false;bool addon=false;std::size_t addonArchive=0;};\n'
    if i == 1:
        return o.replace('{out={input.looseMembers[i],i,true};return true;}', '{out={input.looseMembers[i],i,true,false,0};return true;}') + t.replace('{out={view,i,true,a};return true;}', '{out={view,i,false,true,a};return true;}')
    if i == 2:
        return o + t
    if i == 3:
        return t + '''                // PORT EXTENSION (DLC add-on courses): loose package files take namespace 0x554C.
                if(member.loose){
                    if(cel.view.identity)return fail(d,E::provenance);
                    source.identity=0x554C000000000000ull|((member.globalOrdinal+1ull)<<33)|source.pstOffset;
                }
'''
    if i == 4:
        return o + t
    raise AssertionError(i)


hunks('src/assets/original_ui_image_bank50kg.cpp', bank)


def drive(i, o, t):
    if i == 0:
        return o + t
    return '''  // DLC phase 3 (146c, PORT EXTENSION): one dlc_course line per add-on course
  // and the RR_TEST_ADDON_COURSE test selection. No add-on course: silent.
  rr::game::dlc50DL::announceAddonCourses();
''' + t


hunks('tests/native/terrain_interactive_drive50du.cpp', drive)
