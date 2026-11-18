# libSceLibcInternal

## remove [_ZSt10_GetloctxtIcSt19istreambuf_iteratorIcSt11char_traitsIcEEEiRT0_S5_mPKT_

## add Functional

- [_ZSt10_GetloctxtIcSt19istreambuf_iteratorIcSt11char_traitsIcEEEiRT0_S5_mPKT_ and GetLocaleText](../../../core/libs/prx/libSceLibcInternal/TimeGetFields.cpp) (libSceLibcInternal, NID `bsohl1ZrRXE`) - ASCII case-insensitive field matching, 128-column capping, separator counting, and stream buffer/iterator tracking implemented from firmware 13.42 disassembly and native measurements on #22452.
- [_LPow](../../../core/libs/prx/libSceLibcInternal/ScaledPow.cpp) (libSceLibcInternal) - `y == 1` returns `x` leaving `*exponent` untouched, matching firmware 13.42 disassembly and native measurements on #22452. Other `y` values with non-null exponent still throw `NotImplemented`.
