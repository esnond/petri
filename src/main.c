#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "sim.h"
#include "tui.h"
#include "evolution.h"

#define RENDER_EVERY_MS 33.0f

static void sleep_ms(float ms)
{
    struct timespec ts;
    if (ms <= 0.0f)
        return;
    ts.tv_sec = (time_t)(ms / 1000.0f);
    ts.tv_nsec = (long)((ms - (float)ts.tv_sec * 1000.0f) * 1e6f);
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000L;
    }
    nanosleep(&ts, NULL);
}

static void usage(FILE* out)
{
    int tty = isatty(fileno(out));
    const char* h1 = tty ? "\x1b[1;36m" : "";
    const char* dim = tty ? "\x1b[2m" : "";
    const char* r = tty ? "\x1b[0m" : "";
    fprintf(out,
"%spetri %s— whole animal connectome simulator for caenorhabditis elegans%s\n"
"\n"
"%sRUN%s\n"
"  petri                        interactive dashboard (needs a tty)\n"
"  petri --headless -d 120      batch run, metrics csv to stdout\n"
"  petri --headless -d 60 -s 7  same worm, different noise realization\n"
"\n"
"%sWHAT THE WORM SEES%s\n"
"  -c, --connectome F     wiring dataset: varshney_full, herm_302, (in data/),\n"
"                         cook2019_male, witvliet_adult, or builtin\n"
"  -F, --no-food          empty dish\n"
"  -G, --thermo           thermal gradient\n"
"  -T, --tap MS           mechanical tap every MS milliseconds\n"
"\n"
"%sWHAT YOU DO TO THE WORM%s\n"
"  -a, --ablate LIST      silence neurons by name prefix (AVA, PVC)\n"
"  -S, --surgery LIST     cut all outputs of the listed neurons\n"
"  -Q, --mutant NAME      apply a mutant (unc-54, unc-25, unc-13...)\n"
"  -J, --opsin LIST       express opsins, Neuron:ChETA|NpHR\n"
"  -K, --light-protocol F json light events\n"
"  -L, --light-zone C     closed loop circle cx,cy,r_mm\n"
"\n"
"%sWHAT THE WORM BECOMES%s\n"
"  -E, --evolve N         evolve weights for N generations\n"
"  -n, --evolve-fitness M path, net or cost\n"
"  -I, --inverse F        evolve toward a drawn path\n"
"  -W, --swarm N          N worms in one dish\n"
"  -U, --shuffle-connectome N  degree preserving null rewiring\n"
"\n"
"%sMEASUREMENT%s\n"
"  -d, --duration S       simulated seconds (default 180)\n"
"  -s, --seed N           noise realization, same seed same bytes\n"
"  -N, --trace F          gcamp traces of the command pool\n"
"  -M, --trace-all F      every membrane potential\n"
"  -D, --dump-edges F     ground truth adjacency\n"
"  -A, --ablate-screen    every single neuron, one row each\n"
"  -e, --eval F           score a genome json\n"
"  -q, --quiet            suppress csv rows\n"
"  -h, --help             this text\n"
"\n"
"%sbatch runs are deterministic: same seed, same bytes. all published\n"
"numbers reproduce with sh run_all.sh (~15 min). docs/ holds the lab\n"
"notebook: calibration, validation, atlas, predictions, open defects.%s\n",
        h1, r, "", h1, r, h1, r, h1, r, h1, r, h1, r, dim, r);
}


int main(int argc, char** argv)
{
    static sim_t sim;
    sim_opts_t opts;
    int rc = 0;
    int paused = 0;
    int have_duration = 0;
    float speed_mult = 1.0f;

    memset(&opts, 0, sizeof(opts));
    opts.duration_s = 180.0;
    opts.seed = 1;

    {
        static const struct option lo[] = {
            { "headless", no_argument, NULL, 'H' },
            { "duration", required_argument, NULL, 'd' },
            { "seed", required_argument, NULL, 's' },
            { "connectome", required_argument, NULL, 'c' },
            { "ablate", required_argument, NULL, 'a' },
            { "ablate-screen", no_argument, NULL, 'A' },
            { "eval", required_argument, NULL, 'e' },
            { "evolve", required_argument, NULL, 'E' },
            { "population", required_argument, NULL, 'P' },
            { "evolve-output", required_argument, NULL, 'O' },
            { "evolve-fitness", required_argument, NULL, 'n' },
            { "conditioning", required_argument, NULL, 'C' },
            { "tap", required_argument, NULL, 'T' },
            { "no-food", no_argument, NULL, 'F' },
            { "thermo", no_argument, NULL, 'G' },
            { "quiet", no_argument, NULL, 'q' },
            { "surgery", required_argument, NULL, 'S' },
            { "inverse", required_argument, NULL, 'I' },
            { "swarm", required_argument, NULL, 'W' },
            { "trace", required_argument, NULL, 'N' },
            { "opsin", required_argument, NULL, 'J' },
            { "light-protocol", required_argument, NULL, 'K' },
            { "light-zone", required_argument, NULL, 'L' },
            { "mutant", required_argument, NULL, 'Q' },
            { "trace-all", required_argument, NULL, 'M' },
            { "dump-edges", required_argument, NULL, 'D' },
            { "shuffle-connectome", required_argument, NULL, 'U' },
            { "help", no_argument, NULL, 'h' },
            { "version", no_argument, NULL, 'V' },
            { NULL, 0, NULL, 0 }
        };
        int ch;
        while ((ch = getopt_long(argc, argv, "Hd:s:c:a:Ae:E:P:O:n:C:T:FGSIWMDNUQJKLVqh", lo, NULL)) != -1) {
            switch (ch) {
            case 'H': opts.headless = 1; break;
            case 'd': opts.duration_s = atof(optarg); have_duration = 1; break;
            case 's': opts.seed = (uint64_t)strtoull(optarg, NULL, 10); break;
            case 'c': opts.connectome_path = optarg; break;
            case 'a': opts.ablate_list = optarg; break;
            case 'A': opts.ablate_screen = 1; opts.headless = 1; break;
            case 'e': opts.eval_path = optarg; opts.headless = 1; break;
            case 'E': opts.evolve_gens = atoi(optarg); opts.headless = 1; break;
            case 'P': opts.population = atoi(optarg); break;
            case 'O': opts.evolve_output = optarg; break;
            case 'n':
                opts.evolve_fitness_net = (strcmp(optarg, "net") == 0);
                opts.evolve_fitness_cost = (strcmp(optarg, "cost") == 0);
                break;
            case 'C':
                opts.conditioning = strcmp(optarg, "paired") == 0 ? 1 :
                                    (strcmp(optarg, "control") == 0 ? 2 : 0);
                opts.headless = 1;
                break;
            case 'T': opts.tap_interval_ms = (float)atof(optarg); break;
            case 'F': opts.no_food = 1; break;
            case 'G': opts.thermo = 1; break;
            case 'S': opts.surgery_list = optarg; break;
            case 'I': opts.inverse_path = optarg; opts.headless = 1; break;
            case 'W': opts.swarm_n = atoi(optarg); opts.headless = 1;
                      opts.swarm_seed = opts.seed; break;
            case 'N': opts.trace_path = optarg; break;
            case 'J': opts.opsin_list = optarg; break;
            case 'K': opts.light_protocol = optarg; break;
            case 'L': opts.light_zone = optarg; break;
            case 'Q': opts.mutant = optarg; break;
            case 'M': opts.trace_all_path = optarg; break;
            case 'D': opts.edges_path = optarg; break;
            case 'U': opts.shuffle_seed = (uint64_t)strtoull(optarg, NULL, 10); break;
            case 'q': opts.quiet = 1; break;
            case 'V': printf("petri 1.0\n"); return 0;
            case 'h': usage(stdout); return 0;
            default: usage(stderr); return 1;
            }
        }
        /* conflicting batch modes are a user error, not a precedence rule */
        if (opts.eval_path && opts.evolve_gens > 0) {
            fprintf(stderr, "error: --eval and --evolve are mutually exclusive\n");
            return 1;
        }
        if (opts.ablate_screen && opts.evolve_gens > 0) {
            fprintf(stderr, "error: --ablate-screen and --evolve are mutually exclusive\n");
            return 1;
        }
        if (opts.ablate_screen && opts.eval_path) {
            fprintf(stderr, "error: --ablate-screen and --eval are mutually exclusive\n");
            return 1;
        }
        /* spec defaults per mode when the user did not pin a duration */
        if (!have_duration) {
            if (opts.evolve_gens > 0)
                opts.duration_s = 300.0;
            else if (opts.ablate_screen)
                opts.duration_s = 60.0;
        }
    }

    if (sim_init(&sim, &opts) != 0) {
        rc = 1;
        goto cleanup;
    }

    if (sim.opts.ablate_screen) {
        sim_ablate_screen(&sim, sim.opts.duration_s);
        goto cleanup;
    }

    if (sim.opts.eval_path) {
        sim_run(&sim, NULL, NULL);
        printf("fitness,%.4f\n", sim_fitness(&sim));
        printf("chemotaxis_index,%.4f\n", sim.metrics.chemotaxis_index);
        printf("mean_speed_mm_s,%.4f\n", sim.metrics.speed_ema);
        printf("muscle_cost,%.2f\n", sim.metrics.energy_cost);
        printf("speed_per_cost,%.5f\n", sim.metrics.speed_ema / (sim.metrics.energy_cost + 1.0f));
        printf("omega_turns,%ld\n", sim.metrics.n_omega);
        goto cleanup;
    }

    if (sim.opts.swarm_n > 0) {
        static sim_t swarm[32];
        sim_opts_t so = sim.opts;
        int nw = sim.opts.swarm_n > 32 ? 32 : sim.opts.swarm_n;
        int i, j;
        long step, n_steps = (long)(sim.opts.duration_s * 1000.0 / DT_MS);
        FILE* pf = fopen("results/swarm_pos.csv", "w");
        if (nw < 2) nw = 2;
        if (!pf) { fprintf(stderr, "swarm: cannot open results/swarm_pos.csv\n"); rc = 1; goto cleanup; }
        fprintf(pf, "t_s,worm,x_mm,y_mm\n");
        for (i = 0; i < nw; i++) {
            so.seed = sim.opts.swarm_seed + (uint64_t)i;
            so.quiet = 1;
            so.headless = 1;
            if (sim_init(&swarm[i], &so) != 0) { rc = 1; goto cleanup; }
        }
        fprintf(stderr, "swarm: %d worms, %d s\n", nw, (int)sim.opts.duration_s);
        for (step = 0; step < n_steps; step++) {
            float t_s = (float)step * DT_MS / 1000.0f;
            for (i = 0; i < nw; i++)
                sim_step(&swarm[i]);
            /* worm to worm touch: nose within 0.35 mm of another worm's
               head or tail tip fires the contact path of both */
            for (i = 0; i < nw; i++) {
                for (j = i + 1; j < nw; j++) {
                    float dx = swarm[i].body.x[0] - swarm[j].body.x[0];
                    float dy = swarm[i].body.y[0] - swarm[j].body.y[0];
                    float dx2 = swarm[i].body.x[0] - swarm[j].body.x[N_SEG];
                    float dy2 = swarm[i].body.y[0] - swarm[j].body.y[N_SEG];
                    if (dx * dx + dy * dy < 0.1225f ||
                        dx2 * dx2 + dy2 * dy2 < 0.1225f) {
                        swarm[i].body.contact_hint = 1;
                        swarm[j].body.contact_hint = 1;
                    }
                }
            }
            if (step % 100 == 0) {
                for (i = 0; i < nw; i++)
                    fprintf(pf, "%.1f,%d,%.3f,%.3f\n", t_s, i,
                            swarm[i].body.x[0], swarm[i].body.y[0]);
            }
            if (step % 10000 == 0) {
                float nn_sum = 0.0f;
                int on_lawn = 0;
                for (i = 0; i < nw; i++) {
                    float best = 1e9f;
                    float lx = swarm[i].body.x[0] - 12.0f;
                    float ly = swarm[i].body.y[0] - 8.0f;
                    if (lx * lx + ly * ly < 12.25f) on_lawn++;
                    for (j = 0; j < nw; j++) {
                        float dx, dy, dd;
                        if (j == i) continue;
                        dx = swarm[i].body.x[0] - swarm[j].body.x[0];
                        dy = swarm[i].body.y[0] - swarm[j].body.y[0];
                        dd = dx * dx + dy * dy;
                        if (dd < best) best = dd;
                    }
                    nn_sum += sqrtf(best);
                }
                fprintf(stderr, "swarm t=%.0fs mean_nn=%.2fmm on_lawn=%d/%d\n",
                        t_s, nn_sum / nw, on_lawn, nw);
            }
        }
        fclose(pf);
        goto cleanup;
    }

    if (sim.opts.inverse_path) {
        int gens = sim.opts.evolve_gens > 0 ? sim.opts.evolve_gens : 30;
        fprintf(stderr, "inverse: %d generations, target %s\n", gens,
                sim.opts.inverse_path);
        evolution_run_inverse(sim.opts.inverse_path, gens,
                              (int)sim.opts.duration_s,
                              sim.opts.connectome_path,
                              "results/inverse_best");
        goto cleanup;
    }

    if (sim.opts.evolve_gens > 0) {
        evolution_t evo;
        int eval_dur = (int)sim.opts.duration_s;
        const char* out_path = sim.opts.evolve_output ? sim.opts.evolve_output : "best_genome.json";
        evolution_init(&evo, sim.opts.seed,
                       sim.opts.population > 0 ? sim.opts.population : 20,
                       sim.opts.evolve_fitness_net);
        evolution_run(&evo, sim.opts.evolve_gens, eval_dur,
                      sim.opts.connectome_path, out_path);
        fprintf(stderr, "evolution complete, best genome saved to %s\n", out_path);
        goto cleanup;
    }

    if (sim.opts.headless) {
        sim_run(&sim, NULL, NULL);
        fprintf(stderr, "final ci %.3f speed %.3f omega %ld reversals %ld\n",
                sim.metrics.chemotaxis_index, sim.metrics.speed_ema,
                sim.metrics.n_omega, sim.metrics.n_reversals);
        goto cleanup;
    }

    /* interactive dashboard, refuse to start without a real tty */
    if (!have_duration) {
        /* an interactive session runs until q, not until the batch timer
           runs out, the batch default only exists for the scripted modes */
        opts.duration_s = 1.0e9;
    }
    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "terminal: stdin is not a tty, use --headless\n");
        rc = 1;
        goto cleanup;
    }
    if (tui_enter() != 0 || tui_stdin_nonblock() != 0) {
        fprintf(stderr, "terminal: raw mode unavailable, use --headless\n");
        rc = 1;
        goto cleanup;
    }

    fprintf(stderr, "dashboard: q quit, p pause, t tap, f food, +/- sim speed\n");
    {
        static tui_t tui;
        long n_steps = isfinite(sim.opts.duration_s)
                       ? (long)(sim.opts.duration_s * 1000.0 / DT_MS) : 0;
        long steps_per_frame = (long)(RENDER_EVERY_MS / DT_MS);
        long frame_step = 0;
        long i;
        memset(&tui, 0, sizeof(tui));
        tui.compact = tui_wants_compact();
        {
            int sel = -1;
            if (sim.ns.n > 0)
                sel = 0;
        for (i = 0; i < n_steps; i++) {
            if (!paused)
                sim_step(&sim);
            frame_step++;
            if (frame_step >= (long)((float)steps_per_frame * speed_mult)) {
                char buf[64];
                frame_step = 0;
                tui_render(&tui, &sim, paused, speed_mult, sel);
                /* non blocking buffered drain, one read grabs every pending
                   byte so bursts are not lost between frames */
                for (;;) {
                    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
                    if (n > 0) {
                        int k;
                        for (k = 0; k < (int)n; k++) {
                            if (buf[k] == 'q') { tui_leave(); goto cleanup; }
                            if (buf[k] == 'p') paused = !paused;
                            if (buf[k] == 't') sim.tap_active_ms = (int)TAP_MS;
                            if (buf[k] == 'f') sim.world.enable_food = !sim.world.enable_food;
                            if (buf[k] == '+') speed_mult *= 1.5f;
                            if (buf[k] == '-') speed_mult /= 1.5f;
                            if (speed_mult > 16.0f) speed_mult = 16.0f;
                            if (speed_mult < 0.25f) speed_mult = 0.25f;
                            if (buf[k] == ',' && sel > 0)
                                sel--;
                            if (buf[k] == '.' && sel < sim.ns.n - 1)
                                sel++;
                            if (buf[k] == '\n' && sel >= 0)
                                sim_cut_outputs(&sim, sim.ns.cells[sel].name);
                            if (buf[k] == 'x' && sel >= 0)
                                sim_restore_outputs(&sim, sim.ns.cells[sel].name);
                            if (buf[k] == 'w')
                                sim_surgery_session(&sim, "surgery_session.json");
                        }
                        continue;
                    }
                    if (n < 0 && errno == EINTR)
                        continue;
                    /* eagain or eof, no pending input */
                    break;
                }
                sleep_ms(RENDER_EVERY_MS / speed_mult);
            }
        }
        if (n_steps < (1L << 40))
            fprintf(stderr, "duration reached\n");
        }
    }

cleanup:
    tui_leave();
    return rc;
}
