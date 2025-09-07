/**
 * @brief adds basic MSVC C-Runtime support to interface with the engine's C++ functionality
 * 
 */
#pragma once

class type_info;

struct _PMD
{
    int mdisp; // member offset
    int pdisp; // offset of the vtable
    int vdisp; // offset to displacment inside the vtable
};

// ??_R1 prefix
struct _RTTIBaseClassDescriptor
{
    type_info *typeDescriptor;
    int numContainedBases;
    _PMD pmd;
    int attributes;
};
    

// ??_R2 prefix
struct _RTTIBaseClassArray
{
    _RTTIBaseClassDescriptor *BaseClass[/* variable */ 1];
};

// ??_R3 prefix
struct _RTTIClassHierarchyDescriptor
{
    int signature;
    int attributes;
    int numBaseClasses;
    _RTTIBaseClassArray *baseClassArray;
};

// ??_R4 prefix
struct _RTTICompleteObjectLocator
{
    int signature;
    int offset;
    int cdOffset;
    type_info *typeDescriptor;
    _RTTIClassHierarchyDescriptor *classDescriptor;
};



// note that most classes have array-destroying optimized out
enum class EDestroyType {
	DT_Destructor = 0,
	DT_Delete = 1,
	DT_ArrayDestructor = 2,
	DT_ArrayDelete = 3,
};


/**
 * To have complete control over virtual calls (as the virtual tables are
 * defined by the engine), all virtual classes will have this form.
 *
 * Any virtual call like
 *    obj->Func();
 * will be called like
 *    obj->Virt()->Func(obj);
 */
struct VirtualClass {
    // copy this bit to the top of your class
    using vtable_t = struct vtable_ {
        //void (__thiscall *dtr)(VirtualClass*, EDestroyType);
    };
    vtable_t* Virt() const { return reinterpret_cast<vtable_t*>(vtable); }



    _RTTICompleteObjectLocator* GetLocator() const {
        return reinterpret_cast<_RTTICompleteObjectLocator*>(vtable - 1);
    }
    const VirtualClass* GetMostDerivedObj() const {
        _RTTICompleteObjectLocator* loc = GetLocator();
        char* self = (char*)this;
        char* obj = self - loc->offset;
        if (loc->cdOffset) {
            obj -= *(int*)(self - loc->cdOffset);
        }
        return reinterpret_cast<const VirtualClass*>(obj);
    }

    vtable_* vtable;
};


struct __type_info_node
{
    void *memPtr;
    __type_info_node* next;
};

// __type_info_node __type_info_root_node asm("0x00FB7D44"); // no bueno
__type_info_node* __ptype_info_root_node = (__type_info_node*) 0x00FB7D44;

// ??_R0 prefix, RTTI Type Descriptor
struct type_info : VirtualClass
{
    using vtable_t = struct vtable_type_info : vtable_ {
        void (__thiscall *dtr)(type_info*, EDestroyType);
    };
    vtable_t* Virt() const { return reinterpret_cast<vtable_t*>(vtable); }

    const char* __thiscall name(__type_info_node* __ptype_info_node = __ptype_info_root_node) const asm("0x00A8242D");
    bool __thiscall operator==(type_info*) const asm("0x00A8247D");

    const char *_m_data;
    char _m_d_name[/* variable */ 1];
};

void* __cdecl fn_dynamic_cast(void* ptr, int offset, type_info* from, type_info* to, int doThrow) asm("0x00A82A0C");
type_info* __cdecl _RTtypeid(void* ptr) asm("0x00A8262F");
