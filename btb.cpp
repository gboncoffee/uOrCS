#include "btb.hpp"

#include <sys/types.h>

#include <cstdint>

#include "opcode_package.hpp"
#include "simulator.hpp"

btb_t::btb_t() {
    for (auto i = 0; i < btb::BUFFER_SIZE; ++i) {
        for (auto j = 0; j < btb::BLOCK_SIZE; ++j) {
            this->buffer[i][j].valid = false;
        }
    }
}

btb_entry_t *btb_t::get_entry(uint64_t address) {
    int block_idx, idx;
    // And here's why BUFFER_SIZE shall be a power of 2.
    // This gets exactly the number of bits we want from the address.
    //
    // Example:
    // If our BUFFER_BITS is 10, BUFFER_SIZE is 1024 (0b10000000000). So it's
    // complement is 0b01111111111, so, with and AND, it's exactly the lower 10
    // bits of the address.
    block_idx = address & ~btb::BUFFER_SIZE;
    btb_block_t *block = &this->buffer[block_idx];
    idx = 0;
    // Using two loops avoids using two entries for the same instruction.
    for (int i = 0; i < btb::BLOCK_SIZE; ++i) {
        if ((*block)[i].address == address) {
            return &(*block)[i];
        }
    }
    for (int i = 0; i < btb::BLOCK_SIZE; ++i) {
        if (!(*block)[i].valid) {
            return &(*block)[i];
        }
        if ((*block)[i].last_access >= (*block)[idx].last_access) {
            idx = i;
        }
    }
    return &(*block)[idx];
}

bool btb_t::fetch_uncond(uint64_t address, btb_entry_t *entry) {
    return (entry->valid && entry->address == address);
}

void btb_t::set_uncond(uint64_t address, btb_entry_t *entry) {
    entry->address = address;
    entry->is_conditional = false;
    entry->valid = true;
}

int btb_t::check_instruction(opcode_package_t *instruction, uint64_t cicle) {
    if (instruction->opcode_operation == INSTRUCTION_OPERATION_BRANCH &&
        instruction->branch_type != BRANCH_RETURN) {
        btb_entry_t *entry = get_entry(instruction->opcode_address);
        entry->last_access = cicle;

        if (instruction->branch_type == BRANCH_COND) {
            // TODO
        } else {
            bool hit = btb_t::fetch_uncond(instruction->opcode_address, entry);
            btb_t::set_uncond(instruction->opcode_address, entry);
            return hit ? 0 : btb::LATENCY_UNCOND;
        }
    }

    return 0;
}
