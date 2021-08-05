#include "main.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "GlobalNamespace/RelativeScoreAndImmediateRankCounter.hpp"
#include "GlobalNamespace/FlyingScoreEffect.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

MAKE_HOOK_MATCH(RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, &GlobalNamespace::RelativeScoreAndImmediateRankCounter::UpdateRelativeScoreAndImmediateRank, void, GlobalNamespace::RelativeScoreAndImmediateRankCounter* self, int score, int modifiedScore, int maxPossibleScore, int maxPossibleModifiedScore) {
    // do nothing so the acc stays at 100% and rank stays at SS
}

MAKE_HOOK_MATCH(FlyingScoreEffect_InitAndPresent, &GlobalNamespace::FlyingScoreEffect::InitAndPresent, void, GlobalNamespace::FlyingScoreEffect* self, ByRef<GlobalNamespace::NoteCutInfo> noteCutInfo, int multiplier, float duration, UnityEngine::Vector3 targetPos, UnityEngine::Quaternion rotation, UnityEngine::Color color) {
    // run original method
    FlyingScoreEffect_InitAndPresent(self, noteCutInfo, multiplier, duration, targetPos, rotation, color);

    // get _maxCutDistanceScoreIndicator and set the enabled property to true
    auto* underlineIndicator = RET_V_UNLESS(getLogger(), il2cpp_utils::GetFieldValue(self, "_maxCutDistanceScoreIndicator"));
    RET_V_UNLESS(getLogger(), il2cpp_utils::SetPropertyValue(underlineIndicator, "enabled", true));
}

MAKE_HOOK_MATCH(FlyingScoreEffect_HandleSaberSwingRatingCounterDidChange, &GlobalNamespace::FlyingScoreEffect::HandleSaberSwingRatingCounterDidChange, void, GlobalNamespace::FlyingScoreEffect* self, GlobalNamespace::ISaberSwingRatingCounter* saberSwingRatingCounter, float rating) {
    // run original method
    FlyingScoreEffect_HandleSaberSwingRatingCounterDidChange(self, saberSwingRatingCounter, rating);

    // set the text from _text to 115
    auto* textObj = RET_V_UNLESS(getLogger(), il2cpp_utils::GetFieldValue(self, "_text"));
    RET_V_UNLESS(getLogger(), il2cpp_utils::SetPropertyValue(textObj, "text", il2cpp_utils::newcsstr("115")));
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank);
    INSTALL_HOOK(getLogger(), FlyingScoreEffect_InitAndPresent);
    INSTALL_HOOK(getLogger(), FlyingScoreEffect_HandleSaberSwingRatingCounterDidChange);
    getLogger().info("Installed all hooks!");
}