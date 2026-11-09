#pragma once
// Unattended test only (RR_TEST_RESULTS_CONFIRM): a player at a focused
// window tapping Enter on the Results pages. The loop sets these; the host
// applies them to its next poll BEFORE the Results input observes it, so the
// press reaches the same fixed UI edge path a real key press takes.
namespace rr::platform {
struct TestResultsInput50PD {
    bool focus=false; // treat the window as focused for this poll
    bool enter=false; // one fresh Enter press on this poll (consumed)
};
inline TestResultsInput50PD& testResultsInput50PD()noexcept{static TestResultsInput50PD state;return state;}
}
