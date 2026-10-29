// Private actual Course Overlay_Text cohort; only authenticated static/dynamic rows admitted.
    using CourseScreenText=OriginalUiGarageScreenText50KG; // Actual shared primitive body, no Garage grant.
    using CourseFonts=assets::OriginalApplicationFonts50LA;
    using CourseDictionary=assets::OriginalApplicationLocalization50KG;
    struct CourseTextLine : OriginalUiTextOverlayFields50KG {
        C entity;std::unique_ptr<CourseScreenText> screenText30_;
        std::shared_ptr<const CourseFonts::Entry> font34_;
        const CourseScreenText* sourceScreenText30_=nullptr;
        const CourseFonts::Entry* sourceFont34_=nullptr; // Actual constructorNULL; retained publication history.
        bool constructed=false,screenPointerKnown=false,loaded=false,attached=false,screenRetired=false,fontReleased=false;
        std::int32_t capacity=0;
    };
    struct CourseTextRow {
        std::unique_ptr<OriginalUiOverlayTextFields50KG> outer;
        std::unique_ptr<C*[]> lineArray;
        std::array<std::unique_ptr<CourseTextLine>,10> lines;
        std::unique_ptr<C*[]> failedReallocArray; // Old allocation remains owned when actual realloc result publishes NULL.
        std::size_t arrayCapacity=0,failedReallocCapacity=0;
        std::shared_ptr<const CourseDictionary::SelectedText> selected;
        std::int32_t lookupOrdinal=-1;
        std::array<const C*,10> sourceLineSlots_{};std::array<bool,10> slotKnown{};
        // Historical source array slots are never dereferenced after child retirement.
        bool attempted=false,constructed=false,arrayPublished=false,loaded=false,arrayReleased=false;
        std::array<char,20> justificationStack280{};bool justificationStackKnown=false;
        bool dynamicLength16CKnown=false; // Static TextID skips producer; backing0 is NOT source knowledge.
    };
    std::array<CourseTextRow,84> courseTextRows_;
    struct CourseStaticTextSourceRow {
        std::size_t ordinal;const char* font;const char* material;const char* textId;
        CourseFonts::Name fontName;std::int32_t capacity,lookup,x,y,justification;
        const char* authoredJustification;std::array<float,3> color;
        float leading=1;bool dynamic=false;std::size_t expectedLines=1;
    };
    inline static constexpr std::array<CourseStaticTextSourceRow,34> courseStaticTextSourceRows_{{
        {0,"denmrk16","denmrk16Top","STR_COURSE_SELECTION_PAGE_RACE",CourseFonts::Name::frontendDenmrk16,4,120,516,385,0,"Left",{120,149,179}},
        {1,"denmrk16","denmrk16Top","STR_BACK",CourseFonts::Name::frontendDenmrk16,4,5,429,385,0,"Left",{137,170,204}},
        {26,"venus16","venus16Top","STR_TRACK_TATOOINE_A",CourseFonts::Name::frontendVenus16,17,106,320,59,3,"CenterBoth",{220,220,220}},
        {27,"venus16","venus16Top","STR_TRACK_GAMORR_A",CourseFonts::Name::frontendVenus16,14,107,320,59,3,"CenterBoth",{220,220,220}},
        {29,"venus16","venus16Top","STR_TRACK_RYLOTH_A",CourseFonts::Name::frontendVenus16,15,108,320,59,3,"CenterBoth",{220,220,220}},
        {30,"venus16","venus16Top","STR_TRACK_SULLUST_A",CourseFonts::Name::frontendVenus16,18,109,320,59,3,"CenterBoth",{220,220,220}},
        {31,"venus16","venus16Top","STR_TRACK_MON_CALAMARI_A",CourseFonts::Name::frontendVenus16,12,110,320,59,3,"CenterBoth",{220,220,220}},
        {32,"venus16","venus16Top","STR_TRACK_TATOOINE_B",CourseFonts::Name::frontendVenus16,12,111,320,59,3,"CenterBoth",{220,220,220}},
        {33,"venus16","venus16Top","STR_TRACK_GAMORR_B",CourseFonts::Name::frontendVenus16,24,112,320,59,3,"CenterBoth",{220,220,220}},
        {37,"venus16","venus16Top","STR_TRACK_MON_CALAMARI_B",CourseFonts::Name::frontendVenus16,19,114,320,59,3,"CenterBoth",{220,220,220}},
        {38,"venus16","venus16Top","STR_TRACK_TATOOINE_C",CourseFonts::Name::frontendVenus16,18,115,320,59,3,"CenterBoth",{220,220,220}},
        {39,"venus16","venus16Top","STR_TRACK_GAMORR_C",CourseFonts::Name::frontendVenus16,11,116,320,59,3,"CenterBoth",{220,220,220}},
        {40,"venus16","venus16Top","STR_TRACK_RYLOTH_C",CourseFonts::Name::frontendVenus16,14,117,320,59,3,"CenterBoth",{220,220,220}},
        {42,"venus16","venus16Top","STR_TRACK_SULLUST_C",CourseFonts::Name::frontendVenus16,14,119,320,59,3,"CenterBoth",{220,220,220}},
        {43,"venus16","venus16Top","STR_TRACK_MON_CALAMARI_C",CourseFonts::Name::frontendVenus16,15,118,320,59,3,"CenterBoth",{220,220,220}},
        {51,"venus16","venus16Top","STR_COURSE_SELECTION_PAGE_SELECT_RACE",CourseFonts::Name::frontendVenus16,12,128,30,386,0,"Left",{194,166,132}},
        {59,"denmrk14","denmrk14",nullptr,CourseFonts::Name::frontendDenmrk14,40,-1,100,90,0,"Left",{204,204,204},0.8f,true,1},
        {60,"denmrk14","denmrk14",nullptr,CourseFonts::Name::frontendDenmrk14,40,-1,100,130,0,"Left",{204,204,204},0.8f,true,1},
        {61,"denmrk14","denmrk14","STR_COURSE_SELECTION_PAGE_BRIBE",CourseFonts::Name::frontendDenmrk14,83,135,30,175,0,"Left",{204,204,204},0.9f,false,3},
        {62,"denmrk14","denmrk14",nullptr,CourseFonts::Name::frontendDenmrk14,60,-1,100,110,0,"Left",{204,204,204},0.8f,true,1},
        {67,"denmrk16","denmrk16Top","STR_OK",CourseFonts::Name::frontendDenmrk16,2,90,546,385,0,"Left",{120,149,179},1,false,1},
        {68,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_FORMAT_WAITING",CourseFonts::Name::frontendDenmrk16,142,95,320,150,3,"CenterBoth",{255,255,255},1,false,7},
        {69,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_FORMAT_SUCCESS",CourseFonts::Name::frontendDenmrk16,18,93,320,220,3,"CenterBoth",{255,255,255},1,false,1},
        {70,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_FORMAT_FAILURE",CourseFonts::Name::frontendDenmrk16,14,94,320,220,3,"CenterBoth",{255,255,255},1,false,1},
        {71,"denmrk16","denmrk16Top","STR_YES",CourseFonts::Name::frontendDenmrk16,3,0,546,385,0,"Left",{120,149,179},1,false,1},
        {72,"denmrk16","denmrk16Top","STR_NO",CourseFonts::Name::frontendDenmrk16,2,1,459,385,0,"Left",{120,149,179},1,false,1},
        {73,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_FORMAT_CONFIRMATION",CourseFonts::Name::frontendDenmrk16,105,92,320,190,3,"CenterBoth",{255,255,255},1,false,3},
        {74,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_NO_CARD_3",CourseFonts::Name::frontendDenmrk16,337,98,320,118,3,"CenterBoth",{255,255,255},1,false,10},
        {75,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_OUT_OF_MEMORY",CourseFonts::Name::frontendDenmrk16,150,100,320,140,3,"CenterBoth",{255,255,255},1,false,3},
        {76,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_SAVE_WAITING",CourseFonts::Name::frontendDenmrk16,148,55,320,150,3,"CenterBoth",{255,255,255},1,false,7},
        {77,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_SAVE_SUCCESS",CourseFonts::Name::frontendDenmrk16,26,56,320,220,3,"CenterBoth",{255,255,255},1,false,1},
        {78,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_SAVE_FAILURE",CourseFonts::Name::frontendDenmrk16,22,57,320,220,3,"CenterBoth",{255,255,255},1,false,1},
        {79,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_SAVE_CONFIRMATION",CourseFonts::Name::frontendDenmrk16,95,54,320,190,3,"CenterBoth",{255,255,255},1,false,3},
        {80,"denmrk16","denmrk16Top","STR_MEMORY_CARD_PAGE_CARD_SWAPPED",CourseFonts::Name::frontendDenmrk16,195,91,320,190,3,"CenterBoth",{255,255,255},1,false,6},
    }};
    static const CourseStaticTextSourceRow* courseStaticTextSourceRow(std::size_t i)noexcept {
        for(const auto& row:courseStaticTextSourceRows_)if(row.ordinal==i)return &row;
        return nullptr;
    }
    static bool courseOverlayTextOrdinalAdmitted(std::size_t i)noexcept{return courseStaticTextSourceRow(i)!=nullptr;}
    CourseFonts::Name courseScreenTextFontName(std::size_t i)const noexcept {
        const auto* row=courseStaticTextSourceRow(i);return row?row->fontName:static_cast<CourseFonts::Name>(255);
    }
    std::string_view courseScreenTextMaterialName(std::size_t i)const noexcept {
        const auto* row=courseStaticTextSourceRow(i);return row?std::string_view(row->material):std::string_view{};
    }
    std::shared_ptr<const assets::OriginalUiArchive50KG> courseTextArchive()const noexcept {
        return loaderConfig_?loaderConfig_->archive_:nullptr;
    }
    static std::int32_t courseExpectedLineCapacity(std::size_t i,std::size_t j)noexcept {
        const auto* row=courseStaticTextSourceRow(i);if(!row||j>=row->expectedLines)return -1;
        if(i==61){constexpr std::int32_t caps[]{26,30,25};return caps[j];}
        switch(i){
            case 67:{constexpr std::int32_t caps[]{2};return caps[j];}
            case 68:{constexpr std::int32_t caps[]{13,1,31,44,1,25,21};return caps[j];}
            case 69:{constexpr std::int32_t caps[]{18};return caps[j];}
            case 70:{constexpr std::int32_t caps[]{14};return caps[j];}
            case 71:{constexpr std::int32_t caps[]{3};return caps[j];}
            case 72:{constexpr std::int32_t caps[]{2};return caps[j];}
            case 73:{constexpr std::int32_t caps[]{41,37,25};return caps[j];}
            case 74:{constexpr std::int32_t caps[]{29,42,1,45,38,10,1,49,62,51};return caps[j];}
            case 75:{constexpr std::int32_t caps[]{39,59,50};return caps[j];}
            case 76:{constexpr std::int32_t caps[]{19,1,31,44,1,25,21};return caps[j];}
            case 77:{constexpr std::int32_t caps[]{26};return caps[j];}
            case 78:{constexpr std::int32_t caps[]{22};return caps[j];}
            case 79:{constexpr std::int32_t caps[]{29,35,29};return caps[j];}
            case 80:{constexpr std::int32_t caps[]{36,60,1,29,35,29};return caps[j];}
            default:break;
        }
        return row->capacity;
    }
    std::size_t courseScreenTextLineOrdinal(const CourseScreenText* text,std::size_t i)const noexcept {
        if(!text||!courseOverlayTextOrdinalAdmitted(i))return 84;
        for(std::size_t j=0;j<10;++j)if(courseTextRows_[i].lines[j]&&courseTextRows_[i].lines[j]->screenText30_.get()==text)return j;
        return 84;
    }
    std::int32_t courseScreenTextCapacity(const CourseScreenText* text,std::size_t i)const noexcept {
        const auto j=courseScreenTextLineOrdinal(text,i);
        return j<10?courseTextRows_[i].lines[j]->capacity:-1;
    }
    bool ownsCourseScreenTextOccurrence(const CourseScreenText* text,std::size_t i)const noexcept {
        if(thread_!=std::this_thread::get_id()||!courseOverlayTextOrdinalAdmitted(i)||!text||!construction_||!issuer_||
           !construction_->registrationCurrent())return false;
        const auto j=courseScreenTextLineOrdinal(text,i);if(j>=10)return false;
        const auto& r=courseTextRows_[i];const auto& l=*r.lines[j];
        const C* const* backing=r.lineArray?r.lineArray.get():r.failedReallocArray.get();
        const auto slots=r.lineArray?r.arrayCapacity:r.failedReallocCapacity;
        return r.constructed&&r.outer&&l.constructed&&r.slotKnown[j]&&backing&&j<slots&&
            scope_&&construction_->courseOverlayOuters_[i]==&r.outer->entity&&
            construction_->courseTextLines_[i][j]==&l.entity&&r.outer->manager30==&scope_->manager_&&
            backing[j]==&l.entity&&r.sourceLineSlots_[j]==&l.entity&&r.outer->lines60==r.lineArray.get()&&
            l.screenPointerKnown&&l.sourceScreenText30_==text&&l.capacity==courseExpectedLineCapacity(i,j);
    }
    C* courseWidgetEntity(std::size_t i)noexcept {
        return courseOverlayTextOrdinalAdmitted(i)&&courseTextRows_[i].constructed?&courseTextRows_[i].outer->entity:nullptr;
    }
    bool courseSelectedText(std::size_t i,std::string_view& text)const noexcept {
        if(!courseOverlayTextOrdinalAdmitted(i)||!issuer_||!issuer_->sourceLocalization32COwner_)return false;
        const auto& row=courseTextRows_[i];
        if(!CourseDictionary::SelectedText::BorrowReader::readCurrent(row.selected,*issuer_,
            *issuer_->sourceLocalization32COwner_,row.lookupOrdinal,text)||
           text.size()!=std::size_t(courseStaticTextSourceRow(i)->capacity))return false;
        if(i==61&&text!="WATTO'S BRIBE: WATTO NEEDS@SPARE PARTS - HE WILL INCREASE@YOUR WINNINGS FOR EACH KO")return false;
        // Authenticate line boundaries against the actual selected byte owner.
        // AE stays the authored single byte, never UTF-8 transcoded or substituted.
        const auto* source=courseStaticTextSourceRow(i);std::size_t offset=0;
        for(std::size_t j=0;j<source->expectedLines;++j){
            const auto delimiter=text.find('@',offset);const auto end=delimiter==std::string_view::npos?text.size():delimiter;
            if(end-offset!=std::size_t(courseExpectedLineCapacity(i,j)))return false;
            if(j+1==source->expectedLines){if(delimiter!=std::string_view::npos)return false;}
            else if(delimiter==std::string_view::npos)return false;
            offset=end+1;
        }
        return text.find_first_of("\r\n")==std::string_view::npos;
    }
    static void courseTextGuard(std::atomic<std::uint8_t>& byte,std::atomic<std::int32_t>& word)noexcept {
        if(!byte.load(std::memory_order_acquire)){word.store(1,std::memory_order_relaxed);byte.store(1,std::memory_order_release);}
    }
    bool loadCourseOverlayText1CD160(std::size_t i,const assets::OriginalUiDatSection50KG& section,Diagnostic& d)noexcept {
        d.sourcePC=0x1CD384;
        if(thread_!=std::this_thread::get_id()||terminal_||!courseOverlayTextOrdinalAdmitted(i)||courseTextRows_[i].attempted||
           !issuer_||!construction_||!modeEffect_||!loaderConfig_||!loaderConfig_->parsedCurrent()||
           !courseWidgetsConfig_||!courseWidgetsConfig_->parsedCurrent()||courseWidgetOrdinal_!=i||
           loaderStopPC_!=0x1CD8A0||!courseWidgetOnTopKnown_||courseWidgetOnTopStack64C_!=0||
           courseWidgetsConfig_->config_->records_.section(courseWidgetSectionStack70_)!=&section||
           std::fegetround()!=FE_TONEAREST||
           !modeEffect_->current(*issuer_,video_,*construction_,frontend_,managerIdentity_))return false;
        const auto fail=[&](std::uint32_t pc)noexcept{d.sourcePC=pc;return false;};
        const auto* type=section.find("Type");if(!type||type->text!="Overlay_Text")return fail(0x1CD384);
        auto& r=courseTextRows_[i];r.attempted=true;
        r.outer.reset(new(std::nothrow) OriginalUiOverlayTextFields50KG); //1CD3D4.
        if(!r.outer)return fail(0x2A9038); //2A9000 reaches first getter with NULL-derived output3C.
        auto& w=*r.outer;d.entity=construction_->constructCourseOverlayText(i,w.entity);
        if(!d.entity.okay())return fail(d.entity.sourcePC);
        r.constructed=true;w.manager30=&scope_->manager_;
        //2A7760 and2A8F00 explicit stores; untouched fields have no reads yet.
        w.type34=w.field38=w.field3C=w.field40=w.field44=w.field48=0;
        w.field4C=w.field50=w.field54=w.field58=0;
        w.lines60=nullptr;w.field64=0;w.lineCount168=0;w.color17C=0;
        w.highlight194=w.highlight198=w.disabled19C=w.disabled1A0=0;
        w.height188=0;w.alpha18C=255;w.depth190=1;w.dynamic178=0;w.field1A4=0;
        const char* keys[]{"EnableOnSelect","DisableOnSelect","EnableOnDeSelect","DisableOnDeSelect"};
        std::uint32_t* flags[]{&w.field3C,&w.field40,&w.field44,&w.field48};
        for(std::size_t n=0;n<4;++n){bool value=false;const auto pc=std::uint32_t(0x2A9038+24*n);
            if(!assets::originalUiBoolean50KG(section,keys[n],false,value,d.config,pc))return fail(pc);
            *flags[n]=value;
        }
        const auto* font=section.find("FontName");
        if(!font||font->text!=courseStaticTextSourceRow(i)->font)return fail(0x2A90B0);
        std::memcpy(w.font68.data(),font->text.c_str(),font->text.size()+1);
        const auto* material=section.find("MaterialName");
        const auto materialText=material?std::string_view(material->text):std::string_view(font->text);
        if(materialText!=courseStaticTextSourceRow(i)->material||(i>=59&&i<=62&&material))return fail(0x2A90CC);
        std::memcpy(w.materialE8.data(),materialText.data(),materialText.size());w.materialE8[materialText.size()]=0;
        if(!assets::originalUiInteger50KG(section,"XPosition",0,w.x180,d.config,0x2A90E4))return fail(0x2A90E4);
        if(!assets::originalUiInteger50KG(section,"YPosition",0,w.y184,d.config,0x2A90FC))return fail(0x2A90FC);
        // Actual158AD0 writes source stack280 before the later getters. Only
        // exact cohort strings (well within20 bytes) are admitted here.
        const auto* justification=section.find("Justification");
        const std::string_view justificationText=justification?std::string_view(justification->text):std::string_view("Left");
        if(justificationText!=courseStaticTextSourceRow(i)->authoredJustification)return fail(0x2A911C);
        std::memcpy(r.justificationStack280.data(),justificationText.data(),justificationText.size());
        r.justificationStack280[justificationText.size()]=0;r.justificationStackKnown=true;
        if(!assets::originalUiNumber50KG(section,"LeadingFactor",1,w.leading174,d.config,0x2A9138))return fail(0x2A9138);
        const auto* id=section.find("TextID");
        const bool dynamic=courseStaticTextSourceRow(i)->dynamic;
        if(dynamic){
            if(id)return fail(0x2A9150);
            if(!assets::originalUiInteger50KG(section,"TextDynamicLineLength",0,w.dynamicLength16C,d.config,0x2A9174))return fail(0x2A9174);
            r.dynamicLength16CKnown=true;w.dynamic178=section.find("TextDynamicLineLength")?1:0; // Actual getter return, not capacity.
            if(!assets::originalUiInteger50KG(section,"TextDynamicLineCount",0,w.lineCount168,d.config,0x2A9190))return fail(0x2A9190);
        }else if(!id||id->text!=courseStaticTextSourceRow(i)->textId)return fail(0x2A9150);
        // TextID-present skips BOTH dynamic getters; its16C remains unknown.
        if(!w.dynamic178&&!assets::originalUiInteger50KG(section,"NumberOfLines",1,w.lineCount168,d.config,0x2A91B4))return fail(0x2A91B4);
        if(!assets::originalUiInteger50KG(section,"Alpha",255,w.alpha18C,d.config,0x2A91D0))return fail(0x2A91D0);
        if(!assets::originalUiNumber50KG(section,"Depth",1,w.depth190,d.config,0x2A91EC))return fail(0x2A91EC);
        const auto color=[&](const char* key,std::array<float,3> fallback,std::array<float,3> expected,
            std::uint32_t& output,std::uint32_t* present,std::uint32_t pc)noexcept {
            std::array<float,3> rgb{};
            if(!assets::originalUiTriple50KG(section,key,fallback,rgb,d.config,pc)||rgb!=expected)return false;
            if(present&&section.find(key))*present=1;
            output=0xff000000u|(std::uint32_t(rgb[0])<<16)|(std::uint32_t(rgb[1])<<8)|std::uint32_t(rgb[2]);return true;
        };
        if(!color("Color",{40,40,40},courseStaticTextSourceRow(i)->color,w.color17C,nullptr,0x2A921C))return fail(0x2A921C);
        if(!color("HighlightColor",{255,255,255},{255,255,255},w.highlight198,&w.highlight194,0x2A9290))return fail(0x2A9290);
        if(!color("DisabledColor",{128,128,128},{128,128,128},w.disabled1A0,&w.disabled19C,0x2A9314))return fail(0x2A9314);
        if(!assets::originalUiInteger50KG(section,"Height",0,w.height188,d.config,0x2A9380))return fail(0x2A9380);
        w.type34=4; //2A9398 delay store before actual strcmp sequence.
        if(!r.justificationStackKnown)return fail(0x2A9394);
        const std::string_view producedJustification(r.justificationStack280.data());
        w.justify170=producedJustification=="Right"?4:producedJustification=="CenterBoth"?3:
            producedJustification=="CenterVertical"?2:producedJustification=="CenterHorizontal"?1:0;
        // The real line stores justification74 later; do not pre-center authored X/Y.

        if(w.x180!=courseStaticTextSourceRow(i)->x||w.y184!=courseStaticTextSourceRow(i)->y||w.leading174!=courseStaticTextSourceRow(i)->leading||w.lineCount168!=1||w.height188||w.alpha18C!=255||w.depth190!=1)
            return fail(0x2A9438);
        std::string_view text;
        if(dynamic){if(!r.dynamicLength16CKnown||w.dynamicLength16C!=courseStaticTextSourceRow(i)->capacity||w.dynamic178!=1)return fail(0x2A9568);}
        else {
        const auto dictionary=issuer_->sourceLocalization32COwner_;
        if(!dictionary||!dictionary->current())return fail(0x2A944C);
        CourseDictionary::Observation observed;if(!dictionary->inspect(observed))return fail(0x2A9458);
        r.lookupOrdinal=0;const bool found=dictionary->lookup20ED30(id->text,r.lookupOrdinal);
        courseTextGuard(OriginalUiTextHashGuards50KG::hashGuard8098(),OriginalUiTextHashGuards50KG::hashAssertion3D6B10());
        if(OriginalUiTextHashGuards50KG::hashAssertion3D6B10().load()&&!observed.indexLoaded)return fail(0x2A94A0);
        courseTextGuard(OriginalUiTextHashGuards50KG::idGuard809C(),OriginalUiTextHashGuards50KG::idAssertion3D6B18());
        if(OriginalUiTextHashGuards50KG::idAssertion3D6B18().load()&&!found)return fail(0x2A9508);
        if(!found||r.lookupOrdinal!=courseStaticTextSourceRow(i)->lookup)return fail(0x2A9458);
        assets::OriginalApplicationLocalizationDiagnostic50KG localization;
        if(!issuer_->borrowLocalizationText18AC60(*dictionary,r.lookupOrdinal,r.selected,localization))return fail(localization.sourcePC);
        if(!courseSelectedText(i,text))return fail(0x18AC60);
        }
        // Fresh source60 is NULL; no previously loaded child is replaced in this cold cohort.
        if(dynamic){r.lineArray.reset(new(std::nothrow) C*[std::size_t(w.lineCount168)]);
            r.arrayCapacity=r.lineArray?std::size_t(w.lineCount168):0;w.lines60=r.lineArray.get();r.arrayPublished=true; //2A9574.
        }else w.lineCount168=0; //2A9B68.
        std::size_t textOffset=0;
        for(std::size_t j=0;j<courseStaticTextSourceRow(i)->expectedLines;++j){
            std::int32_t capacity=courseStaticTextSourceRow(i)->capacity;
            if(!dynamic){
                if(!courseSelectedText(i,text)||textOffset>text.size())return fail(0x18AC60);
                const auto delimiter=text.find('@',textOffset);
                const auto end=delimiter==std::string_view::npos?text.size():delimiter;
                capacity=static_cast<std::int32_t>(end-textOffset);
                if(capacity!=courseExpectedLineCapacity(i,j))return fail(0x2A9BE0);
                ++w.lineCount168; //2A9BF0 precedes actual realloc, including failure.
                std::unique_ptr<C*[]> grown(new(std::nothrow) C*[std::size_t(w.lineCount168)]);
                if(grown){for(std::size_t k=0;k<r.arrayCapacity;++k)grown[k]=r.lineArray[k];r.lineArray=std::move(grown);r.arrayCapacity=std::size_t(w.lineCount168);}
                else {r.failedReallocArray=std::move(r.lineArray);r.failedReallocCapacity=r.arrayCapacity;r.arrayCapacity=0;}
                w.lines60=r.lineArray.get();r.arrayPublished=true; //2A9C0C publishes the actual NULL result too.
            }
            r.lines[j].reset(new(std::nothrow) CourseTextLine); //2A9580 or2A9C08, even after array NULL.
            if(r.lines[j]){
                d.entity=construction_->constructCourseTextLine(i,j,w.entity,r.lines[j]->entity);
                if(!d.entity.okay())return fail(d.entity.sourcePC);
            auto& l=*r.lines[j];l.constructed=true;
            l.justification74_=3;l.enabled70_=0;l.blink7C_=0;l.timer78_=0;l.blinkOn80_=1;l.hold84_=-1;l.indefinite88_=0;l.scene8C_=nullptr;
            l.elapsed68_=l.alphaElapsed9C_=l.scaleElapsedAC_=l.rotationElapsedBC_=0;
            l.duration6C_=l.field98_=l.alphaDurationA0_=l.scaleDurationB0_=l.rotationDurationC0_=0;
            l.scaleC4_=l.scaleC8_=1;l.lockCC_=0;l.secondDelayD0_=-1;l.fieldsE0_={};l.nameF0_[0]=l.name130_[0]=0;
            l.colorDirty180_=l.alphaDirty184_=l.scaleDirty188_=l.rotationDirty18C_=0;
            l.capacity=capacity;
        }
            if(!r.lineArray)return fail(dynamic?0x2A95B4:0x2A9C3C);
            r.lineArray[j]=r.lines[j]?&r.lines[j]->entity:nullptr;r.sourceLineSlots_[j]=r.lineArray[j];r.slotKnown[j]=true;
            if(!r.lines[j])return fail(0x29CA58);
        auto& l=*r.lines[j];
        l.scene8C_=nullptr;l.scaleC4_=l.scaleC8_=1;
        l.rectangle38_[0].store(502);l.rectangle38_[1].store(315);l.rectangle38_[2].store(630);l.rectangle38_[3].store(331);
        assets::OriginalApplicationFontDiagnostic50LA fontDiagnostic;
        if(!issuer_->loadGarageTextFont29CB74(video_->fontArchive,courseScreenTextFontName(i),fontDiagnostic))return fail(fontDiagnostic.sourcePC);
        if(!issuer_->fonts328Owner_)return fail(0x29CB88);
        l.font34_=issuer_->fonts328Owner_->find282210(courseScreenTextFontName(i));
        l.sourceFont34_=l.font34_.get();
        if(!l.font34_)return fail(0x29CBAC);
        l.screenText30_.reset(new(std::nothrow) CourseScreenText);l.sourceScreenText30_=l.screenText30_.get();l.screenPointerKnown=true; //29CBC0, NULL included.
        if(!l.screenText30_)return fail(0x29CBD8); // Before2DFD90 NULL receiver body.
        OriginalUiGarageScreenTextDiagnostic50KG primitive;
        if(!l.screenText30_->loadCourseSource2DFD90(*this,i,primitive))return fail(primitive.sourcePC);
        auto& screen=*l.screenText30_;
        if(!screen.coldInputsCurrent()||screen.color54_!=0xffffffffu||screen.alpha58_!=255||screen.scale40_!=1||screen.angle44_!=0)
            return fail(0x29CBE0);
        l.alphaTarget94_.store(255);l.alphaFrom90_.store(255);l.secondAlphaD4_.store(255);
        l.scaleTargetA8_.store(1);l.scaleFromA4_.store(1);l.secondScaleD8_.store(1);
        l.rotationTargetB4_.store(0);l.rotationFromB8_.store(0);l.secondRotationDC_.store(0);l.loaded=true;
            const auto startLine=[&]()noexcept{
        l.lockCC_=0;l.enabled70_=1;l.timer78_=l.blink7C_=0;l.blinkOn80_=1;l.indefinite88_=1;
        l.alphaTarget94_.store(255);l.alpha174_.store(255);l.alphaDirty184_=1;
        l.scaleTargetA8_.store(1);l.scale178_.store(1);l.scaleDirty188_=1;
        l.rotationTargetB4_.store(0);l.rotation17C_.store(0);l.rotationDirty18C_=1;
        l.secondDelayD0_=-1;l.secondAlphaD4_.store(255);l.secondScaleD8_.store(1);l.secondRotationDC_.store(0);
            };
            if(dynamic){
                startLine(); //2A9618 before justification/color/text/position/alpha/depth/attachment.
                l.justification74_=w.justify170;l.colorDirty180_=1;l.color170_.store(w.color17C);
                // Real29D940 receives a NONNULL empty string; actual unchanged setter branch performs no stores.
                if(!screen.writeText29D990(std::string_view(""),primitive))return fail(primitive.sourcePC);
            }else{
                d.entity=construction_->attachCourseTextLine(i,j,w.entity,l.entity);if(!d.entity.okay())return fail(d.entity.sourcePC);
                l.attached=true;
                //29D940 sees the FULL remaining localized string; its own capacity truncation selects this line.
                if(!courseSelectedText(i,text)||!screen.writeText29D990(text.substr(textOffset),primitive))return fail(primitive.sourcePC?primitive.sourcePC:0x18AC60);
                l.alphaDurationA0_=0;l.alphaElapsed9C_=0;l.alphaTarget94_.store(w.alpha18C);l.alphaFrom90_.store(255);
                if(screen.depth48_!=w.depth190){screen.depth48_=w.depth190;screen.flags5A_|=2u;}
                l.justification74_=w.justify170;l.colorDirty180_=1;l.color170_.store(w.color17C);
            }
        // Actual281730 scans glyph/texture even for line ordinal0.
        const auto* glyph=l.font34_->metrics().firstGlyph();const auto* texture=l.font34_->texture281F90();float height=-1;
        if(glyph){if(!texture||!texture->sourceHeight18()||*texture->sourceHeight18()<=0)return fail(0x281788);
            height=mul(float(*texture->sourceHeight18()),sub(glyph->bottom,glyph->top));}
        if(!std::isfinite(height))return fail(0x2817AC);
        const float y=add(float(w.y184),mul(float(j),mul(w.leading174,height)));std::int32_t ypos=0;
        if(!game::roundOriginalUiTextMetric50KG(y,1,ypos))return fail(dynamic?0x2A96B4:0x2A9D4C);
        l.duration6C_=l.elapsed68_=0;l.rectangle38_[8].store(w.x180);l.rectangle38_[9].store(ypos);
        l.rectangle38_[4]=l.rectangle38_[0];l.rectangle38_[5]=l.rectangle38_[1];
        for(std::size_t n=0;n<4;++n)l.rectangle38_[n]=l.rectangle38_[8+n]; // Propagate unknown60/64, never read them.
            if(dynamic){
                l.alphaDurationA0_=0;l.alphaElapsed9C_=0;l.alphaTarget94_.store(w.alpha18C);l.alphaFrom90_.store(255);
                if(screen.depth48_!=w.depth190){screen.depth48_=w.depth190;screen.flags5A_|=2u;}
                d.entity=construction_->attachCourseTextLine(i,j,w.entity,l.entity);if(!d.entity.okay())return fail(d.entity.sourcePC);
                l.attached=true;
            }else {startLine();textOffset+=std::size_t(capacity)+1;}
        }
        r.selected.reset();r.loaded=true;d.sourcePC=0x1CD8A8;return true;
    }
    bool preflightNativeCourseOverlayTexts()const noexcept {
        if(thread_!=std::this_thread::get_id())return false;
        bool counted=false;for(const auto& row:courseTextRows_){counted|=row.constructed;
            for(const auto& line:row.lines)counted|=line&&line->constructed;}
        // Global count/topology and payload proofs precede ANY resource withdrawal.
        if(counted&&(!construction_||construction_->pageAcquired_||!construction_->registrationCurrent()))return false;
        for(std::size_t i=0;i<courseTextRows_.size();++i){const auto& row=courseTextRows_[i];
            if(row.constructed&&(!row.outer||construction_->courseOverlayOuters_[i]!=&row.outer->entity))return false;
            if(row.arrayPublished&&!row.arrayReleased&&(!row.outer||row.outer->lines60!=row.lineArray.get()))return false;
            if(row.lineArray&&row.failedReallocArray)return false;
            const C* const* backing=row.lineArray?row.lineArray.get():row.failedReallocArray.get();
            const auto slots=row.lineArray?row.arrayCapacity:row.failedReallocCapacity;
            for(std::size_t j=0;j<10;++j){
                if(row.slotKnown[j]&&!row.arrayReleased&&(!backing||j>=slots||backing[j]!=row.sourceLineSlots_[j]))return false;
                if(row.lines[j]){const auto& line=*row.lines[j];
                    if(line.constructed&&(!row.constructed||construction_->courseTextLines_[i][j]!=&line.entity))return false;
                    if(row.slotKnown[j]&&row.sourceLineSlots_[j]!=&line.entity)return false;
                    if(!line.fontReleased&&line.font34_.get()!=line.sourceFont34_)return false;
                    if(line.screenText30_){
                        if(line.screenRetired||!line.screenPointerKnown||line.sourceScreenText30_!=line.screenText30_.get()||
                           !ownsCourseScreenTextOccurrence(line.screenText30_.get(),i))return false;
                        OriginalUiGarageScreenTextDiagnostic50KG diagnostic;
                        if(!line.screenText30_->preflightRetirement2E1340(diagnostic))return false;
                    }else if(line.sourceScreenText30_&&!line.screenRetired)return false;
                }else if(counted&&construction_->courseTextLines_[i][j])return false;
            }
        }
        return true;
    }
    bool abortNativeCourseOverlayTextRange(std::size_t first,std::size_t end)noexcept {
        if(!preflightNativeCourseOverlayTexts()||first>end||end>courseTextRows_.size())return false;
        for(std::size_t later=end;later<courseTextRows_.size();++later){
            if(courseTextRows_[later].outer)return false;
            for(const auto& line:courseTextRows_[later].lines)if(line)return false;
        }
        for(std::size_t n=end;n>first;--n){const auto i=n-1;auto& row=courseTextRows_[i];
            bool acquired=row.constructed;for(const auto& line:row.lines)acquired|=line&&line->constructed;
            if(acquired&&(!construction_||!construction_->courseOverlayRetirementAdmitted(i)))return false;
            // Native abort, NOT original2A8F70 completion. Preserve actual source pointers as history.
            // Per-line tail gate precedes payload withdrawal; this remains safe after a prior line retired.
            for(std::size_t nline=10;nline>0;--nline){const auto j=nline-1;
                if(!row.lines[j])continue;auto& line=*row.lines[j];
                if(line.constructed&&(!construction_||!construction_->courseTextLineRetirementAdmitted(i,j)))return false;
                if(line.screenText30_){OriginalUiGarageScreenTextDiagnostic50KG d;
                    if(!line.screenText30_->preflightRetirement2E1340(d)||!line.screenText30_->retire2E1340(d))return false;
                    line.screenRetired=true;line.screenText30_.reset();
                }
                line.fontReleased=true;line.font34_.reset();
                if(line.constructed){const auto step=construction_->abortNativeCourseTextLine(i,j,row.outer->entity,line.entity);
                    if(!step.okay())return false;}
                row.lines[j].reset();
            }
            row.selected.reset();
            row.lineArray.reset();row.failedReallocArray.reset();row.arrayReleased=true;
            if(row.constructed){const auto step=construction_->abortNativeCourseOverlayText(i,row.outer->entity);
                if(!step.okay())return false;row.constructed=false;}
            row.outer.reset();
        }
        return true;
    }
    bool abortNativeCourseFinalOverlayTexts()noexcept{return abortNativeCourseOverlayTextRange(67,84);}
    bool abortNativeCourseNewestOverlayTexts()noexcept{return abortNativeCourseOverlayTextRange(59,67);}
    bool abortNativeCourseLateOverlayTexts()noexcept{return abortNativeCourseOverlayTextRange(26,59);}
    bool abortNativeCourseOverlayTexts()noexcept{return abortNativeCourseOverlayTextRange(0,2);}
