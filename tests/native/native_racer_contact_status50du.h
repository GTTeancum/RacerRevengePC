#pragma once

// Same canonical status type and enumerator order used by both contact owners.
// Telemetry may name it without importing collision resource/query bodies.
namespace native_racer_contact50HT {
enum class Status {Complete,Unprepared,InvalidInput,GeometryRejected,ResponseRejected};
}
namespace native_racer_contact50HW {
using native_racer_contact50HT::Status;
}
