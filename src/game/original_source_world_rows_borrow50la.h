#pragma once
#include <memory>
#include <cstdint>
#include <cstddef>
#include <thread>
#include <exception>
#include <utility>
namespace original_main_entry50KG {class Application;}
namespace original_racer_manager_entity {class Owner;}
namespace ra_cold_aggregate50LA {class Owner;}
namespace rr::game {
class OriginalPlayerConstructorPrefix50LA;
class OriginalRaceConstructor50LA;
// Native lifetime custody of actual source World rows and read-through fields.
// NOT World-load completion, water/NONE branch evidence, or caller admission.
// No issuer exists until the authentic outer Race/manager occurrence is joined.
class OriginalSourceWorldRowsBorrow50LA final {
    friend class ::ra_cold_aggregate50LA::Owner;
    friend class ::original_racer_manager_entity::Owner;
    friend class OriginalPlayerConstructorPrefix50LA;
    using Reader=bool(*)(const void*,const original_main_entry50KG::Application&,
        std::uint64_t,const void*&,const void*&)noexcept;
    using FieldsReader=bool(*)(const void*,const original_main_entry50KG::Application&,
        std::uint64_t,const void*&,const void*&,const OriginalRaceConstructor50LA*&)noexcept;
    std::shared_ptr<const void> world_;
    const original_main_entry50KG::Application* application_=nullptr;
    const void *visibility_=nullptr,*collision_=nullptr;
    std::size_t* borrowers_=nullptr;
    Reader reader_=nullptr;
    FieldsReader fieldsReader_=nullptr;
    std::uint64_t generation_=0;
    std::thread::id thread_;
    OriginalSourceWorldRowsBorrow50LA(std::shared_ptr<const void> world,
        const original_main_entry50KG::Application& app,std::uint64_t generation,
        const void* visibility,const void* collision,std::size_t& borrowers,Reader reader,FieldsReader fieldsReader)
        :world_(std::move(world)),application_(&app),visibility_(visibility),collision_(collision),
         borrowers_(&borrowers),reader_(reader),fieldsReader_(fieldsReader),generation_(generation),thread_(std::this_thread::get_id()) {
        // Future private issuer must preflight identity and counter overflow.
        ++*borrowers_;
    }
    bool read1AE0C8(const original_main_entry50KG::Application& app,
        const void*& visibility,const void*& collision)const noexcept {
        if(thread_!=std::this_thread::get_id()||!world_||application_!=&app||
           !generation_||!borrowers_||!*borrowers_||!reader_)return false;
        const void *v=nullptr,*c=nullptr;
        if(!reader_(world_.get(),app,generation_,v,c)||!v||!c||v!=visibility_||c!=collision_)return false;
        visibility=v;collision=c;return true;
    }
    bool read2107FC(const original_main_entry50KG::Application& app,
        const void*& terrain,const void*& water,const OriginalRaceConstructor50LA*& race)const noexcept {
        const void *v=nullptr,*c=nullptr;
        if(!read1AE0C8(app,v,c)||!fieldsReader_)return false;
        // Callback reads the SAME retained World, with its own retained epoch.
        // Known NULL is valid; native water-domain/NONE receipts are not used.
        return fieldsReader_(world_.get(),app,generation_,terrain,water,race);
    }
public:
    OriginalSourceWorldRowsBorrow50LA(const OriginalSourceWorldRowsBorrow50LA&)=delete;
    OriginalSourceWorldRowsBorrow50LA& operator=(const OriginalSourceWorldRowsBorrow50LA&)=delete;
    ~OriginalSourceWorldRowsBorrow50LA(){
        if(thread_!=std::this_thread::get_id()||!borrowers_||!*borrowers_)std::terminate();
        --*borrowers_; //World keepalive still owns the counter until after this body.
    }
};
}
