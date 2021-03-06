#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <slash/string.h>
#include <slash/class.h>
#include <slash/value.h>
#include <slash/lib/float.h>
#include <slash/lib/bignum.h>
#include <math.h>
#include <string.h>

static sl_object_t*
allocate_float(sl_vm_t* vm)
{
    sl_object_t* obj = sl_alloc(vm->arena, sizeof(sl_float_t));
    obj->primitive_type = SL_T_FLOAT;
    return obj;
}

static sl_float_t*
get_float(sl_vm_t* vm, SLVAL val)
{
    sl_expect(vm, val, vm->lib.Float);
    return (sl_float_t*)sl_get_ptr(val);
}

static SLVAL
sl_float_succ(sl_vm_t* vm, SLVAL self)
{
    return sl_make_float(vm, sl_get_float(vm, self) + 1.0);
}

static SLVAL
sl_float_pred(sl_vm_t* vm, SLVAL self)
{
    return sl_make_float(vm, sl_get_float(vm, self) - 1.0);
}

static SLVAL
sl_float_negate(sl_vm_t* vm, SLVAL self)
{
    return sl_make_float(vm, -sl_get_float(vm, self));
}

SLVAL
sl_float_to_s(sl_vm_t* vm, SLVAL self)
{
    double d = sl_get_float(vm, self);

    if(isnan(d)) {
        return sl_make_cstring(vm, "NaN");
    } else if(!isfinite(d)) {
        if(d > 0) {
            return sl_make_cstring(vm, "Infinity");
        } else {
            return sl_make_cstring(vm, "-Infinity");
        }
    }

    char buff[1024];
    sprintf(buff, "%f", d);
    for(size_t i = strlen(buff) - 1; i > 0; i--) {
        if(buff[i] != '0' || (i > 0 && buff[i - 1] == '.')) {
            break;
        }
        buff[i] = 0;
    }
    return sl_make_cstring(vm, buff);
}

static SLVAL
sl_float_to_i(sl_vm_t* vm, SLVAL self)
{
    double d = sl_get_float(vm, self);
    if(d > INT_MAX / 2 || d < INT_MIN / 2) {
        return sl_make_bignum_f(vm, d);
    } else {
        return sl_make_int(vm, (int)d);
    }
}

static SLVAL
sl_float_to_f(sl_vm_t* vm, SLVAL self)
{
    (void)vm;
    return self;
}

static SLVAL
sl_float_round(sl_vm_t* vm, SLVAL self)
{
    double d = round(sl_get_float(vm, self));
    if(d < SL_MIN_INT || d > SL_MAX_INT) {
        return sl_make_bignum_f(vm, d);
    } else {
        return sl_make_int(vm, (int)d);
    }
}

static SLVAL
sl_float_floor(sl_vm_t* vm, SLVAL self)
{
    return sl_make_float(vm, floor(sl_get_float(vm, self)));
}

static SLVAL
sl_float_ceil(sl_vm_t* vm, SLVAL self)
{
    return sl_make_float(vm, ceil(sl_get_float(vm, self)));
}

static SLVAL
sl_float_nan(sl_vm_t* vm, SLVAL self)
{
    return sl_make_bool(vm, isnan(sl_get_float(vm, self)));
}

static SLVAL
sl_float_finite(sl_vm_t* vm, SLVAL self)
{
    return sl_make_bool(vm, isfinite(sl_get_float(vm, self)));
}

static SLVAL
sl_float_infinite(sl_vm_t* vm, SLVAL self)
{
    return sl_make_bool(vm, !isfinite(sl_get_float(vm, self)));
}

void
sl_init_float(sl_vm_t* vm)
{
    vm->lib.Float = sl_define_class(vm, "Float", vm->lib.Number);
    sl_class_set_allocator(vm, vm->lib.Float, allocate_float);
    sl_define_method(vm, vm->lib.Float, "to_s", 0, sl_float_to_s);
    sl_define_method(vm, vm->lib.Float, "inspect", 0, sl_float_to_s);
    sl_define_method(vm, vm->lib.Float, "to_i", 0, sl_float_to_i);
    sl_define_method(vm, vm->lib.Float, "to_f", 0, sl_float_to_f);
    sl_define_method(vm, vm->lib.Float, "succ", 0, sl_float_succ);
    sl_define_method(vm, vm->lib.Float, "pred", 0, sl_float_pred);
    sl_define_method(vm, vm->lib.Float, "-self", 0, sl_float_negate);
    sl_define_method(vm, vm->lib.Float, "round", 0, sl_float_round);
    sl_define_method(vm, vm->lib.Float, "floor", 0, sl_float_floor);
    sl_define_method(vm, vm->lib.Float, "ceil", 0, sl_float_ceil);
    sl_define_method(vm, vm->lib.Float, "+", 1, sl_float_add);
    sl_define_method(vm, vm->lib.Float, "-", 1, sl_float_sub);
    sl_define_method(vm, vm->lib.Float, "*", 1, sl_float_mul);
    sl_define_method(vm, vm->lib.Float, "/", 1, sl_float_div);
    sl_define_method(vm, vm->lib.Float, "%", 1, sl_float_mod);
    sl_define_method(vm, vm->lib.Float, "**", 1, sl_float_pow);
    sl_define_method(vm, vm->lib.Float, "==", 1, sl_float_eq);
    sl_define_method(vm, vm->lib.Float, "<=>", 1, sl_float_cmp);
    sl_define_method(vm, vm->lib.Float, "nan", 0, sl_float_nan);
    sl_define_method(vm, vm->lib.Float, "finite", 0, sl_float_finite);
    sl_define_method(vm, vm->lib.Float, "infinite", 0, sl_float_infinite);
    
    sl_class_set_const(vm, vm->lib.Object, "Infinity", sl_make_float(vm, 1.0/0.0));
    sl_class_set_const(vm, vm->lib.Object, "NaN", sl_make_float(vm, 0.0/0.0));
}

SLVAL
sl_make_float(sl_vm_t* vm, double d)
{
    SLVAL fl = sl_allocate(vm, vm->lib.Float);
    get_float(vm, fl)->d = d;
    return fl;
}

double
sl_get_float(sl_vm_t* vm, SLVAL f)
{
    return get_float(vm, f)->d;
}

SLVAL
sl_float_add(sl_vm_t* vm, SLVAL self, SLVAL other)
{
    if(sl_is_a(vm, other, vm->lib.Bignum)) {
        return sl_float_add(vm, self, sl_bignum_to_f(vm, other));
    }
    if(sl_is_a(vm, other, vm->lib.Int)) {
        return sl_float_add(vm, self, sl_make_float(vm, sl_get_int(other)));
    }
    return sl_make_float(vm, sl_get_float(vm, self) + sl_get_float(vm, other));
}

SLVAL
sl_float_sub(sl_vm_t* vm, SLVAL self, SLVAL other)
{
    if(sl_is_a(vm, other, vm->lib.Bignum)) {
        return sl_float_sub(vm, self, sl_bignum_to_f(vm, other));
    }
    if(sl_is_a(vm, other, vm->lib.Int)) {
        return sl_float_sub(vm, self, sl_make_float(vm, sl_get_int(other)));
    }
    return sl_make_float(vm, sl_get_float(vm, self) - sl_get_float(vm, other));
}

SLVAL
sl_float_mul(sl_vm_t* vm, SLVAL self, SLVAL other)
{
    if(sl_is_a(vm, other, vm->lib.Bignum)) {
        return sl_float_mul(vm, self, sl_bignum_to_f(vm, other));
    }
    if(sl_is_a(vm, other, vm->lib.Int)) {
        return sl_float_mul(vm, self, sl_make_float(vm, sl_get_int(other)));
    }
    return sl_make_float(vm, sl_get_float(vm, self) * sl_get_float(vm, other));
}

SLVAL
sl_float_div(sl_vm_t* vm, SLVAL self, SLVAL other)
{
    if(sl_is_a(vm, other, vm->lib.Bignum)) {
        return sl_float_div(vm, self, sl_bignum_to_f(vm, other));
    }
    if(sl_is_a(vm, other, vm->lib.Int)) {
        return sl_float_div(vm, self, sl_make_float(vm, sl_get_int(other)));
    }
    return sl_make_float(vm, sl_get_float(vm, self) / sl_get_float(vm, other));
}

SLVAL
sl_float_mod(sl_vm_t* vm, SLVAL self, SLVAL other)
{
    if(sl_is_a(vm, other, vm->lib.Bignum)) {
        return sl_float_mod(vm, self, sl_bignum_to_f(vm, other));
    }
    if(sl_is_a(vm, other, vm->lib.Int)) {
        return sl_float_mod(vm, self, sl_make_float(vm, sl_get_int(other)));
    }
    return sl_make_float(vm, fmod(sl_get_float(vm, self), sl_get_float(vm, other)));
}

SLVAL
sl_float_pow(sl_vm_t* vm, SLVAL self, SLVAL other)
{
    if(sl_is_a(vm, other, vm->lib.Bignum)) {
        return sl_float_pow(vm, self, sl_bignum_to_f(vm, other));
    }
    if(sl_is_a(vm, other, vm->lib.Int)) {
        return sl_float_pow(vm, self, sl_make_float(vm, sl_get_int(other)));
    }
    return sl_make_float(vm, pow(sl_get_float(vm, self), sl_get_float(vm, other)));
}

SLVAL
sl_float_eq(sl_vm_t* vm, SLVAL self, SLVAL other)
{
    if(sl_is_a(vm, other, vm->lib.Int)) {
        return sl_float_eq(vm, self, sl_make_float(vm, sl_get_int(other)));
    }
    if(sl_is_a(vm, other, vm->lib.Bignum)) {
        if(fmod(sl_get_float(vm, self), 1.0) == 0.0) {
            return sl_bignum_eq(vm, sl_make_bignum_f(vm, sl_get_float(vm, self)), other);
        } else {
            return vm->lib._false;
        }
    }
    if(!sl_is_a(vm, other, vm->lib.Float)) {
        return vm->lib._false;
    }
    return sl_make_bool(vm, sl_get_float(vm, self) == sl_get_float(vm, other));
}

SLVAL
sl_float_cmp(sl_vm_t* vm, SLVAL self, SLVAL other)
{
    if(sl_is_a(vm, other, vm->lib.Int)) {
        return sl_float_cmp(vm, self, sl_make_float(vm, sl_get_int(other)));
    }
    if(sl_is_a(vm, other, vm->lib.Bignum)) {
        return sl_make_int(vm, -sl_get_int(sl_bignum_cmp(vm, other, self)));
    }
    sl_expect(vm, other, vm->lib.Float);
    if(sl_get_float(vm, self) < sl_get_float(vm, other)) {
        return sl_make_int(vm, -1);
    } else if(sl_get_float(vm, self) > sl_get_float(vm, other)) {
        return sl_make_int(vm, 1);
    } else {
        return sl_make_int(vm, 0);
    }
}
