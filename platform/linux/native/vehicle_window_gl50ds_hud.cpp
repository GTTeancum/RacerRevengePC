#include "vehicle_window_gl50ds_storage.h"

namespace rr::platform {
namespace vehicle_window_gl50ds_detail {
constexpr std::uint64_t hudGlyph(U a, U b, U c, U d, U e, U f, U g) noexcept {
    return std::uint64_t(a) | (std::uint64_t(b) << 5) | (std::uint64_t(c) << 10) |
        (std::uint64_t(d) << 15) | (std::uint64_t(e) << 20) |
        (std::uint64_t(f) << 25) | (std::uint64_t(g) << 30);
}

std::uint64_t hostHudGlyph(char letter) noexcept {
    if (letter >= 'a' && letter <= 'z') letter = char(letter - 'a' + 'A');
    switch (letter) {
    case 'A': return hudGlyph(14,17,17,31,17,17,17);
    case 'B': return hudGlyph(30,17,17,30,17,17,30);
    case 'C': return hudGlyph(14,17,16,16,16,17,14);
    case 'F': return hudGlyph(31,16,16,30,16,16,16);
    case '/': return hudGlyph(1,1,2,4,8,16,16);
    case ':': return hudGlyph(0,4,4,0,4,4,0);
    case '.': return hudGlyph(0,0,0,0,0,4,4);
    case ';': return hudGlyph(0,4,4,0,4,4,8);
    case ',': return hudGlyph(0,0,0,0,4,4,8);
    case '-': return hudGlyph(0,0,0,31,0,0,0);
    case '+': return hudGlyph(0,4,4,31,4,4,0);
    case '!': return hudGlyph(4,4,4,4,4,0,4);
    case 39: return hudGlyph(4,4,8,0,0,0,0); // apostrophe
    case 'D': return hudGlyph(30,17,17,17,17,17,30);
    case 'E': return hudGlyph(31,16,16,30,16,16,31);
    case 'G': return hudGlyph(14,17,16,23,17,17,14);
    case 'H': return hudGlyph(17,17,17,31,17,17,17);
    case 'I': return hudGlyph(31,4,4,4,4,4,31);
    case 'J': return hudGlyph(7,2,2,2,18,18,12);
    case 'K': return hudGlyph(17,18,20,24,20,18,17);
    case 'L': return hudGlyph(16,16,16,16,16,16,31);
    case 'M': return hudGlyph(17,27,21,21,17,17,17);
    case 'N': return hudGlyph(17,25,25,21,19,19,17);
    case 'O': return hudGlyph(14,17,17,17,17,17,14);
    case 'P': return hudGlyph(30,17,17,30,16,16,16);
    case 'Q': return hudGlyph(14,17,17,17,21,18,13);
    case 'R': return hudGlyph(30,17,17,30,20,18,17);
    case 'S': return hudGlyph(15,16,16,14,1,1,30);
    case 'T': return hudGlyph(31,4,4,4,4,4,4);
    case 'U': return hudGlyph(17,17,17,17,17,17,14);
    case 'V': return hudGlyph(17,17,17,17,17,10,4);
    case 'W': return hudGlyph(17,17,17,21,21,21,10);
    case 'X': return hudGlyph(17,17,10,4,10,17,17);
    case 'Y': return hudGlyph(17,17,10,4,4,4,4);
    case 'Z': return hudGlyph(31,1,2,4,8,16,31);
    case '(': return hudGlyph(2,4,8,8,8,4,2);
    case ')': return hudGlyph(8,4,2,2,2,4,8);
    case '0': return hudGlyph(14,17,17,17,17,17,14);
    case '1': return hudGlyph(4,12,4,4,4,4,14);
    case '2': return hudGlyph(14,17,1,2,4,8,31);
    case '3': return hudGlyph(30,1,1,14,1,1,30);
    case '4': return hudGlyph(18,18,18,31,2,2,2);
    case '5': return hudGlyph(31,16,16,30,1,1,30);
    case '6': return hudGlyph(14,16,16,30,17,17,14);
    case '7': return hudGlyph(31,1,2,4,8,8,8);
    case '8': return hudGlyph(14,17,17,14,17,17,14);
    case '9': return hudGlyph(14,17,17,15,1,1,14);
    default: return 0; // The only other admitted character is a space.
    }
}

bool validHostHudName(const char (&name)[maxVehicleWindowRacerName50DS + 1]) noexcept {
    bool visible = false;
    for (U i = 0; i <= maxVehicleWindowRacerName50DS; ++i) {
        const char letter = name[i];
        if (!letter) return i != 0 && visible;
        if (i == maxVehicleWindowRacerName50DS) return false;
        if (letter == ' ') continue;
        if (!((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z') ||
              (letter >= '0' && letter <= '9') || letter == '(' || letter == ')')) return false;
        visible = true;
    }
    return false;
}

void HostHudGeometry::quad(const float (&positions)[4][2], U rgb) noexcept {
        if (count + 4 > hostHudMaximumQuads * 4) { overflow = true; return; }
        for (U i = 0; i < 4; ++i) {
            auto& vertex = vertices[count++];
            vertex.position[0] = (8.f + positions[i][0]) * scale * inverseWidth - 1.f;
            vertex.position[1] = (64.f - positions[i][1]) * scale * inverseHeight - 1.f;
            vertex.position[2] = vertex.uv[0] = vertex.uv[1] = 0.f;
            vertex.color[0] = static_cast<std::uint8_t>(rgb >> 16);
            vertex.color[1] = static_cast<std::uint8_t>(rgb >> 8);
            vertex.color[2] = static_cast<std::uint8_t>(rgb);
            vertex.color[3] = 255;
        }
    }

void HostHudGeometry::rectangle(float x, float y, float w, float h, U rgb) noexcept {
        if (w <= 0.f || h <= 0.f) return;
        const float positions[4][2] = {{x,y+h}, {x+w,y+h}, {x+w,y}, {x,y}};
        quad(positions, rgb);
    }

void HostHudGeometry::text(float x, float y, const char* label, U rgb) noexcept {
        for (; *label; ++label, x += 6.f) {
            const auto glyph = hostHudGlyph(*label);
            for (U row = 0; row < 7; ++row) {
                const U pixels = static_cast<U>((glyph >> (row * 5)) & 31);
                for (U column = 0; column < 5;) {
                    if (!(pixels & (16u >> column))) { ++column; continue; }
                    const U first = column++;
                    while (column < 5 && (pixels & (16u >> column))) ++column;
                    rectangle(x + static_cast<float>(first), y + static_cast<float>(row),
                        static_cast<float>(column - first), 1.f, rgb);
                }
            }
        }
    }

void HostHudGeometry::bar(float y, float value, U rgb) noexcept {
        rectangle(44.f, y - 1.f, 110.f, 9.f, 0x697A8B);
        rectangle(45.f, y, 108.f, 7.f, 0x1D2935);
        rectangle(45.f, y, 108.f * value, 7.f, rgb);
    }

bool HostHudGeometry::buildMenu(const VehicleWindowHostMenu50DS& input, I width, I height) noexcept {
        count = 0; overflow = false;
        constexpr float panelWidth = 312.f;
        const float panelHeight = 62.f + 15.f * float(input.rowCount);
        scale = std::min(2.f, std::min(float(width) / (panelWidth + 24.f),
            float(height) / (panelHeight + 24.f)));
        inverseWidth = 2.f / float(width); inverseHeight = 2.f / float(height);
        const float x = (float(width) / scale - panelWidth) * .5f - 8.f;
        const float y = 64.f - (float(height) / scale + panelHeight) * .5f;
        rectangle(x, y, panelWidth, panelHeight, 0x617589);
        rectangle(x + 1.f, y + 1.f, panelWidth - 2.f, panelHeight - 2.f, 0x0A111B);
        text(x + 12.f, y + 10.f, input.title, 0x5DDFFF);
        text(x + 12.f, y + 23.f, input.subtitle, 0xA8C5DE);
        for (U i = 0; i < input.rowCount; ++i) {
            const float rowY = y + 41.f + 15.f * float(i);
            const bool selected = i == input.selectedRow;
            if (selected) rectangle(x + 7.f, rowY - 3.f, panelWidth - 14.f, 13.f, 0x173547);
            text(x + 12.f, rowY, input.rows[i], selected ? 0x5DDFFF : 0xEDF3F8);
        }
        text(x + 12.f, y + panelHeight - 12.f, input.footer, 0xA8C5DE);
        return !overflow;
    }

bool HostHudGeometry::buildResults(const VehicleWindowHostHud50DS& input, I width, I height) noexcept {
        count = 0; overflow = false;
        constexpr float panelWidth = 380.f;
        const float timingY = 79.f + 11.f * float(input.standingsCount);
        const float actionY = timingY + (input.completedLapTimesValid ?
            27.f + 11.f * float(input.completedLapTimeCount) : 20.f);
        const float panelHeight = actionY + 70.f;
        scale = std::min(2.f, std::min(float(width) / (panelWidth + 24.f),
            float(height) / (panelHeight + 24.f)));
        inverseWidth = 2.f / float(width); inverseHeight = 2.f / float(height);
        const float x = (float(width) / scale - panelWidth) * .5f - 8.f;
        const float y = 64.f - (float(height) / scale + panelHeight) * .5f;
        rectangle(x, y, panelWidth, panelHeight, 0x617589);
        rectangle(x + 1.f, y + 1.f, panelWidth - 2.f, panelHeight - 2.f, 0x0A111B);
        text(x + 12.f, y + 10.f, "RACE RESULTS", 0x5DDFFF);
        for (U i = 0; i < input.standingsCount; ++i)
            if (input.standings[i].racerIndex == input.standingsPlayerIndex)
                text(x + 12.f, y + 24.f, input.standings[i].displayName, 0xEDF3F8);
        if (input.raceFinished) {
            char place[32]{}, finish[40]{};
            std::snprintf(place, sizeof(place), "PLACE %u / %u", input.resultsFinishPlace, input.raceRacerCount);
            // Explicit sealed finish only. Native CC0 durations below retain
            // their separate provenance; neither elapsed time nor rank can
            // manufacture a finish for a retired, unfinished racer.
            const U finishCentiseconds = static_cast<U>(input.resultsFinishSeconds * 100.0 + .5);
            std::snprintf(finish, sizeof(finish), "FINISH TIME %u:%02u.%02u", finishCentiseconds / 6000u,
                (finishCentiseconds / 100u) % 60u, finishCentiseconds % 100u);
            text(x + 12.f, y + 40.f, place, 0xA8E6B4);
            text(x + 158.f, y + 40.f, finish, 0xA8E6B4);
        } else {
            text(x + 12.f, y + 40.f, "DNF", 0xFFBF55);
            text(x + 158.f, y + 40.f, "NO FINISH TIME", 0xFFBF55);
        }
        text(x + 12.f, y + 58.f, "STANDINGS AT RACE END", 0xA8C5DE);
        text(x + 232.f, y + 58.f, "LAPS", 0xA8C5DE);
        for (U i = 0; i < input.standingsCount; ++i) {
            const auto& row = input.standings[i];
            const float rowY = y + 72.f + 11.f * float(i);
            const bool player = row.racerIndex == input.standingsPlayerIndex;
            if (player) rectangle(x + 7.f, rowY - 2.f, panelWidth - 14.f, 11.f, 0x173547);
            const U color = player ? 0x5DDFFF : 0xEDF3F8;
            char rank[4]{}, laps[12]{};
            std::snprintf(rank, sizeof(rank), "%u", row.place);
            std::snprintf(laps, sizeof(laps), "%u/%u", row.completedLaps, input.raceLapTarget);
            text(x + 12.f, rowY, rank, color);
            text(x + 30.f, rowY, row.displayName, color);
            text(x + 232.f, rowY, laps, color);
            text(x + 300.f, rowY, row.finished ? "FINISHED" : row.retirementCD4 ? "DNF" : "UNFINISHED",
                !row.finished && row.retirementCD4 ? 0xFFBF55 : 0xA8C5DE);
        }
        if (input.completedLapTimesValid) {
            text(x + 12.f, y + timingY, "COMPLETED LAP TIMES", 0xA8C5DE);
            const auto duration = [&](float rowY, float seconds) noexcept {
                const U centiseconds = static_cast<U>(double(seconds) * 100.0 + .5);
                char value[20]{};
                std::snprintf(value, sizeof(value), "%u:%02u.%02u", centiseconds / 6000u,
                    (centiseconds / 100u) % 60u, centiseconds % 100u);
                text(x + 78.f, rowY, value, 0xEDF3F8);
            };
            for (U i = 0; i < input.completedLapTimeCount; ++i) {
                const char label[] = {'L','A','P',' ',char('1' + i),'\0'};
                const float rowY = y + timingY + 13.f + 11.f * float(i);
                text(x + 12.f, rowY, label, 0xEDF3F8);
                duration(rowY, input.completedLapSeconds[i]);
            }
            if (input.completedLapTimeCount) {
                const float bestY = y + timingY + 13.f + 11.f * float(input.completedLapTimeCount);
                text(x + 12.f, bestY, "BEST", 0xA8E6B4);
                duration(bestY, input.bestLapSeconds);
            } else text(x + 12.f, y + timingY + 13.f, "NO COMPLETED LAPS", 0xEDF3F8);
        } else text(x + 12.f, y + timingY, "LAP TIMES UNAVAILABLE", 0xA8C5DE);
        const char* actions[] = {"RESTART RACE", "MAIN MENU", "EXIT"};
        if (input.resultsConfirming) {
            text(x + 12.f, y + actionY, "LEAVE THESE RESULTS", 0xFFBF55);
            const char* choices[] = {"CANCEL", actions[input.resultsSelectedAction]};
            for (U i = 0; i < 2; ++i) {
                const float rowY = y + actionY + 16.f + 16.f * float(i);
                const bool selected = i == unsigned(input.resultsConfirmAccept);
                if (selected) rectangle(x + 7.f, rowY - 3.f, panelWidth - 14.f, 13.f, 0x173547);
                text(x + 12.f, rowY, choices[i], selected ? 0x5DDFFF : 0xEDF3F8);
            }
        } else for (U i = 0; i < 3; ++i) {
            const float rowY = y + actionY + 16.f * float(i);
            const bool selected = i == input.resultsSelectedAction;
            if (selected) rectangle(x + 7.f, rowY - 3.f, panelWidth - 14.f, 13.f, 0x173547);
            text(x + 12.f, rowY, actions[i], selected ? 0x5DDFFF : 0xEDF3F8);
        }
        text(x + 12.f, y + panelHeight - 12.f, input.resultsConfirming ?
            "UP/DOWN SELECT  ENTER CONFIRM  ESC CANCEL" :
            "UP/DOWN SELECT  ENTER CONFIRM  ESC PAUSE", 0xA8C5DE);
        return !overflow;
    }

void HostHudGeometry::standingsPanel(const VehicleWindowHostHud50DS& input, I width, I height,
        float reservedTop,float maximumScale) noexcept {
        if (!input.standingsCount) return;
        // Leave a four-cell gap below the race readout, including optional
        // clocks. Only this panel contracts when the viewport cannot fit it
        // above the bottom heat/damage panel or beside the right route cue.
        const float hudScale = scale;
        const float panelWidth = 222.f, standingsHeight = 22.f + 11.f * float(input.standingsCount);
        const float timingHeight = input.completedLapTimesValid ?
            30.f + 11.f * float(input.completedLapTimeCount) : 0.f;
        const float panelHeight = standingsHeight + timingHeight;
        const float clockHeight = input.raceClocksValid ?
            (input.raceFinished || input.raceRetirementCD4 ? 14.f : 28.f) : 0.f;
        const float top = reservedTop>0.f?reservedTop:(44.f + clockHeight) * hudScale;
        const float availableHeight = float(height) - top - 68.f * hudScale;
        const float availableWidth = float(width) - 68.f * hudScale;
        if (availableHeight <= 0.f || availableWidth <= 0.f) return;
        scale = std::min(maximumScale>0.f?maximumScale:hudScale,
            std::min(availableWidth / panelWidth, availableHeight / panelHeight));
        const float x = 8.f * hudScale / scale - 8.f;
        const float y = 64.f - (float(height) - top) / scale;
        rectangle(x, y, panelWidth, panelHeight, 0x617589);
        rectangle(x + 1.f, y + 1.f, panelWidth - 2.f, panelHeight - 2.f, 0x0A111B);
        text(x + 6.f, y + 5.f, "STANDINGS", 0xA8C5DE);
        text(x + 168.f, y + 5.f, "DONE", 0xA8C5DE);
        for (U i = 0; i < input.standingsCount; ++i) {
            const auto& row = input.standings[i];
            const float rowY = y + 19.f + 11.f * float(i);
            const bool player = row.racerIndex == input.standingsPlayerIndex;
            if (player) rectangle(x + 3.f, rowY - 2.f, panelWidth - 6.f, 11.f, 0x173547);
            const U color = player ? 0x5DDFFF : 0xEDF3F8;
            const char place[2] = {char('0' + row.place), '\0'};
            char completed[4]{};
            std::snprintf(completed, sizeof(completed), "%u", row.completedLaps);
            text(x + 6.f, rowY, place, color);
            text(x + 18.f, rowY, row.displayName, color);
            text(x + 168.f, rowY, completed, color);
            if (row.finished) text(x + 198.f, rowY, "FIN", 0xA8E6B4);
            else if (row.retirementCD4) text(x + 198.f, rowY, "DNF", 0xFFBF55);
        }
        if (input.completedLapTimesValid) {
            const float timingY = y + standingsHeight;
            text(x + 6.f, timingY + 4.f, "COMPLETED LAP TIMES", 0xA8C5DE);
            if (!input.completedLapTimeCount) {
                text(x + 6.f, timingY + 18.f, "NO COMPLETED LAPS", 0xEDF3F8);
            } else {
                const auto duration = [&](float rowY, float seconds, U color) noexcept {
                    // Round binary32 storage back to its published centisecond;
                    // flooring can otherwise drop a displayed hundredth.
                    const U centiseconds = static_cast<U>(double(seconds) * 100.0 + 0.5);
                    char value[20]{};
                    std::snprintf(value, sizeof(value), "%u:%02u.%02u", centiseconds / 6000u,
                        (centiseconds / 100u) % 60u, centiseconds % 100u);
                    text(x + 66.f, rowY, value, color);
                };
                for (U i = 0; i < input.completedLapTimeCount; ++i) {
                    const float rowY = timingY + 18.f + 11.f * float(i);
                    const char label[] = {'L', 'A', 'P', ' ', char('1' + i), '\0'};
                    text(x + 6.f, rowY, label, 0xEDF3F8);
                    duration(rowY, input.completedLapSeconds[i], 0xEDF3F8);
                }
                const float bestY = timingY + 18.f + 11.f * float(input.completedLapTimeCount);
                text(x + 6.f, bestY, "BEST", 0xA8E6B4);
                duration(bestY, input.bestLapSeconds, 0xA8E6B4);
            }
        }
        scale = hudScale;
    }

bool HostHudGeometry::build(const VehicleWindowHostHud50DS& input, I width, I height,
        float admittedOriginalBottom) noexcept {
        if (input.resultsPanelVisible) return buildResults(input, width, height);
        count = 0; overflow = false;
        const float clockHeight = input.raceClocksValid ?
            (input.raceFinished || input.raceRetirementCD4 ? 14.f : 28.f) : 0.f;
        scale = width >= 960 && height >= 540 ? 2.f : 1.f;
        scale = std::min(scale, std::min(float(width) / 176.f,
            float(height) / (input.raceEnabled ? 112.f + clockHeight : input.lapCountValid ? 100.f : 72.f)));
        inverseWidth = 2.f / float(width); inverseHeight = 2.f / float(height);
        rectangle(0.f, 0.f, 160.f, 56.f, 0x617589);
        rectangle(1.f, 1.f, 158.f, 54.f, 0x0A111B);
        text(6.f, 5.f, "POD STATUS", 0xA8C5DE);
        text(104.f, 5.f, input.overheated ? "OVERHEAT" : input.boostActive ? "BOOST" : "IDLE",
            input.overheated ? 0xFF6556 : input.boostActive ? 0x5DDFFF : 0xA8E6B4);
        text(6.f, 19.f, "HEAT", 0xEDF3F8);
        text(6.f, 31.f, "L DMG", 0xEDF3F8);
        text(6.f, 43.f, "R DMG", 0xEDF3F8);
        bar(19.f, input.heat01, input.overheated ? 0xFF6556 : 0xFFBF55);
        bar(31.f, input.leftDamage01, 0xEF7767);
        bar(43.f, input.rightDamage01, 0xEF7767);
        float standingsTop=0.f,standingsScale=0.f;bool omitStandings=false;
        if (input.raceEnabled&&admittedOriginalBottom>0.f) {
            // This bound comes only from this renderer's accepted TimeStats
            // quad copy. Original coordinates and source draw stay untouched.
            const float hudScale=scale;
            const float top=admittedOriginalBottom+8.f*hudScale;
            const float panelHeight=input.standingsCount?
                22.f+11.f*float(input.standingsCount)+(input.completedLapTimesValid?
                    30.f+11.f*float(input.completedLapTimeCount):0.f):0.f;
            const float availableHeight=float(height)-top-68.f*hudScale;
            const float availableWidth=float(width)-68.f*hudScale;
            if(availableHeight<=0.f||availableWidth<=0.f)omitStandings=true;
            else {
                // Fit the compact status and standings together above heat/damage;
                // leave the same right-side clearance used for NEXT.
                scale=std::min(hudScale,std::min(availableWidth/(input.standingsCount?222.f:168.f),
                    availableHeight/(32.f+(input.standingsCount?4.f+panelHeight:0.f))));
                const float x=8.f*hudScale/scale-8.f;
                const float y=64.f-(float(height)-top)/scale;
                rectangle(x,y,168.f,32.f,0x617589);
                rectangle(x+1.f,y+1.f,166.f,30.f,0x0A111B);
                char place[32]{},countdown[32]{};
                std::snprintf(place,sizeof(place),"POS %u/%u",input.racePlace,input.raceRacerCount);
                std::snprintf(countdown,sizeof(countdown),"START IN %u",input.raceCountdown);
                text(x+6.f,y+5.f,place,0xEDF3F8);
                const char* status=input.raceResultsReady?"RESULTS":input.raceFinished?"FINISHED":
                    input.raceRetirementCD4?"DNF":input.raceCountdown?countdown:
                    input.raceGo?"GO":input.wrongWay?"WRONG WAY":"RACING";
                text(x+6.f,y+19.f,status,input.raceFinished||input.raceGo?0xA8E6B4:
                    input.raceRetirementCD4?0xFFBF55:input.wrongWay?0xFF6556:0xA8C5DE);
                standingsTop=top+36.f*scale;standingsScale=scale;
                scale=hudScale;
            }
        } else if (input.raceEnabled) {
            const float y = 72.f - float(height) / scale;
            rectangle(0.f, y, 168.f, 32.f + clockHeight, 0x617589);
            rectangle(1.f, y + 1.f, 166.f, 30.f + clockHeight, 0x0A111B);
            char laps[32]{}, place[32]{}, countdown[32]{};
            std::snprintf(laps, sizeof(laps), "LAP %u/%u", input.raceLap, input.raceLapTarget);
            std::snprintf(place, sizeof(place), "POS %u/%u", input.racePlace, input.raceRacerCount);
            std::snprintf(countdown, sizeof(countdown), "START IN %u", input.raceCountdown);
            text(6.f, y + 5.f, laps, 0xEDF3F8);
            text(90.f, y + 5.f, place, 0xEDF3F8);
            const char* status = input.raceResultsReady ? "RESULTS" : input.raceFinished ? "FINISHED" :
                input.raceRetirementCD4 ? "DNF" : input.raceCountdown ? countdown :
                input.raceGo ? "GO" : input.wrongWay ? "WRONG WAY" : "RACING";
            text(6.f, y + 19.f, status, input.raceFinished || input.raceGo ? 0xA8E6B4 :
                input.raceRetirementCD4 ? 0xFFBF55 : input.wrongWay ? 0xFF6556 : 0xA8C5DE);
            if (input.raceClocksValid) {
                const auto clock = [&](float rowY, float seconds) noexcept {
                    // These live fields are not CC0's quantized completed laps.
                    // Truncate for presentation; never advance a source clock.
                    const U centiseconds = static_cast<U>(double(seconds) * 100.0);
                    char value[20]{};
                    std::snprintf(value, sizeof(value), "%u:%02u.%02u", centiseconds / 6000u,
                        (centiseconds / 100u) % 60u, centiseconds % 100u);
                    text(66.f, rowY, value, 0xEDF3F8);
                };
                text(6.f, y + 33.f, "RACE", 0xA8C5DE);
                clock(y + 33.f, input.raceElapsedSeconds);
                if (!input.raceFinished && !input.raceRetirementCD4) {
                    text(6.f, y + 47.f, "LAP TIME", 0xA8C5DE);
                    clock(y + 47.f, input.currentLapSeconds);
                }
            }
        } else if (input.lapCountValid) {
            // Completed count plus optional caller-supplied direction warning.
            // Same fixed batch, top-left opposite NEXT; at most 143 quads.
            const float y = 72.f - float(height) / scale;
            rectangle(0.f, y, 94.f, 18.f, 0x617589);
            rectangle(1.f, y + 1.f, 92.f, 16.f, 0x0A111B);
            text(6.f, y + 5.f, input.wrongWay ? "WRONG WAY" : "HOST LAPS",
                input.wrongWay ? 0xFF6556 : 0xA8C5DE);
            char digits[4]{};
            U length = 0;
            if (input.completedLaps >= 100)
                digits[length++] = char('0' + input.completedLaps / 100);
            if (input.completedLaps >= 10)
                digits[length++] = char('0' + (input.completedLaps / 10) % 10);
            digits[length] = char('0' + input.completedLaps % 10);
            text(68.f, y + 5.f, digits, 0xEDF3F8);
        }
        if (input.routeValid) {
            // Same screen-space batch. Anchor a 44x60-cell panel eight cells
            // from the top/right; route bearing never alters the scene/camera.
            const float x = float(width) / scale - 60.f;
            const float y = 72.f - float(height) / scale;
            rectangle(x, y, 44.f, 60.f, 0x617589);
            rectangle(x + 1.f, y + 1.f, 42.f, 58.f, 0x0A111B);
            text(x + 10.f, y + 15.f, "NEXT", 0xEDF3F8);
            const float cx = x + 22.f, cy = y + 41.f;
            const float dx = input.routeX, dy = -input.routeY;
            const float px = -dy, py = dx;
            const float shaft[4][2] = {
                {cx - 7.f*dx - 2.f*px, cy - 7.f*dy - 2.f*py},
                {cx + 3.f*dx - 2.f*px, cy + 3.f*dy - 2.f*py},
                {cx + 3.f*dx + 2.f*px, cy + 3.f*dy + 2.f*py},
                {cx - 7.f*dx + 2.f*px, cy - 7.f*dy + 2.f*py}};
            // A repeated corner makes one indexed quad a single arrowhead triangle.
            const float head[4][2] = {
                {cx + 2.f*dx - 7.f*px, cy + 2.f*dy - 7.f*py},
                {cx + 13.f*dx, cy + 13.f*dy},
                {cx + 2.f*dx + 7.f*px, cy + 2.f*dy + 7.f*py},
                {cx + 2.f*dx + 7.f*px, cy + 2.f*dy + 7.f*py}};
            quad(shaft, 0x5DDFFF);
            quad(head, 0x5DDFFF);
        }
        if(!omitStandings)standingsPanel(input, width, height,standingsTop,standingsScale);
        return !overflow;
    }

} // namespace vehicle_window_gl50ds_detail

bool VehicleWindowGl50DS::Storage::compileHostHudShader(U type, const char* source, U& name) noexcept {
        name = shaderGl.createShader(type);
        if (!check("create host HUD shader")) return false;
        if (!name) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero host HUD shader name");
        shaderGl.shaderSource(name, 1, &source, nullptr);
        shaderGl.compileShader(name);
        I compiled = 0;
        shaderGl.getShaderiv(name, 0x8B81, &compiled);
        if (!check("compile host HUD shader")) return false;
        if (compiled) return true;
        shaderGl.getShaderInfoLog(name, static_cast<I>(sizeof(diagnostic->shader.log)),
            nullptr, diagnostic->shader.log);
        diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
        check("read host HUD shader log");
        return fail(*diagnostic, VehicleWindowError50DS::shader, "host HUD shader compilation failed");
    }

bool VehicleWindowGl50DS::Storage::createHostHud() noexcept {
        if (!request.enableHostHud) return true;
        constexpr const char* vertexSource = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec4 rrColor;
out vec4 hudColor;
void main() { gl_Position = vec4(rrPosition, 1.0); hudColor = rrColor; }
)GLSL";
        constexpr const char* fragmentSource = R"GLSL(#version 150 core
in vec4 hudColor;
out vec4 viewerOutput;
void main() { viewerOutput = hudColor; }
)GLSL";
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::vertex;
        if (!compileHostHudShader(0x8B31, vertexSource, hostHudVertexShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::fragment;
        if (!compileHostHudShader(0x8B30, fragmentSource, hostHudFragmentShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::link;
        hostHudProgram = shaderGl.createProgram();
        if (!check("create host HUD program")) return false;
        if (!hostHudProgram)
            return fail(*diagnostic, VehicleWindowError50DS::shader, "zero host HUD program name");
        shaderGl.attachShader(hostHudProgram, hostHudVertexShader);
        shaderGl.attachShader(hostHudProgram, hostHudFragmentShader);
        shaderGl.bindAttribLocation(hostHudProgram, vehicleGlPositionAttribute50CZ, "rrPosition");
        shaderGl.bindAttribLocation(hostHudProgram, vehicleGlColorAttribute50CZ, "rrColor");
        shaderGl.bindFragDataLocation(hostHudProgram, vehicleGlColorOutput50CZ, "viewerOutput");
        shaderGl.linkProgram(hostHudProgram);
        I linked = 0;
        shaderGl.getProgramiv(hostHudProgram, 0x8B82, &linked);
        if (!check("link host HUD program")) return false;
        if (!linked) {
            shaderGl.getProgramInfoLog(hostHudProgram, static_cast<I>(sizeof(diagnostic->shader.log)),
                nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
            check("read host HUD program log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, "host HUD program link failed");
        }
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::none;
        hostHudGeometry.reset(new (std::nothrow) HostHudGeometry);
        if (!hostHudGeometry)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded cold host HUD vertices");
        gl.genBuffers(1, &hostHudVertexBuffer);
        drawGl.bindBuffer(arrayBuffer, hostHudVertexBuffer);
        gl.bufferData(arrayBuffer, sizeof(hostHudGeometry->vertices), nullptr, dynamicDraw);
        if (!check("allocate fixed host HUD vertex buffer")) return false;
        if (!hostHudVertexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero host HUD vertex buffer");
        if (!verifyBuffer(arrayBuffer, sizeof(hostHudGeometry->vertices), dynamicDraw)) return false;
        U indices[hostHudMaximumQuads * 6];
        constexpr U corners[] = {0, 1, 2, 0, 2, 3};
        for (U i = 0; i < hostHudMaximumQuads * 6; ++i)
            indices[i] = (i / 6) * 4 + corners[i % 6];
        gl.genBuffers(1, &hostHudIndexBuffer);
        drawGl.bindBuffer(arrayBuffer, hostHudIndexBuffer);
        gl.bufferData(arrayBuffer, sizeof(indices), indices, staticDraw);
        if (!check("upload immutable host HUD quad indices")) return false;
        if (!hostHudIndexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero host HUD index buffer");
        if (!verifyBuffer(arrayBuffer, sizeof(indices), staticDraw)) return false;
        U uploaded[256];
        for (U i = 0; i < hostHudMaximumQuads * 6; i += 256) {
            const auto bytes = std::min<U>(256, hostHudMaximumQuads * 6 - i) * sizeof(U);
            gl.getBufferSubData(arrayBuffer, std::intptr_t(i) * sizeof(U), bytes, uploaded);
            if (!check("verify host HUD index bytes")) return false;
            if (std::memcmp(indices + i, uploaded, bytes))
                return fail(*diagnostic, VehicleWindowError50DS::indices, "GPU host HUD index contents mismatch");
        }
        gl.genVertexArrays(1, &hostHudVertexArray);
        drawGl.bindVertexArray(hostHudVertexArray);
        drawGl.bindBuffer(arrayBuffer, hostHudVertexBuffer);
        drawGl.bindBuffer(elementBuffer, hostHudIndexBuffer);
        constexpr I sizes[] = {3, 2, 4};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        for (U a = 0; a < 3; ++a) {
            gl.enableVertexAttribArray(a);
            gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
        }
        if (!check("create host HUD Vertex48B layout")) return false;
        if (!hostHudVertexArray)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero host HUD vertex array");
        return verifyLayout(hostHudVertexBuffer, hostHudIndexBuffer);
    }

bool VehicleWindowGl50DS::Storage::drawHostHud() noexcept {
        if (!frameHostHud) return true;
        resetNativeGcLighting();
        // Same pending color target; depth is neither tested nor modified.
        // No texture access, blending, framebuffer change, or heap allocation.
        gl.disable(0x0B71); gl.depthMask(0);
        gl.useProgram(hostHudProgram);
        drawGl.bindVertexArray(hostHudVertexArray);
        drawGl.bindBuffer(arrayBuffer, hostHudVertexBuffer);
        drawGl.bufferSubData(arrayBuffer, 0,
            std::intptr_t(hostHudGeometry->count) * sizeof(assets::Vertex48B), hostHudGeometry->vertices);
        drawGl.bindBuffer(elementBuffer, hostHudIndexBuffer);
        bool success = check("upload bounded live host HUD geometry");
        if (success) {
            const U indices = (hostHudGeometry->count / 4) * 6;
            drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(indices), 0x1405, nullptr, 0);
            success = check("draw host HUD before private publication");
            if (success) {
                diagnostic->hostHudDraws = 1;
                diagnostic->hostHudVertices = hostHudGeometry->count;
                diagnostic->hostHudIndices = indices;
            }
        }
        // Restore the renderer's own state on success and failure. The next
        // begin also establishes this state, including its world matrix.
        gl.enable(0x0B71); gl.depthMask(1);
        gl.useProgram(shader->program());
        drawGl.bindVertexArray(0); drawGl.bindBuffer(arrayBuffer, 0);
        const bool restored = check("restore world rendering state after host HUD");
        return success && restored;
    }

bool VehicleWindowGl50DS::Storage::hasRaceHudResources() const noexcept {
    return raceHud&&(raceHud->target||raceHud->color||raceHud->depth||raceHud->textures.ownsResources());
}

float VehicleWindowGl50DS::Storage::hudMul(float a,float b) noexcept {volatile float v=a*b;return v;}

float VehicleWindowGl50DS::Storage::hudAdd(float a,float b) noexcept {volatile float v=a+b;return v;}

float VehicleWindowGl50DS::Storage::hudSub(float a,float b) noexcept {volatile float v=a-b;return v;}

float VehicleWindowGl50DS::Storage::hudDiv(float a,float b) noexcept {volatile float v=a/b;return v;}

bool VehicleWindowGl50DS::Storage::hudName(const std::uint8_t* p,std::size_t n,const char* expected) noexcept {
    std::size_t i=0;for(;i<n&&expected[i];++i){char c=char(p[i]);
        if(c=='\\')c='/';if(c>='A'&&c<='Z')c=char(c+32);
        if(c!=expected[i])return false;}
    return i==n&&!expected[i];
}

bool VehicleWindowGl50DS::Storage::hudSampler(const assets::OriginalUiNativeMaterial50KG& p,MaterialTextureSampler50EB& s) noexcept {
    if(p.minification!=p.magnification||p.environmentContext||p.depthMethod!=2||!p.depthWrite||
        p.colorScale!=255.f/128.f||p.storedAlphaScale!=255.f/128.f)return false;
    s.minification=s.magnification=p.minification==assets::OriginalUiMaterialFilter50KG::nearest?
        MaterialTextureFilter50EB::nearest:MaterialTextureFilter50EB::linear;
    s.s=p.u==assets::OriginalUiMaterialWrap50KG::repeat?MaterialTextureWrap50EB::repeat:MaterialTextureWrap50EB::clampToEdge;
    s.t=p.v==assets::OriginalUiMaterialWrap50KG::repeat?MaterialTextureWrap50EB::repeat:MaterialTextureWrap50EB::clampToEdge;
    return true;
}

bool VehicleWindowGl50DS::Storage::prepareRaceHudResources(const game::OriginalRaceHud50KG& owner) noexcept {
    if(raceHud||frontendOnly||active||frameSet||!owner.loaded()||owner.viewCount()!=1)
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh single-view original race HUD required");
    raceHud.reset(new(std::nothrow) RaceHud);
    if(!raceHud)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original race HUD storage");
    auto& h=*raceHud;const auto* t=owner.textAt(0,0x12c);const auto* images=owner.images();
    const auto* bank=images?images->textures():nullptr;
    if(!t||!t->viewport||!t->viewport->prepared||!t->viewport->sceneKey||!t->viewport->sessionGeneration||
        t->viewport->x1E0||t->viewport->y1E4||t->viewport->width1E8!=640||t->viewport->height1EC!=448||!bank||bank->textureCount()!=4)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original HUD viewport and four authentic images required");
    h.owner=&owner;h.viewport=t->viewport;h.scene=t->viewport->sceneKey;h.generation=t->viewport->sessionGeneration;
    constexpr const char* names[]={"font/denmarkbold14gleam","font/denmarkbold16ingame","font/denmarkbold30gleam",
        "font/denmarkbold16explosion","font/denmarkbold30explosion","ui/textures/gleam","ui/textures/gleamclear"};
    const auto table=owner.materialRegistry();
    if(!table.identity||!table.data||table.size!=70962)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original HUD material registry unavailable");
    std::array<bool,7> found{};std::size_t cursor=0;U token=0;
    const auto delimiter=[](std::uint8_t c){return c==' '||c=='\r'||c=='\n';};
    while(cursor<table.size){while(cursor<table.size&&delimiter(table.data[cursor]))++cursor;
        if(cursor==table.size)break;const auto start=cursor;
        while(cursor<table.size&&!delimiter(table.data[cursor]))++cursor;
        for(U i=0;i<7;++i)if(hudName(table.data+start,cursor-start,names[i])){
            if(found[i])return fail(*diagnostic,VehicleWindowError50DS::material,"duplicate original HUD registration");
            found[i]=true;h.ranks[i]=token;}
        ++token;
    }
    for(bool value:found)if(!value)return fail(*diagnostic,VehicleWindowError50DS::material,"missing original HUD registration");
    if(token!=3235||!(h.ranks[6]<h.ranks[0]&&h.ranks[0]<h.ranks[2]&&h.ranks[2]<h.ranks[5]))
        return fail(*diagnostic,VehicleWindowError50DS::material,"original clear, glyph and gleam order differs");
    assets::OriginalUiMaterialDiagnostic50KG materialDiagnostic;
    for(U i: {0u,1u,2u}){const auto* m=owner.material(i);const auto* decoded=images->decodedTexture(i);
        if(!m||m->passes.size()!=1||!decoded||
            !assets::prepareOriginalUiNativeMaterial50KG(m->passes[0],*decoded,h.policies[i],materialDiagnostic)||
            h.policies[i].blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha||!hudSampler(h.policies[i],h.samplers[i]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"original HUD font policy rejected");
        const auto& group=m->passes[0].shaderPassGroup;
        if(!hudName(reinterpret_cast<const std::uint8_t*>(group.data()),group.size(),i==1?"none":"gleam"))
            return fail(*diagnostic,VehicleWindowError50DS::material,"original HUD font group differs");
    }
    for(U i:{5u,6u}){const auto* m=owner.material(i);assets::OriginalRaceHudGleamMaterial50KG policy;
        if(!m||m->passes.size()!=1||!assets::prepareOriginalRaceHudGleamMaterial50KG(m->passes[0],*images,3,policy,materialDiagnostic)||
            policy.clearFramebufferOnly!=(i==6)||!hudSampler(policy.raster,h.samplers[3]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"original stored-alpha gleam policy rejected");
        h.policies[i]=policy.raster;
    }
    // The same gleam image has exactly one immutable sampler in both passes.
    const auto& a=h.policies[5];const auto& b=h.policies[6];
    if(a.minification!=b.minification||a.magnification!=b.magnification||a.u!=b.u||a.v!=b.v)
        return fail(*diagnostic,VehicleWindowError50DS::material,"original gleam sampler conflict");
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original HUD source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original HUD extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original HUD source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original HUD extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original HUD requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=4;
    requestTextures.maximumGpuBytes=16u*1024u*1024u;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original HUD images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        raceHudUiCreationFailed=true;raceHudPreparing=true;
        const bool okay=createOriginalUi();raceHudPreparing=false;if(!okay)return false;
        raceHudUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original HUD texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original HUD private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original HUD source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original HUD source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original HUD private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}

bool VehicleWindowGl50DS::Storage::prepareRaceHudFrame(const game::OriginalRaceHud50KG* owner) noexcept {
    if(raceHud){raceHud->frame=false;raceHud->quads=raceHud->packetCount=0;}
    if(!owner)return true;
    if(!raceHud||!raceHud->ready||owner!=raceHud->owner)return false;
    auto& h=*raceHud;const auto* v=h.viewport;
    if(!owner->loaded()||owner->viewCount()!=1||!v||!v->prepared||v->sceneKey!=h.scene||
        v->sessionGeneration!=h.generation||v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
        !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    const auto* images=owner->images();const auto* bank=images?images->textures():nullptr;if(!bank)return false;
    // These four objects are the only joined lap emitters. Never accidentally
    // render countdown, status, numeric-count, KO or an inactive second view.
    constexpr U offsets[]={0x12c,0x130,0x134,0x138,0x13c,0x140,0x144,0x164,0x16c,0x170,0x17c};
    for(U offset:offsets){const auto* t=owner->textAt(0,offset);if(!t||t->viewport!=v)return false;
        if(t->enabled70&&offset!=0x12c&&offset!=0x130&&offset!=0x134&&offset!=0x164)return false;}
    const auto put=[&](float x,float y,float u,float vv,const std::array<std::uint8_t,4>& color,U index){
        if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(u)||!std::isfinite(vv)||color[3]>128)return false;
        auto& vertex=h.vertices[index];vertex.position[0]=x;vertex.position[1]=y;vertex.position[2]=16777214.f;
        vertex.uv[0]=u;vertex.uv[1]=vv;for(U c=0;c<4;++c)vertex.color[c]=color[c];return true;};
    // 2E6960 prepares both embedded ScreenTextures, then enqueues bright and
    // clear. Their real registry leaves drain clear before text before bright.
    for(U offset:{0x184u,0x194u}){const auto* g=owner->gleamAt(0,offset);if(!g||g->viewport!=v)return false;
        if(!g->active11C)continue;
        for(float f:{g->x104,g->y100,g->width108,g->height10C,g->travel110,g->offset114,g->duration120,g->elapsed124,g->angle118})
            if(!std::isfinite(f))return false;
        if(g->width108!=30||!(g->height10C==50||g->height10C==100)||g->offset114!=0||g->duration120!=.75f||
            g->elapsed124<0||g->elapsed124>g->duration120||g->angle118!=.523598730564117431640625f)return false;
        const float ratio=hudDiv(g->elapsed124,g->duration120);
        const float x=hudAdd(g->x104,hudMul(g->travel110,ratio)),y=hudAdd(g->y100,hudMul(g->offset114,ratio));
        const float cx=hudAdd(hudMul(.5f,g->width108),x),cy=hudAdd(hudMul(.5f,g->height10C),y);
        game::VisualVector49R trig{};
        if(!game::detail::rotateVector49U({{0,0,1,0}},{{1,0,0,0}},g->angle118,trig))return false;
        const float cosine=trig.v[0],sine=trig.v[1];
        const float xs[]={x,hudAdd(x,g->width108),hudAdd(x,g->width108),x};
        const float ys[]={y,y,hudAdd(y,g->height10C),hudAdd(y,g->height10C)};
        constexpr U sourceLane[]={0,1,3,2};constexpr float uv[4][2]={{0,0},{1,0},{1,1},{0,1}};
        for(U material:{5u,6u}){const auto first=h.quads;
            if(h.quads>=RaceHud::maximumQuads||h.packetCount>=RaceHud::maximumPackets)return false;
            for(U i=0;i<4;++i){const float dx=hudSub(xs[i],cx),dy=hudSub(ys[i],cy);
                const float px=hudAdd(cx,hudSub(hudMul(dx,cosine),hudMul(dy,sine)));
                const float py=hudAdd(cy,hudAdd(hudMul(dx,sine),hudMul(dy,cosine)));
                const auto& color=material==5?g->brightColor[sourceLane[i]]:g->clearColor[sourceLane[i]];
                // Clear coverage must dominate every later destination-alpha
                // read. Preserve source zero packet RGBA, not a host opacity.
                if(material==6&&color!=std::array<std::uint8_t,4>{{0,0,0,0}})return false;
                if(!put(px,py,uv[i][0],uv[i][1],color,h.quads*4+i))return false;}
            ++h.quads;h.packets[h.packetCount++]={first,1,material};}
    }
    for(U offset:{0x12cu,0x130u,0x134u,0x164u}){const auto& t=*owner->textAt(0,offset);
        if(!t.enabled70||!t.blinkOn80)continue;
        const U font=offset==0x164?2u:0u;
        if(t.fontIndex!=font||t.materialIndex!=font||t.textLength>40||t.text[t.textLength]!=0||t.justification74!=3||
            t.rotation17C!=0||!std::isfinite(t.scale178)||t.scale178<=0||!std::isfinite(t.alpha174)||t.alpha174<0||t.alpha174>255)return false;
        const auto* atlas=bank->texture(font);const auto* f=images->font(font);if(!atlas||!f)return false;
        game::OriginalUiTextInput50KG in{f,&atlas->texture,reinterpret_cast<const std::uint8_t*>(t.text.data()),t.textLength,0,0};
        game::OriginalUiTextDiagnostic50KG d;
        if(!game::layoutOriginalUiText50KG(in,h.glyphs,d))return false;
        I measuredWidth=0,measuredHeight=0;
        if(!game::roundOriginalUiTextMetric50KG(h.glyphs.widthBeforeScaleAndRounding,t.scale178,measuredWidth)||
            !game::roundOriginalUiTextMetric50KG(h.glyphs.firstGlyphHeightBeforeScaleAndRounding,t.scale178,measuredHeight))return false;
        //2E13A0 CVT.W.S follows scaled integer metrics;2E0A30 divides the
        //positioned origin before the genuine CEL half-pixel glyph arithmetic.
        float x=float(t.position[0]),y=float(t.position[1]);
        if(t.textLength){x=hudSub(x,hudMul(float(measuredWidth),.5f));y=hudSub(y,hudMul(float(measuredHeight),.5f));}
        I px=0,py=0;if(!game::roundOriginalUiTextMetric50KG(x,1,px)||!game::roundOriginalUiTextMetric50KG(y,1,py))return false;
        in.localX=hudDiv(float(px),t.scale178);in.localY=hudDiv(float(py),t.scale178);
        if(!game::layoutOriginalUiText50KG(in,h.glyphs,d)||h.quads+h.glyphs.count>RaceHud::maximumQuads)return false;
        if(!h.glyphs.count)continue;
        if(h.packetCount>=RaceHud::maximumPackets)return false;
        const auto color=game::originalUiTextPacketColor50KG(t.color170,static_cast<std::int16_t>(t.alpha174));
        const U first=h.quads;
        for(U q=0;q<h.glyphs.count;++q){for(U i=0;i<4;++i){const auto& vertex=h.glyphs.quads[q].vertices[i];
                if(!put(hudMul(vertex.x,t.scale178),hudMul(vertex.y,t.scale178),vertex.u,vertex.v,color,h.quads*4+i))return false;}
            ++h.quads;}
        h.packets[h.packetCount++]={first,h.quads-first,font};
    }
    // Stable insertion sort on the authenticated registry leaf rank. The three
    // font14 packets retain their actual object FIFO within that shared leaf.
    for(U i=1;i<h.packetCount;++i){auto p=h.packets[i];U j=i;
        while(j&&h.ranks[h.packets[j-1].material]>h.ranks[p.material]){h.packets[j]=h.packets[j-1];--j;}h.packets[j]=p;}
    h.frame=h.quads!=0;return true;
}

bool VehicleWindowGl50DS::Storage::drawRaceHud(U bank) noexcept {
    if(!raceHud||!raceHud->frame)return true;
    auto& h=*raceHud;
    if(!h.ready||!h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,h.target);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,h.target);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // Initial scratch alpha is deliberately irrelevant: every bright sweep's
    // entire raster coverage is first written by its identical clear quad.
    // Glyph source-over never reads Ad. No genuine gameplay alpha is claimed.
    gl.colorMask(1,1,1,1);gl.enable(0x0B71);gl.depthFunc(0x0206);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),float(2*ox/width-1),float(1-2*oy/height));
    gl.scissor(I(std::floor(ox)),I(std::floor(oy)),I(std::ceil(ox+640*scale))-I(std::floor(ox)),
        I(std::ceil(oy+448*scale))-I(std::floor(oy)));gl.enable(0x0C11);
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,1);
    gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiSourceFogEnabledUniform,0);
    gl.uniform1i(uiSourceAlphaTestUniform,0);gl.uniform1f(uiAlphaUniform,1);gl.uniform1i(uiImageUniform,0);
    if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
        if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,h.color);}
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(h.quads)*4*sizeof(assets::Vertex48B),h.vertices.data());
    drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    bool okay=check("prepare authentic lap glyph/gleam source pass");
    constexpr U image[]={0,1,2,1,2,3,3};
    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        // Original clear's failed alpha test keeps framebuffer writes while
        // suppressing depth writes. Its exact zero RGBA leaves RGB untouched
        // and replaces source A with zero through the ordinary source equation.
        // Adjacent packets of one leaf retain the preceding checked material state.
        if(p==0 || packet.material!=h.packets[p-1].material){
            gl.depthMask(packet.material==6?0:1);gl.uniform1i(uiSourceBlendUniform,packet.material==5?3:1);
            gl.bindTexture(texture2D,h.textures.texture(image[packet.material]));
        }
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original HUD source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original HUD source triangle");}
        }
    }
    if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
    gl.bindTexture(texture2D,0);gl.disable(0x0C11);gl.disable(0x0BE2);
    if(okay){
        // This is the sole write to the pending gameplay color target. A blit
        // failure aborts the existing whole-frame transaction before publish.
        gl.bindFramebuffer(readFramebuffer,h.target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy authentic original HUD into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original HUD pass");
    if(okay&&restored){diagnostic->originalRaceHudQuads=h.quads;diagnostic->originalRaceHudLeaves=h.packetCount;}
    return okay&&restored;
}

bool VehicleWindowGl50DS::Storage::deleteRaceHudNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if(!raceHud)return true;auto& h=*raceHud;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    raceHud.reset();return true;
}

bool VehicleWindowGl50DS::Storage::hasTimeStatsResources()const noexcept {
    return timeStats&&(timeStats->target||timeStats->color||timeStats->depth||timeStats->textures.ownsResources());
}

bool VehicleWindowGl50DS::Storage::prepareTimeStatsResources(std::shared_ptr<const game::OriginalTimeStats50LA> owner) noexcept {
    if(timeStats||frontendOnly||active||frameSet||!owner||!owner->nativeCpuResourcesPrepared())
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh original TimeStats cold owner required");
    timeStats.reset(new(std::nothrow) TimeStats);
    if(!timeStats)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original TimeStats storage");
    auto& h=*timeStats;h.owner=std::move(owner);
    const auto* images=h.owner->nativePixels();const auto* bank=images?images->textures():nullptr;
    if(!bank||bank->textureCount()!=2)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original TimeStats two-image bridge required");
    assets::OriginalUiMaterialDiagnostic50KG md;
    for(U i=0;i<2;++i){
        const auto* m=h.owner->nativeMaterial(i);const U image=h.owner->nativeImage(i);
        const auto* decoded=images->decodedTexture(image);
        if(image>=2||!m||m->passes.size()!=1||!decoded||
           !assets::prepareOriginalUiNativeMaterial50KG(m->passes[0],*decoded,h.policies[i],md)||
           h.policies[i].blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha||
           !hudSampler(h.policies[i],h.samplers[image]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original TimeStats source raster");
        // Owner already authenticated exact source identity and canonical map.
        // Keep that authored group; never relabel unique to force admission.
        const auto& group=m->passes[0].shaderPassGroup;
        if(!hudName(reinterpret_cast<const std::uint8_t*>(group.data()),group.size(),i?"none":"unique"))
            return fail(*diagnostic,VehicleWindowError50DS::material,"original TimeStats group differs");
        h.images[i]=image;
    }
    if(h.images[0]==h.images[1])return fail(*diagnostic,VehicleWindowError50DS::assets,"distinct TimeStats source images required");
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original TimeStats source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original TimeStats extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original TimeStats source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original TimeStats extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original TimeStats requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=2;
    requestTextures.maximumGpuBytes=16u*1024u*1024u;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original TimeStats images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        timeStatsUiCreationFailed=true;timeStatsPreparing=true;
        const bool okay=createOriginalUi();timeStatsPreparing=false;if(!okay)return false;
        timeStatsUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original TimeStats texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original TimeStats private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original TimeStats source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original TimeStats source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original TimeStats private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}

bool VehicleWindowGl50DS::Storage::prepareTimeStatsFrame(const game::OriginalTimeStats50LA* owner) noexcept {
    if(timeStats){timeStats->frame=false;timeStats->quads=timeStats->packetCount=0;}
    if(!owner)return true;
    if(!timeStats||!timeStats->ready||owner!=timeStats->owner.get()||!owner->nativeCpuResourcesPrepared())return false;
    auto& h=*timeStats;const auto& source=owner->nativeDraw();const auto* v=source.viewport;
    if(!source.prepared||!source.serial||source.serial<h.serial||!source.cameraPublication||
       !v||!v->prepared||!v->matricesReady||!v->sceneKey||!v->sessionGeneration||
       v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
       (h.viewport&&(h.viewport!=v||h.scene!=v->sceneKey||h.generation!=v->sessionGeneration))||
       source.quadCount>TimeStats::maximumQuads||source.packetCount>TimeStats::maximumPackets||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    std::array<bool,TimeStats::maximumQuads> covered{};
    std::int32_t previousLeaf=-1;
    for(U p=0;p<source.packetCount;++p){const auto& packet=source.packets[p];
        if(packet.material>=2||packet.leaf<0||packet.leaf<previousLeaf||!packet.count||
           packet.first>source.quadCount||packet.count>source.quadCount-packet.first)return false;
        previousLeaf=packet.leaf;
        for(U q=packet.first;q<packet.first+packet.count;++q){
            if(covered[q])return false;covered[q]=true;
            const auto& quad=source.quads[q];
            if(quad.depth!=16777214.f)return false;
            for(U i=0;i<4;++i){const auto& in=quad.vertices[i];const auto& c=quad.packetColors[i];
                if(!std::isfinite(in.x)||!std::isfinite(in.y)||!std::isfinite(in.u)||!std::isfinite(in.v)||
                   c[3]>h.policies[packet.material].maximumVertexAlpha)return false;
                auto& out=h.vertices[q*4+i];out.position[0]=in.x;out.position[1]=in.y;out.position[2]=quad.depth;
                out.uv[0]=in.u;out.uv[1]=in.v;for(U lane=0;lane<4;++lane)out.color[lane]=c[lane];
            }
        }
        h.packets[p]=packet;
    }
    for(U q=0;q<source.quadCount;++q)if(!covered[q])return false;
    h.viewport=v;h.scene=v->sceneKey;h.generation=v->sessionGeneration;h.serial=source.serial;
    h.quads=source.quadCount;h.packetCount=source.packetCount;h.frame=h.quads!=0;return true;
}

bool VehicleWindowGl50DS::Storage::drawTimeStats(U bank) noexcept {
    if(!timeStats||!timeStats->frame)return true;
    auto& h=*timeStats;
    if(!h.ready||!h.owner||!h.owner->nativeCpuResourcesPrepared()||
       !h.owner->nativeDraw().prepared||h.owner->nativeDraw().serial!=h.serial||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,h.target);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,h.target);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // Both authenticated source-alpha equations use As, never Ad.
    // Source alpha remains private; gameplay framebuffer alpha is not claimed.
    gl.colorMask(1,1,1,1);gl.enable(0x0B71);gl.depthFunc(0x0206);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),float(2*ox/width-1),float(1-2*oy/height));
    gl.scissor(I(std::floor(ox)),I(std::floor(oy)),I(std::ceil(ox+640*scale))-I(std::floor(ox)),
        I(std::ceil(oy+448*scale))-I(std::floor(oy)));gl.enable(0x0C11);
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,1);
    gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiSourceFogEnabledUniform,0);
    gl.uniform1i(uiSourceAlphaTestUniform,0);gl.uniform1f(uiAlphaUniform,1);gl.uniform1i(uiImageUniform,0);
    if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
        if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,h.color);}
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(h.quads)*4*sizeof(assets::Vertex48B),h.vertices.data());
    drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    bool okay=check("prepare authentic TimeStats source pass");

    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        // Both actual policies were admitted as source-alpha, GEQUAL/write.
        // Preserve owner packet order; no synthetic registration/rank sorting.
        // Depth/blend policy is constant across this pass's checked packets.
        if(p==0){gl.depthMask(1);gl.uniform1i(uiSourceBlendUniform,1);}
        if(p==0 || packet.material!=h.packets[p-1].material)
            gl.bindTexture(texture2D,h.textures.texture(h.images[packet.material]));
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original TimeStats source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original TimeStats source triangle");}
        }
    }
    if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
    gl.bindTexture(texture2D,0);gl.disable(0x0C11);gl.disable(0x0BE2);
    if(okay){
        // This is the sole write to the pending gameplay color target. A blit
        // failure aborts the existing whole-frame transaction before publish.
        gl.bindFramebuffer(readFramebuffer,h.target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy authentic original TimeStats into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original TimeStats pass");
    if(okay&&restored){diagnostic->originalTimeStatsQuads=h.quads;diagnostic->originalTimeStatsLeaves=h.packetCount;}
    return okay&&restored;
}

bool VehicleWindowGl50DS::Storage::deleteTimeStatsNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if(!timeStats)return true;auto& h=*timeStats;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    // Whole-renderer close or failed-bind cleanup has completed glFinish.
    // Release the strong canonical owner only after acknowledged name retirement.
    timeStats.reset();return true;
}

bool VehicleWindowGl50DS::Storage::hasPositionStatsResources()const noexcept {
    return positionStats&&(positionStats->target||positionStats->color||positionStats->depth||positionStats->textures.ownsResources());
}

bool VehicleWindowGl50DS::Storage::positionStatsSampler(const assets::OriginalUiNativeMaterial50KG& p,
    MaterialTextureSampler50EB& sampler)noexcept {
    if(p.minification!=p.magnification||p.environmentContext||p.depthMethod>3||
       p.colorScale!=255.f/128.f||p.storedAlphaScale!=255.f/128.f||
       !p.requiresSeparateAlphaStore)return false;
    sampler.minification=sampler.magnification=p.minification==assets::OriginalUiMaterialFilter50KG::nearest?
        MaterialTextureFilter50EB::nearest:MaterialTextureFilter50EB::linear;
    sampler.s=p.u==assets::OriginalUiMaterialWrap50KG::repeat?MaterialTextureWrap50EB::repeat:MaterialTextureWrap50EB::clampToEdge;
    sampler.t=p.v==assets::OriginalUiMaterialWrap50KG::repeat?MaterialTextureWrap50EB::repeat:MaterialTextureWrap50EB::clampToEdge;
    return true;
}

bool VehicleWindowGl50DS::Storage::preparePositionStatsResources(std::shared_ptr<const game::OriginalPositionStatsNativeOwner50LA> owner) noexcept {
    if(positionStats||frontendOnly||active||frameSet||!owner||!owner->nativeCpuResourcesPrepared())
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh original PositionStats cold owner required");
    positionStats.reset(new(std::nothrow) PositionStats);
    if(!positionStats)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original PositionStats storage");
    auto& h=*positionStats;h.owner=std::move(owner);
    const auto* images=h.owner->nativePixels();const auto* bank=images?images->textures():nullptr;
    if(!bank||bank->textureCount()!=2)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original PositionStats two-image bridge required");
    assets::OriginalUiMaterialDiagnostic50KG md;
    for(U i=0;i<3;++i){
        const auto* m=h.owner->nativeMaterial(i);const U image=h.owner->nativeImage(i);
        const auto* decoded=images->decodedTexture(image);
        if(image>=2||!m||m->passes.size()!=1||!decoded||
           !assets::prepareOriginalUiNativeMaterial50KG(m->passes[0],*decoded,h.policies[i],md)||
           h.policies[i].blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha||
           !positionStatsSampler(h.policies[i],h.samplers[image]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original PositionStats source raster");
        h.images[i]=image;
    }
    // stats and maptop genuinely share the stats atlas. The owner proves the
    // canonical FINAL pass and map association; identical image references are
    // not distinct allocations. Require consistent actual sampler parameters.
    for(U i=0;i<3;++i)for(U j=0;j<i;++j)if(h.images[i]==h.images[j]){
        MaterialTextureSampler50EB sampler;
        if(!positionStatsSampler(h.policies[j],sampler)||
           sampler.minification!=h.samplers[h.images[i]].minification||
           sampler.magnification!=h.samplers[h.images[i]].magnification||
           sampler.s!=h.samplers[h.images[i]].s||sampler.t!=h.samplers[h.images[i]].t)
            return fail(*diagnostic,VehicleWindowError50DS::material,"PositionStats shared atlas sampler mismatch");
    }
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original PositionStats source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original PositionStats extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original PositionStats source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original PositionStats extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original PositionStats requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=2;
    requestTextures.maximumGpuBytes=16u*1024u*1024u;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original PositionStats images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        positionStatsUiCreationFailed=true;positionStatsPreparing=true;
        const bool okay=createOriginalUi();positionStatsPreparing=false;if(!okay)return false;
        positionStatsUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original PositionStats texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original PositionStats private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original PositionStats source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original PositionStats source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original PositionStats private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}

bool VehicleWindowGl50DS::Storage::preparePositionStatsFrame(const game::OriginalPositionStatsNativeOwner50LA* owner) noexcept {
    if(positionStats){positionStats->frame=false;positionStats->quads=positionStats->packetCount=0;}
    if(!owner)return true;
    if(!positionStats||!positionStats->ready||owner!=positionStats->owner.get()||!owner->nativeCpuResourcesPrepared())return false;
    auto& h=*positionStats;const auto& source=owner->nativeDraw();const auto* v=source.viewport;
    if(!source.prepared||!source.serial||source.serial<h.serial||!source.cameraPublication||
       !v||!v->prepared||!v->matricesReady||!v->sceneKey||!v->sessionGeneration||
       v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
       (h.viewport&&(h.viewport!=v||h.scene!=v->sceneKey||h.generation!=v->sessionGeneration))||
       source.quadCount>PositionStats::maximumQuads||source.packetCount>PositionStats::maximumPackets||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    std::array<bool,PositionStats::maximumQuads> covered{};
    const auto* radar=h.owner->nativeRadarPolicy();
    if(radar){
        if(radar->depthMethod!=1||radar->depthWrite||radar->requiresOriginalDepthOwner||
           radar->blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha||
           radar->colorScale!=1.f||radar->storedAlphaScale!=1.f||
           radar->blendAlphaScale!=255.f/128.f||radar->alphaStoreFromBlendFactor!=128.f/255.f||
           radar->maximumVertexAlpha!=128||radar->environmentContext||!radar->requiresSeparateAlphaStore)return false;
        h.radar=*radar;
    }
    // The source owner supplies canonical ordering, including radar/markers.
    // Do not sort or merge spans by image or force globally ascending leaves.

    for(U p=0;p<source.packetCount;++p){const auto& packet=source.packets[p];
        if(packet.material>=4||packet.leaf<0||(packet.material==3&&!radar)||!packet.count||
           packet.first>source.quadCount||packet.count>source.quadCount-packet.first)return false;
        for(U q=packet.first;q<packet.first+packet.count;++q){
            if(covered[q])return false;covered[q]=true;
            const auto& quad=source.quads[q];
            if(packet.material==3?quad.depth!=0.f:quad.depth!=16777214.f)return false;
            for(U i=0;i<4;++i){const auto& in=quad.vertices[i];const auto& c=quad.packetColors[i];
                if(!std::isfinite(in.x)||!std::isfinite(in.y)||!std::isfinite(in.u)||!std::isfinite(in.v)||
                   c[3]>(packet.material==3?h.radar.maximumVertexAlpha:h.policies[packet.material].maximumVertexAlpha))return false;
                auto& out=h.vertices[q*4+i];out.position[0]=in.x;out.position[1]=in.y;out.position[2]=quad.depth;
                out.uv[0]=in.u;out.uv[1]=in.v;for(U lane=0;lane<4;++lane)out.color[lane]=c[lane];
            }
        }
        h.packets[p]=packet;
    }
    for(U q=0;q<source.quadCount;++q)if(!covered[q])return false;
    h.viewport=v;h.scene=v->sceneKey;h.generation=v->sessionGeneration;h.serial=source.serial;
    h.quads=source.quadCount;h.packetCount=source.packetCount;h.frame=h.quads!=0;return true;
}

bool VehicleWindowGl50DS::Storage::drawPositionStats(U bank) noexcept {
    if(!positionStats||!positionStats->frame)return true;
    auto& h=*positionStats;
    if(!h.ready||!h.owner||!h.owner->nativeCpuResourcesPrepared()||
       !h.owner->nativeDraw().prepared||h.owner->nativeDraw().serial!=h.serial||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,h.target);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,h.target);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // Both authenticated source-alpha equations use As, never Ad.
    // Source alpha remains private; gameplay framebuffer alpha is not claimed.
    gl.colorMask(1,1,1,1);gl.enable(0x0B71);gl.depthFunc(0x0206);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),float(2*ox/width-1),float(1-2*oy/height));
    gl.scissor(I(std::floor(ox)),I(std::floor(oy)),I(std::ceil(ox+640*scale))-I(std::floor(ox)),
        I(std::ceil(oy+448*scale))-I(std::floor(oy)));gl.enable(0x0C11);
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,1);
    gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiSourceFogEnabledUniform,0);
    gl.uniform1i(uiSourceAlphaTestUniform,0);gl.uniform1f(uiAlphaUniform,1);gl.uniform1i(uiImageUniform,0);
    if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
        if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,h.color);}
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(h.quads)*4*sizeof(assets::Vertex48B),h.vertices.data());
    drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    bool okay=check("prepare authentic PositionStats source pass");

    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        // Preserve actual source order and effective per-pass depth. Radar
        // is genuine zero-map ALWAYS/no-write with inert native vertex Z0.
        // Textured screen quads retain original packet depth and state.
        // Material identity includes radar's distinct depth/texturing policy.
        if(p==0 || packet.material!=h.packets[p-1].material){
            const bool radar=packet.material==3;
            const auto depth=radar?h.radar.depthMethod:h.policies[packet.material].depthMethod;
            gl.depthFunc(depth==0?0x0200:depth==1?0x0207:depth==2?0x0206:0x0204);
            gl.depthMask((radar?h.radar.depthWrite:h.policies[packet.material].depthWrite)?1:0);
            // Blend mode stays source-alpha across every checked material transition.
            if(p==0)gl.uniform1i(uiSourceBlendUniform,1);
            gl.uniform1i(uiTexturedUniform,radar?0:1);
            gl.bindTexture(texture2D,radar?0:h.textures.texture(h.images[packet.material]));
        }
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original PositionStats source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original PositionStats source triangle");}
        }
    }
    if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
    gl.bindTexture(texture2D,0);gl.disable(0x0C11);gl.disable(0x0BE2);
    if(okay){
        // This is the sole write to the pending gameplay color target. A blit
        // failure aborts the existing whole-frame transaction before publish.
        gl.bindFramebuffer(readFramebuffer,h.target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy authentic original PositionStats into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original PositionStats pass");
    if(okay&&restored){diagnostic->originalPositionStatsQuads=h.quads;diagnostic->originalPositionStatsLeaves=h.packetCount;}
    return okay&&restored;
}

bool VehicleWindowGl50DS::Storage::deletePositionStatsNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if(!positionStats)return true;auto& h=*positionStats;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    // Whole-renderer close or failed-bind cleanup has completed glFinish.
    // Release the strong canonical owner only after acknowledged name retirement.
    positionStats.reset();return true;
}

bool VehicleWindowGl50DS::Storage::hasDamageResources()const noexcept {
    return damage&&(damage->target||damage->color||damage->depth||damage->textures.ownsResources());
}

bool VehicleWindowGl50DS::Storage::prepareDamageResources(std::shared_ptr<const game::OriginalDamage50LA> owner) noexcept {
    if(damage||frontendOnly||active||frameSet||!owner||!owner->nativeCpuResourcesPrepared())
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh original Damage cold owner required");
    damage.reset(new(std::nothrow) Damage);
    if(!damage)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original Damage storage");
    auto& h=*damage;h.owner=std::move(owner);
    const auto* images=h.owner->nativePixels();const auto* bank=images?images->textures():nullptr;
    if(!bank||bank->textureCount()!=1)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original Damage single-atlas bridge required");
    assets::OriginalUiMaterialDiagnostic50KG md;
    for(U i=0;i<1;++i){
        const auto* m=h.owner->nativeMaterial(i);const U image=h.owner->nativeImage(i);
        const auto* decoded=images->decodedTexture(image);
        if(image!=0||!m||m->passes.size()!=1||!decoded||
           !assets::prepareOriginalUiNativeMaterial50KG(m->passes[0],*decoded,h.policies[i],md)||
           h.policies[i].blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha||
           !h.policies[i].requiresSeparateAlphaStore||
           !hudSampler(h.policies[i],h.samplers[image]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original Damage source raster");
        // The owner proves actual ScreenMT fallback, canonical FINAL leaf,
        // and the dmgradar PST association. No invented PSM/group identity.
        h.images[i]=image;
    }
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original Damage source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original Damage extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original Damage source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original Damage extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original Damage requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=1;
    requestTextures.maximumGpuBytes=16u*1024u*1024u;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original Damage images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        damageUiCreationFailed=true;damagePreparing=true;
        const bool okay=createOriginalUi();damagePreparing=false;if(!okay)return false;
        damageUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original Damage texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original Damage private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original Damage source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original Damage source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original Damage private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}

bool VehicleWindowGl50DS::Storage::prepareDamageFrame(const game::OriginalDamage50LA* owner) noexcept {
    if(damage){damage->frame=false;damage->quads=damage->packetCount=0;}
    if(!owner)return true;
    if(!damage||!damage->ready||owner!=damage->owner.get()||!owner->nativeCpuResourcesPrepared())return false;
    auto& h=*damage;const auto& source=owner->nativeDraw();const auto* v=source.viewport;
    if(!source.prepared||!source.serial||source.serial<h.serial||!source.cameraPublication||
       !v||!v->prepared||!v->matricesReady||!v->sceneKey||!v->sessionGeneration||
       v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
       (h.viewport&&(h.viewport!=v||h.scene!=v->sceneKey||h.generation!=v->sessionGeneration))||
       source.quadCount>Damage::maximumQuads||source.packetCount>Damage::maximumPackets||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    std::array<bool,Damage::maximumQuads> covered{};
    std::int32_t previousLeaf=-1;
    for(U p=0;p<source.packetCount;++p){const auto& packet=source.packets[p];
        if(packet.material!=0||packet.leaf<0||packet.leaf<previousLeaf||!packet.count||
           packet.first>source.quadCount||packet.count>source.quadCount-packet.first)return false;
        previousLeaf=packet.leaf;
        for(U q=packet.first;q<packet.first+packet.count;++q){
            if(covered[q])return false;covered[q]=true;
            const auto& quad=source.quads[q];
            if(quad.depth!=16777214.f)return false;
            for(U i=0;i<4;++i){const auto& in=quad.vertices[i];const auto& c=quad.packetColors[i];
                if(!std::isfinite(in.x)||!std::isfinite(in.y)||!std::isfinite(in.u)||!std::isfinite(in.v)||
                   c[3]>h.policies[packet.material].maximumVertexAlpha)return false;
                auto& out=h.vertices[q*4+i];out.position[0]=in.x;out.position[1]=in.y;out.position[2]=quad.depth;
                out.uv[0]=in.u;out.uv[1]=in.v;for(U lane=0;lane<4;++lane)out.color[lane]=c[lane];
            }
        }
        h.packets[p]=packet;
    }
    for(U q=0;q<source.quadCount;++q)if(!covered[q])return false;
    h.viewport=v;h.scene=v->sceneKey;h.generation=v->sessionGeneration;h.serial=source.serial;h.cameraPublication=source.cameraPublication;
    h.quads=source.quadCount;h.packetCount=source.packetCount;h.frame=h.quads!=0;return true;
}

bool VehicleWindowGl50DS::Storage::drawDamage(U bank) noexcept {
    if(!damage||!damage->frame)return true;
    auto& h=*damage;
    if(!h.ready||!h.owner||!h.owner->nativeCpuResourcesPrepared()||
       !h.owner->nativeDraw().prepared||h.owner->nativeDraw().serial!=h.serial||
       h.owner->nativeDraw().cameraPublication!=h.cameraPublication||h.owner->nativeDraw().viewport!=h.viewport||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,h.target);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,h.target);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // The authenticated source-alpha equation uses As, never Ad.
    // Source alpha remains private; gameplay framebuffer alpha is not claimed.
    gl.colorMask(1,1,1,1);gl.enable(0x0B71);gl.depthFunc(0x0206);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),float(2*ox/width-1),float(1-2*oy/height));
    gl.scissor(I(std::floor(ox)),I(std::floor(oy)),I(std::ceil(ox+640*scale))-I(std::floor(ox)),
        I(std::ceil(oy+448*scale))-I(std::floor(oy)));gl.enable(0x0C11);
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,1);
    gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiSourceFogEnabledUniform,0);
    gl.uniform1i(uiSourceAlphaTestUniform,0);gl.uniform1f(uiAlphaUniform,1);gl.uniform1i(uiImageUniform,0);
    if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
        if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,h.color);}
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(h.quads)*4*sizeof(assets::Vertex48B),h.vertices.data());
    drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    bool okay=check("prepare authentic Damage source pass");

    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        // The actual policy was admitted as source-alpha, GEQUAL/write.
        // Preserve owner packet order; no synthetic registration/rank sorting.
        // A checked adjacent packet with the same material retains this setup.
        if(p==0 || packet.material!=h.packets[p-1].material){
            gl.depthMask(1);gl.uniform1i(uiSourceBlendUniform,1);
            gl.bindTexture(texture2D,h.textures.texture(h.images[packet.material]));
        }
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original Damage source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original Damage source triangle");}
        }
    }
    if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
    gl.bindTexture(texture2D,0);gl.disable(0x0C11);gl.disable(0x0BE2);
    if(okay){
        // This is the sole write to the pending gameplay color target. A blit
        // failure aborts the existing whole-frame transaction before publish.
        gl.bindFramebuffer(readFramebuffer,h.target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy authentic original Damage into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original Damage pass");
    if(okay&&restored){diagnostic->originalDamageQuads=h.quads;diagnostic->originalDamageLeaves=h.packetCount;}
    return okay&&restored;
}

bool VehicleWindowGl50DS::Storage::deleteDamageNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if(!damage)return true;auto& h=*damage;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    // Whole-renderer close or failed-bind cleanup has completed glFinish.
    // Release the strong canonical owner only after acknowledged name retirement.
    damage.reset();return true;
}

bool VehicleWindowGl50DS::Storage::hasPauseDerivedResources()const noexcept {
    return pauseDerived&&(pauseDerived->target||pauseDerived->color||pauseDerived->depth||pauseDerived->textures.ownsResources());
}

bool VehicleWindowGl50DS::Storage::preparePauseDerivedResources(std::shared_ptr<const game::OriginalPauseNativeDraw50LA> owner) noexcept {
    if(pauseDerived||frontendOnly||active||frameSet||!owner||!owner->current())
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh original PauseDerived cold owner required");
    pauseDerived.reset(new(std::nothrow) PauseDerived);
    if(!pauseDerived)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original PauseDerived storage");
    auto& h=*pauseDerived;h.owner=std::move(owner);
    const auto* bank=h.owner->pixels();
    if(!bank||bank->textureCount()>h.samplers.size())
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original Pause derived image bank required");
    for(U i=0;i<h.policies.size();++i){
        const auto* p=h.owner->material(i);h.policies[i]=p;h.images[i]=h.owner->image(i);
        if(!p||!p->enabled)continue;
        const auto blend=p->hasTexture?p->textured.blend:p->untextured.blend;
        const auto depth=p->hasTexture?p->textured.depthMethod:p->untextured.depthMethod;
        const auto context=p->hasTexture?p->textured.environmentContext:p->untextured.environmentContext;
        // The inherited gameplay target does not supply genuine GS destination
        // alpha; never admit a destination-alpha-dependent Pause pass here.
        if(blend==assets::OriginalUiMaterialBlend50KG::destinationAlphaAdditive||depth>3||context)
            return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original Pause derived raster");
        if(p->hasTexture){
            const auto* frames=h.owner->frameImages(i);
            if(!frames||frames->empty()||h.images[i]>=bank->textureCount())
                return fail(*diagnostic,VehicleWindowError50DS::material,"missing original Pause frame bank");
            for(const auto image:*frames)
                if(image>=bank->textureCount()||!positionStatsSampler(p->textured,h.samplers[image]))
                    return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original Pause sampler");
        }
    }
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original PauseDerived source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original PauseDerived extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original PauseDerived source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original PauseDerived extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original PauseDerived requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=bank->textureCount();
    requestTextures.maximumGpuBytes=assets::maxMaterialTextureRGBABytes50EA;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original PauseDerived images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        pauseDerivedUiCreationFailed=true;pauseDerivedPreparing=true;
        const bool okay=createOriginalUi();pauseDerivedPreparing=false;if(!okay)return false;
        pauseDerivedUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original PauseDerived texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original PauseDerived private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original PauseDerived source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original PauseDerived source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original PauseDerived private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}

bool VehicleWindowGl50DS::Storage::pauseDisplayIdentityCurrent()const noexcept {
    const auto& p=pauseDisplayPublication;
    if(!pauseDerived||!pauseDerived->ready||!p.owner||!p.target||
       p.target!=displayedTarget||p.target!=menuTargets[menuPublished]||
       !imageCommitted||p.width!=committedWidth||p.height!=committedHeight||
       p.width!=raceWidth||p.height!=raceHeight||p.width<=0||p.height<=0||
       !retainedRaceDisplayCompleted||retainedRaceContainsPause||
       !p.cameraPublication||p.cameraPublication!=retainedRaceCameraPublication)return false;
    const auto& h=*pauseDerived;
    // These are the prepared draw's actual owner/viewport/material identities,
    // not a later supplied frame or a claim that the Pause remains active.
    // close1BE830 does not replace the already displayed native pixels.
    return h.owner.get()==p.owner&&p.viewport&&h.viewport==p.viewport&&
        p.serial&&h.serial==p.serial&&p.scene&&h.scene==p.scene&&
        p.generation&&h.generation==p.generation&&h.publication==p.texturePublication&&
        h.cameraPublication==p.cameraPublication;
}
void VehicleWindowGl50DS::Storage::stagePauseDisplayPublication(U target,I w,I height)noexcept {
    pauseDisplayPublication={};
    if(!pauseDerived||!pauseDerived->ready||!pauseDerived->owner)return;
    const auto& h=*pauseDerived;
    pauseDisplayPublication={h.owner.get(),h.viewport,target,w,height,h.serial,h.scene,
        h.generation,h.publication,h.cameraPublication,0};
}
void VehicleWindowGl50DS::Storage::completePauseDisplayPublication()noexcept {
    // Called after successful actual swap/headless completion and all existing
    // completion owners. No GL, allocation, user callback or source mutation.
    if(!pauseDisplayIdentityCurrent()||!gameplayHistoryValid||!gameplayHistoryGeneration||
       gameplayHistoryStaged||gameplayAwaitingTarget||
       gameplayHistoryWidth!=committedWidth||gameplayHistoryHeight!=committedHeight){
        pauseDisplayPublication={};return;
    }
    pauseDisplayPublication.completedHistory=gameplayHistoryGeneration;
}
bool VehicleWindowGl50DS::Storage::completedPauseDisplayCurrent()const noexcept {
    const auto& p=pauseDisplayPublication;
    return p.completedHistory&&p.completedHistory==gameplayHistoryGeneration&&
        gameplayHistoryValid&&!gameplayHistoryStaged&&!gameplayAwaitingTarget&&
        gameplayHistoryWidth==p.width&&gameplayHistoryHeight==p.height&&pauseDisplayIdentityCurrent()&&
        pauseDerived->owner->current()&&
        pauseDerived->textures.validBorrow(request.context,request.isCurrent,p.texturePublication);
}

bool VehicleWindowGl50DS::Storage::preparePauseDerivedFrame(const game::OriginalPauseNativeDraw50LA* owner) noexcept {
    if(pauseDerived){pauseDerived->frame=false;pauseDerived->quads=pauseDerived->packetCount=0;}
    if(!owner)return true;
    if(!pauseDerived||!pauseDerived->ready||owner!=pauseDerived->owner.get()||!owner->current())return false;
    auto& h=*pauseDerived;const auto* draw=owner->frame();if(!draw)return false;
    const auto& source=*draw;const auto* v=source.viewport;
    if(!source.prepared||!source.serial||source.serial<h.serial||!source.cameraPublication||
       !v||!v->prepared||!v->matricesReady||!v->sceneKey||!v->sessionGeneration||
       v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
       (h.viewport&&(h.viewport!=v||h.scene!=v->sceneKey||h.generation!=v->sessionGeneration))||
       source.quadCount>PauseDerived::maximumQuads||source.packetCount>PauseDerived::maximumPackets||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    std::array<bool,PauseDerived::maximumQuads> covered{};
    for(U p=0;p<source.packetCount;++p){const auto& packet=source.packets[p];
        if(packet.material>=h.policies.size()||!h.policies[packet.material]||!h.policies[packet.material]->enabled||packet.leaf<0||!packet.count||
           packet.first>source.quadCount||packet.count>source.quadCount-packet.first)return false;
        if(h.policies[packet.material]->hasTexture){
            const auto image=owner->image(packet.material);
            const auto* bank=owner->pixels();
            if(!bank||image>=bank->textureCount())return false;
            h.images[packet.material]=image;
        }
        for(U q=packet.first;q<packet.first+packet.count;++q){
            if(covered[q])return false;covered[q]=true;
            const auto& quad=source.quads[q];const auto& policy=*h.policies[packet.material];
            if(packet.leaf!=policy.canonical.origin.finalLeaf)return false;
            for(U i=0;i<4;++i){const auto& in=quad.vertices[i];
                if(in.position[2]!=16777214.f||!std::isfinite(in.position[0])||!std::isfinite(in.position[1])||
                   !std::isfinite(in.uv[0])||!std::isfinite(in.uv[1])||
                   in.color[3]>(policy.hasTexture?policy.textured.maximumVertexAlpha:policy.untextured.maximumVertexAlpha))return false;
                h.vertices[q*4+i]=in;
            }
        }
        h.packets[p]=packet;
    }
    for(U q=0;q<source.quadCount;++q)if(!covered[q])return false;
    h.viewport=v;h.scene=v->sceneKey;h.generation=v->sessionGeneration;h.serial=source.serial;h.cameraPublication=source.cameraPublication;
    h.quads=source.quadCount;h.packetCount=source.packetCount;h.frame=h.quads!=0;return true;
}

bool VehicleWindowGl50DS::Storage::drawPauseDerived(U bank) noexcept {
    return drawPauseDerivedTargets(targets[bank],targets[bank]);
}

bool VehicleWindowGl50DS::Storage::drawPauseDerivedTargets(U sourceTarget,U destinationTarget) noexcept {
    if(!pauseDerived||!pauseDerived->frame)return true;
    auto& h=*pauseDerived;
    if(!h.ready||!h.owner||!h.owner->current()||
       !h.owner->frame()||!h.owner->frame()->prepared||h.owner->frame()->serial!=h.serial||
       h.owner->frame()->cameraPublication!=h.cameraPublication||h.owner->frame()->viewport!=h.viewport||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);gl.disable(0x0B44);
    gl.bindFramebuffer(readFramebuffer,sourceTarget);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,h.target);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,h.target);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // The authenticated source-alpha equation uses As, never Ad.
    // Source alpha remains private; gameplay framebuffer alpha is not claimed.
    gl.colorMask(1,1,1,1);gl.enable(0x0B71);gl.depthFunc(0x0206);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),float(2*ox/width-1),float(1-2*oy/height));
    gl.scissor(I(std::floor(ox)),I(std::floor(oy)),I(std::ceil(ox+640*scale))-I(std::floor(ox)),
        I(std::ceil(oy+448*scale))-I(std::floor(oy)));gl.enable(0x0C11);
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,1);
    gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiSourceFogEnabledUniform,0);
    gl.uniform1i(uiSourceAlphaTestUniform,0);gl.uniform1f(uiAlphaUniform,1);gl.uniform1i(uiImageUniform,0);
    if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
        if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,h.color);}
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(h.quads)*4*sizeof(assets::Vertex48B),h.vertices.data());
    drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    bool okay=check("prepare authentic PauseDerived source pass");

    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        const auto& policy=*h.policies[packet.material];
        const auto depth=policy.hasTexture?policy.textured.depthMethod:policy.untextured.depthMethod;
        gl.depthFunc(depth==0?0x0200:depth==1?0x0207:depth==2?0x0206:0x0204);
        gl.depthMask((policy.hasTexture?policy.textured.depthWrite:policy.untextured.depthWrite)?1:0);
        const auto blend=policy.hasTexture?policy.textured.blend:policy.untextured.blend;
        gl.uniform1i(uiSourceBlendUniform,static_cast<I>(blend));
        gl.uniform1i(uiTexturedUniform,policy.hasTexture?1:0);
        gl.bindTexture(texture2D,policy.hasTexture?h.textures.texture(h.images[packet.material]):0);
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(packet.count*6),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original PauseDerived source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<(packet.first+packet.count)*6;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original PauseDerived source triangle");}
        }
    }
    if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
    gl.bindTexture(texture2D,0);gl.disable(0x0C11);gl.disable(0x0BE2);
    if(okay){
        // This is the sole write to the pending gameplay color target. A blit
        // failure aborts the existing whole-frame transaction before publish.
        gl.bindFramebuffer(readFramebuffer,h.target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,destinationTarget);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy authentic original PauseDerived into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,destinationTarget);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original PauseDerived pass");
    if(okay&&restored){diagnostic->originalPauseDerivedQuads=h.quads;diagnostic->originalPauseDerivedLeaves=h.packetCount;}
    return okay&&restored;
}

bool VehicleWindowGl50DS::Storage::deletePauseDerivedNames(VehicleWindowDiagnostic50DS& d) noexcept {
    pauseDisplayPublication={};
    if(!pauseDerived)return true;auto& h=*pauseDerived;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    // Whole-renderer close, explicit binding retirement or failed-bind cleanup
    // has completed glFinish.
    // Release the strong canonical owner only after acknowledged name retirement.
    pauseDerived.reset();return true;
}

bool VehicleWindowGl50DS::Storage::hasSpeedBoostResources()const noexcept {
    return speedBoost&&(speedBoost->target||speedBoost->color||speedBoost->depth||speedBoost->textures.ownsResources());
}

bool VehicleWindowGl50DS::Storage::prepareSpeedBoostResources(std::shared_ptr<const game::OriginalSpeedBoost50LA> owner) noexcept {
    if(speedBoost||frontendOnly||active||frameSet||!owner||!owner->nativeCpuResourcesPrepared())
        return fail(*diagnostic,VehicleWindowError50DS::request,"fresh original SpeedBoost cold owner required");
    speedBoost.reset(new(std::nothrow) SpeedBoost);
    if(!speedBoost)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate original SpeedBoost storage");
    auto& h=*speedBoost;h.owner=std::move(owner);
    const auto* images=h.owner->nativePixels();const auto* bank=images?images->textures():nullptr;
    if(!bank||bank->textureCount()!=12)
        return fail(*diagnostic,VehicleWindowError50DS::assets,"original SpeedBoost twelve-image bridge required");
    assets::OriginalUiMaterialDiagnostic50KG md;
    for(U i=0;i<12;++i){
        const auto* m=h.owner->nativeMaterial(i);const U image=h.owner->nativeImage(i);
        const auto* decoded=images->decodedTexture(image);
        if(image>=12||!m||m->passes.size()!=1||!decoded||
           !assets::prepareOriginalUiNativeMaterial50KG(m->passes[0],*decoded,h.policies[i],md)||
           h.policies[i].blend!=assets::OriginalUiMaterialBlend50KG::sourceAlpha||
           !hudSampler(h.policies[i],h.samplers[image]))
            return fail(*diagnostic,VehicleWindowError50DS::material,"unsupported original SpeedBoost source raster");
        // Owner already authenticated exact source identity and canonical map.
        // Keep that authored group; never relabel unique to force admission.
        const auto& group=m->passes[0].shaderPassGroup;
        if(!hudName(reinterpret_cast<const std::uint8_t*>(group.data()),group.size(),"none"))
            return fail(*diagnostic,VehicleWindowError50DS::material,"original SpeedBoost group differs");
        h.images[i]=image;
    }
    for(U i=0;i<12;++i)for(U j=0;j<i;++j)if(h.images[i]==h.images[j])
        return fail(*diagnostic,VehicleWindowError50DS::assets,"distinct SpeedBoost source images required");
    // A texture-feedback path is needed only for this native private target.
    // Do not change the already compiled UI program's coherent-fetch variant.
    if(!uiSourceCoherentFramebufferFetch){
        I major=0,minor=0,units=0,count=0;gl.getIntegerv(0x821B,&major);gl.getIntegerv(0x821C,&minor);
        gl.getIntegerv(0x8872,&units);bool barrier=major>4||(major==4&&minor>=5);
        if(!load(gl.getStringi,"glGetStringi")||!check("query original SpeedBoost source-alpha capabilities"))return false;
        gl.getIntegerv(0x821D,&count);
        if(!check("query original SpeedBoost extensions")||count<0||count>65536||units<2)
            return fail(*diagnostic,VehicleWindowError50DS::context,"bounded original SpeedBoost source-alpha support unavailable");
        for(I i=0;i<count;++i){const auto* p=gl.getStringi(0x1F03,U(i));
            if(!check("read original SpeedBoost extension")||!p)return false;
            if(!std::strcmp(reinterpret_cast<const char*>(p),"GL_ARB_texture_barrier"))barrier=true;}
        if(!barrier||!load(gl.textureBarrier,"glTextureBarrier"))
            return fail(*diagnostic,VehicleWindowError50DS::context,"original SpeedBoost requires GL4.5 or ARB_texture_barrier");
    }
    if(!load(gl.genTextures,"glGenTextures")||!load(gl.deleteTextures,"glDeleteTextures")||
        !load(gl.scissor,"glScissor")||!load(gl.activeTexture,"glActiveTexture")||!load(gl.bindTexture,"glBindTexture")||
        !load(gl.texImage2D,"glTexImage2D")||!load(gl.texParameteri,"glTexParameteri")||
        !load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv")||!load(gl.framebufferTexture2D,"glFramebufferTexture2D")||
        !load(gl.blitFramebuffer,"glBlitFramebuffer"))return false;
    MaterialTextureGlRequest50EB requestTextures{};requestTextures.context=request.context;
    requestTextures.originalUploadBinding=request.originalUploadBinding;
    requestTextures.isCurrent=request.isCurrent;requestTextures.samplers=h.samplers.data();requestTextures.samplerCount=12;
    requestTextures.maximumGpuBytes=16u*1024u*1024u;const char* missing=nullptr;
    MaterialTextureGlDiagnostic50EB textureDiagnostic;
    if(!loadMaterialTextureFunctions50EE(request.context,request.procAddress,requestTextures.gl,missing)||
        !h.textures.create(requestTextures,*bank,textureDiagnostic))
        return fail(*diagnostic,VehicleWindowError50DS::material,"upload authentic original SpeedBoost images");
    h.publication=h.textures.publicationToken();
    if(!uiProgram){
        // A failed late compilation can retain partially created shared names.
        // They remain renderer-owned for close; never reuse them as readiness.
        speedBoostUiCreationFailed=true;speedBoostPreparing=true;
        const bool okay=createOriginalUi();speedBoostPreparing=false;if(!okay)return false;
        speedBoostUiCreationFailed=false;
    }
    I limit=0;gl.getIntegerv(0x0D33,&limit);
    if(!check("query original SpeedBoost texture extent")||limit<maximumWidth||limit<maximumHeight)
        return fail(*diagnostic,VehicleWindowError50DS::limits,"original SpeedBoost private target exceeds texture limit");
    gl.genFramebuffers(1,&h.target);gl.bindFramebuffer(framebuffer,h.target);
    gl.genTextures(1,&h.color);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,h.color);
    gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
    gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
    gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);drawGl.bindBuffer(0x88EC,0);
    gl.texImage2D(texture2D,0,rgba8,maximumWidth,maximumHeight,0,rgba,unsignedByte,nullptr);
    if(!check("allocate native-size original SpeedBoost source target")||!h.target||!h.color)return false;
    I w=0,height=0,format=0;gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);
    gl.getTexLevelParameteriv(texture2D,0,0x1001,&height);gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
    if(!check("verify original SpeedBoost source storage")||w!=maximumWidth||height!=maximumHeight||format!=I(rgba8))return false;
    gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,h.color,0);
    if(!createAttachment(h.depth,depth24,depthAttachment))return false;
    gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);gl.bindFramebuffer(framebuffer,0);
    if(!check("complete original SpeedBoost private target")||status!=framebufferComplete){diagnostic->framebufferStatus=status;return false;}
    h.ready=true;return true;
}

bool VehicleWindowGl50DS::Storage::prepareSpeedBoostFrame(const game::OriginalSpeedBoost50LA* owner) noexcept {
    if(speedBoost){speedBoost->frame=false;speedBoost->primitives=speedBoost->packetCount=0;}
    if(!owner)return true;
    if(!speedBoost||!speedBoost->ready||owner!=speedBoost->owner.get()||!owner->nativeCpuResourcesPrepared())return false;
    auto& h=*speedBoost;const auto& source=owner->nativeDraw();const auto* v=source.viewport;
    if(!source.prepared||!source.serial||source.serial<h.serial||!source.cameraPublication||
       !v||!v->prepared||!v->matricesReady||!v->sceneKey||!v->sessionGeneration||
       v->x1E0||v->y1E4||v->width1E8!=640||v->height1EC!=448||
       (h.viewport&&(h.viewport!=v||h.scene!=v->sceneKey||h.generation!=v->sessionGeneration))||
       source.primitiveCount>SpeedBoost::maximumPrimitives||source.packetCount>SpeedBoost::maximumPackets||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    std::array<bool,SpeedBoost::maximumPrimitives> covered{};
    std::int32_t previousLeaf=-1;
    for(U p=0;p<source.packetCount;++p){const auto& packet=source.packets[p];
        if(packet.material>=12||packet.leaf<0||packet.leaf<previousLeaf||!packet.count||
           packet.first>source.primitiveCount||packet.count>source.primitiveCount-packet.first||
           (packet.vertexCount!=3&&packet.vertexCount!=4)||
           (packet.vertexCount==3&&(packet.count!=1||packet.material!=9))||
           (packet.vertexCount==4&&packet.material==9))return false;
        previousLeaf=packet.leaf;
        for(U q=packet.first;q<packet.first+packet.count;++q){
            if(covered[q])return false;covered[q]=true;
            const auto& quad=source.primitives[q];
            if(quad.depth!=16777214.f||quad.vertexCount!=packet.vertexCount)return false;
            for(U i=0;i<4;++i)h.vertices[q*4+i]={};
            for(U i=0;i<quad.vertexCount;++i){const auto& in=quad.vertices[i];const auto& c=quad.colors[i];
                if(!std::isfinite(in.x)||!std::isfinite(in.y)||!std::isfinite(in.u)||!std::isfinite(in.v)||
                   c[3]>h.policies[packet.material].maximumVertexAlpha)return false;
                auto& out=h.vertices[q*4+i];out.position[0]=in.x;out.position[1]=in.y;out.position[2]=quad.depth;
                out.uv[0]=in.u;out.uv[1]=in.v;for(U lane=0;lane<4;++lane)out.color[lane]=c[lane];
            }
        }
        h.packets[p]=packet;
    }
    for(U q=0;q<source.primitiveCount;++q)if(!covered[q])return false;
    h.viewport=v;h.scene=v->sceneKey;h.generation=v->sessionGeneration;h.serial=source.serial;
    h.primitives=source.primitiveCount;h.packetCount=source.packetCount;h.frame=h.primitives!=0;return true;
}

bool VehicleWindowGl50DS::Storage::drawSpeedBoost(U bank) noexcept {
    if(!speedBoost||!speedBoost->frame)return true;
    auto& h=*speedBoost;
    if(!active||!frameSet||!diagnostic||!h.ready||!h.owner||!h.owner->nativeCpuResourcesPrepared()||
       !h.owner->nativeDraw().prepared||h.owner->nativeDraw().serial!=h.serial||
       !h.textures.validBorrow(request.context,request.isCurrent,h.publication))return false;
    // Keep the native image at its real drawable resolution. Only this private
    // scratch target receives raw source alpha and a separate UI depth plane.
    gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
    // Original ScreenObject geometry is two-sided here. Never inherit opaque
    // model culling: the genuine boost triangle is not a degenerate quad.
    gl.disable(0x0B44);
    gl.bindFramebuffer(readFramebuffer,targets[bank]);gl.readBuffer(colorAttachment);
    gl.bindFramebuffer(drawFramebuffer,h.target);gl.drawBuffer(colorAttachment);
    gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
    gl.bindFramebuffer(framebuffer,h.target);gl.viewport(0,0,width,height);
    gl.depthMask(1);gl.clearDepth(0);gl.clear(depthBufferBit);
    // Both authenticated source-alpha equations use As, never Ad.
    // Source alpha remains private; gameplay framebuffer alpha is not claimed.
    gl.colorMask(1,1,1,1);gl.enable(0x0B71);gl.depthFunc(0x0206);gl.depthRange(0,1);
    resetNativeGcLighting();gl.useProgram(uiProgram);
    const double scale=std::min(double(width)/640.0,double(height)/448.0);
    const double ox=(width-640.0*scale)*.5,oy=(height-448.0*scale)*.5;
    gl.uniform4f(uiCanvasUniform,float(2*scale/width),float(-2*scale/height),float(2*ox/width-1),float(1-2*oy/height));
    gl.scissor(I(std::floor(ox)),I(std::floor(oy)),I(std::ceil(ox+640*scale))-I(std::floor(ox)),
        I(std::ceil(oy+448*scale))-I(std::floor(oy)));gl.enable(0x0C11);
    gl.uniform1i(gcParticleModeUniform,0);gl.uniform1i(uiSourceCompositionUniform,1);
    gl.uniform1i(uiTexturedUniform,1);gl.uniform1i(uiSourceRasterUniform,0);gl.uniform1i(uiSourceFogEnabledUniform,0);
    gl.uniform1i(uiSourceAlphaTestUniform,0);gl.uniform1f(uiAlphaUniform,1);gl.uniform1i(uiImageUniform,0);
    if(!uiSourceCoherentFramebufferFetch){gl.uniform1i(uiSourceTargetUniform,1);gl.activeTexture(textureUnit0+1);
        if(gl.bindSampler)gl.bindSampler(1,0);gl.bindTexture(texture2D,h.color);}
    gl.activeTexture(textureUnit0);if(gl.bindSampler)gl.bindSampler(0,0);
    drawGl.bindVertexArray(uiVertexArray);drawGl.bindBuffer(arrayBuffer,uiVertexBuffer);
    drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(h.primitives)*4*sizeof(assets::Vertex48B),h.vertices.data());
    drawGl.bindBuffer(elementBuffer,uiIndexBuffer);
    bool okay=check("prepare authentic SpeedBoost source pass");

    for(U p=0;okay&&p<h.packetCount;++p){const auto& packet=h.packets[p];
        // Both actual policies were admitted as source-alpha, GEQUAL/write.
        // Preserve owner packet order; no synthetic registration/rank sorting.
        // Depth/blend policy is constant across this pass's checked packets.
        if(p==0){gl.depthMask(1);gl.uniform1i(uiSourceBlendUniform,1);}
        if(p==0 || packet.material!=h.packets[p-1].material)
            gl.bindTexture(texture2D,h.textures.texture(h.images[packet.material]));
        const U indexCount=packet.vertexCount==3?3u:packet.count*6u;
        // Shared index slot is0,1,2,0,2,3. A triangle consumes exactly its
        // first three indices and retains the source's three real vertices.
        if(uiSourceCoherentFramebufferFetch){
            drawGl.drawElementsBaseVertex(0x0004,I(indexCount),0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(packet.first)*6*sizeof(U)),0);
            okay=check("draw ordered original SpeedBoost source leaf");
        }else{
            for(U i=packet.first*6;okay&&i<packet.first*6+indexCount;i+=3){gl.textureBarrier();
                drawGl.drawElementsBaseVertex(0x0004,3,0x1405,reinterpret_cast<const void*>(std::uintptr_t(i)*sizeof(U)),0);
                okay=check("draw ordered original SpeedBoost source triangle");}
        }
    }
    if(!uiSourceCoherentFramebufferFetch){gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);gl.activeTexture(textureUnit0);}
    gl.bindTexture(texture2D,0);gl.disable(0x0C11);gl.disable(0x0BE2);
    if(okay){
        // This is the sole write to the pending gameplay color target. A blit
        // failure aborts the existing whole-frame transaction before publish.
        gl.bindFramebuffer(readFramebuffer,h.target);gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer,targets[bank]);gl.drawBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,width,height,0,0,width,height,colorBufferBit,0x2600);
        okay=check("copy authentic original SpeedBoost into pending gameplay image");
    }
    gl.bindFramebuffer(framebuffer,targets[bank]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
    // This native presentation owns opaque alpha. Source Ad stays private and
    // never becomes the input to a later gameplay effect or retained UI frame.
    gl.colorMask(0,0,0,1);gl.clearColor(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
    gl.depthMask(1);gl.clearDepth(1);gl.depthFunc(0x0201);gl.useProgram(shader->program());
    drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
    const bool restored=check("restore gameplay state after original SpeedBoost pass");
    if(okay&&restored){diagnostic->originalSpeedBoostPrimitives=h.primitives;diagnostic->originalSpeedBoostLeaves=h.packetCount;}
    return okay&&restored;
}

bool VehicleWindowGl50DS::Storage::deleteSpeedBoostNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if(!speedBoost)return true;auto& h=*speedBoost;h.ready=h.frame=false;
    MaterialTextureGlDiagnostic50EB td;
    if(!h.textures.close(td)){if(td.cleanupIncomplete){d.cleanupIncomplete=true;return false;}
        if(!d.cleanupGlError)d.cleanupGlError=td.glError?td.glError:td.cleanupGlError;}
    if(!deleteName(h.target,gl.deleteFramebuffers,d)||!deleteName(h.color,gl.deleteTextures,d)||
        !deleteName(h.depth,gl.deleteRenderbuffers,d))return false;
    // Whole-renderer close or failed-bind cleanup has completed glFinish.
    // Release the strong canonical owner only after acknowledged name retirement.
    speedBoost.reset();return true;
}

} // namespace rr::platform
