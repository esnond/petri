#ifndef CE_BODY_H
#define CE_BODY_H

#include "connectome.h"

#define N_SEG 24
#define MAX_MOTOR_NEURONS 128

typedef enum {
    MC_NONE = 0,
    MC_DB,   /* forward dorsal */
    MC_VB,   /* forward ventral */
    MC_DA,   /* reverse dorsal */
    MC_VA,   /* reverse ventral */
    MC_DD,   /* cross inhibition, active on dorsal contraction */
    MC_VD    /* cross inhibition, active on ventral contraction */
} motor_class_t;

typedef struct {
    int n_mn;
    int   idx[MAX_MOTOR_NEURONS];    /* index into nervous_t */
    float pos[MAX_MOTOR_NEURONS];    /* body position 0 head .. 1 tail */
    uint8_t mclass[MAX_MOTOR_NEURONS];
    float wscale[MAX_MOTOR_NEURONS]; /* per motor neuron drive normalization */
} motor_map_t;

typedef struct {
    motor_map_t mm;
    float k[N_SEG];            /* curvature, 1/mm, positive is dorsal bend */
    float k_read[N_SEG];       /* lowpassed curvature, the proprioceptive copy */
    float theta[N_SEG];        /* absolute segment orientation, rad */
    float x[N_SEG + 1], y[N_SEG + 1];  /* joint positions, mm */
    float px[N_SEG + 1], py[N_SEG + 1]; /* previous step shape, head anchored */
    float m_d[N_SEG];          /* dorsal muscle activation */
    float m_v[N_SEG];          /* ventral muscle activation */
    float vx, vy, omega;       /* rigid frame velocity from force balance, mm/ms and rad/ms */
    float head_x, head_y;      /* mm */
    float head_angle;          /* rad */
    float speed;               /* nose direction speed of the head, mm/s */
    float last_drot;           /* frame rotation applied last step, rad */
    int   shape_ready;         /* set once a previous shape exists */
    int   contact_hint;
    float energy_cost;     /* integrated muscle drive change, work proxy */        /* swarm driver worm to worm touch, world
                                  folds it into nose_contact once */
    int   on_food;
    int   nose_contact;        /* wall contact at the head */
    int   tail_contact;
    long  bend_count;          /* head bend zero crossings */
    int   k_sign;
} body_t;

void  body_init(body_t* b);
void  body_build_motor_map(body_t* b, const connectome_t* c, nervous_t* ns);
void  body_proprioception(body_t* b, nervous_t* ns);
void  body_step(body_t* b, nervous_t* ns, float nmj_gain, float dt);
float body_signed_speed(const body_t* b);

#endif
