#include "time.h"
#include "wren.h"

void timeNow(WrenVM* vm) {
    uv_timeval64_t time;
    wrenEnsureSlots(vm, 1);
    uv_gettimeofday(&time);
    // returns milliseconds like JavaScript
    wrenSetSlotDouble(vm, 0, time.tv_sec * 1000 + time.tv_usec/1000);
}

void timeHighResolution(WrenVM* vm) {
    wrenEnsureSlots(vm, 1);
    wrenSetSlotDouble(vm, 0, uv_hrtime());
}