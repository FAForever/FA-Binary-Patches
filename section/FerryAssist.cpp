#include <cstddef>
#include <cstdint>

#include "moho.h"
#include "global.h"

namespace
{
    using CommandVector = vector<SCommand>;

    using GetFerryCommandsFunction = void (__attribute__((stdcall)) *)(
        CUnitFerryTask *ferryTask,
        CommandVector *routeCommands
    );

    using CreateFerryTaskFunction = CUnitFerryTask *(__attribute__((cdecl)) *)(
        void *parentTask,
        Unit *targetUnit
    );

    CUnitCommand *GetCommand(const SCommand &entry)
    {
        if (!entry.weakCommandLink)
        {
            return nullptr;
        }

        // SCommand stores the command's weak-object link at object + 4.
        return reinterpret_cast<CUnitCommand *>(
            reinterpret_cast<std::uintptr_t>(entry.weakCommandLink) - 4
        );
    }

    std::uint32_t GetCommandType(CUnitCommand *command)
    {
        return *reinterpret_cast<std::uint32_t *>(
            reinterpret_cast<std::uintptr_t>(command) + CUnitCommandTypeOffset
        );
    }

    CUnitCommandQueue *GetCommandQueue(Unit *unit)
    {
        return *reinterpret_cast<CUnitCommandQueue **>(
            reinterpret_cast<std::uintptr_t>(unit) + UnitCommandQueueOffset
        );
    }

    void *GetWeakObject(void *objectLink)
    {
        if (!objectLink)
        {
            return nullptr;
        }

        return reinterpret_cast<void *>(
            reinterpret_cast<std::uintptr_t>(objectLink) - 4
        );
    }

    bool IsFerryRoute(const CommandVector &commands)
    {
        if (!commands.begin || !commands.end || commands.begin == commands.end)
        {
            return false;
        }

        for (const SCommand *entry = commands.begin; entry != commands.end; ++entry)
        {
            CUnitCommand *command = GetCommand(*entry);
            if (
                !command
                || GetCommandType(command) != static_cast<std::uint32_t>(UnitCommandType::Ferry)
            )
            {
                return false;
            }
        }

        return true;
    }

    void InsertCommandToQueue(CUnitCommandQueue *queue, CUnitCommand *command, int index)
    {
        // Moho::CUnitCommandQueue::InsertCommandToQueue uses a custom calling
        // convention: command in EAX, queue in EBX, and index on the stack.
        asm volatile(
            "push %[index];"
            "call 0x006EDCB0;"
            : "+a" (command)
            : "b" (queue),
              [index] "r" (index)
            : "ecx", "edx", "memory"
        );
    }

    void RemoveFirstCommandFromQueue(CUnitCommandQueue *queue)
    {
        asm volatile(
            "push %[queue];"
            "call 0x006EDE70;"
            :
            : [queue] "r" (queue)
            : "eax", "ecx", "edx", "memory"
        );
    }

    void DestroyRouteCommands(CommandVector *routeCommands)
    {
        if (!routeCommands || !routeCommands->begin)
        {
            return;
        }

        // Destroy the intrusive weak links before releasing the vector's
        // engine-allocated storage.
        asm volatile(
            "call 0x005A2270;"
            :
            : "c" (routeCommands->begin),
              "d" (routeCommands->end)
            : "eax", "memory"
        );

        void *storage = routeCommands->begin;
        asm volatile(
            "push %[storage];"
            "call 0x00957A60;"
            "add esp, 4;"
            :
            : [storage] "r" (storage)
            : "eax", "ecx", "edx", "memory"
        );

        routeCommands->begin = nullptr;
        routeCommands->end = nullptr;
        routeCommands->capacity_end = nullptr;
    }

    void PromoteAssistant(CUnitFerryTask *ferryTask, CommandVector *routeCommands)
    {
        if (!ferryTask || !routeCommands || !ferryTask->followsBeacon)
        {
            return;
        }

        void *beacon = GetWeakObject(ferryTask->beaconLink);
        if (!beacon || GetWeakObject(ferryTask->routeUnitLink) != beacon)
        {
            return;
        }

        Unit *assistant = ferryTask->assistant;
        if (!assistant)
        {
            return;
        }

        CUnitCommandQueue *assistantQueue = GetCommandQueue(assistant);
        if (!assistantQueue)
        {
            return;
        }

        CommandVector &assistantCommands = assistantQueue->commands;
        CUnitCommand *assistantCommand = assistantCommands.size() == 1
            ? GetCommand(assistantCommands[0])
            : nullptr;

        if (
            !assistantCommand
            || GetCommandType(assistantCommand) != static_cast<std::uint32_t>(UnitCommandType::Guard)
            || !IsFerryRoute(*routeCommands)
        )
        {
            return;
        }

        // Add the shared ferry commands after the active guard command. The
        // guard is removed last so the executing task never sees an empty queue.
        int insertionIndex = 1;
        for (const SCommand *entry = routeCommands->begin; entry != routeCommands->end; ++entry)
        {
            InsertCommandToQueue(assistantQueue, GetCommand(*entry), insertionIndex);
            ++insertionIndex;
        }

        RemoveFirstCommandFromQueue(assistantQueue);
    }
}

// Guard assistance creates a fully initialized CUnitFerryTask before returning
// here. Resolve and copy its existing route immediately, before the normal
// ferry-assist pickup cycle begins. The later GetUnitCommands hooks remain a
// fallback when the route is not yet resolvable at construction time.
extern "C" CUnitFerryTask *__attribute__((cdecl, used)) CreateFerryTaskAndPromoteAssistant(
    void *parentTask,
    Unit *targetUnit
) asm("CreateFerryTaskAndPromoteAssistant");

extern "C" CUnitFerryTask *__attribute__((cdecl, used)) CreateFerryTaskAndPromoteAssistant(
    void *parentTask,
    Unit *targetUnit
)
{
    CUnitFerryTask *ferryTask = reinterpret_cast<CreateFerryTaskFunction>(
        CreateCUnitFerryTaskAddress
    )(parentTask, targetUnit);

    CommandVector routeCommands{};
    reinterpret_cast<GetFerryCommandsFunction>(CUnitFerryTaskGetUnitCommandsAddress)(
        ferryTask,
        &routeCommands
    );
    PromoteAssistant(ferryTask, &routeCommands);
    DestroyRouteCommands(&routeCommands);

    return ferryTask;
}

// The original routine finds another transport using the same beacon and
// returns its ferry commands. Reuse those command objects so the assistant
// becomes an owner of the existing route instead of creating a new beacon.
extern "C" void __attribute__((stdcall, used)) GetFerryCommandsAndPromoteAssistant(
    CUnitFerryTask *ferryTask,
    CommandVector *routeCommands
) asm("GetFerryCommandsAndPromoteAssistant");

extern "C" void __attribute__((stdcall, used)) GetFerryCommandsAndPromoteAssistant(
    CUnitFerryTask *ferryTask,
    CommandVector *routeCommands
)
{
    reinterpret_cast<GetFerryCommandsFunction>(CUnitFerryTaskGetUnitCommandsAddress)(
        ferryTask,
        routeCommands
    );

    PromoteAssistant(ferryTask, routeCommands);
}
