#pragma once
#include "ta_object_lighting50js.h"

namespace rr::game::native_light_publication50JS {
namespace light=ta_object_lighting50JS;
enum class Status : std::uint8_t {Complete,Identity,AlreadyPublished,StalePublication,Projection};
struct Result {Status status=Status::Identity;light::Status projection=light::Status::UnboundObject;};
// Explicit native per-frame cache policy, authorized for the native port.
// One stable owner per actual model, outside transient racer banks. Every new
// render publication recomputes original-backed current selection from genuine
// scene/query/zone/portal inputs. It does not assert retail A4 null or cache miss.
// Retail caches by visibility membership and can reuse an earlier component's
// matrices; this native policy can therefore differ and costs one source-math
// evaluation per model per rendered frame. No source light ordering is changed.
class Owner final {
    std::uint64_t identity_=0,sequence_=0;
    light::Output output_{};
public:
    Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
    Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
    // Call after the actual model and query owners have prepared their current
    // inputs. A failed calculation leaves this owner and output unchanged.
    Result publish(const light::Input& original,light::Output& out) noexcept {
        if(!original.objectIdentity||!original.publicationSequence||
           (identity_&&identity_!=original.objectIdentity))return {};
        if(sequence_==original.publicationSequence)return {Status::AlreadyPublished,light::Status::Complete};
        if(sequence_>original.publicationSequence)return {Status::StalePublication,light::Status::UnboundObject};
        auto input=original;input.cache=light::CacheAdmission::NativePerPublicationRecompute;
        light::Output next{};const auto result=light::project(input,next);
        if(result!=light::Status::Complete)return {Status::Projection,result};
        identity_=input.objectIdentity;sequence_=input.publicationSequence;output_=next;out=next;
        return {Status::Complete,light::Status::Complete};
    }
    // Explicit same-publication read only. Never recompute from changed inputs
    // under the same frame number, or expose a previous frame as current.
    const light::Output* current(std::uint64_t model,std::uint64_t publication)const noexcept {
        return identity_&&identity_==model&&sequence_==publication?&output_:nullptr;
    }
};
} //namespace rr::game::native_light_publication50JS
