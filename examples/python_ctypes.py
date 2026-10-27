#!/usr/bin/env python3
# runs the petri core through ctypes, no subprocess, no dependencies
# build first: make libpetri.so
import ctypes, sys, os

lib = ctypes.CDLL(os.path.join(os.path.dirname(__file__), "..", "libpetri.so"))
lib.petri_open_default.argtypes = [ctypes.c_uint64, ctypes.c_double]
lib.petri_open.argtypes = [ctypes.c_char_p, ctypes.c_uint64, ctypes.c_double]
lib.petri_steps.argtypes = [ctypes.c_long]
lib.petri_speed.restype = ctypes.c_float
lib.petri_head_x.restype = ctypes.c_float
lib.petri_head_y.restype = ctypes.c_float
lib.petri_time.restype = ctypes.c_double
lib.petri_reversals.restype = ctypes.c_long

seed = int(sys.argv[1]) if len(sys.argv) > 1 else 7
if lib.petri_open_default(seed, 60.0) != 0:
    sys.exit("open failed")
for _ in range(12):          # twelve seconds of simulated time
    lib.petri_steps(1000)    # dt is 1 ms
    print(f"t={lib.petri_time():5.1f}s speed={lib.petri_speed():+.3f} mm/s "
          f"head=({lib.petri_head_x():.2f},{lib.petri_head_y():.2f}) "
          f"reversals={lib.petri_reversals()}")
