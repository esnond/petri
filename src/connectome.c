#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "connectome.h"
#include "config.h"

/*
 * edges are collected into a flat staging array, then counting sorted into
 * csr. no allocation, all storage is static.
 */
typedef struct {
    int src, dst;
    uint8_t kind;
    uint8_t delay;
    uint8_t stsp;
    float w;
} staged_edge_t;

static staged_edge_t staged[MAX_SYN];
static int n_staged;

static const char* const GABA_PREFIX[] = {
    "DD", "VD", "AVL", "RIS", "RME", "DVB", "RIG", NULL
};
static const char* const SENSORY_PREFIX[] = {
    "ASE", "AWA", "AWC", "ASH", "AFD", "ALM", "PLM", "AVM", "PVD", "ADL",
    "URX", "OLL", "OLQ", "FLP", "IL1", "IL2", "ASHV", NULL
};
static const char* const MOTOR_PREFIX[] = {
    "DB", "VB", "DA", "VA", "DD", "VD", "AS", "SMB", "SMD", NULL
};
static const char* const COMMAND_PREFIX[] = {
    "AVA", "AVB", "AVD", "AVE", "PVC", NULL
};

static int has_prefix(const char* name, const char* const* prefixes)
{
    int i;
    for (i = 0; prefixes[i]; i++)
        if (strncmp(name, prefixes[i], strlen(prefixes[i])) == 0)
            return 1;
    return 0;
}

int conn_is_inhibitory(const char* name)
{
    return has_prefix(name, GABA_PREFIX);
}

static int motor_delay_of(const char* name)
{
    /* sensory fibers are fast, interneuron paths add one conduction hop */
    if (has_prefix(name, SENSORY_PREFIX))
        return 1;
    if (has_prefix(name, MOTOR_PREFIX) || has_prefix(name, COMMAND_PREFIX))
        return 1;
    return 2;
}

static int add_neuron(connectome_t* c, nervous_t* ns, const char* name, int type)
{
    int id;
    if (ns->n >= MAX_NEURONS)
        return -1;
    id = ns->n;
    snprintf(c->names[id], NEURON_NAME_MAX, "%s", name);
    snprintf(ns->cells[id].name, NEURON_NAME_MAX, "%s", name);
    ns->cells[id].type = (uint8_t)type;
    ns->cells[id].tau_base = (type == NT_SENSORY) ? 40.0f : 25.0f;
    ns->n++;
    c->n_neurons = ns->n;
    return id;
}

static int intern_neuron(connectome_t* c, nervous_t* ns, const char* name)
{
    int id = nervous_find(ns, name);
    if (id >= 0)
        return id;
    {
        int type = NT_INTER;
        if (has_prefix(name, SENSORY_PREFIX))
            type = NT_SENSORY;
        else if (has_prefix(name, MOTOR_PREFIX))
            type = NT_MOTOR;
        else if (has_prefix(name, COMMAND_PREFIX))
            type = NT_COMMAND;
        return add_neuron(c, ns, name, type);
    }
}

static int stage(connectome_t* c, nervous_t* ns, const char* src,
                 const char* dst, int kind, float w, int delay_ms)
{
    int s, d;
    uint8_t stsp = STSP_NONE;
    if (n_staged >= MAX_SYN)
        return -1;
    s = intern_neuron(c, ns, src);
    d = intern_neuron(c, ns, dst);
    if (s < 0 || d < 0)
        return -1;
    /* mechanosensors depress, interneurons facilitate, tsodyks markram */
    if (kind == SYN_EXC || kind == SYN_INH) {
        if (ns->cells[s].type == NT_SENSORY)
            stsp = STSP_DEPRESSING;
        else if (ns->cells[s].type == NT_INTER)
            stsp = STSP_FACILITATING;
    }
    staged[n_staged].src = s;
    staged[n_staged].dst = d;
    staged[n_staged].kind = (uint8_t)kind;
    staged[n_staged].w = w;
    staged[n_staged].delay = (uint8_t)(delay_ms / (int)DT_MS);
    staged[n_staged].stsp = stsp;
    n_staged++;
    return 0;
}

static void build_csr(connectome_t* c)
{
    int i, pos;
    memset(c->row_ptr, 0, sizeof(c->row_ptr));
    c->n_syn = n_staged;
    for (i = 0; i < n_staged; i++)
        c->row_ptr[staged[i].src + 1]++;
    for (i = 0; i < c->n_neurons; i++)
        c->row_ptr[i + 1] += c->row_ptr[i];
    {
        static int cursor[MAX_NEURONS + 1];
        memcpy(cursor, c->row_ptr, sizeof(int) * (size_t)(c->n_neurons + 1));
        for (i = 0; i < n_staged; i++) {
            pos = cursor[staged[i].src]++;
            c->col_ind[pos] = staged[i].dst;
            c->w[pos] = staged[i].w;
            c->w_mult[pos] = 1.0f;
            c->kind[pos] = staged[i].kind;
            c->delay[pos] = staged[i].delay;
            c->stsp_mode[pos] = staged[i].stsp;
            c->stsp_x[pos] = 1.0f;
            c->stsp_u[pos] = staged[i].stsp == STSP_FACILITATING ? STSP_U_FAC : STSP_U_DEP;
        }
    }
}

static const struct { const char* s, *d; int kind; float w; int dly; } BUILTIN_EDGES[] = {
    /* appetitive chemotaxis path, wes and bargmann 2001 */
    { "AWAL", "AIAL", SYN_EXC, 3.0f, 2 },
    { "AWAL", "AIYL", SYN_EXC, 2.0f, 2 },
    { "AWAR", "AIAL", SYN_EXC, 3.0f, 2 },
    { "AWAR", "AIYL", SYN_EXC, 2.0f, 2 },
    { "AWCL", "AIYL", SYN_EXC, 3.0f, 2 },
    { "AWCL", "AIAL", SYN_EXC, 2.0f, 2 },
    { "AWCR", "AIYL", SYN_EXC, 3.0f, 2 },
    { "AWCR", "AIAL", SYN_EXC, 1.0f, 2 },
    { "ASEL", "AIAL", SYN_EXC, 2.0f, 1 },
    { "ASER", "AIAL", SYN_EXC, 1.0f, 1 },
    { "ASER", "AIZL", SYN_EXC, 2.0f, 1 },
    { "AFDL", "AIYL", SYN_EXC, 2.0f, 3 },
    { "AFDR", "AIYL", SYN_EXC, 2.0f, 3 },
    /* nociception drives reversal command, hilliard 2002.
       inhibitory weights are inflated relative to excitatory ones because
       the chloride driving force is 22 mv against 50 mv for excitation */
    { "ASHL", "AVAL", SYN_EXC, 4.0f, 1 },
    { "ASHL", "AVDL", SYN_EXC, 2.0f, 1 },
    { "ASHL", "AIBL", SYN_EXC, 1.0f, 1 },
    { "ASHR", "AVAR", SYN_EXC, 4.0f, 1 },
    { "ASHR", "AVDR", SYN_EXC, 2.0f, 1 },
    { "ASHR", "AIBR", SYN_EXC, 1.0f, 1 },
    /* gentle touch, chalfie circuit, anterior touch reverses */
    { "ALML", "AVDL", SYN_EXC, 5.0f, 1 },
    { "ALMR", "AVDR", SYN_EXC, 5.0f, 1 },
    { "ALML", "AVAL", SYN_EXC, 2.0f, 1 },
    { "ALMR", "AVAR", SYN_EXC, 2.0f, 1 },
    { "PLML", "PVCL", SYN_EXC, 5.0f, 1 },
    { "PLMR", "PVCR", SYN_EXC, 5.0f, 1 },
    { "PLML", "AVDL", SYN_EXC, 1.0f, 1 },
    { "PLMR", "AVDR", SYN_EXC, 1.0f, 1 },
    /* first and second order interneurons */
    { "AIAL", "AIBL", SYN_EXC, 2.0f, 2 },
    { "AIAR", "AIBR", SYN_EXC, 2.0f, 2 },
    { "AIAL", "RIAL", SYN_EXC, 2.0f, 2 },
    { "AIAR", "RIAR", SYN_EXC, 2.0f, 2 },
    { "AIBL", "AVAL", SYN_EXC, 3.0f, 2 },
    { "AIBR", "AVAR", SYN_EXC, 3.0f, 2 },
    { "AIBL", "AVBL", SYN_EXC, 1.0f, 3 },
    { "AIBR", "AVBR", SYN_EXC, 1.0f, 3 },
    { "AIYL", "RIML", SYN_INH, 2.0f, 2 },
    { "AIYR", "RIMR", SYN_INH, 2.0f, 2 },
    { "AIYL", "AIZL", SYN_INH, 1.0f, 2 },
    { "AIYR", "AIZR", SYN_INH, 1.0f, 2 },
    { "AIYL", "AVBL", SYN_EXC, 1.0f, 3 },
    { "AIYR", "AVBR", SYN_EXC, 1.0f, 3 },
    { "AIZL", "AVAL", SYN_EXC, 2.0f, 2 },
    { "AIZR", "AVAR", SYN_EXC, 2.0f, 2 },
    { "RIAL", "AVBL", SYN_EXC, 2.0f, 2 },
    { "RIAR", "AVBR", SYN_EXC, 2.0f, 2 },
    { "RIAL", "AVAL", SYN_INH, 1.0f, 2 },
    { "RIAR", "AVAR", SYN_INH, 1.0f, 2 },
    { "RIML", "AVAL", SYN_EXC, 2.0f, 2 },
    { "RIMR", "AVAR", SYN_EXC, 2.0f, 2 },
    { "RIML", "AVBL", SYN_INH, 6.0f, 2 },
    { "RIMR", "AVBR", SYN_INH, 6.0f, 2 },
    { "RIBL", "AVBL", SYN_EXC, 1.0f, 2 },
    { "RIBR", "AVBR", SYN_EXC, 1.0f, 2 },
    { "DVA",  "AVBL", SYN_EXC, 1.0f, 2 },
    { "DVA",  "AVBR", SYN_EXC, 1.0f, 2 },
    { "AVDL", "AVAL", SYN_EXC, 5.0f, 1 },
    { "AVDR", "AVAR", SYN_EXC, 5.0f, 1 },
    { "AVEL", "AVAL", SYN_EXC, 2.0f, 1 },
    { "AVER", "AVAR", SYN_EXC, 2.0f, 1 },
    /* ava and rim form the reverse loop, rim also shunts the forward pool */
    { "AVAL", "RIML", SYN_EXC, 2.0f, 2 },
    { "AVAR", "RIMR", SYN_EXC, 2.0f, 2 },
    { "PVCL", "AVBL", SYN_EXC, 1.0f, 1 },
    { "PVCR", "AVBR", SYN_EXC, 1.0f, 1 },
    /* electrical coupling between bilateral twins */
    { "AVAL", "AVAR", SYN_GAP, 2.0f, 0 },
    { "AVBL", "AVBR", SYN_GAP, 2.0f, 0 },
    { "RIAL", "RIAR", SYN_GAP, 1.0f, 0 },
    { "AIYL", "AIYR", SYN_GAP, 1.0f, 0 },
    { NULL, NULL, 0, 0.0f, 0 }
};

static const char* MN_CLASSES[] = { "DB", "VB", "DA", "VA", "DD", "VD" };
/* real ganglion counts, white 1986, must match mn_class_count in body.c.
   building only six per class leaves the posterior ventral body with no
   motor neurons at all, the old standing bug that kept the tail quiet */
static const int  MN_COUNT[] = { 7, 11, 9, 12, 6, 13 };

int conn_load_builtin(connectome_t* c, nervous_t* ns)
{
    int i, j, cls;
    /* 16 bytes so the compiler can see snprintf always fits */
    char src[16], dst[16];

    n_staged = 0;
    ns->n = 0;
    for (i = 0; BUILTIN_EDGES[i].s; i++) {
        if (stage(c, ns, BUILTIN_EDGES[i].s, BUILTIN_EDGES[i].d,
                  BUILTIN_EDGES[i].kind, BUILTIN_EDGES[i].w,
                  BUILTIN_EDGES[i].dly) != 0)
            return -1;
    }

    /* regular command to motor pool wiring, white 1986.
       ava drives the reverse classes and inhibits the forward ones, avb mirrors it */
    for (j = 1; j <= MN_COUNT[3]; j++) {
        for (cls = 0; cls < 6; cls++) {
            const char* mn = MN_CLASSES[cls];
            int reverse = (strcmp(mn, "DA") == 0 || strcmp(mn, "VA") == 0);
            if (j > MN_COUNT[cls])
                continue;
            snprintf(src, sizeof(src), "%s%d", MN_CLASSES[cls], j);
            /* ava must out-shout the avb bias and the wave self drive on the forward
               pools, the chloride driving force is only 22 mv, hence w9 */
            if (stage(c, ns, "AVAL", src, reverse ? SYN_EXC : SYN_INH,
                      reverse ? 2.0f : 9.0f, 1) != 0) return -1;
            if (stage(c, ns, "AVAR", src, reverse ? SYN_EXC : SYN_INH,
                      reverse ? 2.0f : 9.0f, 1) != 0) return -1;
            if (stage(c, ns, "AVBL", src, reverse ? SYN_INH : SYN_EXC,
                      reverse ? 5.0f : 2.0f, 1) != 0) return -1;
            if (stage(c, ns, "AVBR", src, reverse ? SYN_INH : SYN_EXC,
                      reverse ? 5.0f : 2.0f, 1) != 0) return -1;
        }
        /* b motor neurons recruit the cross inhibiting d partner */
        if (j <= MN_COUNT[4] && j <= MN_COUNT[0]) {
            snprintf(dst, sizeof(dst), "DD%d", j);
            snprintf(src, sizeof(src), "DB%d", j);
            if (stage(c, ns, src, dst, SYN_EXC, 2.0f, 1) != 0) return -1;
        }
        if (j <= MN_COUNT[5] && j <= MN_COUNT[1]) {
            snprintf(dst, sizeof(dst), "VD%d", j);
            snprintf(src, sizeof(src), "VB%d", j);
            if (stage(c, ns, src, dst, SYN_EXC, 2.0f, 1) != 0) return -1;
        }
        /* electrotonic chains along each motor class spread activation,
           chain weight sets how strongly neighbors phase lock */
        {
            static const char* chained[] = { "DB", "VB", "DA", "VA", "DD", "VD" };
            for (cls = 0; cls < 6; cls++) {
                if (j + 1 > MN_COUNT[cls])
                    continue;
                snprintf(src, sizeof(src), "%s%d", chained[cls], j);
                snprintf(dst, sizeof(dst), "%s%d", chained[cls], j + 1);
                if (stage(c, ns, src, dst, SYN_GAP, MN_CHAIN_GAP_W, 0) != 0) return -1;
            }
        }
    }

    /* command to motor pool boosters, pvc forward, ava rim reverse */
    for (j = 1; j <= MN_COUNT[3]; j++) {
        if (j <= MN_COUNT[0]) {
            snprintf(dst, sizeof(dst), "DB%d", j);
            if (stage(c, ns, "PVCL", dst, SYN_EXC, 2.0f, 1) != 0) return -1;
        }
        if (j <= MN_COUNT[1]) {
            snprintf(dst, sizeof(dst), "VB%d", j);
            if (stage(c, ns, "PVCR", dst, SYN_EXC, 2.0f, 1) != 0) return -1;
        }
        if (j <= MN_COUNT[3]) {
            snprintf(dst, sizeof(dst), "VA%d", j);
            if (stage(c, ns, "AVDL", dst, SYN_EXC, 2.0f, 1) != 0) return -1;
            snprintf(dst, sizeof(dst), "VA%d", j);
            if (stage(c, ns, "AVEL", dst, SYN_EXC, 2.0f, 1) != 0) return -1;
            snprintf(dst, sizeof(dst), "VA%d", j);
            if (stage(c, ns, "AVDR", dst, SYN_EXC, 3.0f, 1) != 0) return -1;
            snprintf(dst, sizeof(dst), "VA%d", j);
            if (stage(c, ns, "RIMR", dst, SYN_EXC, 1.5f, 1) != 0) return -1;
        }
        if (j <= MN_COUNT[2]) {
            snprintf(dst, sizeof(dst), "DA%d", j);
            if (stage(c, ns, "AVER", dst, SYN_EXC, 2.0f, 1) != 0) return -1;
            /* avd premotor bypass, one hop less attenuation on the touch path */
            snprintf(dst, sizeof(dst), "DA%d", j);
            if (stage(c, ns, "AVDL", dst, SYN_EXC, 3.0f, 1) != 0) return -1;
            snprintf(dst, sizeof(dst), "DA%d", j);
            if (stage(c, ns, "RIML", dst, SYN_EXC, 1.5f, 1) != 0) return -1;
        }
        snprintf(dst, sizeof(dst), "VA%d", j);
        if (stage(c, ns, "RIMR", dst, SYN_EXC, 1.5f, 1) != 0) return -1;
    }

    c->n_neurons = ns->n;
    build_csr(c);
    return 0;
}

static void str_upper(char* s)
{
    for (; *s; s++)
        *s = (char)toupper((unsigned char)*s);
}

int conn_load_csv(connectome_t* c, nervous_t* ns, const char* path)
{
    FILE* f = fopen(path, "r");
    static char line[512];
    static uint8_t recv_flag[MAX_SYN];
    int i, j, kept;
    n_staged = 0;
    memset(recv_flag, 0, sizeof(recv_flag));
    ns->n = 0;
    c->n_neurons = 0;
    if (!f)
        return -1;

    while (fgets(line, sizeof(line), f)) {
        char src[NEURON_NAME_MAX] = {0}, dst[NEURON_NAME_MAX] = {0};
        char typ[16] = {0};
        float w = 1.0f;
        int sign = 0, nf;
        char* p = line;
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r')
            continue;
        /* optional header */
        if (strncmp(line, "Source", 6) == 0 || strncmp(line, "source", 6) == 0)
            continue;
        nf = sscanf(p, "%11[^,],%11[^,],%15[^,],%f,%d",
                    src, dst, typ, &w, &sign);
        if (nf < 4)
            continue;
        str_upper(src);
        str_upper(dst);
        str_upper(typ);

        if (strcmp(typ, "EJ") == 0) {
            /* body wall muscle cells are synaptic sinks, not gap partners
               of neurons; the whole animal male table lists a few of those
               rows and they would inject phantom currents into real cells */
            if (strstr(src, "BWM") || strstr(dst, "BWM"))
                continue;
            /* gap junctions are listed once per partner, keep one copy */
            int s0 = nervous_find(ns, src), d0 = nervous_find(ns, dst);
            int dup = 0;
            for (j = 0; j < n_staged; j++) {
                if (staged[j].kind != SYN_GAP)
                    continue;
                if ((staged[j].src == s0 && staged[j].dst == d0) ||
                    (staged[j].src == d0 && staged[j].dst == s0)) {
                    dup = 1;
                    break;
                }
            }
            if (!dup && stage(c, ns, src, dst, SYN_GAP, w, 0) != 0)
                { fclose(f); return -1; }
        } else if (strcmp(typ, "NMJ") == 0) {
            continue;   /* neuromuscular junctions are rebuilt from motor map */
        } else if (typ[0] == 'R') {
            /* rp and r rows are listed from the receiver side, the synapse
               actually runs the other way */
            int kind = (sign < 0 || conn_is_inhibitory(dst)) ? SYN_INH : SYN_EXC;
            int at = n_staged;
            if (stage(c, ns, dst, src, kind, w, motor_delay_of(dst)) != 0)
                { fclose(f); return -1; }
            recv_flag[at] = 1;
        } else {
            /* s and sp rows run sender to receiver */
            int kind = (sign < 0 || conn_is_inhibitory(src)) ? SYN_INH : SYN_EXC;
            if (stage(c, ns, src, dst, kind, w, motor_delay_of(src)) != 0)
                { fclose(f); return -1; }
        }
    }
    fclose(f);

    /* a receive row whose synapse already exists as a send row is the same
       synapse listed twice, drop the copy */
    for (i = 0; i < n_staged; i++) {
        if (!recv_flag[i])
            continue;
        for (j = 0; j < n_staged; j++) {
            if (recv_flag[j])
                continue;
            if (staged[j].src == staged[i].src && staged[j].dst == staged[i].dst) {
                recv_flag[i] = 2;   /* drop marker */
                break;
            }
        }
    }
    kept = 0;
    for (i = 0; i < n_staged; i++) {
        if (recv_flag[i] == 2)
            continue;
        if (kept != i)
            staged[kept] = staged[i];
        kept++;
    }
    n_staged = kept;

    if (n_staged == 0)
        return -1;
    c->n_neurons = ns->n;
    build_csr(c);
    return 0;
}

void conn_classify(connectome_t* c, nervous_t* ns)
{
    int i;
    for (i = 0; i < ns->n; i++) {
        int type = NT_INTER;
        if (has_prefix(ns->cells[i].name, SENSORY_PREFIX))
            type = NT_SENSORY;
        else if (has_prefix(ns->cells[i].name, MOTOR_PREFIX))
            type = NT_MOTOR;
        else if (has_prefix(ns->cells[i].name, COMMAND_PREFIX))
            type = NT_COMMAND;
        ns->cells[i].type = (uint8_t)type;
        if (type == NT_SENSORY)
            ns->cells[i].tau_base = 40.0f;
    }
    (void)c;
}
