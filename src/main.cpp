#include "main.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "bsml/shared/BSML.hpp"
#include "config.hpp"
#include "hooks.hpp"
#include "scotland2/shared/modloader.h"

static modloader::ModInfo modInfo = {MOD_ID, VERSION, 0};

extern "C" void setup(CModInfo* info) {
    *info = modInfo.to_c();

    getConfig().Init(modInfo);

    logger.info("Completed setup!");
}

extern "C" void late_load() {
    il2cpp_functions::Init();
    BSML::Init();
    BSML::Register::RegisterSettingsMenu("Simple Sabers", DidActivate, false);
    Hooks::Install();
}
