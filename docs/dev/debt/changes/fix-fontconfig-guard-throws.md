# libfontconfig

## change [Faulting arguments]

- [Faulting arguments](../../../core/libs/prx/libfontconfig/Objects.cpp) throw `std::runtime_error` for unsupported null arguments. `FcPatternReference(nullptr)` throws and `FcPatternDestroy(nullptr)` does nothing as host-safety choices; the firmware 13.42 disassembly in the review above dereferences both at `0x9f0` and `0x7e0` respectively. Native pointer faults are not reproduced; inaccessible non-null pointers and stale handles are not validated.
