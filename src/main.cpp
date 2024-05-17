#include "nexus/Nexus.h"
#include "addon/Addon.hpp"

HMODULE hSelf;
AddonDefinition AddonDef{};

extern "C" __declspec(dllexport) AddonDefinition *GetAddonDef()
{
    //TODO: Change signature (!!!), name, desc, author, desc
    AddonDef.Signature = -918434798;
    AddonDef.APIVersion = NEXUS_API_VERSION;
    AddonDef.Name = "Meson/Ninja Template";
    AddonDef.Version.Major = 0;
    AddonDef.Version.Minor = 1;
    AddonDef.Version.Build = 0;
    AddonDef.Version.Revision = 0;
    AddonDef.Author = "Author";
    AddonDef.Description = "Template using meson and ninja to build";
    AddonDef.Load = Addon::load;
    AddonDef.Unload = Addon::unload;
    AddonDef.Flags = EAddonFlags_None;
    AddonDef.Provider = EUpdateProvider_None;
    AddonDef.UpdateLink = nullptr;

    return &AddonDef;
}

int APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            hSelf = hModule;
            break;
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }
    return 1;
}