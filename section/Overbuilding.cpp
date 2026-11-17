#include <stddef.h>
#include <stdint.h>

namespace {

constexpr uintptr_t kResolveCommandType = 0x008B4140;
constexpr uintptr_t kResolveCommandUnits = 0x008B43F0;

constexpr int32_t kReclaimCommand = 0x13;

constexpr size_t kSessionUserArmiesBegin = 0x03F0;
constexpr size_t kSessionUserArmiesEnd = 0x03F4;
constexpr size_t kSessionCommandManager = 0x03FC;
constexpr size_t kSessionFocusArmy = 0x0488;
constexpr size_t kCommandManagerMapHead = 0x0CB8;

constexpr size_t kUserEntityArmy = 0x0120;
constexpr size_t kUserUnitIsEngineer = 0x03DD;

// MSVC8 std::map<CmdId, UserCommandIssueHelper*> node.
struct CommandMapNode {
    CommandMapNode *left;
    CommandMapNode *parent;
    CommandMapNode *right;
    int32_t commandId;
    void *helper;
    uint8_t color;
    uint8_t isSentinel;
    uint8_t padding[2];
};

static_assert(offsetof(CommandMapNode, helper) == 0x10);
static_assert(offsetof(CommandMapNode, isSentinel) == 0x15);
static_assert(sizeof(CommandMapNode) == 0x18);

// WeakSet<UserUnit> tree node. Its two weak-owner fields make it four bytes
// larger than the command-map node and place the sentinel flag at +0x19.
struct UnitSetNode {
    UnitSetNode *left;
    UnitSetNode *parent;
    UnitSetNode *right;
    uint32_t unit;
    void *ownerLinkSlot;
    void *nextOwner;
    uint8_t color;
    uint8_t isSentinel;
    uint8_t padding[2];
};

static_assert(offsetof(UnitSetNode, unit) == 0x0C);
static_assert(offsetof(UnitSetNode, isSentinel) == 0x19);
static_assert(sizeof(UnitSetNode) == 0x1C);

// WeakSet<T> stores its allocator/debug word, tree-head pointer, and size.
struct UnitSet {
    void *allocator;
    UnitSetNode *head;
    uint32_t size;
};

static_assert(offsetof(UnitSet, head) == 0x04);
static_assert(sizeof(UnitSet) == 0x0C);

using ResolveCommandTypeFn = int32_t(__attribute__((stdcall)) *)(void *);
using ResolveCommandUnitsFn = UnitSet *(__attribute__((stdcall)) *)(void *);

CommandMapNode *NextCommandNode(CommandMapNode *node) {
    if (!node->right->isSentinel) {
        node = node->right;
        while (!node->left->isSentinel)
            node = node->left;
        return node;
    }

    CommandMapNode *parent = node->parent;
    while (!parent->isSentinel && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

UnitSetNode *NextUnitNode(UnitSetNode *node) {
    if (!node->right->isSentinel) {
        node = node->right;
        while (!node->left->isSentinel)
            node = node->left;
        return node;
    }

    UnitSetNode *parent = node->parent;
    while (!parent->isSentinel && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

bool HasFocusArmyEngineer(UnitSet *units, void *focusArmy) {
    if (!units || !units->head || units->size == 0)
        return false;

    UnitSetNode *const sentinel = units->head;
    for (UnitSetNode *node = sentinel->left; node != sentinel;
         node = NextUnitNode(node)) {
        auto *unit = reinterpret_cast<uint8_t *>(node->unit);
        if (!unit)
            continue;

        void *const army = *reinterpret_cast<void **>(unit + kUserEntityArmy);
        if (army == focusArmy && unit[kUserUnitIsEngineer] != 0)
            return true;
    }

    return false;
}

} // namespace

// Native target resolution expects the helper in EBX and returns the current
// entity target after applying pending local SetTarget events.
extern "C" void *__attribute__((stdcall, naked))
OverbuildingResolveCommandTarget(void *) {
    asm volatile(
        "PUSH EBX;"
        "MOV EBX,[ESP+8];"
        "CALL 0x00824550;"
        "POP EBX;"
        "RET 4;"
    );
}

extern "C" bool __cdecl OverbuildingHasQueuedReclaim(void *session,
                                                       void *blocker) {
    if (!session || !blocker)
        return false;

    auto *sessionBytes = reinterpret_cast<uint8_t *>(session);
    const int32_t focusIndex =
        *reinterpret_cast<int32_t *>(sessionBytes + kSessionFocusArmy);
    if (focusIndex < 0)
        return false;

    void **const armiesBegin =
        *reinterpret_cast<void ***>(sessionBytes + kSessionUserArmiesBegin);
    void **const armiesEnd =
        *reinterpret_cast<void ***>(sessionBytes + kSessionUserArmiesEnd);
    if (!armiesBegin || !armiesEnd || armiesEnd < armiesBegin ||
        static_cast<size_t>(focusIndex) >=
            static_cast<size_t>(armiesEnd - armiesBegin)) {
        return false;
    }

    void *const focusArmy = armiesBegin[focusIndex];
    auto *const manager = *reinterpret_cast<uint8_t **>(
        sessionBytes + kSessionCommandManager);
    if (!focusArmy || !manager)
        return false;

    CommandMapNode *const sentinel = *reinterpret_cast<CommandMapNode **>(
        manager + kCommandManagerMapHead);
    if (!sentinel || !sentinel->isSentinel)
        return false;

    const auto resolveType =
        reinterpret_cast<ResolveCommandTypeFn>(kResolveCommandType);
    const auto resolveUnits =
        reinterpret_cast<ResolveCommandUnitsFn>(kResolveCommandUnits);

    for (CommandMapNode *node = sentinel->left; node != sentinel;
         node = NextCommandNode(node)) {
        void *const helper = node->helper;
        if (!helper || resolveType(helper) != kReclaimCommand)
            continue;

        if (OverbuildingResolveCommandTarget(helper) != blocker)
            continue;

        if (HasFocusArmyEngineer(resolveUnits(helper), focusArmy))
            return true;
    }

    return false;
}

// Hook continuation for USERUNIT_CanBeBuiltAt. AL contains the vanilla skirt
// overlap result, EBX is the blocker, and EBP is the current world session.
extern "C" __attribute__((naked)) void OverbuildingPlacementHook() {
    asm volatile(
        "TEST AL,AL;"
        "JZ OverbuildingPlacementAccepted;"
        "PUSH EBX;"
        "PUSH EBP;"
        "CALL %[hasQueuedReclaim];"
        "ADD ESP,8;"
        "TEST AL,AL;"
        "JZ OverbuildingPlacementRejected;"
        "OverbuildingPlacementAccepted:;"
        "JMP 0x008C1A57;"
        "OverbuildingPlacementRejected:;"
        "JMP 0x008C1B96;"
        :
        : [hasQueuedReclaim] "i" (OverbuildingHasQueuedReclaim)
        :
    );
}
