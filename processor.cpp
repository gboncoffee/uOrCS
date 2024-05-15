#include "processor.hpp"  // allocate, clock, processor_t, statistics

// If you use clangd, it'll give you a warning on this include, but it's used by
// ORCS_PRINTF. Try removing it to see what happens.
// ~ Gabriel
#include <cstdio>

#include "opcode_package.hpp"  // opcode_package_t
#include "orcs_engine.hpp"     // orcs_engine_t
#include "simulator.hpp"       // ORCS_PRINTF
#include "trace_reader.hpp"

// =============================================================================
processor_t::processor_t(){};

// =============================================================================
void processor_t::allocate(){};

// =============================================================================
void processor_t::clock() {
    // Get the next instruction from the trace.
    opcode_package_t new_instruction;
    if (!orcs_engine.trace_reader->trace_fetch(&new_instruction)) {
        // If EOF.
        orcs_engine.simulator_alive = false;
    }
};

// =============================================================================
void processor_t::statistics() {
    ORCS_PRINTF("######################################################\n");
    ORCS_PRINTF("processor_t\n");
};
