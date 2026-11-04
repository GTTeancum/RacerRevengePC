//Included after FreshFileCapacity and shared numeric source-bank I/O.
bool OriginalSourceSkyFile50LA::sourceReadBoundsCurrentNative()const noexcept {
    const auto check=[](const auto& row,std::size_t requested,std::uint32_t count,bool initializedSuffix=false){
        const bool positiveClamp=row.copied&&row.copied<requested;
        if(row.pendingSourceItems)return row.attempted&&!row.returned&&!row.items&&positiveClamp&&
            *row.pendingSourceItems==count&&row.before&&row.after&&*row.after==*row.before+row.copied;
        // Legacy consumers may assume known entire items only in admitted full
        //orzero cases. Never admit the old positive-clamp copied/size result.
        if(row.returned&&positiveClamp)return initializedSuffix&&row.attempted&&row.items==count&&
            row.before&&row.after&&*row.after==*row.before+row.copied;
        return true;
    };
    //129320 initialized count7D8. Item128880 initialized header48 and every
    //parsed receiver field; field0 is a distinct, uninitialized stack flags halfword.
    if(!check(read_,4,1)||!check(countRead_,4,1,true)||!check(paletteOffsetRead_,4,1))return false;
    constexpr std::size_t sizes[3]={0x300,0x8000,0x10000};
    for(std::size_t i=0;i<3;++i)if(!check(paletteHeaderReads_[i],4,1)||!check(rawPaletteReads_[i],sizes[i],1))return false;
    for(const auto& pointer:followingPalettes_){if(!pointer)return false;const auto& row=*pointer;
        if(!check(row.offsetRead,4,1))return false;
        for(std::size_t i=0;i<3;++i)if(!check(row.headers[i],4,1)||!check(row.payloads[i],sizes[i],1))return false;}
    for(std::size_t i=0;i<(firstItem_?followingItems_.size()+1:0);++i){const auto* row=itemOccurrence(i);if(!row)return false;
        if(!check(row->offsetRead,4,1)||!check(row->header48,4,1,true))return false;
        for(std::size_t j=0;j<row->fields.size();++j)if(!check(row->fields[j],itemFieldSpec(j).size,1,j!=0))return false;}
    for(std::size_t item=0;item<itemResourceSeeks_.size();++item){const auto& pointer=itemResourceSeeks_[item];if(!pointer)return false;
      for(std::size_t ordinal=0;ordinal<=pointer->resourceEntries.size();++ordinal){const auto* resource=resourceFileOccurrence(item,ordinal);if(!resource)return false;const auto& row=*resource;
        if(!check(row.resourceSizeA8,4,1)||!check(row.compressedPayloadSize,4,1,true)||
           !check(row.rawPayload,row.rawPayload.requested,static_cast<std::uint32_t>(row.rawPayload.requested)))return false;
        //Only B0/B4/B8/BC have source zero initialization before the call.
        //AC and unproved legacy words retain their bounds. The admitted indexed
        //offset/size calls below have separately source-initialized destinations.
        for(std::size_t header=0;header<row.compressedHeaders.size();++header)
            if(!check(row.compressedHeaders[header],4,1,header!=0))return false;
        for(const auto& step:row.compressedOffsets)if(!check(step.read,4,1,true))return false;
        //New compressed byte transfer explicitly models the source return and
        //tracks unknown suffix separately; it does not use the legacy bound.
        if(row.compressedPayload.pendingSourceItems)return false;
        for(const auto& size:row.followingCompressedSizes)if(!size||!check(*size,4,1,true))return false;
        for(const auto& payload:row.followingCompressedPayloads)if(!payload||payload->pendingSourceItems)return false;
        for(const auto& step:row.rawExtendedOffsets)if(!check(step.read,4,1,true))return false;
        //New row readers separately validate original item counts versus real
        //copied/known bytes. A full-clamped read may return requested rows.
        for(const auto& payload:row.rawExtendedRows)if(!payload||payload->pendingSourceItems)return false;
    }
    }
    return true;
}

bool OriginalSourceSkyFile50LA::preflightNative()const noexcept {
    if(!sourceReadBoundsCurrentNative())return false;
    if(thread_!=std::this_thread::get_id()||(borrowed_&&!attempted_)||
       (searchReturned_&&(!before168_||!after168_))||(found_&&!searchReturned_)||
       (openReturned_&&!searchReturned_)||(sourceHandle_.has_value()!=openReturned_)||
       (sourceHandle_&&(*sourceHandle_<-1||(!found_&&*sourceHandle_!=-1)||(found_&&*sourceHandle_<0)))||
       (read_.attempted&&(!openReturned_||!sourceHandle_||*sourceHandle_<0))||
       (read_.copied>4)||(read_.returned&&(!read_.attempted||!read_.before||!read_.after||!read_.items))||
       (read_.items&&(!read_.returned||*read_.items!=read_.copied/4))||
       (read_.returned&&*read_.after!=*read_.before+read_.copied))return false;
    const Read4* previous=nullptr;
    for(const auto* row:{&read_,&countRead_,&paletteOffsetRead_}){
        const auto items=row==&countRead_?(row->copied?1u:0u):row->copied/4;
        if(row->copied>4||(row->attempted&&(!openReturned_||!sourceHandle_||*sourceHandle_<0))||
           (row->returned&&(!row->attempted||!row->before||!row->after||!row->items))||
           (row->items&&(!row->returned||*row->items!=items))||
           (row->returned&&*row->after!=*row->before+row->copied)||
           (row->attempted&&previous&&(!previous->returned||previous->items!=1u||row->before!=previous->after)))return false;
        previous=row;
    }
    std::optional<std::size_t> latest=seekReturned_?resourceSeekTarget_:paletteOffsetRead_.after?paletteOffsetRead_.after:
        countRead_.after?countRead_.after:read_.after;
    const auto compressedPaletteCurrent=[&](const auto& blocks,const auto& headers,const void* receiver,bool retired)noexcept {
        std::array<std::uint8_t,4> sizeWord{};unsigned known=0;
        constexpr std::size_t outputSizes[3]={0x300,0x8000,0x10000},offsets[3]={0x18,0x718,0x8718};
        for(unsigned i=0;i<3;++i){const auto& block=blocks[i];const auto& size=block.size;const auto& payload=block.payload;
            if(size.pendingSourceItems||payload.pendingSourceItems||size.copied>4||
               (size.attempted&&(headers[2].items!=1u||(i&&blocks[i-1].payload.items!=1u)||size.before!=latest))||
               (size.returned&&(!size.attempted||!size.before||!size.after||size.items!=(size.copied?1u:0u)))||
               (size.items&&!size.returned)||(size.after&&(!size.before||*size.after!=*size.before+(size.returned?size.copied:0u))))return false;
            if(size.attempted){std::uint32_t encoding=0;for(unsigned k=0;k<4;++k)encoding|=std::uint32_t(headers[2].bytes[k])<<(8*k);if(!encoding)return false;}
            for(std::size_t k=0;k<size.copied;++k){sizeWord[k]=size.bytes[k];known|=1u<<k;}
            if(size.after)latest=size.after;
            if(!payload.attempted){if(payload.copied||payload.returned||payload.items||payload.before||payload.after||block.requested||block.direct||block.destination||block.nativeWithdrawn)return false;continue;}
            std::uint32_t length=0;if(known!=15u||!size.returned||size.items!=1u)return false;
            for(unsigned k=0;k<4;++k)length|=std::uint32_t(sizeWord[k])<<(8*k);
            if(length>0x10000u||block.requested!=length||block.direct!=(length==outputSizes[i])||payload.copied>length||payload.before!=latest||
               (payload.returned&&(!payload.before||!payload.after||payload.items!=(payload.copied?1u:0u)||!block.nativeWithdrawn))||
               (payload.items&&!payload.returned)||(payload.after&&(!payload.before||*payload.after!=*payload.before+(payload.returned?payload.copied:0u)))||
               (block.nativeWithdrawn&&(block.destination||(!payload.returned&&!retired)))||(!block.nativeWithdrawn&&!block.destination))return false;
            if(block.direct&&!block.nativeWithdrawn&&(!receiver||block.destination!=static_cast<const std::uint8_t*>(receiver)+offsets[i]))return false;
            if(payload.after)latest=payload.after;
        }
        return true;
    };
    const Read4* previousPalette=nullptr;
    for(const auto& row:paletteHeaderReads_){
        if(row.copied>4||(row.attempted&&(!paletteEntered_||!seekReturned_||row.before!=latest))||
           (row.returned&&(!row.attempted||!row.before||!row.after||!row.items))||
           (row.items&&(!row.returned||*row.items!=row.copied/4))||
           (row.returned&&*row.after!=*row.before+row.copied)||
           (row.attempted&&previousPalette&&previousPalette->items!=1u))return false;
        if(row.after)latest=row.after;previousPalette=&row;
    }
    if(!compressedPaletteCurrent(compressedPaletteReads_,paletteHeaderReads_,paletteReceiver_,paletteWithdrawn_||paletteReleasedSource_))return false;
    constexpr std::size_t payloadSizes[3]={0x300,0x8000,0x10000};
    for(std::size_t i=0;i<rawPaletteReads_.size();++i){const auto& row=rawPaletteReads_[i];
        if(row.copied>payloadSizes[i]||(row.attempted&&(!paletteEntered_||row.before!=latest||
           paletteHeaderReads_[2].items!=1u||(i&&rawPaletteReads_[i-1].items!=1u)))||
           (row.returned&&(!row.attempted||!row.before||!row.after||!row.items))||
           (row.items&&(!row.returned||*row.items!=row.copied/payloadSizes[i]))||
           (row.returned&&*row.after!=*row.before+row.copied))return false;
        if(row.attempted){for(const auto byte:paletteHeaderReads_[2].bytes)if(byte)return false;}
        if(row.after)latest=row.after;
    }
    if((restoreAttempted_&&!seekReturned_)||(restoreReturned_&&(!restoreAttempted_||!restoreTarget_||restoreTarget_!=resourceStart_)))return false;
    if(restoreReturned_)latest=restoreTarget_;
    for(const auto& owned:followingPalettes_){
        if(!owned||!restoreReturned_)return false;const auto& step=*owned;const auto& row=step.offsetRead;
        if(row.copied>4||(row.attempted&&row.before!=latest)||
           (row.returned&&(!row.attempted||!row.before||!row.after||!row.items))||
           (row.items&&(!row.returned||*row.items!=row.copied/4))||
           (row.returned&&*row.after!=*row.before+row.copied)||
           (step.tellAttempted&&row.items!=1u)||(step.tellReturned&&(!step.tellAttempted||!step.start||step.start!=row.after))||
           (step.seekAttempted&&!step.tellReturned)||(step.seekReturned&&(!step.seekAttempted||!step.target)))return false;
        if(row.after)latest=row.after;if(step.seekReturned)latest=step.target;
        if((step.parserEntered&&!step.seekReturned)||(step.parserWithdrawn&&!step.parserEntered)||
           (step.parserEntered&&!step.parserWithdrawn&&!step.parserReleasedSource&&!step.parserReceiver)||((step.parserWithdrawn||step.parserReleasedSource)&&step.parserReceiver))return false;
        if((step.parserReleasedSource&&(!step.parserEntered||!step.scanReturned))||
           (step.scanMatchDetected&&(!step.scanAttempted||step.scanReturned||step.parserReleasedSource||step.parserWithdrawn))||
           (step.scanReturned&&(!step.scanAttempted||!step.scanCount||*step.scanCount<2||
              step.scanVisited!=static_cast<std::uint32_t>(*step.scanCount))))return false;
        for(std::size_t i=0;i<3;++i){const auto& read=step.headers[i];
            if(read.copied>4||(read.attempted&&(!step.parserEntered||read.before!=latest||(i&&step.headers[i-1].items!=1u)))||
               (read.returned&&(!read.attempted||!read.before||!read.after||!read.items))||
               (read.items&&(!read.returned||*read.items!=read.copied/4))||
               (read.returned&&*read.after!=*read.before+read.copied))return false;
            if(read.after)latest=read.after;
        }
        if(!compressedPaletteCurrent(step.compressed,step.headers,step.parserReceiver,step.parserWithdrawn||step.parserReleasedSource))return false;
        for(std::size_t i=0;i<3;++i){
            const auto& read=step.payloads[i];
            if(read.copied>payloadSizes[i]||(read.attempted&&(!step.parserEntered||step.headers[2].items!=1u||
               read.before!=latest||(i&&step.payloads[i-1].items!=1u)))||
               (read.returned&&(!read.attempted||!read.before||!read.after||!read.items))||
               (read.items&&(!read.returned||*read.items!=read.copied/payloadSizes[i]))||
               (read.returned&&*read.after!=*read.before+read.copied))return false;
            if(read.attempted){for(const auto byte:step.headers[2].bytes)if(byte)return false;}
            if(read.after)latest=read.after;
        }
        if((step.restoreAttempted&&!step.seekReturned)||(step.restoreReturned&&(!step.restoreAttempted||
           !step.restoreTarget||step.restoreTarget!=step.start)))return false;
        if(step.restoreReturned)latest=step.restoreTarget;
    }
    if(!firstItem_&&!followingItems_.empty())return false;
    for(std::size_t occurrence=0;occurrence<(firstItem_?followingItems_.size()+1:0);++occurrence){
        const auto* selected=itemOccurrence(occurrence);if(!selected||(occurrence&&!itemOccurrence(occurrence-1)->restoreReturned))return false;
        const auto& item=*selected;const auto& row=item.offsetRead;
        if(row.copied>4||(row.attempted&&row.before!=latest)||
           (row.returned&&(!row.attempted||!row.before||!row.after||!row.items))||
           (row.items&&(!row.returned||*row.items!=row.copied/4))||
           (row.returned&&*row.after!=*row.before+row.copied)||
           (item.tellAttempted&&row.items!=1u)||(item.tellReturned&&(!item.tellAttempted||!item.start||item.start!=row.after))||
           (item.seekAttempted&&!item.tellReturned)||(item.seekReturned&&(!item.seekAttempted||!item.target))||
           (item.parserEntered&&!item.seekReturned)||(item.parserEntered&&!item.nativeWithdrawn&&!item.parserReleasedSource&&!item.receiver)||
           ((item.nativeWithdrawn||item.parserReleasedSource)&&item.receiver))return false;
        if(item.parserReleasedSource){bool failed=item.header48.returned&&item.header48.items!=1u;
            for(const auto& field:item.fields)if(field.returned&&field.items!=1u)failed=true;
            if(!item.parserEntered||!failed||item.tailEntered||item.restoreAttempted)return false;}
        if(row.after)latest=row.after;if(item.seekReturned)latest=item.target;
        const auto& read=item.header48;
        if(read.copied>4||(read.attempted&&(!item.parserEntered||read.before!=latest))||
           (read.returned&&(!read.attempted||!read.before||!read.after||!read.items))||
           (read.items&&(!read.returned||*read.items!=(read.copied?1u:0u)))||
           (read.returned&&*read.after!=*read.before+read.copied))return false;
        if(read.after)latest=read.after;
        for(std::size_t i=0;i<item.fields.size();++i){const auto& field=item.fields[i];const auto spec=itemFieldSpec(i);
            const auto items=i?(field.copied?1u:0u):field.copied/spec.size;
            if(field.copied>spec.size||(field.attempted&&(!item.parserEntered||!itemFieldRequired(item,i)||field.before!=latest||
               itemFieldPredecessor(item,i)!=1u))||
               (field.returned&&(!field.attempted||!field.before||!field.after||!field.items))||
               (field.items&&(!field.returned||*field.items!=items))||
               (field.returned&&*field.after!=*field.before+field.copied))return false;
            if(field.after)latest=field.after;
        }
        if(item.tailEntered){
            for(std::size_t i=0;i<item.fields.size();++i)if(itemFieldRequired(item,i)&&item.fields[i].items!=1u)return false;
            if(item.alignmentSelected!=bool(item.fields[0].bytes[0]&2u))return false;
        }
        if((item.alignmentTellAttempted&&(!item.tailEntered||!item.alignmentSelected))||
           (item.alignmentTellReturned&&(!item.alignmentTellAttempted||!item.alignmentPosition||item.alignmentPosition!=latest))||
           (item.alignmentPadding&&(!item.alignmentTellReturned||*item.alignmentPosition>0x7fffffffu||
            *item.alignmentPadding!=2048u-(*item.alignmentPosition%2048u)))||
           (item.alignmentSeekAttempted&&(!item.alignmentPadding||*item.alignmentPadding==2048u||
            !item.alignmentTarget||*item.alignmentTarget!=*item.alignmentPosition+*item.alignmentPadding))||
           (item.alignmentSeekReturned&&!item.alignmentSeekAttempted))return false;
        if(item.alignmentSeekReturned)latest=item.alignmentTarget;
        if((item.finalTellAttempted&&(!item.tailEntered||(item.alignmentSelected&&(!item.alignmentPadding||
           (*item.alignmentPadding!=2048u&&!item.alignmentSeekReturned)))))||
           (item.finalTellReturned&&(!item.finalTellAttempted||!item.finalPosition||item.finalPosition!=latest)))return false;
        if((item.restoreAttempted&&(!item.finalTellReturned||!item.start||item.restoreTarget!=item.start))||
           (item.restoreReturned&&(!item.restoreAttempted||item.restoreResult!=0))||
           (item.restoreResult&&(!item.restoreReturned||*item.restoreResult!=0)))return false;
        if(item.restoreReturned)latest=item.restoreTarget;
    }
    for(std::size_t i=0;i<itemResourceSeeks_.size();++i){const auto* row=itemResourceSeeks_[i].get();const auto* item=itemOccurrence(i);
        if(!row||row->occurrence!=i||!item||!item->restoreReturned||!item->finalPosition||
           (row->attempted&&(row->before!=latest||!row->target||*row->target!=static_cast<std::uint32_t>(*item->finalPosition)))||
           (row->returned&&(!row->attempted||row->result!=0))||(row->result&&(!row->returned||*row->result!=0)))return false;
        if(row->returned)latest=row->target;
        const auto* root=row;if(root->resourceEntries.size()>95)return false;
        for(std::size_t ordinal=0;ordinal<=root->resourceEntries.size();++ordinal){
        const auto* row=resourceFileOccurrence(i,ordinal);if(!row||row->resourceOrdinal!=ordinal)return false;
        if(ordinal){const auto* previous=resourceFileOccurrence(i,ordinal-1);if(!previous||!previous->resourceAlignmentReturned||!row->entryOpened)return false;}
        if(row->entryOpened!=(ordinal?true:root->returned)||(row->entryOpened&&row->entryCursor!=latest)||(!row->entryOpened&&row->entryCursor))return false;
        const auto& read=row->resourceSizeA8;
        if(read.copied>4||(read.attempted&&(!row->entryOpened||read.before!=latest))||
           (read.returned&&(!read.attempted||!read.before||!read.after||!read.items))||
           (read.items&&(!read.returned||*read.items!=read.copied/4))||
           (read.returned&&*read.after!=*read.before+read.copied))return false;
        if(read.after)latest=read.after;
        for(std::size_t header=0;header<5;++header){const auto& part=row->compressedHeaders[header];
            if(part.copied>4||(part.attempted&&(!read.returned||part.before!=latest||(header&&!row->compressedHeaders[header-1].returned)))||
               (part.returned&&(!part.attempted||!part.before||!part.after||!part.items))||
               (part.items&&(!part.returned||*part.items!=(header?(part.copied?1u:0u):part.copied/4)))||
               (part.returned&&*part.after!=*part.before+part.copied))return false;
            if(part.after)latest=part.after;
        }
        const auto validTell=[&](const CompressedTell& tell){return
            (!tell.returned||(tell.attempted&&tell.position&&tell.position==latest))&&(!tell.position||tell.returned);};
        if(!validTell(row->compressedInitialTell)||(row->compressedInitialTell.attempted&&!row->compressedHeaders[4].returned))return false;
        for(std::size_t j=0;j<12;++j){const auto& part=row->compressedOffsets[j];const auto& rd=part.read;
            if(!validTell(part.tell)||(part.tell.attempted&&(!row->compressedInitialTell.returned||
               (j&&(row->compressedOffsets[j-1].read.items!=1u||row->compressedOffsets[j-1].terminal))))||
               rd.copied>4||(rd.attempted&&(!part.tell.returned||rd.before!=latest))||
               (rd.returned&&(!rd.attempted||!rd.before||!rd.after||!rd.items))||
               (rd.items&&(!rd.returned||*rd.items!=(rd.copied?1u:0u)))||
               (rd.after&&(!rd.before||*rd.after!=*rd.before+rd.copied)))return false;
            if(rd.after)latest=rd.after;
        }
        const auto& sizeRead=row->compressedPayloadSize;
        bool terminalComplete=false;for(const auto& part:row->compressedOffsets)if(part.terminal&&part.read.items==1u)terminalComplete=true;
        if(sizeRead.copied>4||(sizeRead.attempted&&(!terminalComplete||sizeRead.before!=latest))||
           (sizeRead.returned&&(!sizeRead.attempted||!sizeRead.before||!sizeRead.after||!sizeRead.items))||
           (sizeRead.items&&(!sizeRead.returned||*sizeRead.items!=(sizeRead.copied?1u:0u)))||
           (sizeRead.after&&(!sizeRead.before||*sizeRead.after!=*sizeRead.before+sizeRead.copied)))return false;
        if(sizeRead.after)latest=sizeRead.after;
        const auto& compressed=row->compressedPayload;
        if(compressed.copied>compressed.requested||(compressed.attempted&&(!sizeRead.returned||compressed.before!=latest))||
           (compressed.returned&&(!compressed.attempted||!compressed.before||!compressed.after||!compressed.items))||
           (compressed.items&&(!compressed.returned||*compressed.items!=(compressed.copied?compressed.requested:0u)))||
           (compressed.returned&&*compressed.after!=*compressed.before+compressed.copied)||
           (compressed.copied&&!compressed.nativeWithdrawn&&!compressed.destination)||
           (compressed.nativeWithdrawn&&compressed.destination))return false;
        if(compressed.after)latest=compressed.after;
        //File chronology is size[j], payload[j], size[j+1], never two
        //separate traversals. A missing/partial final occurrence stays final.
        if(row->followingCompressedSizes.size()>4||row->followingCompressedPayloads.size()>row->followingCompressedSizes.size())return false;
        for(std::size_t j=0;j<row->followingCompressedSizes.size();++j){
            const auto& pointer=row->followingCompressedSizes[j];if(!pointer)return false;const auto& next=*pointer;
            const auto* prior=j?row->followingCompressedPayloads.size()>=j?row->followingCompressedPayloads[j-1].get():nullptr:&compressed;
            if(!prior||!prior->returned||prior->items!=prior->requested||next.copied>4||
               (next.attempted&&next.before!=latest)||
               (next.returned&&(!next.attempted||!next.before||!next.after||!next.items))||
               (next.items&&(!next.returned||*next.items!=(next.copied?1u:0u)))||
               (next.returned&&*next.after!=*next.before+next.copied))return false;
            if(next.after)latest=next.after;
            if(j>=row->followingCompressedPayloads.size())continue;
            const auto& payloadPointer=row->followingCompressedPayloads[j];if(!payloadPointer)return false;const auto& part=*payloadPointer;
            if(!next.returned||part.copied>part.requested||
               (part.attempted&&part.before!=latest)||
               (part.returned&&(!part.attempted||!part.before||!part.after||!part.items))||
               (part.items&&(!part.returned||*part.items!=(part.copied?part.requested:0u)))||
               (part.returned&&*part.after!=*part.before+part.copied)||
               (part.copied&&!part.nativeWithdrawn&&!part.destination)||(part.nativeWithdrawn&&part.destination))return false;
            if(part.after)latest=part.after;
        }
        if(!validTell(row->compressedFinalTell))return false;
        if(row->compressedFinalTell.attempted){const auto* last=row->followingCompressedPayloads.empty()?&compressed:row->followingCompressedPayloads.back().get();
            if(!last||!last->returned||last->items!=last->requested||row->followingCompressedSizes.size()!=row->followingCompressedPayloads.size())return false;}
        if(row->rawExtendedEntered&&(!row->entryOpened||row->resourceSizeA8.attempted||row->compressedHeaders[0].attempted||row->rawPayload.attempted))return false;
        if(!validTell(row->rawExtendedInitialTell)||(row->rawExtendedInitialTell.attempted&&!row->rawExtendedEntered))return false;
        bool rawTerminal=false;
        for(std::size_t j=0;j<12;++j){const auto& part=row->rawExtendedOffsets[j];const auto& rd=part.read;
            if(!validTell(part.tell)||(part.tell.attempted&&(!row->rawExtendedInitialTell.returned||
               (j&&(row->rawExtendedOffsets[j-1].read.items!=1u||row->rawExtendedOffsets[j-1].terminal))))||
               rd.copied>4||(rd.attempted&&(!part.tell.returned||rd.before!=latest))||
               (rd.returned&&(!rd.attempted||!rd.before||!rd.after||rd.items!=(rd.copied?1u:0u)))||
               (rd.items&&!rd.returned)||(rd.after&&(!rd.before||*rd.after!=*rd.before+(rd.returned?rd.copied:0u))))return false;
            if(rd.after)latest=rd.after;
            if(part.terminal&&rd.items==1u)rawTerminal=true;
        }
        if(row->rawExtendedRows.size()>10)return false;
        for(std::size_t j=0;j<row->rawExtendedRows.size();++j){const auto& pointer=row->rawExtendedRows[j];if(!pointer)return false;const auto& part=*pointer;
            if(!rawTerminal||part.dimension!=(1u<<j)||part.rows!=part.dimension||
               (part.rowBytes!=part.dimension*2u&&part.rowBytes!=part.dimension*4u)||part.requested!=std::size_t(part.rowBytes)*part.rows||
               (j&&(!row->rawExtendedRows[j-1]->returned||row->rawExtendedRows[j-1]->items!=row->rawExtendedRows[j-1]->rows))||
               part.copied>part.requested||(part.attempted&&part.before!=latest)||
               (part.returned&&(!part.attempted||!part.before||!part.after||part.items!=(part.copied?part.rows:0u)))||
               (part.items&&!part.returned)||(part.after&&(!part.before||*part.after!=*part.before+(part.returned?part.copied:0u)))||
               (part.attempted&&!part.nativeWithdrawn&&!part.destination)||(part.nativeWithdrawn&&part.destination))return false;
            if(part.after)latest=part.after;
        }
        const auto& payload=row->rawPayload;
        if(payload.copied>payload.requested||(payload.attempted&&(!row->entryOpened||payload.before!=latest))||
           (payload.returned&&(!payload.attempted||!payload.before||!payload.after||!payload.items))||
           (payload.items&&(!payload.returned||*payload.items!=payload.copied))||
           (payload.returned&&*payload.after!=*payload.before+payload.copied)||
           (payload.attempted&&!payload.nativeWithdrawn&&!payload.destination)||
           (payload.nativeWithdrawn&&payload.destination))return false;
        if(payload.after)latest=payload.after;
        const auto& alignment=row->resourceAlignmentTell;
        if(!validTell(alignment)||(alignment.attempted&&(!row->resourceTailEntered||!row->resourceAlignmentSelected))||
           (row->resourceAlignmentPadding&&(!alignment.returned||*alignment.position>0x7fffffffu||*row->resourceAlignmentPadding!=2048u-(*alignment.position%2048u)))||
           (row->resourceAlignmentSeekAttempted&&(!row->resourceAlignmentPadding||*row->resourceAlignmentPadding==2048u||!row->resourceAlignmentTarget||*row->resourceAlignmentTarget!=*alignment.position+*row->resourceAlignmentPadding))||
           (row->resourceAlignmentSeekReturned&&!row->resourceAlignmentSeekAttempted))return false;
        if(row->resourceAlignmentSeekReturned)latest=row->resourceAlignmentTarget;
        if(row->resourceAlignmentReturned&&(!row->resourceTailEntered||(row->resourceAlignmentSelected&&(!row->resourceAlignmentPadding||(*row->resourceAlignmentPadding!=2048u&&!row->resourceAlignmentSeekReturned)))))return false;

    }
    }
    if((paletteEntered_&&!seekReturned_)||(paletteWithdrawn_&&!paletteEntered_)||
       (paletteEntered_&&!paletteWithdrawn_&&!paletteReleasedSource_&&!paletteReceiver_)||
       ((paletteWithdrawn_||paletteReleasedSource_)&&paletteReceiver_))return false;
    if((paletteReleasedSource_&&(!paletteEntered_||!paletteScanReturned_))||
       (paletteScanMatchDetected_&&(!paletteScanAttempted_||paletteScanReturned_||paletteReleasedSource_||paletteWithdrawn_))||
       (paletteScanReturned_&&(!paletteScanAttempted_||!paletteScanCount_||*paletteScanCount_<2||
          paletteScanVisited_!=static_cast<std::uint32_t>(*paletteScanCount_))))return false;
    if((countRead_.attempted&&!receiver_)||(tellAttempted_&&paletteOffsetRead_.items!=1u)||
       (tellReturned_&&(!tellAttempted_||!resourceStart_||resourceStart_!=paletteOffsetRead_.after))||
       (seekAttempted_&&!tellReturned_)||(seekReturned_&&(!seekAttempted_||!resourceSeekTarget_))||
       (expectedCursor_!=latest))return false;
    if(nativeClosed_)return !borrowed_&&!resolver_&&!provider_&&!root_&&!selectedRoot_&&handle_==-1;
    if(!attempted_)return !borrowed_&&!resolver_&&!provider_&&!root_&&handle_==-1;
    if(!borrowed_||!resolver_||!provider_||!root_||!resolver_->freshFileCapacity_||!resolver_->scratch_)return false;
    const auto& s=*resolver_->freshFileCapacity_;
    if(s.sourceSkyFileBorrower!=this||s.provider!=provider_||!provider_->sealed()||
       s.track.root!=root_.get()||!s.track.joined||s.track.generation!=trackGeneration_)return false;
    if(handle_>=0){
        if(!selectedRoot_||!resolver_->sourceBankMemberCurrent(selectedRoot_,this,handle_))return false;
        if(sourceHandle_&&*sourceHandle_!=handle_)return false;
        if(expectedCursor_&&s.members150[handle_].cursor10!=*expectedCursor_)return false;
    }else if(sourceHandle_&&*sourceHandle_>=0)return false;
    return true;
}
bool OriginalSourceSkyFile50LA::current()const noexcept {
    return preflightNative()&&!nativeClosed_&&borrowed_&&openReturned_&&sourceHandle_&&*sourceHandle_>=0&&handle_>=0;
}
bool OriginalSourceSkyFile50LA::begin131230(std::shared_ptr<OriginalStandardTextureResolver50LA> resolver,
    std::shared_ptr<OriginalStandardArchiveProvider50LA> provider,std::shared_ptr<OriginalRaTrackSourceFile50LA> root,
    const std::string& name,D& d)noexcept {
    d={};if(attempted_||nativeClosed_||thread_!=std::this_thread::get_id()||!resolver||!provider||!root||
       !provider->sealed()||!resolver->freshFileCapacity_||!resolver->scratch_||name.empty()||
       name.find('\0')!=std::string::npos)return fail(d,E::phase,0x131230);
    auto& s=*resolver->freshFileCapacity_;
    if(s.provider!=provider||s.track.root!=root.get()||!s.track.joined||!s.track.cacheReturned||s.sourceSkyFileBorrower)
        return fail(d,E::phase,0x131230);
    try{name_=name;}catch(...){return fail(d,E::allocation,0x131230);}
    resolver_=std::move(resolver);provider_=std::move(provider);root_=std::move(root);
    attempted_=true;trackGeneration_=s.track.generation;s.sourceSkyFileBorrower=this;borrowed_=true;pc_=0x1315b0;
    if(!s.members150||s.memberCount154<=0||s.cursor168< -1||s.cursor168>=s.memberCount154)
        return fail(d,E::phase,pc_);
    before168_=s.cursor168;++s.cursor168;if(s.cursor168==s.memberCount154)s.cursor168=0;
    auto index=static_cast<std::int32_t>(s.cursor168);const auto start=index;
    do{const auto& row=s.members150[index];
        if(row.filename08&&courseSourceName308F00(name_,row.filename08)){found_=true;break;}
        if(++index==s.memberCount154)index=0;
    }while(index!=start);
    if(found_)s.cursor168=index;
    after168_=s.cursor168;searchReturned_=true;d.sourceEffectsCommitted=true;
    if(!found_){
        pc_=0x131260;observedMode170_=s.sourceFileMode170;
        if(*observedMode170_!=0){pc_=0x12dcb0;return fail(d,E::unsupported,pc_);}
        //Actual mode literal rb contains r, no plus:131348 signed -1, no fallback.
        pc_=0x131348;sourceHandle_=-1;openReturned_=true;pc_=0x131378;return true;
    }
    pc_=0x131350;
    const auto device=s.members150[index].deviceID00;
    if(device==s.track.device)selectedRoot_=s.track.root;
    else if(device==s.courseDevice&&s.courseRootOwner&&
       s.courseStage==OriginalStandardTextureResolver50LA::FreshFileCapacity::CourseStage::joined)selectedRoot_=s.courseRootOwner;
    else {pc_=0x131380;return fail(d,E::unsupported,pc_);}
    pc_=0x131380;
    if(!resolver_->openSourceBankMember131380(selectedRoot_,this,index,handle_,d)){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    sourceHandle_=handle_;openReturned_=true;pc_=0x131378;return true;
}
bool OriginalSourceSkyFile50LA::read4(Read4& row,std::uint32_t resume,D& d,bool initializedSuffix)noexcept {
    d={};if(!current()||row.attempted)return fail(d,E::phase,0x12fb30);
    row.attempted=true;pc_=0x12fb30;
    row.before=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    const bool returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,row.bytes.data(),4,row.copied,d);
    row.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=row.after;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    //Positive EOF clamp still returns the original requested item count1.
    //Only occurrence-qualified callers with independently known suffix bytes
    //may continue; this flag does not initialize row.bytes or change copied.
    if(row.copied&&row.copied<4&&!initializedSuffix){row.pendingSourceItems=1;pc_=0x12ff34;return fail(d,E::unsupported,pc_);}
    row.returned=true;row.items=row.copied?1u:0u;pc_=resume;return true;
}
bool OriginalSourceSkyFile50LA::readFirst4(D& d)noexcept {
    return read4(read_,0x129638,d);
}
bool OriginalSourceSkyFile50LA::readCount4(D& d)noexcept {
    if(!receiver_||read_.items!=1u)return fail(d,E::phase,0x129654);
    //The sole caller admits constructor-zeroed receiver7D8 before this read.
    return read4(countRead_,0x12965c,d,true);
}
bool OriginalSourceSkyFile50LA::readPaletteOffset4(D& d)noexcept {
    if(countRead_.items!=1u)return fail(d,E::phase,0x129684);
    return read4(paletteOffsetRead_,0x12968c,d);
}
bool OriginalSourceSkyFile50LA::tellResourceStart(D& d)noexcept {
    d={};if(!current()||paletteOffsetRead_.items!=1u||tellAttempted_)return fail(d,E::phase,0x12f520);
    tellAttempted_=true;pc_=0x12f520;std::size_t position=0;
    if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
    resourceStart_=position;tellReturned_=true;pc_=0x1296a8;return true;
}
bool OriginalSourceSkyFile50LA::seekResourceAbsolute(std::uint32_t offset,D& d)noexcept {
    d={};if(!current()||!tellReturned_||seekAttempted_)return fail(d,E::phase,0x12f640);
    seekAttempted_=true;resourceSeekTarget_=offset;pc_=0x12f640;
    //Actual A3=0 (SEEK_SET), not size relative to current cursor. Native shared
    //reader admits in-member nonnegative offsets only; no invented -1 return.
    if(offset>0x7fffffffu)return fail(d,E::unsupported,pc_);
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,offset,d))return false;
    expectedCursor_=offset;seekReturned_=true;pc_=0x1296c8;return true;
}
bool OriginalSourceSkyFile50LA::beginPalette2E38C0(void* receiver,std::size_t occurrence)noexcept {
    if(!current()||!receiver)return false;
    if(!occurrence){if(!seekReturned_||paletteEntered_)return false;paletteEntered_=true;paletteReceiver_=receiver;return true;}
    if(occurrence>followingPalettes_.size())return false;
    if(occurrence==1){if(paletteEntered_&&!paletteReleasedSource_)return false;}
    else {const auto& previous=*followingPalettes_[occurrence-2];if(previous.parserEntered&&!previous.parserReleasedSource)return false;}
    auto& step=*followingPalettes_[occurrence-1];if(!step.seekReturned||step.parserEntered)return false;
    step.parserEntered=true;step.parserReceiver=receiver;return true;
}
bool OriginalSourceSkyFile50LA::readPaletteHeader4(std::size_t index,D& d,std::size_t occurrence)noexcept {
    if(index>=3||occurrence>followingPalettes_.size())return fail(d,E::phase,0x12fb30);
    auto& rows=occurrence?followingPalettes_[occurrence-1]->headers:paletteHeaderReads_;
    const bool live=occurrence?(followingPalettes_[occurrence-1]->parserEntered&&!followingPalettes_[occurrence-1]->parserWithdrawn&&!followingPalettes_[occurrence-1]->parserReleasedSource&&
        followingPalettes_[occurrence-1]->parserReceiver):(paletteEntered_&&!paletteWithdrawn_&&!paletteReleasedSource_&&paletteReceiver_);
    if(!live||(index&&rows[index-1].items!=1u))return fail(d,E::phase,0x12fb30);
    constexpr std::uint32_t resumes[3]={0x2e3910,0x2e3938,0x2e395c};
    return read4(rows[index],resumes[index],d);
}
bool OriginalSourceSkyFile50LA::readRawPaletteBlock(std::size_t index,D& d,std::size_t occurrence)noexcept {
    d={};if(!current()||index>=3||occurrence>followingPalettes_.size())return fail(d,E::phase,0x12fb30);
    auto& headers=occurrence?followingPalettes_[occurrence-1]->headers:paletteHeaderReads_;
    auto& rows=occurrence?followingPalettes_[occurrence-1]->payloads:rawPaletteReads_;
    auto* receiver=occurrence?followingPalettes_[occurrence-1]->parserReceiver:paletteReceiver_;
    const bool live=occurrence?(followingPalettes_[occurrence-1]->parserEntered&&!followingPalettes_[occurrence-1]->parserWithdrawn&&!followingPalettes_[occurrence-1]->parserReleasedSource):
        (paletteEntered_&&!paletteWithdrawn_&&!paletteReleasedSource_);
    if(!live||!receiver||headers[2].items!=1u||(index&&rows[index-1].items!=1u))return fail(d,E::phase,0x12fb30);
    for(const auto byte:headers[2].bytes)if(byte)return fail(d,E::phase,0x12fb30);
    auto& row=rows[index];if(row.attempted)return fail(d,E::phase,0x12fb30);
    constexpr std::size_t offsets[3]={0x18,0x718,0x8718},sizes[3]={0x300,0x8000,0x10000};
    constexpr std::uint32_t resumes[3]={0x2e3b84,0x2e3bac,0x2e3bd8};
    row.attempted=true;pc_=0x12fb30;row.before=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    const bool returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,
        static_cast<std::uint8_t*>(receiver)+offsets[index],sizes[index],row.copied,d);
    row.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=row.after;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    if(row.copied&&row.copied<sizes[index]){row.pendingSourceItems=1;pc_=0x12ff34;return fail(d,E::unsupported,pc_);}
    row.returned=true;row.items=row.copied?1u:0u;pc_=resumes[index];return true;
}
bool OriginalSourceSkyFile50LA::readCompressedPaletteSize(std::size_t occurrence,std::size_t index,D& d)noexcept {
    d={};if(!current()||index>=3||occurrence>followingPalettes_.size())return fail(d,E::phase,0x12fb30);
    auto& headers=occurrence?followingPalettes_[occurrence-1]->headers:paletteHeaderReads_;
    auto& blocks=occurrence?followingPalettes_[occurrence-1]->compressed:compressedPaletteReads_;
    const bool live=occurrence?(followingPalettes_[occurrence-1]->parserEntered&&!followingPalettes_[occurrence-1]->parserWithdrawn&&!followingPalettes_[occurrence-1]->parserReleasedSource&&followingPalettes_[occurrence-1]->parserReceiver):
        (paletteEntered_&&!paletteWithdrawn_&&!paletteReleasedSource_&&paletteReceiver_);
    if(!live||headers[2].items!=1u||(index&&blocks[index-1].payload.items!=1u))return fail(d,E::phase,0x12fb30);
    std::uint32_t encoding=0;for(unsigned k=0;k<4;++k)encoding|=std::uint32_t(headers[2].bytes[k])<<(8*k);
    auto& row=blocks[index].size;if(!encoding||row.attempted)return fail(d,E::phase,0x12fb30);
    row.attempted=true;row.before=expectedCursor_;pc_=0x12fb30;
    const bool returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,row.bytes.data(),4,row.copied,d);
    row.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=row.after;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    row.returned=true;row.items=row.copied?1u:0u;
    constexpr std::uint32_t resumes[3]={0x2e39a0,0x2e3a38,0x2e3ad0};pc_=resumes[index];return true;
}
bool OriginalSourceSkyFile50LA::readCompressedPaletteBlock(std::size_t occurrence,std::size_t index,std::uint8_t* scratch,D& d)noexcept {
    d={};if(!current()||!scratch||index>=3||occurrence>followingPalettes_.size())return fail(d,E::phase,0x12fb30);
    auto& blocks=occurrence?followingPalettes_[occurrence-1]->compressed:compressedPaletteReads_;
    auto* receiver=occurrence?followingPalettes_[occurrence-1]->parserReceiver:paletteReceiver_;
    const bool live=occurrence?(followingPalettes_[occurrence-1]->parserEntered&&!followingPalettes_[occurrence-1]->parserWithdrawn&&!followingPalettes_[occurrence-1]->parserReleasedSource):
        (paletteEntered_&&!paletteWithdrawn_&&!paletteReleasedSource_);
    auto& block=blocks[index];if(!live||!receiver||!block.size.returned||block.size.items!=1u||block.payload.attempted)return fail(d,E::phase,0x12fb30);
    std::array<std::uint8_t,4> word{};unsigned known=0;
    for(std::size_t i=0;i<=index;++i)for(std::size_t k=0;k<blocks[i].size.copied;++k){word[k]=blocks[i].size.bytes[k];known|=1u<<k;}
    if(known!=15u)return fail(d,E::unsupported,0x12fb30);
    std::uint32_t length=0;for(unsigned k=0;k<4;++k)length|=std::uint32_t(word[k])<<(8*k);
    if(length>0x10000u)return fail(d,E::unsupported,0x12fb30);
    constexpr std::size_t sizes[3]={0x300,0x8000,0x10000},offsets[3]={0x18,0x718,0x8718};
    block.direct=length==sizes[index];block.requested=length;block.destination=block.direct?static_cast<std::uint8_t*>(receiver)+offsets[index]:scratch;
    auto& row=block.payload;row.attempted=true;row.before=expectedCursor_;pc_=0x12fb30;
    const bool returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,block.destination,length,row.copied,d);
    row.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=row.after;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    row.returned=true;row.items=row.copied?1u:0u;
    //End this native file-read destination lease only after the source call
    //returns. This is not a source free; caller/codec retain actual storage.
    block.destination=nullptr;block.nativeWithdrawn=true;
    constexpr std::uint32_t direct[3]={0x2e39d4,0x2e3a6c,0x2e3b08},coded[3]={0x2e39fc,0x2e3a94,0x2e3b30};
    pc_=block.direct?direct[index]:coded[index];return true;
}
bool OriginalSourceSkyFile50LA::withdrawPaletteNative()noexcept {
    if(!preflightNative()||paletteScanMatchDetected_)return false;
    for(const auto& step:followingPalettes_)if(step->scanMatchDetected)return false;
    for(auto& block:compressedPaletteReads_)if(block.payload.attempted){block.destination=nullptr;block.nativeWithdrawn=true;}
    for(auto& step:followingPalettes_)for(auto& block:step->compressed)if(block.payload.attempted){block.destination=nullptr;block.nativeWithdrawn=true;}
    if(paletteEntered_&&!paletteWithdrawn_&&!paletteReleasedSource_){paletteReceiver_=nullptr;paletteWithdrawn_=true;}
    for(auto& step:followingPalettes_)if(step->parserEntered&&!step->parserWithdrawn&&!step->parserReleasedSource){step->parserReceiver=nullptr;step->parserWithdrawn=true;}
    return true; //Withdraw constructor destinations before backing, not guest destructors.
}
bool OriginalSourceSkyFile50LA::restorePaletteStart(D& d)noexcept {
    d={};if(!current()||!seekReturned_||!resourceStart_||restoreAttempted_)return fail(d,E::phase,0x12f640);
    restoreAttempted_=true;restoreTarget_=resourceStart_;pc_=0x12f640;
    //S4 was signextended at1296B4/C4. Negative offsets are outside this native adapter.
    if(*restoreTarget_>0x7fffffffu)return fail(d,E::unsupported,pc_);
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,*restoreTarget_,d))return false;
    expectedCursor_=restoreTarget_;restoreReturned_=true;pc_=0x129730;return true;
}
bool OriginalSourceSkyFile50LA::restoreFollowingPaletteStart(std::size_t index,D& d)noexcept {
    d={};if(!current()||index>=followingPalettes_.size()||index+1!=followingPalettes_.size())return fail(d,E::phase,0x12f640);
    auto& step=*followingPalettes_[index];if(!step.seekReturned||!step.start||step.restoreAttempted)return fail(d,E::phase,0x12f640);
    step.restoreAttempted=true;step.restoreTarget=step.start;pc_=0x12f640;
    if(*step.start>0x7fffffffu)return fail(d,E::unsupported,pc_);
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,*step.start,d))return false;
    expectedCursor_=step.start;step.restoreReturned=true;pc_=0x129730;return true;
}
bool OriginalSourceSkyFile50LA::readFollowingPaletteOffset(D& d)noexcept {
    d={};if(!current()||!restoreReturned_||(!followingPalettes_.empty()&&!followingPalettes_.back()->restoreReturned))return fail(d,E::phase,0x129670);
    //Each following loop owns a distinct stable occurrence. A later directory
    //entry is authorized only by its predecessor's actual saved-cursor restore.
    try{auto next=std::unique_ptr<PaletteIteration>(new PaletteIteration);followingPalettes_.push_back(std::move(next));}catch(...){return fail(d,E::allocation,0x129670);}
    return read4(followingPalettes_.back()->offsetRead,0x12968c,d);
}
bool OriginalSourceSkyFile50LA::tellFollowingPaletteStart(D& d)noexcept {
    d={};if(!current()||followingPalettes_.empty())return fail(d,E::phase,0x12f520);
    auto& step=*followingPalettes_.back();if(step.offsetRead.items!=1u||step.tellAttempted)return fail(d,E::phase,0x12f520);
    step.tellAttempted=true;pc_=0x12f520;std::size_t position=0;
    if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
    step.start=position;step.tellReturned=true;pc_=0x1296a8;return true;
}
bool OriginalSourceSkyFile50LA::seekFollowingPaletteAbsolute(std::uint32_t target,D& d)noexcept {
    d={};if(!current()||followingPalettes_.empty())return fail(d,E::phase,0x12f640);
    auto& step=*followingPalettes_.back();if(!step.tellReturned||step.seekAttempted)return fail(d,E::phase,0x12f640);
    step.seekAttempted=true;step.target=target;pc_=0x12f640;
    if(target>0x7fffffffu)return fail(d,E::unsupported,pc_);
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,target,d))return false;
    expectedCursor_=target;step.seekReturned=true;pc_=0x1296c8;return true;
}
bool OriginalSourceSkyFile50LA::scanFreshPalette1327D0(D& d,std::size_t occurrence)noexcept {
    d={};if(!current()||occurrence>followingPalettes_.size())return fail(d,E::phase,0x1327d0);
    auto* step=occurrence?followingPalettes_[occurrence-1].get():nullptr;
    auto& receiver=step?step->parserReceiver:paletteReceiver_;
    auto& attempted=step?step->scanAttempted:paletteScanAttempted_;
    auto& returned=step?step->scanReturned:paletteScanReturned_;
    auto& matched=step?step->scanMatchDetected:paletteScanMatchDetected_;
    auto& count=step?step->scanCount:paletteScanCount_;
    auto& visited=step?step->scanVisited:paletteScanVisited_;
    if(!(step?step->parserEntered:paletteEntered_)||(step?step->parserWithdrawn:paletteWithdrawn_)||
       (step?step->parserReleasedSource:paletteReleasedSource_)||!receiver||attempted)return fail(d,E::phase,0x1327d0);
    attempted=true;pc_=0x1327d0;auto& s=*resolver_->freshFileCapacity_;count=s.memberCount154;
    if(!s.members150||s.memberCount154<2)return fail(d,E::unsupported,0x1327d4);
    for(std::int32_t i=0;i<s.memberCount154;++i){++visited;
        if(s.members150[i].associatedObject1C==receiver){matched=true;
            pc_=i+1==s.memberCount154?0x132810u:0x1327f4u;return fail(d,E::phase,pc_);}
    }
    returned=true;pc_=0x132814;return true; //Actual no-match scan; nonescape proof, not a canonical cache clear.
}
bool OriginalSourceSkyFile50LA::retirePaletteDestinationSource(std::size_t occurrence)noexcept {
    if(!current()||occurrence>followingPalettes_.size())return false;
    if(!occurrence){
        if(!paletteScanReturned_||paletteReleasedSource_||paletteWithdrawn_||!paletteReceiver_)return false;
        paletteReceiver_=nullptr;paletteReleasedSource_=true;return true;
    }
    auto& step=*followingPalettes_[occurrence-1];
    if(!step.scanReturned||step.parserReleasedSource||step.parserWithdrawn||!step.parserReceiver)return false;
    step.parserReceiver=nullptr;step.parserReleasedSource=true;return true;
}
bool OriginalSourceSkyFile50LA::readItemOffset(D& d,std::size_t occurrence)noexcept {
    d={};if(!current()||itemOccurrence(occurrence)||occurrence!=(firstItem_?followingItems_.size()+1:0)||
       (occurrence&&!itemOccurrence(occurrence-1)->restoreReturned))return fail(d,E::phase,0x129770);
    try{auto row=std::unique_ptr<ItemFileOccurrence>(new ItemFileOccurrence);
        if(occurrence)followingItems_.push_back(std::move(row));else firstItem_=std::move(row);
    }catch(...){return fail(d,E::allocation,0x129770);}
    return read4(itemOccurrence(occurrence)->offsetRead,0x12978c,d);
}
bool OriginalSourceSkyFile50LA::tellItemStart(D& d,std::size_t occurrence)noexcept {
    auto* row=itemOccurrence(occurrence);
    d={};if(!current()||!row||occurrence!=followingItems_.size()||row->offsetRead.items!=1u||row->tellAttempted)return fail(d,E::phase,0x12f520);
    auto& item=*row;item.tellAttempted=true;pc_=0x12f520;std::size_t position=0;
    if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
    item.start=position;item.tellReturned=true;pc_=0x1297a8;return true;
}
bool OriginalSourceSkyFile50LA::seekItemAbsolute(std::uint32_t target,D& d,std::size_t occurrence)noexcept {
    auto* row=itemOccurrence(occurrence);
    d={};if(!current()||!row||occurrence!=followingItems_.size()||!row->tellReturned||row->seekAttempted)return fail(d,E::phase,0x12f640);
    auto& item=*row;item.seekAttempted=true;item.target=target;pc_=0x12f640;
    if(target>0x7fffffffu)return fail(d,E::unsupported,pc_);
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,target,d))return false;
    expectedCursor_=target;item.seekReturned=true;pc_=0x1297c8;return true;
}
bool OriginalSourceSkyFile50LA::restoreItemStart(D& d,std::size_t occurrence)noexcept {
    auto* row=itemOccurrence(occurrence);
    d={};if(!current()||!row||occurrence!=followingItems_.size()||!row->finalTellReturned||!row->start||row->restoreAttempted)return fail(d,E::phase,0x12982c);
    row->restoreAttempted=true;row->restoreTarget=row->start;pc_=0x12f640;
    if(*row->start>0x7fffffffu)return fail(d,E::unsupported,pc_);
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,*row->start,d))return false;
    expectedCursor_=row->start;row->restoreReturned=true;row->restoreResult=0;pc_=0x129834;
    return true; //Actual12F778 sets V0=0; native refusal never becomes a forged source error result.
}
bool OriginalSourceSkyFile50LA::beginItemParser(void* receiver,std::size_t occurrence)noexcept {
    auto* selected=itemOccurrence(occurrence);
    if(occurrence!=followingItems_.size())return false;
    if(!current()||!selected||!selected->seekReturned||selected->parserEntered||!receiver)return false;
    selected->receiver=receiver;selected->parserEntered=true;return true;
}
bool OriginalSourceSkyFile50LA::readItemHeader48(D& d,std::size_t occurrence)noexcept {
    auto* selected=itemOccurrence(occurrence);
    if(occurrence!=followingItems_.size())return false;
    if(!selected||!selected->parserEntered||selected->nativeWithdrawn||!selected->receiver)return fail(d,E::phase,0x128a00);
    //The caller proves this occurrence's constructor-initialized receiver48.
    return read4(selected->header48,0x128a08,d,true);
}
bool OriginalSourceSkyFile50LA::readItemField(std::size_t index,D& d,std::size_t occurrence)noexcept {
    auto* selected=itemOccurrence(occurrence);
    if(occurrence!=followingItems_.size())return false;
    d={};if(!current()||!selected||!selected->parserEntered||selected->nativeWithdrawn||
       !selected->receiver||index>=selected->fields.size())return fail(d,E::phase,0x12fb30);
    auto& item=*selected;auto& row=item.fields[index];const auto spec=itemFieldSpec(index);
    if(row.attempted||!itemFieldRequired(item,index)||itemFieldPredecessor(item,index)!=1u)return fail(d,E::phase,spec.call);
    row.attempted=true;pc_=0x12fb30;row.before=resolver_->freshFileCapacity_->members150[handle_].cursor10;
    const bool returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,row.bytes.data(),spec.size,row.copied,d);
    row.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=row.after;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    //Only index0 targets unknown stack bytes. All other destinations have
    //source constructor bytes, checked by the caller before this occurrence.
    //Keep the untouched receiver suffix; never read the unknown row.bytes tail.
    if(index==0&&row.copied&&row.copied<spec.size){row.pendingSourceItems=1;pc_=0x12ff34;return fail(d,E::unsupported,pc_);}
    row.returned=true;row.items=row.copied?1u:0u;pc_=spec.resume;return true;
}
bool OriginalSourceSkyFile50LA::finishItemReads(D& d,std::size_t occurrence)noexcept {
    auto* selected=itemOccurrence(occurrence);
    if(occurrence!=followingItems_.size())return false;
    d={};if(!current()||!selected||selected->nativeWithdrawn||!selected->receiver||selected->tailEntered)return fail(d,E::phase,0x128c00);
    auto& item=*selected;
    for(std::size_t i=0;i<item.fields.size();++i)if(itemFieldRequired(item,i)&&item.fields[i].items!=1u)return fail(d,E::phase,0x128c00);
    item.tailEntered=true;item.alignmentSelected=(item.fields[0].bytes[0]&2u)!=0;pc_=0x128c00;
    if(item.alignmentSelected){item.alignmentTellAttempted=true;pc_=0x12f520;std::size_t position=0;
        if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
        item.alignmentPosition=position;item.alignmentTellReturned=true;pc_=0x30e5c8;
        //Native bounded subset of actual signed64 remainder. No negative tell
        //or wrapped cursor is converted into an invented positive seek.
        if(position>0x7fffffffu)return fail(d,E::unsupported,pc_);
        item.alignmentPadding=static_cast<std::uint32_t>(2048u-position%2048u);pc_=0x128c40;
        if(*item.alignmentPadding!=2048u){item.alignmentSeekAttempted=true;item.alignmentTarget=position+*item.alignmentPadding;pc_=0x12f640;
            //Original A3=1 relative seek; this admitted numeric file adapter
            //takes the equivalent absolute target under the same cursor pin.
            if(*item.alignmentTarget>0x7fffffffu)return fail(d,E::unsupported,pc_);
            if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,*item.alignmentTarget,d))return false;
            expectedCursor_=item.alignmentTarget;item.alignmentSeekReturned=true;pc_=0x128c64;
        }
    }
    item.finalTellAttempted=true;pc_=0x12f520;std::size_t position=0;
    if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
    item.finalPosition=position;item.finalTellReturned=true;pc_=0x128c74;return true;
}
bool OriginalSourceSkyFile50LA::seekItemResource(std::size_t occurrence,D& d)noexcept {
    d={};if(!current()||occurrence!=itemResourceSeeks_.size()||!firstItem_)return fail(d,E::phase,0x129118);
    for(std::size_t i=0;i<=followingItems_.size();++i)if(!itemOccurrence(i)||!itemOccurrence(i)->restoreReturned)return fail(d,E::phase,0x129118);
    const auto* item=itemOccurrence(occurrence);
    if(!item||!item->parserEntered||item->nativeWithdrawn||!item->receiver||!item->finalPosition)return fail(d,E::phase,0x129118);
    try{auto row=std::unique_ptr<ItemResourceSeek>(new ItemResourceSeek);row->occurrence=occurrence;itemResourceSeeks_.push_back(std::move(row));}
    catch(...){return fail(d,E::allocation,0x129118);}
    auto& row=*itemResourceSeeks_.back();row.attempted=true;row.before=expectedCursor_;row.target=static_cast<std::uint32_t>(*item->finalPosition);pc_=0x12f640;
    if(*row.target>0x7fffffffu)return fail(d,E::unsupported,pc_);
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,*row.target,d))return false;
    expectedCursor_=row.target;row.returned=true;row.result=0;row.entryOpened=true;row.entryCursor=expectedCursor_;pc_=0x129120;return true;
}
bool OriginalSourceSkyFile50LA::beginNextResource(std::size_t occurrence,std::size_t ordinal,std::size_t cursor,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size()||ordinal<1||ordinal>95||expectedCursor_!=cursor)return fail(d,E::phase,0x128d50);
    auto& root=*itemResourceSeeks_[occurrence];auto* prior=activeResourceFile(occurrence);
    if(!root.returned||root.resourceEntries.size()+1!=ordinal||!prior||!prior->resourceAlignmentReturned)return fail(d,E::phase,0x128d50);
    try{auto next=std::unique_ptr<ResourceFileOccurrence>(new ResourceFileOccurrence);next->resourceOrdinal=ordinal;next->entryOpened=true;next->entryCursor=expectedCursor_;root.resourceEntries.push_back(std::move(next));}
    catch(...){return fail(d,E::allocation,0x128d50);}
    pc_=0x128d50;return true; //Native occurrence opening, NO original seek/read.
}
bool OriginalSourceSkyFile50LA::readItemResourceSize(std::size_t occurrence,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size()||
       !activeResourceFile(occurrence)->entryOpened)return fail(d,E::phase,0x128e4c);
    return read4(activeResourceFile(occurrence)->resourceSizeA8,0x128e54,d);
}
bool OriginalSourceSkyFile50LA::readCompressedResourceHeader(std::size_t occurrence,std::size_t index,D& d)noexcept {
    d={};if(!current()||index>=5||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size())return fail(d,E::phase,0x128ee0);
    auto& row=*activeResourceFile(occurrence);
    if(!row.entryOpened||!row.resourceSizeA8.returned||row.rawPayload.attempted||
       (index&&!row.compressedHeaders[index-1].returned))return fail(d,E::phase,0x128ee0);
    static constexpr std::uint32_t resumes[5]={0x128ee8,0x128f04,0x128f20,0x128f3c,0x128f58};
    // Source ignores item return: predecessor must return, not return1.
    //Actual128EC0..ECC initialize B0/B4/B8/BC; AC is NOT initialized.
    //Owner merges only copied bytes into those actual zero-backed words and
    //does not interpret the scratch-buffer suffix as transferred file bytes.
    return read4(row.compressedHeaders[index],resumes[index],d,index!=0);
}
bool OriginalSourceSkyFile50LA::alignResource129054(std::size_t occurrence,std::uint16_t flags,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size())return fail(d,E::phase,0x129054);
    auto& row=*activeResourceFile(occurrence);if(row.resourceTailEntered)return fail(d,E::phase,0x129054);
    row.resourceTailEntered=true;row.resourceAlignmentSelected=(flags&2u)!=0;pc_=0x12905c;
    if(row.resourceAlignmentSelected){auto& tell=row.resourceAlignmentTell;tell.attempted=true;pc_=0x12f520;std::size_t position=0;
        if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
        tell.position=position;tell.returned=true;pc_=0x30e5c8;
        if(position>0x7fffffffu)return fail(d,E::unsupported,pc_); //Bounded positive signed64 modulo, no wrapped cursor.
        row.resourceAlignmentPadding=static_cast<std::uint32_t>(2048u-position%2048u);pc_=0x129094;
        if(*row.resourceAlignmentPadding!=2048u){pc_=0x12909c;return true;} //Caller must freshly reload resolver before seek.
    }
    row.resourceAlignmentReturned=true;pc_=0x1290b8;return true;
}
bool OriginalSourceSkyFile50LA::seekResourceAlignment1290B0(std::size_t occurrence,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size())return fail(d,E::phase,0x1290b0);
    auto& row=*activeResourceFile(occurrence);
    if(!row.resourceTailEntered||!row.resourceAlignmentTell.returned||!row.resourceAlignmentPadding||*row.resourceAlignmentPadding==2048u||row.resourceAlignmentSeekAttempted||row.resourceAlignmentReturned)return fail(d,E::phase,0x1290b0);
    row.resourceAlignmentSeekAttempted=true;row.resourceAlignmentTarget=*row.resourceAlignmentTell.position+*row.resourceAlignmentPadding;pc_=0x12f640;
    if(*row.resourceAlignmentTarget>0x7fffffffu||*row.resourceAlignmentTarget>resolver_->freshFileCapacity_->members150[handle_].length14)return fail(d,E::unsupported,pc_);
    //Actual A3=1 relative seek, equivalent absolute target under the same
    //exclusive numeric-member cursor pin. No padding bytes read.
    if(!resolver_->seekSourceBankMember12F640(selectedRoot_,this,handle_,*row.resourceAlignmentTarget,d))return false;
    expectedCursor_=row.resourceAlignmentTarget;row.resourceAlignmentSeekReturned=true;row.resourceAlignmentReturned=true;pc_=0x1290b8;return true;
}
bool OriginalSourceSkyFile50LA::tellCompressedFinal(std::size_t occurrence,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size())return fail(d,E::phase,0x24fc00);
    auto& row=*activeResourceFile(occurrence);auto& tell=row.compressedFinalTell;
    const auto* last=row.followingCompressedPayloads.empty()?&row.compressedPayload:row.followingCompressedPayloads.back().get();
    if(tell.attempted||!last||!last->returned||last->items!=last->requested||row.followingCompressedSizes.size()!=row.followingCompressedPayloads.size())return fail(d,E::phase,0x24fc00);
    tell.attempted=true;pc_=0x12f520;std::size_t position=0;
    if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
    tell.position=position;tell.returned=true;pc_=0x24fc08;return true;
}
bool OriginalSourceSkyFile50LA::tellCompressedOffset(std::size_t occurrence,std::size_t index,bool terminal,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size()||index>12)return fail(d,E::phase,0x12f520);
    auto& row=*activeResourceFile(occurrence);if(!row.compressedHeaders[4].returned||row.rawPayload.attempted)return fail(d,E::phase,0x12f520);
    auto& tell=index?row.compressedOffsets[index-1].tell:row.compressedInitialTell;
    if(tell.attempted||(index&&(!row.compressedInitialTell.returned||
       (index>1&&(row.compressedOffsets[index-2].read.items!=1u||row.compressedOffsets[index-2].terminal)))))return fail(d,E::phase,0x12f520);
    if(index)row.compressedOffsets[index-1].terminal=terminal;
    tell.attempted=true;pc_=0x12f520;std::size_t position=0;
    if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
    tell.position=position;tell.returned=true;pc_=index?(terminal?0x24f654:0x24f5b4):0x24f5a4;return true;
}
bool OriginalSourceSkyFile50LA::readCompressedOffset(std::size_t occurrence,std::size_t index,D& d)noexcept {
    d={};if(!current()||index>=12||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size())return fail(d,E::phase,0x12fb30);
    auto& row=activeResourceFile(occurrence)->compressedOffsets[index];
    if(!row.tell.returned)return fail(d,E::phase,0x12fb30);
    //Actual24F3F0 clears workspace170; every admitted indexed call owns
    //its distinct word. Successful indexed reads subsequently overwrite it
    //with source zero at24F610/24F6B0, irrespective of copied prefix length.
    return read4(row.read,row.terminal?0x24f680:0x24f5e0,d,true);
}
bool OriginalSourceSkyFile50LA::readCompressedPayloadSize(std::size_t occurrence,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size())return fail(d,E::phase,0x24f784);
    auto& row=*activeResourceFile(occurrence);bool terminal=false;
    for(const auto& step:row.compressedOffsets)if(step.terminal&&step.read.items==1u)terminal=true;
    if(!terminal||row.rawPayload.attempted)return fail(d,E::phase,0x24f784);
    // Caller ignores the original item return, preserving stack16C suffix.
    //Admitted indexed caller stores difference of two known zero offset
    //words at24F748. The byte prefix overlays that genuine initialized zero.
    return read4(row.compressedPayloadSize,0x24f78c,d,true);
}
bool OriginalSourceSkyFile50LA::readCompressedPayload(std::size_t occurrence,std::uint8_t* destination,std::size_t count,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size()||count>0x7fffffffu)return fail(d,E::phase,0x24f814);
    auto& owner=*activeResourceFile(occurrence);auto& row=owner.compressedPayload;
    if(!owner.compressedPayloadSize.returned||owner.rawPayload.attempted||row.attempted||row.nativeWithdrawn)return fail(d,E::phase,0x24f814);
    return transferCompressedPayload(row,destination,count,d);
}
bool OriginalSourceSkyFile50LA::transferCompressedPayload(ResourcePayloadRead& row,std::uint8_t* destination,std::size_t count,D& d)noexcept {
    if(!current()||row.attempted||row.nativeWithdrawn||count>0x7fffffffu)return fail(d,E::phase,0x24f814);
    row.destination=destination;row.requested=count;row.attempted=true;row.before=expectedCursor_;pc_=0x12fb30;
    const bool returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,destination,count,row.copied,d);
    row.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=row.after;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    //12FF08/34 returns original T0 count after any positive completed
    //clamped transfer, NOT copied bytes. EOF amount0 instead returns0.
    //Native partial refusal above is not a completed source short read.
    row.returned=true;row.items=row.copied?static_cast<std::uint32_t>(count):0u;pc_=0x24f81c;return true;
}
bool OriginalSourceSkyFile50LA::readFollowingCompressedPayload(std::size_t occurrence,std::size_t level,std::uint8_t* destination,std::size_t count,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size()||level<1||level>4)return fail(d,E::phase,0x24f814);
    auto& row=*activeResourceFile(occurrence);
    if(row.followingCompressedSizes.size()!=level||!row.followingCompressedSizes[level-1]->returned||
       row.followingCompressedPayloads.size()+1!=level||row.rawPayload.attempted)return fail(d,E::phase,0x24f814);
    try{auto next=std::unique_ptr<ResourcePayloadRead>(new ResourcePayloadRead);row.followingCompressedPayloads.push_back(std::move(next));}
    catch(...){return fail(d,E::allocation,0x24f814);}
    return transferCompressedPayload(*row.followingCompressedPayloads.back(),destination,count,d);
}
bool OriginalSourceSkyFile50LA::readFollowingCompressedSize(std::size_t occurrence,std::size_t level,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size()||level<1||level>4)return fail(d,E::phase,0x24f784);
    auto& row=*activeResourceFile(occurrence);
    if(!row.compressedPayload.returned||row.compressedPayload.items!=row.compressedPayload.requested||
       row.followingCompressedSizes.size()+1!=level||row.followingCompressedPayloads.size()+1!=level||
       row.rawPayload.attempted)return fail(d,E::phase,0x24f784);
    if(level>1){const auto& prior=*row.followingCompressedPayloads.back();
        if(!prior.returned||prior.items!=prior.requested)return fail(d,E::phase,0x24f784);}
    try{auto next=std::unique_ptr<Read4>(new Read4);row.followingCompressedSizes.push_back(std::move(next));}catch(...){return fail(d,E::allocation,0x24f784);}
    //Each iteration reexecutes24F728..748 before this call; the previous
    //size is NOT carried forward. Owner keeps separate occurrence receipts.
    return read4(*row.followingCompressedSizes.back(),0x24f78c,d,true);
}
bool OriginalSourceSkyFile50LA::tellRawExtendedOffset(std::size_t occurrence,std::size_t index,bool terminal,D& d)noexcept {
    d={};if(!current()||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size()||index>12)return fail(d,E::phase,0x12f520);
    auto& row=*activeResourceFile(occurrence);
    if(!row.entryOpened||row.resourceSizeA8.attempted||row.compressedHeaders[0].attempted||row.rawPayload.attempted||!row.rawExtendedRows.empty())return fail(d,E::phase,0x12f520);
    auto& tell=index?row.rawExtendedOffsets[index-1].tell:row.rawExtendedInitialTell;
    if(tell.attempted||(index&&(!row.rawExtendedInitialTell.returned||
       (index>1&&(row.rawExtendedOffsets[index-2].read.items!=1u||row.rawExtendedOffsets[index-2].terminal)))))return fail(d,E::phase,0x12f520);
    if(!index){if(row.rawExtendedEntered)return fail(d,E::phase,0x12f520);row.rawExtendedEntered=true;}
    else row.rawExtendedOffsets[index-1].terminal=terminal;
    tell.attempted=true;pc_=0x12f520;std::size_t position=0;
    if(!resolver_->tellSourceBankMember12F520(selectedRoot_,this,handle_,position,d))return false;
    tell.position=position;tell.returned=true;pc_=index?(terminal?0x24f654:0x24f5b4):0x24f5a4;return true;
}
bool OriginalSourceSkyFile50LA::readRawExtendedOffset(std::size_t occurrence,std::size_t index,D& d)noexcept {
    d={};if(!current()||index>=12||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size())return fail(d,E::phase,0x12fb30);
    auto& row=activeResourceFile(occurrence)->rawExtendedOffsets[index];if(!row.tell.returned)return fail(d,E::phase,0x12fb30);
    //Actual stack170..19F was zeroed; this distinct word keeps its real zero
    //suffix after a positive short clamp. It is subsequently ADDed, not zeroed.
    return read4(row.read,row.terminal?0x24f680:0x24f5e0,d,true);
}
bool OriginalSourceSkyFile50LA::readRawExtendedRows(std::size_t occurrence,std::uint8_t* destination,std::uint32_t dimension,std::uint32_t rowBytes,D& d)noexcept {
    d={};if(!current()||!destination||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size())return fail(d,E::phase,0x24f950);
    auto& owner=*activeResourceFile(occurrence);const auto ordinal=owner.rawExtendedRows.size();bool terminal=false;
    for(const auto& offset:owner.rawExtendedOffsets)if(offset.terminal&&offset.read.items==1u)terminal=true;
    if(!owner.rawExtendedEntered||!terminal||ordinal>=10||dimension!=(1u<<ordinal)||
       (rowBytes!=dimension*2u&&rowBytes!=dimension*4u)||
       (ordinal&&(!owner.rawExtendedRows.back()->returned||owner.rawExtendedRows.back()->items!=owner.rawExtendedRows.back()->rows)))return fail(d,E::phase,0x24f950);
    try{owner.rawExtendedRows.emplace_back(new RawExtendedRows);}catch(...){return fail(d,E::allocation,0x24f950);}
    auto& row=*owner.rawExtendedRows.back();row.dimension=dimension;row.rowBytes=rowBytes;row.rows=dimension;
    row.requested=std::size_t(rowBytes)*dimension;row.destination=destination;row.attempted=true;row.before=expectedCursor_;pc_=0x12fb30;
    const bool returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,destination,row.requested,row.copied,d);
    row.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=row.after;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    //The owned lower reader returns only after fully copying its positive
    //clamp. Original12FF34 returns REQUESTED ROWS then, even on EOF clamp.
    //No knowledge is invented: caller must prove the whole real input extent.
    row.returned=true;row.items=row.copied?dimension:0u;pc_=0x24f958;return true;
}
bool OriginalSourceSkyFile50LA::readItemResourcePayload(std::size_t occurrence,std::uint8_t* destination,std::size_t count,D& d)noexcept {
    d={};if(!current()||!destination||occurrence>=itemResourceSeeks_.size()||occurrence+1!=itemResourceSeeks_.size()||
       !activeResourceFile(occurrence)->entryOpened||activeResourceFile(occurrence)->resourceSizeA8.attempted)return fail(d,E::phase,0x24ff88);
    auto& row=activeResourceFile(occurrence)->rawPayload;
    if(row.attempted||row.nativeWithdrawn||count>0xffffffffu)return fail(d,E::phase,0x24ff88);
    row.destination=destination;row.requested=count;row.attempted=true;row.before=expectedCursor_;pc_=0x12fb30;
    const bool returned=resolver_->readSourceBankMember12FB30(selectedRoot_,this,handle_,destination,count,row.copied,d);
    row.after=resolver_->freshFileCapacity_->members150[handle_].cursor10;expectedCursor_=row.after;
    if(!returned){pc_=d.sourcePC?d.sourcePC:pc_;return false;}
    // Fullpositive transfer returns original count; zeroactualbytes returns0.
    // Bound a positive EOF clamp before12FF34 so legacy250EE0 cannot consume
    //an unknown suffix merely because the original reports requestedcount.
    if(row.copied&&row.copied<count){row.pendingSourceItems=static_cast<std::uint32_t>(count);pc_=0x12ff34;return fail(d,E::unsupported,pc_);}
    row.returned=true;row.items=row.copied?static_cast<std::uint32_t>(count):0u;pc_=0x24ff90;return true;
}
bool OriginalSourceSkyFile50LA::releaseItemParserSource(void* receiver,std::size_t occurrence)noexcept {
    if(!current()||!receiver)return false;
    auto* item=const_cast<ItemFileOccurrence*>(itemOccurrence(occurrence));
    if(!item||!item->parserEntered||item->nativeWithdrawn||item->parserReleasedSource||item->receiver!=receiver||
       item->tailEntered||item->restoreAttempted||occurrence<itemResourceSeeks_.size())return false;
    bool failed=item->header48.returned&&item->header48.items!=1u;
    for(const auto& field:item->fields)if(field.returned&&field.items!=1u)failed=true;
    if(!failed)return false;
    //Only retained native metadata is withdrawn here. Reads/cursor/handle stay
    //unchanged; caller still owns the original destructor/delete chronology.
    item->receiver=nullptr;item->parserReleasedSource=true;return true;
}
bool OriginalSourceSkyFile50LA::withdrawFirstItemNative()noexcept {
    if(!preflightNative())return false;
    for(std::size_t item=itemResourceSeeks_.size();item;){--item;
      for(std::size_t ordinal=itemResourceSeeks_[item]->resourceEntries.size()+1;ordinal;){--ordinal;auto* resource=resourceFileOccurrence(item,ordinal);resource->rawPayload.destination=nullptr;resource->rawPayload.nativeWithdrawn=true;resource->compressedPayload.destination=nullptr;resource->compressedPayload.nativeWithdrawn=true;
        for(auto& payload:resource->followingCompressedPayloads){payload->destination=nullptr;payload->nativeWithdrawn=true;}
        for(auto& payload:resource->rawExtendedRows){payload->destination=nullptr;payload->nativeWithdrawn=true;}}
    }
    if(firstItem_){firstItem_->receiver=nullptr;firstItem_->nativeWithdrawn=true;}
    for(auto& row:followingItems_){row->receiver=nullptr;row->nativeWithdrawn=true;}return true;
}
bool OriginalSourceSkyFile50LA::closeNative(D& d)noexcept {
    d={};if(!preflightNative()||(paletteEntered_&&!paletteWithdrawn_&&!paletteReleasedSource_))return fail(d,E::phase,0);if(nativeClosed_)return true;
    for(const auto& step:followingPalettes_)if(step->parserEntered&&!step->parserWithdrawn&&!step->parserReleasedSource)return fail(d,E::phase,0);
    if(firstItem_&&firstItem_->parserEntered&&!firstItem_->nativeWithdrawn&&!firstItem_->parserReleasedSource)return fail(d,E::phase,0);
    for(const auto& row:followingItems_)if(row->parserEntered&&!row->nativeWithdrawn&&!row->parserReleasedSource)return fail(d,E::phase,0);
    if(borrowed_){
        //Handle can already be owned even if scratch selection prevented open return.
        if(handle_>=0&&!resolver_->closeSourceBankMember131040(selectedRoot_,this,handle_,false,d))return false;
        resolver_->freshFileCapacity_->sourceSkyFileBorrower=nullptr;borrowed_=false;
    }
    handle_=-1;selectedRoot_=nullptr;resolver_.reset();provider_.reset();root_.reset();nativeClosed_=true;
    return true; //Native cancellation, not source129954 close/error/destructor receipt.
}
