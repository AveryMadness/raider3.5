#pragma once

#include "SDK.hpp"
#include "game.h"
#include "hooks.h"
#include "Logger.hpp"
#include "ufunctionhooks.h"

DWORD WINAPI Main(LPVOID lpParam)
{
    AllocConsole();

    raider::utils::Logger::Initialize();

    LOG_INFO("Welcome to Raider!");
    LOG_INFO("Initializing hooks!");

    Native::InitializeAll();
    UFunctionHooks::Initialize();

    DETOUR_START
    DetourAttachE(Native::NetDriver::TickFlush, Hooks::TickFlush);
    DetourAttachE(Native::LocalPlayer::SpawnPlayActor, Hooks::LocalPlayerSpawnPlayActor);

    auto Address = Utils::FindPattern(Patterns::NetDebug);
    CheckNullFatal(Address, "Failed to find NetDebug");
    void* (*NetDebug)(void* _this) = nullptr;
    AddressToFunction(Address, NetDebug);

    DetourAttachE(NetDebug, Hooks::NetDebug);
    DetourAttachE(ProcessEvent, Hooks::ProcessEventHook);
    DETOUR_END
    
    LOG_INFO("Base Address: {}", Imagebase);

    CreateConsole();

    return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, 0, Main, hModule, 0, nullptr);

    return true;
}
