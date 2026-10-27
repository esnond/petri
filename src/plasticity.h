#ifndef CE_PLASTICITY_H
#define CE_PLASTICITY_H

#include "modulators.h"

void plasticity_init(connectome_t* c, nervous_t* ns);
void plasticity_step(connectome_t* c, nervous_t* ns, modulators_t* m, float dt);

#endif
