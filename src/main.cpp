#include "main.hpp"
#include "extern/beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "extern/codegen/include/GlobalNamespace/FlyingScoreEffect.hpp"

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


MAKE_HOOK_OFFSETLESS(RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, void, Il2CppObject* self, int score, int modifiedScore, int maxPossibleScore, int maxPossibleModifiedScore) {
    // do nothing so the acc stays at 100% and rank stays at SS
}

MAKE_HOOK_OFFSETLESS(FlyingScoreEffect_InitAndPresent, void, Il2CppObject* self, Il2CppObject* noteCutInfo, int multiplier, float duration, UnityEngine::Vector3 targetPos, UnityEngine::Quaternion rotation, UnityEngine::Color color) {
    // run original code
    FlyingScoreEffect_InitAndPresent(self, noteCutInfo, multiplier, duration, targetPos, rotation, color);

    // get _maxCutDistanceScoreIndicator and set the enabled property to true
    auto* underlineIndicator = RET_V_UNLESS(getLogger(), il2cpp_utils::GetFieldValue(self, "_maxCutDistanceScoreIndicator"));
    RET_V_UNLESS(getLogger(), il2cpp_utils::SetPropertyValue(underlineIndicator, "enabled", true));
}

MAKE_HOOK_OFFSETLESS(FlyingScoreEffect_HandleSaberSwingRatingCounterDidChange, void, Il2CppObject* self, Il2CppObject* saberSwingRatingCounter, float rating) {
    // run original code
    FlyingScoreEffect_HandleSaberSwingRatingCounterDidChange(self, saberSwingRatingCounter, rating);

    // create a new C# string and set the text of _text to the string
    static auto* scoreText = il2cpp_utils::createcsstr("115", il2cpp_utils::StringType::Manual);

    auto* textObj = RET_V_UNLESS(getLogger(), il2cpp_utils::GetFieldValue(self, "_text"));
    RET_V_UNLESS(getLogger(), il2cpp_utils::SetPropertyValue(textObj, "text", scoreText));
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
    INSTALL_HOOK_OFFSETLESS(getLogger(), RelativeScoreAndImmediateRankCounter_UpdateRelativeScoreAndImmediateRank, il2cpp_utils::FindMethodUnsafe("", "RelativeScoreAndImmediateRankCounter", "UpdateRelativeScoreAndImmediateRank", 4));
    INSTALL_HOOK_OFFSETLESS(getLogger(), FlyingScoreEffect_InitAndPresent, il2cpp_utils::FindMethodUnsafe("", "FlyingScoreEffect", "InitAndPresent", 6));
    INSTALL_HOOK_OFFSETLESS(getLogger(), FlyingScoreEffect_HandleSaberSwingRatingCounterDidChange, il2cpp_utils::FindMethodUnsafe("", "FlyingScoreEffect", "HandleSaberSwingRatingCounterDidChange", 2));
    getLogger().info("Installed all hooks!");
}