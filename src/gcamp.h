#ifndef CE_GCAMP_H
#define CE_GCAMP_H

/* forward model of the gcamp6f indicator, chen et al 2013 cell:
   rise 70 to 150 ms, decay 141 to 400 ms for gcamp6f in vivo.
   first order: the indicator follows the transmitter release sigmoid
   of the membrane with a faster rise than decay */

typedef struct {
    float g;   /* fluorescence, normalized 0..1 */
} gcamp_t;

void  gcamp_init(gcamp_t* g);
float gcamp_step(gcamp_t* g, float act, float dt);

#endif
