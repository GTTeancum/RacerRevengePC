// Proposal only. Intentionally included within an authenticated track profile namespace.
// Shared implementation derived from reviewed TA owner; no runtime file inputs.
struct Diagnostic {
    const char* stage="input";
    std::string member,message;
    std::size_t scenes=0,placements=0,collisionOnly=0,surfaces=0,zeroNormals=0;
    std::size_t materials=0,selections=0,secondPasses=0;
    // A runtime digest records a decoded result; it is not an independent pin.
    std::size_t independentlyPinnedImages=0,runtimeIdentityImages=0;
    a::DecodeError48C decode{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
namespace detail {
inline std::string key(std::string name){return shared::normalized(std::move(name));}
inline Json fields(const a::MaterialFields48E& value){
    Json out={{"authored",Json::array()},{"unspecified",Json::array()}};
    for(const auto* list:{&value.authored,&value.unspecified})for(const auto& item:*list)
        out[list==&value.authored?"authored":"unspecified"].push_back(
            {{"name",item.name},{"value",item.value},{"source_offset",item.source_offset}});
    return out;
}
inline std::string field(const a::MaterialFields48E& value,const char* name){
    const auto wanted=key(name);std::string result;bool found=false;
    for(const auto& item:value.authored)if(key(item.name)==wanted){
        shared::need(!found,"Duplicate original TB material field");found=true;result=item.value;
    }
    return result;
}
inline bool clamp(const a::MaterialFields48E& value,const char* name){
    const auto text=field(value,name);
    shared::need(text.empty()||text=="0"||text=="1","Unsupported original TB texture wrap mode");
    return text=="1";
}
inline a::detail::OriginalIpumMipExtent50LA framePst(const Bytes& source,std::uint32_t& mipIndex){
    const auto range=[&](std::size_t at,std::size_t n){return at<=source.size()&&n<=source.size()-at;};
    const auto word=[&](std::size_t at){shared::need(range(at,4),"Truncated original TB IPUM PST");return shared::u32(source.data()+at);};
    const auto dimension=[](std::uint32_t n){return n>=16&&n<=1024&&!(n&(n-1));};
    shared::need(source.size()<=8u*1024u*1024u&&word(0)==1&&word(4)==2&&word(8)>=1&&word(8)<=5,
        "Original TB IPUM PST header rejected");
    const auto count=word(8);std::size_t at=12;std::uint32_t priorW=0,priorH=0;
    a::detail::OriginalIpumMipExtent50LA largest;
    for(std::uint32_t i=0;i<count;++i){
        shared::need(range(at,20),"Original TB IPUM mip header extent");
        const auto h=word(at),w=word(at+4),bytes=word(at+16);const auto p=at+20;
        shared::need(dimension(w)&&dimension(h)&&!word(at+8)&&!word(at+12)&&
            (!i||(w==priorW*2&&h==priorH*2))&&bytes>=25&&range(p,bytes),"Original TB IPUM mip extent/order");
        shared::need(!std::memcmp(source.data()+p,"ipum",4)&&word(p+4)==bytes-8&&
            (std::uint32_t(source[p+8])|(std::uint32_t(source[p+9])<<8))==w&&
            (std::uint32_t(source[p+10])|(std::uint32_t(source[p+11])<<8))==h&&
            word(p+12)==1&&source[p+16]==0,"Original TB IPUM frame header");
        constexpr std::uint8_t end[]{0,0,1,0xb0,0,0,1,0xb1};
        shared::need(!std::memcmp(source.data()+p+bytes-8,end,8),"Original TB IPUM terminators");
        largest={w,h,at,p,bytes};at=p+bytes;priorW=w;priorH=h;
    }
    shared::need(at==source.size(),"Original TB IPUM PST trailing bytes");mipIndex=count-1;return largest;
}
// Native regression outputs observed in the genuine parent cold run and
// independently verified receipt recovery. These are not independent pixel evidence.
struct NativeOutputPin {const char* member;const char* sourceSha;const char* outputSha;};
inline constexpr NativeOutputPin nativeOutputPins[]={
{"TEXTURES/ARINAC01.PST","828d588ec78ec276790a5026be819573abf1593ec56c2c010684a08a232f23ee","fbef20566efa04cecdd92520fd511afaab2d9d7453f417791e7ef889f2ec8afe"},
{"TEXTURES/CNSESNS1.PST","11b988779036a6adf920d33dcaa2d911871ee59ca8480108add7eb7443b35154","21e5f7ba60be79d75a7ce10cb5333478ad2a0416ecd47ad6ac95a54fa38feb64"},
{"TEXTURES/SEATING2.PST","8e57276b4ee07123b255f330620cc237b8ad0eca45389c44c59aa011df8b8d66","ca4c96d3198d96430f0549804a33ea1ec83f19282daf9c036fdfe5628aaf04ee"},
{"TEXTURES/CROWD.ATM","76b78e28cda066154e46a22e1ecda5ab4ade267ea30f44472af8e4304e738031","fcfeb0c00938ab1bbae791a24d705c18989b4b222e9d492ab4593d67dff79e98"},
{"TEXTURES/STSTAND1.PST","6d8c934736487b57652713d6ccf9fee9fc2eee021e51e2b078b863e33ba31c34","32eabe2558224ff27acbb76b67a22869f3d5a2360c0a9386325aa317c62a0f5a"},
{"TEXTURES/STSTAND3.PST","a46edd20287e95ae2b6265049567502f41217c12978715c1ab23992c2ceb4b43","38b0f2acef0b10a84d1b9ac7bfcb5b3023d7d1c0f4ea0255132d544cd7e80bd1"},
{"TEXTURES/FENCE02.PST","b71931c958e36a3afe174d152c565a54b33456a5043251a4a5d50fe648966ab9","a6c00c7afcc1b6b5dca83b1461c0c019a32cf7cf6618fb558ab205f6f7bab179"},
{"TEXTURES/FIX01.PST","3d57b48faa3b9ac354446694de52cd9369eb9e46f09ca7c292fe171c680bdf01","c308f86abe47dbcf7b4b95fd00df8671577ec36ad702eac24d003575cde3e843"},
{"TEXTURES/FLOAT01.PST","a3edc95f2209f453fa20258c1435a315f89603383ae8659202b7036443a13278","c2ddb45bf5f383c3fd6eb7b770ff0c05b93751bb6ee7784bc8f1752906428a71"},
{"TEXTURES/FLOAT02.PST","2343fde19681f55c8ff907a7c961e7579119e4d9244ec079eedf990085551406","bf3bd45df593c75f03a298c147c900d4246d3af1b24cd4306e9cc0d336543e57"},
{"TEXTURES/HUTR01.PST","9095264db8adad3f10a5fffb79fb3e2277449ce668a7b4c924ef22c700abec4a","65ebe076365237e3353885b5b7d7d7c437c0adb091e82b1154a5ced5179e94ab"},
{"TEXTURES/JABARB01.PST","e841b87f24d1b70ee3cb28be18937a601185eec92e7dbd82623858da24fe8ea8","34c37b37746c13a4073a7886a053dd1fc63f47e9741ce0fb6afff2623b40bed3"},
{"TEXTURES/AHUTS06.PST","09d4896293dc1f9ed378a5b14f3160e6e12b9786a4fcea8943f87a3455b70454","345e62e1970cc7e8be8fc6225884d4d9c4c1be0c100df4de79381df3e4d5bd7d"},
{"TEXTURES/AHUTS04.PST","f26535ca2f9a0a0d4bfd6651e133b682246ea822af1a01845e4d91aee2252359","87f20b6fdde7ade850a831148a0dcab0382b2f739370fd0615c8e9801dfc4e80"},
{"TEXTURES/AHUTS05.PST","28ec9821ec922e5cfc516e03715baf5bc0a458fe7fb3af05d1a4ff007c82accb","1a5f9bf25441a7cc595598bf22e1d64372d4a93e21682457fbce58993bad12bc"},
{"TEXTURES/TA_ROCKB_A.PST","6ed4943eff901fdf529c53ec8902c8b86395245d6095e2f850e134652dbf8af1","9a69cba9f4072b9a56349c7cd35dd596e08dcf6ea7bcc1a7f99e1717f45ee792"},
{"TEXTURES/PITHAN01.PST","b3a02d562701bfdefff95dc188a46b982bbdfd9260697ef9ab35d2219cd996a4","f4516dd037fdba2a682f6e8107898f19d5b72035a90874fee4d6e4014b8108b3"},
{"TEXTURES/AHUTS02.PST","fcdf85d0830771516fbb062fdd30f093c7b21360b3c1e54c1f74ae0c82d254b4","3f6ff9fbf5bc143406e24139f849ace5cf6f1180f60e5e6d1948b74f2bd6b01b"},
{"TEXTURES/AHUTS01.PST","7a384b82fc8567484642243cb83683d19fb21d4e721f01bc5246fdf0f2dd206e","45042e248cf6012022a74b2514a38288a5a4454c76fbe910fb65fc8463eeed6c"},
{"TEXTURES/AHUTS03.PST","7e03df8953f376586c53d25a8ca409ee60b051c7cf7709783f1824d82e155412","7fe3eb393553b4e0210da685feff21b1f94a683886228ed05be35741d75da468"},
{"TEXTURES/TRACK.PST","e27d767a7d2aaa595d97b86b7208658404389540d2265d1bfbcaf2bd09ccbfef","3287695155e765342ebd50612c8fb8f06f91f5e514415a0e1ae7a7ad29d07139"},
{"TEXTURES/TRACKDARK.PST","df48a11d8ca7ad3a4c53e0e2840c4c0df57ce01504751dd5ea62248bfcb6c0c2","72796957b8e4c31efb65579c0b8d0c0ba50a70f33019e3f12fecb7c6f558992b"},
{"TEXTURES/TRACKWHEEL.PST","de447b0debdad77b00d3f95d0919c0ec9d2a169229a18a61a5df9c722e240265","7f0f729633d84b80e538db64d8885e44af43c6ddc624fbc7ccfaf6b10ff48b02"},
{"TEXTURES/TRACKSIDE.PST","4db3e6270ec85d8ddd16af7445befd43bad7e03f61cc9399d9559095ae9d5d02","978385960ba70fa91c193f83c95995dde8024d1d8611288fc96e1df24bcfaefa"},
{"TEXTURES/CRAWLERA.PST","0130a604d965a47948f36d02e1b6b64aec3ed4cebcffeb36a389a88454aee7e4","c237c7ffa11e6f88867fc4b3d51dbc3cadcb555a6bd036d5dca1ba179ea90c6b"},
{"TEXTURES/SEATING1.PST","69fb246e2b8adc4e439a115b164178077b510dff3beb8720de72bcb332a49a54","245b6da73f049525f9af76f511017ca86210951a49e35a12bb76588294859e1a"},
{"TEXTURES/SHADE01.PST","17d2aba48656bd40bf0c3ad24fb6dc8be51a818ae76e8536655d44b402c339b7","ad624ff530ad787384e05b7f9f206d4d466862ff8eec3ddfb35bb6af91fcdf3e"},
{"TEXTURES/STOP01.PST","5d969201966fa87911136a85f08fdafad26c118ab2c9436d63f531a54da7f6a4","55b7373249fc0af0826d5c7246c00676cca3c4bba8f45c02a971c2f9e5319f18"},
{"TEXTURES/TOP01.PST","a56bd05f5000d1ea35e5dda73c4bd5055a6742644b96d419f2ee56aa58ebefae","ab28722e7ee87b8841834b64117be3e757e66b353052374d12c85e3ca71352c2"},
{"TEXTURES/TB_SAND1.PST","360f89dd2af6c4041e6984680b70ddd6caa0026f18a1f3bca4b174d48c0d509e","5e28884030af00accec4768ae862d01cff4a5c8cb8ea1aa1ae56f22fb42ee485"},
{"TEXTURES/TB_SAND2.PST","33295b3f6740182682d629c7b962e6a44563ad9b4f0c9cbfcae28bddaae7c269","cab1df3fe31fbb1d444a996582e810d3cd46cf0cab0279473aa0a211064a7382"},
{"TEXTURES/POLE01.PST","1850c927af158e75ec63bdf9c37fe13ecb68cdd56919cf0ec187efc1bf281633","355831ee3e87cbf878bb751715fd136ee392c4d62c18cbcefcd07e745100a9fc"},
{"TEXTURES/ARROWDROID03.PST","7e9021d48bbe4d3f09111710627d384888d959a7b4048abcf23f368a038e8056","ad4268fb740616f590611fe07233269aa13116ede9de97a4950eee71c2035a6b"},
{"TEXTURES/ARROWDROID01.PST","0cc7a8d515a392e195af12ee0c0f4610a19f8c91a0075b3a449f4005d88e4fce","4fa1d4ab73637972b1274e3dec63daf7edfde27a1302124feaa950a4f85b3286"},
{"TEXTURES/CORD.PST","91cb74edd2d0cb4f3ca102abea2f06cc5e539f21f6b5fb42b54d42baafdc39a1","ce5f2c98e85fb10437ac76198e9186d2782f10163717fd563c6fb2e4accb670b"},
{"TEXTURES/POLE02.PST","9667193f3f954bba023bea55969caf0dae6ccd760d5d5a0a82cf68d8dda6ed59","1c33443b9e118e53272bcdc289edf1d38a3035631688a1dd79e0a3aebef9cbdf"},
{"TEXTURES/STARTL01.PST","7b9d407a7dd75cfbcee9ca96b82e033df033f3c847514b97534c9e12db9afbaf","edaf9d1a80a6d1f34276a17f7fb4ad76bb0090573b30eef0b35051c2e79ba3dd"},
{"TEXTURES/JAWA004.PST","bbe430237496846122c54eb1ebe01b8bfccda9ff438dc58c8d0b7750e6766ddb","bbe6fe0fb1ba2c389c2a9b48c55b9e6dfd380eed1e6a0433d364baf93b8732bd"},
{"TEXTURES/FARMLG01.PST","c3b14a8a8677d26508f6212d208bd021ca5f992eda9a34474a14e3b957626897","aff1e7f440bcbbabdc0028eb1b3dd4b04159a0f70a507ad49f1c3a94a1011f15"},
{"TEXTURES/FARMLG02.PST","e648477937a96eac58297dbbf805bff8298129f07fd440045d03018de5e31737","28de47f429f4b3537b15aed27c70392333db0a6e85ebc3e6020a093663ad8887"},
{"TEXTURES/FARMLG03.PST","eb7c7f4ffc41be58faac963eb58a2c1a5e51dc2f61121214a0690570509282ae","f9d484966e1e58802a3e21820ec8261d6a4fd8465ed59079fe8e8ffa8e0d8c4e"},
{"TEXTURES/HANGER01.PST","b0d9ee05feabf5a618469b8ae0b6f3b1fcf7e58c4fdfe74e76d9368596471cb1","6428c850f206bd5ba8d5dba89e8da27c3ac52b8bda09451ca196cf54edc0b82f"},
{"TEXTURES/HANGER02.PST","1acb6731774d553f88bcd843828fa17298ae2a14171b8e16b967cc62da8c940d","018aff95526c3d0dee75dbdcf1021f2efa794b3aefdf48518045810b17bf9390"},
{"TEXTURES/HANGER03.PST","80d9859e170f7a29ca2c97d26fec7553620cf21c18712a1102f8c4844055707a","99896aa67c052bf2d304cad7f6c14757b1a19bbfdcc95de2dc15bb566baa8249"},
{"TEXTURES/HANGER04.PST","b0dce2ffdfd06becbb9bd6873c83e9d65726c491373f1fea61f260414851056f","57203136ddbe09dec1276e3aa6cc9ead86e1534be82068c40e56c40d261a8bd2"},
{"TEXTURES/HANGER05.PST","a4ae640a786bc8e40d71f1a5ba67fa4388a992d76de1986eeffbafe1f7363d23","7397888f49b24d0aaa2c0b4406c5a4743575202bf3dcfbe55cdb6ba7f4c9bf8f"},
{"TEXTURES/MOISFR01.PST","b12d1cb72c1fd1fcc140fefa10e70f9738dbd652684e1a01b1b8d31f87da07dc","de31249426299cd0e117527a4a4155019d3401eff227217154f776bed0e625c0"},
{"TEXTURES/MOISFR02.PST","2b7345ea8415ab0cb11c72a132965ec8757e35805506b45e0ca0f8867bb2d340","06351a57615d66df07fb3b35c1271d280127350720d37a7bd0b562e9fdc0b2cc"},
{"TEXTURES/MOISFR03.PST","a3bd45625cd066b7e079890e691f82aa360dcb409b8620b4bd553f78abe3ca5e","1d9435be63573c0f30b766da81c643d5627a41129194b0c5cbd7fa1dfb696e1a"},
{"TEXTURES/WHOUSE01.PST","ecef81eef0b566e82fc9e991b0b697b59229abcd6508d813dade186f08ab94c7","d2347e7a5dd2b86759213b524aa5ba590aa4839693eae184e4ff345dc15aafd2"},
{"TEXTURES/WHOUSE02.PST","1ea0260dd04866d3fe6863319c07170dad53b64ba74b8ef7f8b5edf733b7e6ed","4f2bb31b030b5a7117feed3cfc71c98e85fabfa8e565be827a5427aa0ecba9ed"},
{"TEXTURES/WHOUSE03.PST","fb3168115531586f52dbec5ca77ed351e79bee6bce094ae089361049962d444b","6beb6d70a41a00f5fe31558ce36c86990a4995da94024eed6ea28b59e84be680"},
{"TEXTURES/WHOUSE04.PST","f9cee3851a9c2cf0d51da4c879612d05cdf34d48b425c3f2082c5639fcc994c5","94a1775170e1dc1d39d044a6dc6e8f38219c364f689a630dff7f59f9338306f2"},
{"TEXTURES/WHOUSE05.PST","d12eb27ad815e9ff668c8b1b91646ed41e371cdbbcac1ae2ecc834e710cce312","e653c5f00c908f2ead293fb06f392c0db96ca3f1e3d4951690723e1139c44fd6"},
{"TEXTURES/WHOUSE06.PST","b4891d9519a6995f280bed3b774d624e08bba9c66226ed3f2807f1e0c53d7a57","572573d0c024bcc1883ab3b96d6efd71fceaab228f1e0294f0bf41ef0168ea6f"},
{"TEXTURES/JAWA003.PST","7381d37c4a4f28c9a53f3d4a2db3ca73bb9b6211b459e6434075481cbecf21cb","6fdc1ddede700b710b8c93b72bc4db075eca4cc01bbb2587365e7f4811684bab"},
{"TEXTURES/JAWA002.PST","b511c2930b5e2301c10b9dd867b3b38b4db7ef0b83be971d72825837a0b72ce2","8416fab162be1587305ae2865aab34a1f556b2c9475a1bd6a467c583663070fb"},
{"TEXTURES/JAWA001.PST","39e8a2e2b688dff9c0e44e329a78e695c928b70e7c7bbb86fdd62347d52c1e86","413d2031cd2a87961d4f503c95aa37cf31dd3b6b1638faf496ee5ff2647f084c"},
{"TEXTURES/JAWA006.PST","853f9ff4a717d47bd2726bda31c8909382d64cbf6b45bb93aea5e7c16d9bb2b9","dd814b18c537a8192a568e5968a4f720dcb0c14b757bf64ca5c0e5fe2c17caba"},
{"TEXTURES/JAWA008.PST","4be12e85413f0fe9998108f1ff9f41988f7477eaa6c00679a4f1df4589b93c42","9f962d18437919973af34d19825fef3fab298fa231f3574f2fd9d1e2f33bc182"},
{"TEXTURES/JAWA009.PST","41362a4c958981fc147de5e8651d26ab5c3bbefd83b1a3033ea9fbe32a4d9ba9","96ca2db2928270b45ca051ff6ebd825439068a70fac14db5a962569b1c9c16ea"},
{"TEXTURES/JAWA010.PST","614302a4d5d08ea6a1df4b2537bdc362539d5485e320fd24ea2f7c291651e288","3ca50ec57f5bc85e46fc3cae7ebe8c50359a1fc7f0792234f26fe27f305edce8"},
{"TEXTURES/UTINNI.PST","07b8b6fc2cc4c24024e287eec1ea76596267f9d128cfe21cacd41e8684bf77df","a9a2aa6554463febd8e2fc4c74dac3366fe45131fefb2ba51379506500ec4568"},
{"TEXTURES/JUNK01.PST","04f22f4e76b1222f3c7d916e2594a0c68de3d95b686d59fa58ab09049d0113d1","a9055397cbae23369b19f33d4d8d926c53361ac3f92a36fbb0ddb8011cb9da29"},
{"TEXTURES/JUNK05.PST","762b50e1415a81b89d02588c56bad69b073a88db2811c69b6ed802013a62fc58","2e36d6f6ca5620d92a99beb26c45e553c771c69c97d35c4ccd0d7ad5276b36f8"},
{"TEXTURES/JUNK06.PST","e51d8e1c55427e8a55566154577863c792d69c4207be57d235d7f6a338b92e1a","b6f49e3470528ee10283275a42bba7c1c714319ce9fd0fb515f5809232b91386"},
{"TEXTURES/JUNK07.PST","6a10879d0ef5c3369ad430b2461f5fbf60f117457bfc0fbc6d174fcad49b2c75","7b205f2430b05b7d771f74a9509c3586ae2ba83bd86104fb1151202b40e305a5"},
{"TEXTURES/JUNK02.PST","a9f188feecb0933757365d213d4f5ed7a7ec090c2cc3f448c4dfa133e767e35f","24e97f21841a4304e55047eb3732509bf346524c155e70caa5721a652450a955"},
{"TEXTURES/JUNK03.PST","53b74a536b34370ff67c03b4cfd2f5ad479f656a5d28246556051f55d06bd898","cce7c4d2649e0f54de31a19c91ffda0ab6d91c4a0696695ab1c1aaef3274eda5"},
{"TEXTURES/JUNK04.PST","13b12f5e95aead2eb48a1c3c2719975866c6036b22e8e9a5fca23ed4a53fd182","d159b217cd5b5b1c6b07198552437101a87760918daa913d26e165efc1238a8d"},
{"TEXTURES/JUNK08.PST","c987dbc94c03a7a2a902a24c6eef31789ad2179589be07fb88e5f0a544ab0e11","ad7b62972ec8e78cbee94b33ebe6799dcb626fffd91c49466d96054ee4f19e5b"},
{"TEXTURES/JUNK12.PST","52710389257b44f1fcef0ad3e6090726a2e4548598c29bd3b0e9eb2e3a344270","a921de43296767e02ee1d093057f2c4debda58dac2a8190628deac0b1437286c"},
{"TEXTURES/JUNK09.PST","c6213dd7d0f40845bf40ed4b5d73e2704d469800de09ce69177d6ce825efc0b2","b67024ca1c35da929f89f74420e4043bed84bed150e5fdf7ab9f121e27f61826"},
{"TEXTURES/JUNK10.PST","180be4ed2629a7053de8b84c6e159afdac73299f9dc5b21c5c517dabf2a02e39","fe3e2f5254502a051a79d8792577cbc5bd82e56a878a9f583df92c398f45c677"},
{"TEXTURES/JUNK11.PST","3b7d6306cc1ac720370b4692259d98c4c6dc12c45aadd39721f58d41916feb4e","1a1f46dd9fa1256e37e3f1e4d53bf01152a6de65ce031289650df8e445839937"},
{"TEXTURES/JUNK13.PST","5b0df64817a62bb6dd68918f7e90d750c08a88e9914db8f6284b4226c8067dc5","fefcd1bf65992c9ab97149785b0d3065961c524648445365abbf763325450293"},
{"TEXTURES/JUNK14.PST","335e5902701139f049898a99328b4efdccee6778862a2e3aebd8ec8824183862","98d1fcb7849cad8c729c70cd0b94e0c5f133d37e8973603c2531208103c5aa50"},
{"TEXTURES/JUNK15.PST","0f28ed52564c820cf8c6ee0f1a8d3c4eb9b1f9d34c21619616d311ea0c2b3a30","61f598cd9cda5513aac6e69c673539dcb7967bf3504f06b73d4808fd99ae4a31"},
{"TEXTURES/MVAP.PST","3c7fdf44934e0ad7eadc47737bf948218b1ff1c8032b83283d564c5018a63699","f698f297745cfd5b48dbd7d9d75bfaebc1b2319fb71a5e9829a1dc84906aec03"},
{"TEXTURES/LANDSPEEDER03.PST","7e0d945859776c31cc77952d18a8b6a5b33f8ad4e008273d5736486c2925db3c","2ed485c6cf0bb15ab8d8e67d5333015d8c0574f245f462fb207ae09b676f8164"},
{"TEXTURES/LANDSPEEDER04.PST","dafe89cc18a36acadc31b65884c0633bd24babbbbf1d95d8bc64d606d6b2e850","5f3d12a7d6f04c9e4c4f0e6d62d2672c9d4c0caac4c52a7b01194f2f40fde061"},
{"TEXTURES/LANDCRUSER01.PST","9c657892e909e2e87b7e972c81f782dd32277cda746c0b8281d494cac41d5996","8a0a395adadda0b3ca0a35db303831dd68fb5e9fff53af0b64cf192a87949cc9"},
{"TEXTURES/MUSH_2A.PST","73b6c594b18b456c25b148c4e1333a3c7784b67e2a4dffd397d0ac16e8304b70","53d6945cebdbd8b271305b2450d3199aad7420efc97b53aecdc42ae782a06e4f"},
{"TEXTURES/MUSH_3.PST","6a5de2b20e6eae20c521fbcc20faa5fe236bc60b349a0b3e534544e317cc48e8","4abff05e8b09743aaef3aa84a828225464ea7d9b6befbf5c93980f6214325f85"},
{"TEXTURES/MUSH_1.PST","b8e14b883f4d2f629cb05873477b9848caa8526c75380690720cd403bad1dff6","f34cd40ac460ceb92b652e530783464eb643375aece9b8040997e1200d4648b6"},
{"TEXTURES/MUSH_3A.PST","d8f96c1c7240483aa988f8f8ab25253060f5919e321795352ca1560d0f2838ad","779670dddd632ee9264b483c349fc805b336907b81994082bd2d9407613451d6"},
{"TEXTURES/MUSH_4.PST","0282cd1d92aeb3a8fd800e69f74c477a55abf568f8fef9ce6ccc0ea80b7e7e6c","a36133ab2fa9606ee9f3a1c78df6356bd8a187ab6ce0bff11789e73b892c78cf"},
{"TEXTURES/TA_ROCKA_A.PST","1637a14459349d79b53674c1f940edeb79109b91073ce56a210162fc45876e5e","875a150b92e4693a73d2c39e5ec97bf82d09931d2eb000e113947670cb1fc789"},
};
inline void checkNativeOutput(const std::string& member,const Bytes& source,const Bytes& output){
    const auto normalized=key(member);
    for(const auto& pin:nativeOutputPins)if(normalized==pin.member){
        shared::need(shared::sha256(source)==pin.sourceSha&&shared::sha256(output)==pin.outputSha,
            "Original TB native regression output differs");return;
    }
    shared::need(false,"Original TB native regression output pin missing");
}
inline const SourceImage* sourceImage(const std::string& member){
    const auto name=key(member);const SourceImage* found=nullptr;
    for(const auto& entry:sourceImages)if(key(entry.member)==name){
        shared::need(!found,"Ambiguous original TB source image pin");found=&entry;
    }
    return found;
}
inline std::uint32_t initialFrame(const a::MaterialFields48E& value){
    const auto text=field(value,"animatedMapInitialFrame");if(text.empty())return 0;
    std::uint32_t result=0;const auto parsed=std::from_chars(text.data(),text.data()+text.size(),result);
    shared::need(parsed.ec==std::errc{}&&parsed.ptr==text.data()+text.size()&&result<64,
        "Original TB ATM initial frame syntax/extent");return result;
}
// Decode only supplied original bytes. The optional output records whether an
// already retained independent image hash was available and checked. Ordinary
// image hashes are recorded after decoding, never used to select pixel data.
inline Bytes image(const std::string& member,const Bytes& source,std::uint32_t& mipIndex,
    bool& animated,a::DecodeError48C& error,bool* independentlyPinned=nullptr){
    shared::need(source.size()>=12&&source.size()<=8u*1024u*1024u,"Original TB texture source extent");
    const auto name=key(member);const auto* pin=sourceImage(name);
    const source_texture50fc_fixture::detail::Entry* outputPin=nullptr;
    for(const auto& entry:exceptionalImages)if(key(entry.member)==name){
        shared::need(!outputPin,"Ambiguous original TB decoded image pin");outputPin=&entry;
    }
    for(const auto& entry:indexedPins)if(key(entry.member)==name){
        if(outputPin)shared::need(std::string(outputPin->outputSha256)==entry.outputSha256,"TB independent pixel pin disagreement");
        outputPin=&entry;
    }
    shared::need(pin||outputPin,"Original TB image has no authenticated source identity");
    const auto sourceSha=shared::sha256(source);
    if(pin)shared::need(source.size()==pin->sourceBytes&&sourceSha==pin->sourceSha256,
        "Original TB static image source identity differs");
    if(outputPin)shared::need(source.size()==outputPin->sourceBytes&&sourceSha==outputPin->sourceSha256,
        "Original TB exceptional image source identity differs");
    animated=name.ends_with(".ATM");
    shared::need(animated||name.ends_with(".PST"),"Unsupported original TB image extension");
    shared::need(!animated||(pin&&source.size()>=24&&pin->initialFrame<pin->frameCount),
        "Original TB ATM lacks its exact frame selection");
    const auto frameIndex=pin?pin->initialFrame:0u;
    const auto kind=shared::u32(source.data()+(animated?16:4));
    if(pin)shared::need(kind==pin->kind,"Original TB source image kind differs");
    Bytes output;
    if(kind==2){
        a::detail::OriginalIpumMipExtent50LA mip;
        if(animated){
            std::vector<a::detail::OriginalIpumFrameExtent50LA> frames;std::size_t bad=0;
            shared::need(a::detail::frameOriginalIpumAtm50LA(source,frames,bad)&&
                frames.size()==pin->frameCount&&frameIndex<frames.size(),
                "Original TB IPUM ATM framing failed at "+std::to_string(bad));
            const auto& frame=frames[frameIndex];mipIndex=frame.count-1;mip=frame.mips[mipIndex];
        }else mip=framePst(source,mipIndex);
        a::TextureMip48E decoded;decoded.width=mip.width;decoded.height=mip.height;std::string why;
        shared::need(a::decodeOriginalIpuRgba(source.data()+mip.payload,mip.bytes,mip.width,mip.height,decoded.rgba,why),
            "Original TB IPUM decode: "+why);
        shared::need(a::encodeTextureLevel48E(decoded,output,error),"Original TB IPUM output framing: "+error.code);
    }else{
        shared::need(kind==0||kind==1,"Original TB indexed texture kind");Bytes pst;
        if(animated){
            a::OriginalUiAtm50KG frames;
            shared::need(a::splitOriginalUiAtm50KG(source.data(),source.size(),frames,error)&&
                frames.frames.size()==pin->frameCount&&frameIndex<frames.frames.size(),
                "Original TB indexed ATM framing: "+error.code);
            const auto& frame=frames.frames[frameIndex];
            pst.assign(source.begin()+frame.pstOffset,source.begin()+frame.pstOffset+frame.pstBytes);
        }
        a::Texture48E decoded;
        shared::need(a::decodeTexture48E(animated?pst:source,decoded,error)&&!decoded.mips.empty(),
            "Original TB indexed PST decode: "+error.code);
        mipIndex=static_cast<std::uint32_t>(decoded.mips.size()-1);output=std::move(decoded.base_rrtex);
    }
    const auto w=pin?pin->width:outputPin->width,h=pin?pin->height:outputPin->height;
    const auto expectedMip=pin?pin->mipIndex:outputPin->mipIndex;
    shared::need(mipIndex==expectedMip&&output.size()==std::size_t(w)*h*4+16&&
        !std::memcmp(output.data(),"RRTEX001",8)&&shared::u32(output.data()+8)==w&&shared::u32(output.data()+12)==h,
        "Original TB decoded image source framing differs");
    if(outputPin)shared::need(mipIndex==outputPin->mipIndex&&w==outputPin->width&&h==outputPin->height&&
        shared::sha256(output)==outputPin->outputSha256,"Original TB independently pinned decoded image differs");
    // Native regression checking does not change independent provenance.
    if(!outputPin)checkNativeOutput(name,source,output);
    if(independentlyPinned)*independentlyPinned=outputPin!=nullptr;
    return output;
}
// The retained original byte buffer alone does not authenticate mutable parsed
// fields or their cached typed order. Re-encode the entire parameter document,
// then compare every typed/source-provenance field with a fresh exact decode.
inline void validateBorrowedWorld(const Bytes& scb,const a::World48F& world,a::DecodeError48C& error){
    Bytes roundtrip;a::World48F canonical;
    shared::need(world.parameters.original_bytes==scb&&
        a::encodeParameters48F(world.parameters,roundtrip,error)&&roundtrip==scb&&
        a::decodeWorld48F(scb,canonical,error)&&a::validateWorldSemantics48F(world,error),
        "Original TB borrowed world fields differ from authenticated SCB");
    const auto& actual=world.parameters;const auto& source=canonical.parameters;
    shared::need(actual.version==source.version&&actual.header_table_offset==source.header_table_offset&&
        actual.preservesDuplicateLabels==source.preservesDuplicateLabels&&actual.blocks.size()==source.blocks.size(),
        "Original TB parameter provenance header differs");
    for(std::size_t i=0;i<source.blocks.size();++i){
        const auto& x=actual.blocks[i];const auto& y=source.blocks[i];
        shared::need(x.name==y.name&&x.name_offset==y.name_offset&&x.table_entry_offset==y.table_entry_offset&&
            x.offset==y.offset&&x.size_without_count==y.size_without_count&&x.fields.size()==y.fields.size(),
            "Original TB parameter block provenance differs");
        for(std::size_t j=0;j<y.fields.size();++j){const auto& f=x.fields[j];const auto& g=y.fields[j];
            shared::need(f.name==g.name&&f.value==g.value&&f.entry_offset==g.entry_offset&&
                f.value_offset==g.value_offset&&f.value_bytes==g.value_bytes,
                "Original TB parameter field provenance differs");
        }
    }
    shared::need(world.scene_info_block==canonical.scene_info_block&&world.instances.size()==expectedInstances&&
        world.instances.size()==canonical.instances.size()&&world.zones.size()==expectedZones&&
        world.zones.size()==canonical.zones.size()&&world.portals.size()==expectedPortals&&world.portals.size()==canonical.portals.size(),
        "Original TB typed world source extent differs");
    for(std::size_t i=0;i<canonical.zones.size();++i){const auto& x=world.zones[i];const auto& y=canonical.zones[i];
        shared::need(x.zone==y.zone&&x.models==y.models&&x.parameter_block==y.parameter_block,
            "Original TB typed zone order/provenance differs");
    }
    const auto same=[](const auto& x,const auto& y){
        return !std::memcmp(x.data(),y.data(),sizeof(x[0])*x.size());
    };
    for(std::size_t i=0;i<canonical.instances.size();++i){const auto& x=world.instances[i];const auto& y=canonical.instances[i];
        shared::need(x.section==y.section&&x.geometry_member==y.geometry_member&&x.collision_member==y.collision_member&&
            x.zone==y.zone&&x.ordinal==y.ordinal&&x.parameter_block==y.parameter_block&&
            x.source_block_offset==y.source_block_offset&&x.use_lighting==y.use_lighting&&
            same(x.position,y.position)&&same(x.look,y.look)&&same(x.up,y.up)&&same(x.matrix,y.matrix),
            "Original TB typed instance order/provenance differs");
    }
    for(std::size_t i=0;i<canonical.portals.size();++i){const auto& x=world.portals[i];const auto& y=canonical.portals[i];
        shared::need(x.section==y.section&&x.parameter_block==y.parameter_block&&x.zones==y.zones,
            "Original TB typed portal order/provenance differs");
        for(std::size_t j=0;j<y.vertices.size();++j)shared::need(same(x.vertices[j],y.vertices[j]),
            "Original TB typed portal vertices differ");
    }
}
} // namespace detail

// Compatible direct-source callback for independently owned TB material consumers.
// Never reads a retained image filename; failures preserve the caller's image.
inline bool decodeSourceTexture(const char* member,const Bytes& source,Bytes& output,
    std::uint32_t& mipIndex,std::string& error)noexcept {
    try{
        shared::need(member&&*member,"Original TB source texture member required");
        a::DecodeError48C diagnostic;std::uint32_t mip=0;bool animated=false;
        auto next=detail::image(member,source,mip,animated,diagnostic);
        output=std::move(next);mipIndex=mip;error.clear();return true;
    }catch(const std::exception& e){try{error=e.what();}catch(...){}return false;}
    catch(...){try{error="Original TB source texture decode failed";}catch(...){}return false;}
}

class Owner final {
    struct Scene {a::Scene48D decoded;std::uint64_t identity=0;std::vector<std::uint32_t> materials;};
    struct Storage {
        std::shared_ptr<const tb_cold_sources50LA::Owner> source;
        std::vector<a::TrackProjectedUv> projectionUvs;
        std::map<std::string,std::size_t> entries;
        std::map<std::string,Scene> scenes;
        std::vector<a::Material48E> materials;
        std::vector<std::vector<std::uint32_t>> materialPasses;
        std::vector<std::uint8_t> independentlyPinnedImages;
        a::Mesh48B mesh;
        std::vector<a::WorldNormal50HK> normals;

        a::MaterialTextureAssets50EA bank;
        std::vector<tm::Span> spans;
        std::vector<tm::Selection> selections;
        std::vector<tm::TextureSelection> textures;
        std::shared_ptr<a::OriginalStandardMaterialDomain50LA> canonicalDomain;
        bool canonicalAttempted=false;
        const a::ResourceEntry48C* entry(const std::string& name)const{
            const auto found=entries.find(detail::key(name));
            return found==entries.end()?nullptr:&source->archive().entries()[found->second];
        }
        Bytes member(const a::ResourceEntry48C& entry,a::DecodeError48C& error)const{
            Bytes bytes;shared::need(entry.bytes&&entry.bytes<=8u*1024u*1024u&&
                source->extractMember(entry.name,bytes,error,entry.bytes)&&bytes.size()==entry.bytes,
                "Original TB archive member extraction: "+entry.name+" "+error.code);return bytes;
        }
    };
    std::unique_ptr<Storage> storage_;
public:
    Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
    bool loaded()const noexcept{return bool(storage_);}
    const a::Mesh48B* mesh()const noexcept{return storage_?&storage_->mesh:nullptr;}
    const std::vector<a::WorldNormal50HK>* normals()const noexcept{return storage_?&storage_->normals:nullptr;}
    const a::MaterialTextureAssets50EA* bank()const noexcept{return storage_?&storage_->bank:nullptr;}
    const std::vector<tm::Span>* spans()const noexcept{return storage_?&storage_->spans:nullptr;}
    const std::vector<tm::Selection>* selections()const noexcept{return storage_?&storage_->selections:nullptr;}
    const std::vector<tm::TextureSelection>* textures()const noexcept{return storage_?&storage_->textures:nullptr;}
    const std::vector<a::Material48E>* materials()const noexcept{return storage_?&storage_->materials:nullptr;}
    const std::vector<std::uint8_t>* independentlyPinnedImages()const noexcept{return storage_?&storage_->independentlyPinnedImages:nullptr;}
    const std::vector<a::TrackProjectedUv>* projectionUvs()const noexcept{return storage_?&storage_->projectionUvs:nullptr;}
    std::shared_ptr<const tb_cold_sources50LA::Owner> sourceOwner()const noexcept{return storage_?storage_->source:nullptr;}
    bool load(std::shared_ptr<const tb_cold_sources50LA::Owner> sourceOwner,Diagnostic& d)noexcept{
        try{
            d={};shared::need(!storage_&&std::fegetround()==FE_TONEAREST,"TB direct cold owner already loaded or rounding mode differs");
            shared::need(sourceOwner&&sourceOwner->ready(),"Original TB shared cold source absent");
            auto next=std::make_unique<Storage>();next->source=std::move(sourceOwner);d.stage="archive";
            const auto& archive=next->source->archive();const auto& world=next->source->world();
            shared::need(archive.entries().size()==entryCount&&world.instances.size()==expectedInstances&&
                world.zones.size()==expectedZones&&world.portals.size()==expectedPortals&&
                world.parameters.original_bytes.size()==scbBytesCount&&shared::sha256(world.parameters.original_bytes)==scbSha,
                "Original TB exact shared source/world identity");
            for(std::size_t i=0;i<archive.entries().size();++i)
                shared::need(next->entries.emplace(detail::key(archive.entries()[i].name),i).second,"Ambiguous original TB member");
            std::map<std::string,std::uint32_t> materialIndices,textureIndices;
            std::vector<Bytes> images;
            std::vector<a::MaterialTextureSlotInput50EA> slots;
            constexpr std::uint64_t textureBase=assetIdentity<<48;
            std::size_t rgbaBytes=0,serializedBytes=0;
            auto material=[&](const std::string& materialName)->std::uint32_t{
                const auto normalized=detail::key(materialName);const auto existing=materialIndices.find(normalized);
                if(existing!=materialIndices.end())return existing->second;
                shared::need(shared::name(materialName),"Invalid original TB material basename");
                d.stage="material";const auto* psm=next->entry("textures\\"+materialName+".psm");
                shared::need(psm,"Original TB world material absent: "+materialName);d.member=psm->name;
                const auto source=next->member(*psm,d.decode);a::Material48E authored;
                const bool decoded=a::decodeMaterial48E(source,authored,d.decode);
                shared::need(decoded&&!authored.passes.empty()&&authored.passes.size()<=2,
                    "Original TB material/program pass count");
                const auto materialIndex=static_cast<std::uint32_t>(next->materials.size());
                shared::need(materialIndex<expectedMaterials,"Original TB PSM count bound");
                const auto materialKey=shared::sha256(std::string("rr-native-material-48E-v1\n")+shared::sha256(source));
                std::vector<std::uint32_t> selectedPasses;
                for(std::size_t pi=0;pi<authored.passes.size();++pi){
                    const auto& pass=authored.passes[pi];const bool projected=pass.microprogram=="ProjectedPrelit";
                    shared::need(pass.pass==pi+1&&pass.textures.size()==1&&pass.textures[0].slot==1&&
                        ((authored.passes.size()==2&&projected)||
                         (authored.passes.size()==1&&!projected&&
                          (detail::key(pass.microprogram)=="PRELITCOMPRESSED"||pass.microprogram=="DiffuseCompressed"||
                           (normalized=="TC_SHIELD"&&pass.microprogram=="diffuseTextransCompressed")))),
                        "Unjoined original TB pass/program/texture subset");
                    const auto& map=pass.textures[0];shared::need(shared::name(map.name),"Invalid original TB texture basename");
                    const auto pstCandidate=map.name+".pst";
                    const auto dot=pstCandidate.find_last_of('.');
                    shared::need(dot!=std::string::npos,"TB source texture candidate lacks extension");
                    const auto stem=pstCandidate.substr(0,dot);
                    const a::ResourceEntry48C* texture=nullptr;
                    for(const char* extension:{".pst",".ipu",".tga",".atm"}){
                        texture=next->entry("textures\\"+stem+extension);if(texture)break;
                    }
                    shared::need(texture,"Original TB required texture absent: "+map.name);
                    const auto textureKey=detail::key(texture->name);const auto* pin=detail::sourceImage(textureKey);
                    shared::need(pin,"Original TB static image lacks exact source pin");
                    const bool animated=textureKey.ends_with(".ATM");
                    if(animated)shared::need(authored.passes.size()==1&&detail::initialFrame(pass.fields)==pin->initialFrame,
                        "Original TB material/ATM initial frame differs");
                    auto found=textureIndices.find(textureKey);
                    if(found==textureIndices.end()){
                        d.stage="texture";d.member=texture->name;const auto bytes=next->member(*texture,d.decode);
                        std::uint32_t mip=0;bool decodedAnimated=false,independent=false;
                        auto image=detail::image(texture->name,bytes,mip,decodedAnimated,d.decode,&independent);
                        shared::need(decodedAnimated==animated,"Original TB texture animation source classification");
                        const auto index=static_cast<std::uint32_t>(images.size());
                        shared::need(index<expectedImages&&image.size()>=16,"TB original texture bank bound");
                        const auto w=shared::u32(image.data()+8),h=shared::u32(image.data()+12);
                        shared::need(w&&h&&w<=1024&&h<=1024&&image.size()==std::size_t(w)*h*4+16,"TB native image extent");
                        shared::add(rgbaBytes,std::size_t(w)*h*4,16u*1024u*1024u,"TB texture RGBA budget");
                        shared::add(serializedBytes,image.size(),16u*1024u*1024u,"TB texture framing budget");
                        tm::TextureSelection selected;selected.identity=textureBase+index+1;selected.bankIndex=index;
                        selected.mipIndex=mip;selected.width=w;selected.height=h;selected.path=texture->name;
                        selected.key="disc:"+textureKey+":"+pin->sourceSha256;
                        selected.sha256=shared::sha256(image); // Actual runtime output identity, not necessarily an independent pin.
                        next->textures.push_back(std::move(selected));images.push_back(std::move(image));
                        next->independentlyPinnedImages.push_back(independent?1:0);
                        if(independent)++d.independentlyPinnedImages;else ++d.runtimeIdentityImages;
                        found=textureIndices.emplace(textureKey,index).first;
                    }
                    const auto index=static_cast<std::uint32_t>(next->selections.size());
                    shared::need(index<expectedSelections,"Original TB authored pass count bound");
                    const auto& image=next->textures[found->second];tm::Selection selected;
                    selected.assetIdentity=assetIdentity;selected.materialIndex=materialIndex;selected.pass=pass.pass;selected.slot=1;
                    selected.bankSlot=static_cast<std::uint32_t>(slots.size());selected.textureIndex=found->second;selected.mipIndex=image.mipIndex;
                    selected.materialName=materialName;selected.materialKey=materialKey;
                    selected.program=pass.microprogram;selected.textureName=map.name;selected.textureKey=image.key;
                    selected.track=selectedTrack;selected.passCount=static_cast<std::uint32_t>(authored.passes.size());
                    selected.clampU=detail::clamp(pass.fields,"wrapModeHorizontal");selected.clampV=detail::clamp(pass.fields,"wrapModeVertical");
                    selected.sourcePassFields=detail::fields(pass.fields);selected.staticAnimationFrame=animated;
                    selected.projectedPrelit=projected;
                    if(projected)selected.texGenMatrix=tm::detail::projection(selected.sourcePassFields);
                    selected.decodedSourceMember=texture->name;selected.decodedSourceSha256=pin->sourceSha256;
                    slots.push_back({{selected.assetIdentity,materialIndex,pass.pass,1},image.identity,a::MaterialTextureRequirement50EA::required});
                    selectedPasses.push_back(index);next->selections.push_back(std::move(selected));
                }
                next->materials.push_back(std::move(authored));next->materialPasses.push_back(std::move(selectedPasses));
                materialIndices.emplace(normalized,materialIndex);return materialIndex;
            };
            constexpr std::array<double,16> identity{{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}};
            for(std::size_t wi=0;wi<world.instances.size();++wi){
                const auto& placement=world.instances[wi];d.stage="scene";d.member=placement.geometry_member;
                const auto* entry=next->entry(placement.geometry_member);
                if(!entry){shared::need(next->entry(placement.collision_member),"TB placement lacks both original PSG and COL");++d.collisionOnly;continue;}
                const auto name=detail::key(entry->name);auto found=next->scenes.find(name);
                if(found==next->scenes.end()){
                    Scene scene;scene.identity=next->scenes.size()+1;
                    const auto originalScene=next->member(*entry,d.decode);
                    a::SceneLimits48D sceneLimits;sceneLimits.allow_signed32_quantized_positions=true;
                    shared::need(a::decodeScene48D(originalScene,scene.decoded,d.decode,nullptr,sceneLimits),
                        "Original TB PSG decode: "+entry->name+" "+d.decode.code);
                    shared::need(scene.decoded.nodes.size()==1&&scene.decoded.nodes[0].parent==-1&&
                        !scene.decoded.identity_matrix_palette&&!scene.decoded.lods.empty(),"Original TB static hierarchy subset");
                    for(const auto& name:scene.decoded.materials)scene.materials.push_back(material(name));
                    found=next->scenes.emplace(name,std::move(scene)).first;
                }
                const auto& scene=found->second;const auto& matrix=scene.decoded.world_space?identity:placement.matrix;
                a::WorldNormalTransform50HK normalTransform;
                shared::need(a::prepareWorldNormalTransform50HK(matrix,normalTransform),"Original TB visual/normal transform");
                for(std::size_t si=0;si<scene.decoded.lods.front().surfaces.size();++si){
                    const auto& surface=scene.decoded.lods.front().surfaces[si];d.stage="surface";d.member=entry->name;
                    shared::need(surface.material_index<scene.materials.size()&&!surface.vertices.empty()&&!surface.indices.empty()&&
                        !(surface.indices.size()%3)&&surface.vertices.size()<=a::max_mesh_vertices-next->mesh.vertices.size()&&
                        surface.indices.size()<=a::max_mesh_indices-next->mesh.indices.size(),"TB original surface/material/geometry bounds");
                    tm::Span span;span.vertexStart=static_cast<std::uint32_t>(next->mesh.vertices.size());
                    span.vertexCount=static_cast<std::uint32_t>(surface.vertices.size());span.indexStart=static_cast<std::uint32_t>(next->mesh.indices.size());
                    span.indexCount=static_cast<std::uint32_t>(surface.indices.size());span.worldInstanceIndex=static_cast<std::uint32_t>(wi);
                    span.surfaceIndex=static_cast<std::uint32_t>(si);span.hasOriginalColors=surface.has_original_colors;
                    const auto& selectedPasses=next->materialPasses.at(scene.materials[surface.material_index]);
                    shared::need(!selectedPasses.empty()&&selectedPasses.size()<=2,"Original TB surface material pass closure");
                    for(const auto& source:surface.vertices){
                        shared::need(source.bone==0,"TB original static vertex bone");a::Vertex48B vertex{};
                        for(unsigned axis=0;axis<3;++axis){double value=0;
                            for(unsigned k=0;k<3;++k)value+=source.position_local[k]*matrix[k*4+axis];value+=matrix[12+axis];
                            vertex.position[axis]=static_cast<float>(value);shared::need(std::isfinite(value)&&std::isfinite(vertex.position[axis]),"TB nonfinite original vertex");
                        }
                        for(unsigned axis=0;axis<2;++axis){vertex.uv[axis]=static_cast<float>(source.uv[axis]);shared::need(std::isfinite(vertex.uv[axis]),"TB nonfinite original UV");}
                        for(unsigned axis=0;axis<4;++axis)vertex.color[axis]=surface.has_original_colors?source.color[axis]:255;
                        a::WorldNormal50HK normal;shared::need(a::transformPreparedWorldNormal50HK(source.normal_local,normalTransform,normal),"TB original normal transform");
                        if(!normal.xyz[0]&&!normal.xyz[1]&&!normal.xyz[2])++d.zeroNormals;
                        next->mesh.vertices.push_back(vertex);next->normals.push_back(normal);
                        // Original ProjectedPrelit consumes quantized source-space XYZ,
                        // before the SCB/world transform, for each authored pass.
                        a::TrackProjectedUv uv{{vertex.uv[0],vertex.uv[1],vertex.uv[0],vertex.uv[1]}};
                        for(const auto selectedPass:selectedPasses){
                            const auto& selection=next->selections[selectedPass];
                            if(!selection.projectedPrelit)continue;
                            for(unsigned axis=0;axis<2;++axis){double value=0;
                                for(unsigned k=0;k<3;++k)value+=(source.position_local[k]-scene.decoded.quantization[4+k])*selection.texGenMatrix[k*4+axis];
                                value+=selection.texGenMatrix[12+axis];
                                shared::need(std::isfinite(value)&&std::isfinite(static_cast<float>(value)),"TB projected source UV nonfinite");
                                uv.uv[(selection.pass-1)*2+axis]=static_cast<float>(value);
                            }
                        }
                        next->projectionUvs.push_back(uv);

                    }
                    for(const auto index:surface.indices){shared::need(index<surface.vertices.size(),"TB original surface index");next->mesh.indices.push_back(span.vertexStart+index);}
                    // Duplicate draw spans only. Vertex/index storage stays shared.
                    for(const auto selectedPass:selectedPasses){
                        const auto& selection=next->selections[selectedPass];
                        span.materialSelection=selectedPass;span.textureIndex=selection.textureIndex;
                        span.sourcePass=selection.pass;span.sourcePassCount=selection.passCount;
                        span.projectionPass=selection.projectedPrelit?selection.pass:0;
                        next->spans.push_back(span);if(selection.pass==2)++d.secondPasses;
                    }
                    ++d.surfaces;
                }
                ++d.placements;
            }
            d.scenes=next->scenes.size();d.stage="closure";d.member.clear();
            d.materials=next->materials.size();d.selections=next->selections.size();
            shared::need(d.scenes==expectedScenes&&d.placements==expectedPlacements&&d.collisionOnly==expectedCollisionOnly&&d.surfaces==expectedSurfaces&&
                d.materials==expectedMaterials&&d.selections==expectedSelections&&d.secondPasses==expectedSecondPasses&&next->spans.size()==expectedSurfaces+expectedSecondPasses&&
                next->mesh.vertices.size()==expectedVertices&&next->mesh.indices.size()==expectedIndices&&images.size()==expectedImages&&
                next->normals.size()==next->mesh.vertices.size()&&
                next->projectionUvs.size()==next->mesh.vertices.size()&&rgbaBytes==expectedRgbaBytes&&d.independentlyPinnedImages==expectedPinnedImages&&d.runtimeIdentityImages==expectedImages-expectedPinnedImages,
                "Original TB full static source closure differs");
            for(unsigned axis=0;axis<3;++axis){next->mesh.bounds.minimum[axis]=std::numeric_limits<float>::infinity();
                next->mesh.bounds.maximum[axis]=-std::numeric_limits<float>::infinity();}
            for(const auto& vertex:next->mesh.vertices)for(unsigned axis=0;axis<3;++axis){
                next->mesh.bounds.minimum[axis]=std::min(next->mesh.bounds.minimum[axis],vertex.position[axis]);
                next->mesh.bounds.maximum[axis]=std::max(next->mesh.bounds.maximum[axis],vertex.position[axis]);}
            std::vector<a::MaterialTextureAssetInput50EA> textureInput;
            for(std::size_t i=0;i<images.size();++i)textureInput.push_back({next->textures[i].identity,&images[i]});
            a::MaterialTextureAssetsInput50EA input;input.textures=textureInput.data();input.textureCount=static_cast<std::uint32_t>(textureInput.size());
            input.slots=slots.data();input.slotCount=static_cast<std::uint32_t>(slots.size());
            input.budgets={expectedImages,expectedSelections,16u*1024u*1024u,16u*1024u*1024u,1024};
            d.stage="pixel_bank";shared::need(next->bank.load(input,d.bank),"Original TB pixel bank rejected: "+d.bank.parserMessage);
            storage_=std::move(next);d.stage="complete";return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original TB native cold preparation failed";}catch(...){}return false;}
    }

    // Same Application-owned domain as the actual installed racers. This does
    // not register synthetic static models or claim source render dispatch for
    // the native world above. Mounting the real complete TB namespace preserves
    // the provider's local resource lookup before genuine racer population.
    bool populateCanonicalRacers(const tm::Request& request,std::size_t availableAdditionalPayloadBytes,tm::Diagnostic& d)noexcept{
        try{
            d={};shared::need(storage_&&!storage_->canonicalAttempted,"TB canonical population requires fresh native source owner");
            const bool canonical=bool(request.canonicalDomain);
            shared::need(canonical==bool(request.canonicalContext)&&canonical==(request.canonicalMaterialPath.actualPrefix36B970!=nullptr),
                "TB canonical Application/Video/path must be supplied together");
            if(!canonical){shared::need(request.canonicalRacerModels.empty(),"TB racer sources lack their canonical domain");return true;}
            shared::need(request.track==selectedTrack&&request.canonicalDomain->prepared()&&
                request.canonicalContext->live()&&*request.canonicalMaterialPath.actualPrefix36B970=="textures\\",
                "TB canonical racers require their actual gameplay directory and live Video");
            storage_->canonicalAttempted=true;storage_->canonicalDomain=request.canonicalDomain;
            const auto& provider=request.canonicalDomain->archiveProvider3E1A70();
            a::OriginalStandardArchiveProvider50LA::Diagnostic mounted;d.stage="tb_original_archive_mount";
            shared::need(provider&&storage_->source->mountCanonicalArchive(*provider,availableAdditionalPayloadBytes,mounted)&&
                provider->sealNativeMounts(mounted)&&request.canonicalDomain->resolver3E1A70()->bindProvider3E1A70(provider,d.canonical.texture),
                "Original TB canonical provider mount rejected");
            std::vector<std::shared_ptr<const a::OriginalStandardMaterialSource50LA>> materials;
            std::vector<a::OriginalStandardTextureSourceInput50LA> textures;
            d.stage="tb_original_racer_sources";
            for(const auto& racer:request.canonicalRacerModels)
                shared::need(racer.resource&&racer.references&&racer.references->table382B38()==request.canonicalDomain->materials382B38().get()&&
                    original_racer_material_sources50LA::appendModelSources(*racer.resource,*racer.references,
                        *request.canonicalMaterialPath.actualPrefix36B970,*provider,materials,textures,d.racerSource),
                    "Original TB canonical racer material/texture closure rejected");
            shared::need(request.canonicalDomain->appendOriginalSources(materials,textures,d.canonical),"Original TB canonical racer source admission rejected");
            d.stage="tb_original_racer_population";
            shared::need(request.canonicalDomain->populate(request.canonicalContext,d.canonical),"Original TB canonical racer population rejected");
            return true;
        }catch(const std::exception& error){try{d.message=error.what();}catch(...){}return false;}
        catch(...){try{d.message="Original TB canonical racer preparation failed";}catch(...){}return false;}
    }

    // Preserve the authenticated TB material-keyed authored-pass alpha policy.
    bool alpha(std::size_t selection,const a::OriginalUiMaterialTemplates50KG& templates,
        const a::OriginalUiMaterialRuntime50KG& runtime,material_alpha50gx_fixture::Policy& output)const noexcept{
        try{return storage_&&selection<storage_->selections.size()&&
            material_alpha50gx_fixture::selectTb(storage_->selections[selection],templates,runtime,output);}
        catch(...){return false;}
    }
    // Borrow the same authenticated archive for a separate TB cold decoder. The
    // archive lifetime is the world lifetime and cannot be replaced by reload.
    const a::IndexedResArchive48C* archive()const noexcept{return storage_?&storage_->source->archive():nullptr;}
};

