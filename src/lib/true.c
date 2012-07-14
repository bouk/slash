#include "value.h"
#include "vm.h"
#include "class.h"
#include <gc.h>

static sl_object_t*
allocate_true()
{
    sl_object_t* obj = GC_MALLOC(sizeof(sl_object_t));
    obj->primitive_type = SL_T_TRUE;
    return obj;
}

void
sl_init_true(sl_vm_t* vm)
{
    vm->lib.True = sl_define_class(vm, "True", vm->lib.Object);
    sl_class_set_allocator(vm, vm->lib.True, allocate_true);
    vm->lib.true = sl_allocate(vm, vm->lib.True);
}