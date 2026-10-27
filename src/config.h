#ifndef CE_CONFIG_H
#define CE_CONFIG_H

/*
 * all biophysical constants in one place.
 * units are written next to every value.
 * potentials in mv, time in ms, length in mm, conductance in arbitrary
 * graded-model units where 1.0 g drives a fully activated synapse.
 */

/* integration */
static const float DT_MS          = 1.0f;    /* ode step, ms */
static const float NOISE_SD_MV    = 0.40f;    /* membrane noise per sqrt step, mv */

/* graded membrane */
static const float V_REST_MV      = -50.0f;  /* resting potential, mv */
static const float TAU_MIN_MS     = 10.0f;   /* floor on membrane tau after modulation, ms */
static const float E_REV_EXC_MV   = 0.0f;    /* non selective cation reversal, mv */
static const float E_REV_INH_MV   = -72.0f;  /* chloride reversal, mv */
static const float ACT_HALF_MV    = -42.0f;  /* sigmoid midpoint of transmitter release, mv */
static const float ACT_SLOPE_MV   = 4.5f;    /* sigmoid slope, mv */
static const float I_SCALE        = 0.04f;   /* synaptic conductance to mv/ms coupling */

/* tsodyks-markram stsp */
static const float STSP_U_DEP     = 0.85f;   /* release probability, depressing mode */
static const float STSP_TAU_REC_DEP  = 900.0f; /* ms, slow recovery = habituation */
static const float STSP_TAU_FAC_DEP  = 60.0f;  /* ms */
static const float STSP_U_FAC     = 0.12f;   /* facilitating mode */
static const float STSP_TAU_REC_FAC = 130.0f; /* ms */
static const float STSP_TAU_FAC_FAC = 700.0f; /* ms, long facilitation = sensitization */
static const float STSP_EVENT_MS  = 10.0f;   /* vesicle turnover timescale per release episode */

/* homeostatic scaling */
static const float HOMEO_TARGET   = 0.30f;   /* desired mean activation */
static const float HOMEO_TAU_MS   = 600000.0f; /* 60 s, very slow */
static const float HOMEO_MIN      = 0.25f;
static const float HOMEO_MAX      = 4.0f;

/* heterosynaptic aversive learning */
static const float HETERO_ETA     = 0.004f;  /* rate per ms of eligibility */
static const float HETERO_TAU_MS  = 20000.0f; /* slow recovery of suppressed synapses */
static const float HETERO_MIN     = 0.10f;

/* neuromodulators, second timescale */
static const float SER_TAU_S      = 6.0f;    /* serotonin kinetics, s */
static const float OCT_TAU_S      = 5.0f;    /* octopamine kinetics, s */
static const float DOP_TAU_S      = 2.5f;    /* dopamine kinetics, s */
static const float MOD_TAU_SCALE_SER = 0.6f; /* ser slows interneuron tau by up to this fraction */
static const float MOD_TAU_SCALE_OCT = 0.3f;
static const float NMJ_GAIN_SER   = 1.6f;    /* ser shrinks nmj drive on food */
static const float NMJ_GAIN_OCT   = 0.9f;    /* oct boosts nmj drive when roaming */
static const float DA_G_MECH_SCALE = 0.5f;   /* da shunts mechanosensory synapses */

/* body mechanics, units mm and ms */
static const float BODY_LEN_MM    = 1.0f;    /* adult hermaphrodite, mm */
static const float K_MAX_INV_MM   = 12.0f;   /* max curvature, 1/mm, cuticle limit */
static const float TAU_BEND_MS    = 220.0f;  /* cuticle viscoelastic relaxation, ms, approx raised from 110 to slow the loop into the target period */
static const float BEND_GAIN      = 7.5f;
static const float BOUNDARY_DAMP  = 3.0f;
static const int   BOUNDARY_SEG   = 3;
static const float DRAG_T         = 0.021f;  /* tangential drag per segment, uN*s/mm */
static const float DRAG_RATIO     = 20.0f;
static const float PROP_GAIN      = 36.0f;
static const int   PROP_LAG_SEG   = 4;       /* segments of spatial phase lag */
static const float PROP_TAU_MS    = 600.0f;
static const float PROP_SELF_W    = 1.0f;    /* own segment bend weight, relaxation oscillator */
static const float PROP_BACK_W    = 1.2f;
static const float MN_CHAIN_GAP_W = 0.5f;    /* electrotonic chain weight along one motor class */

/* endogenous activity of the command circuit, mv of standing bias current.
   graded whole animal models need avb near the sigmoid midpoint or the
   proprioceptive loop has no gain to work with, kunert 2017, boyle 2012 */
static const float BIAS_AVB_MV    = 8.00f;
static const float BIAS_AVA_MV    = 0.0f;   /* ava is command driven, no endogenous drive */
static const float NMJ_SIGMA_SEG  = 0.8f;    /* spatial spread of one mn onto muscle bands */
static const float NMJ_GAIN       = 0.90f;    /* motor activation to muscle activation */
/* per motor neuron weight normalization lives in body.c: the class
   densities differ, white 1986, and the raw nmj coverage would bend the
   body into the coverage pattern instead of the wave */
static const float MUSCLE_TAU_MS  = 180.0f;   /* excitation contraction coupling, ms, approx raised from 90, see tau bend note */

/* sensory transduction */
static const float CHEMO_TAU_ADAPT_S = 3.0f; /* receptor adaptation, s */
static const float CHEMO_G           = 3.0f; /* peak receptor conductance */
static const float CHEMO_DERIV_W     = 2.0f; /* weight of dc/dt versus offset component */
static const float THERMO_TAU_ADAPT_S = 8.0f;
static const float THERMO_G           = 2.5f; /* per degree c */
static const float MEC_G            = 10.0f;
static const float MEC_TAU_MS         = 800.0f; /* mechanoreceptor relaxation, ms */
static const float ASH_TAU_MS         = 2500.0f;
/* spike frequency adaptation: calcium activated potassium current,
   universal in neurons. the cell depolarized with activity and
   recovers on its own time constant, which makes every command burst
   transient instead of a held plateau. rates after the shm2007 strain
   imaging timescales, kato 2015 cell 155: the ava transient decays
   within seconds */
static const float ADAPT_TAU_MS    = 2000.0f;
static const float ADAPT_GAIN_MV   = 9.0f;  /* mv of adaptation per unit activity */
/* command proprioception: the command neurons track the body wave,
   wen 2012 shows b neurons are stretch sensitive. gain in mv per
   unit curvature */
static const float CMD_PROP_GAIN   = 3.0f;
/* optogenetics, cheta and nphr kinetics after chen 2011 / zhang 2007:
   cheta activation 1.5 ms, deactivation 10 ms; nphr shunt gain in
   arbitrary conductance units at unit light intensity */
static const float CHETA_TC_ON_MS   = 1.5f;
static const float CHETA_TC_OFF_MS  = 10.0f;
static const float NPHR_GAIN        = 0.3f;
static const float CHETA_GAIN_MV    = 12.0f;
/* gcamp6f forward model kinetics, chen et al 2013 cell 155: the fast
   variant rises in 70 to 150 ms and decays in 141 to 400 ms in vivo */
static const float GCAMP_TAU_RISE_MS  = 110.0f;
static const float GCAMP_TAU_DECAY_MS = 380.0f;
static const float WALL_FRICTION     = 0.65f;
static const float TAP_MS             = 300.0f; /* mechanical tap duration, ms */

/* world */
static const float ODOR_DIFF_MM2_S  = 0.05f; /* effective diffusivity in agar, mm^2/s */
static const float FOOD_REGEN_S     = 0.02f; /* lawn regrowth per second, fraction of full lawn */
static const float FOOD_EAT_RATE    = 0.08f; /* lawn loss per second while grazing */

#endif
