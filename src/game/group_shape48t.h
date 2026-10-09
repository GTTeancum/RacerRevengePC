#pragma once
#include "body_scene48g.h"
#include "spring_records48n.h"

namespace rr::game {
// Borrowed projection of the geometry at *(shape+114): center at +0C and
// half at +18. Geometry may be shared between source descriptors.
struct GroupShapeGeometry48T {
    Vector3_48C localCenter{},localHalf{};
};
struct GroupShapeSource48T {
    BodyMatrix48G local40{};
    const GroupShapeGeometry48T* geometry=nullptr;
};
// Only the three matrices modified by the tag-0 route. The input local matrix
// and geometry remain borrowed and immutable; no scene/ownership is inferred.
struct GroupShapeState48T {
    BodyMatrix48G composed00{},world80{},previousC0{};
};
struct GroupShapeSphere48T {
    SpringVector4_48N center{};float radius=0;
};
enum class GroupShapeStatus48T { Complete,InvalidInput,MissingGeometry,NonFinite };
// Bounded135CF0 tag0 ->11F680 tag0, after common group XYZ/ratio arithmetic.
// translation replaces only world80 XYZ translation. Those writes happen
// BEFORE world80 is copied to previousC0; previousC0 therefore contains the
// NEW translation, including signed zeros. composed00 = world80 * local40
// uses all four lanes/terms, with separately rounded binary32 operations.
// Sphere center is geometry XYZ transformed as a point with W forced to1;
// radius is the ordered length of the unscaled geometry half, not matrix-scaled.
//
// The full destination/sphere objects must be aligned and disjoint from one
// another, source, translation, initial and the optional geometry object.
// Read-only inputs may alias. Null geometry is an explicit reached boundary.
// Any rejection preserves every destination/sphere byte. Raw prior composed00,
// previousC0 and overwritten world80 translation lanes need not be finite;
// every reached arithmetic lane/intermediate must be finite. No affine/rigid
// matrix restriction. Serial, allocation-free, fixed stack, no retained data.
// Not a general11F680 setter and does not prepare tag0 query bounds.
GroupShapeStatus48T setGroupShape0Bounded48T(
    const GroupShapeSource48T&,const Vector3_48C& translation,
    const GroupShapeState48T& initial,GroupShapeState48T& destination,
    GroupShapeSphere48T& sphere) noexcept;
}
