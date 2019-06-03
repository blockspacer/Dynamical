#define INST_LOAD(name) \
PFN_##name name = (PFN_##name) ( instance->getProcAddr(#name)); \
assert( name != nullptr);

#define DEV_LOAD(name) \
PFN_##name name = (PFN_##name) ( device->getProcAddr(#name)); \
assert( name != nullptr);
