#include "btb.hpp"

#include <cstdint>
#include <cstdio>

#include "opcode_package.hpp"
#include "simulator.hpp"

btb_t::btb_t() {
    for (auto i = 0; i < btb::BUFFER_SIZE; ++i) {
        for (auto j = 0; j < btb::BLOCK_SIZE; ++j) {
            this->buffer[i][j].valid = false;
        }
    }
    this->n_calls = 0;
    this->n_syscalls = 0;
    this->n_uncond = 0;
    this->n_cond = 0;
    this->n_missed_uncond = 0;
    this->n_missed_calls = 0;
    this->n_missed_syscalls = 0;
}

btb_entry_t *btb_t::get_entry(uint64_t address) {
    // And here's why BUFFER_SIZE shall be a power of 2.
    // This gets exactly the number of bits we want from the address.
    //
    // Example:
    // If our BUFFER_BITS is 10, BUFFER_SIZE is 1024 (0b10000000000). So it -1
    // is 0b01111111111, so, with an AND, it's exactly the lower 10 bits of the
    // address.
    int b = address & (btb::BUFFER_SIZE - 1);

    // Using two loops avoids using two entries for the same instruction, as we
    // first check if there's a entry for the instruction we're fetching and, if
    // there's none, we do the usual check of picking the first invalid entry or
    // the oldest accessed.
    for (int i = 0; i < btb::BLOCK_SIZE; ++i) {
        if (this->buffer[b][i].address == address) {
            return &this->buffer[b][i];
        }
    }

    int idx = 0;
    for (int i = 0; i < btb::BLOCK_SIZE; ++i) {
        if (!this->buffer[b][i].valid) {
            return &this->buffer[b][i];
        }
        if (this->buffer[b][i].last_access <= this->buffer[b][idx].last_access) {
            idx = i;
        }
    }
    return &this->buffer[b][idx];
}

bool btb_t::fetch_uncond(uint64_t address, btb_entry_t *entry) {
    return (entry->valid && entry->address == address);
}

void btb_t::set_uncond(uint64_t address, btb_entry_t *entry) {
    entry->address = address;
    entry->is_conditional = false;
    entry->valid = true;
}

void btb_t::increase_stat(branch_t type) {
    switch (type) {
        case BRANCH_COND:
            this->n_cond++;
            break;
        case BRANCH_UNCOND:
            this->n_uncond++;
            break;
        case BRANCH_SYSCALL:
            this->n_syscalls++;
            break;
        case BRANCH_CALL:
            this->n_calls++;
            break;
        // Ignore RETURN as it's unreachable.
        default:
            break;
    }
}

void btb_t::increase_uncond_missed(branch_t type) {
    switch (type) {
        case BRANCH_UNCOND:
            this->n_missed_uncond++;
            break;
        case BRANCH_SYSCALL:
            this->n_missed_syscalls++;
            break;
        case BRANCH_CALL:
            this->n_missed_calls++;
            break;
        // Ignore RETURN and COND as they're unreachable.
        default:
            break;
    }
}

int btb_t::check_instruction(opcode_package_t *instruction, uint64_t cicle) {
    if (instruction->opcode_operation == INSTRUCTION_OPERATION_BRANCH &&
        instruction->branch_type != BRANCH_RETURN) {
        btb_entry_t *entry = get_entry(instruction->opcode_address);
        entry->last_access = cicle;
        increase_stat(instruction->branch_type);

        if (instruction->branch_type == BRANCH_COND) {
            // TODO
        } else {
            bool hit = btb_t::fetch_uncond(instruction->opcode_address, entry);
            btb_t::set_uncond(instruction->opcode_address, entry);

            if (hit) return 0;

            this->increase_uncond_missed(instruction->branch_type);
            return btb::LATENCY_UNCOND;
        }
    }

    return 0;
}

void btb_t::statistics() {
    ORCS_PRINTF("######################################################\n");
    ORCS_PRINTF("btb_t\n");
    ORCS_PRINTF("total BTB accesses: %lu\n", this->get_total_accesses());
    ORCS_PRINTF("number of unconditionals: %lu (missed: %lu)\n", this->n_uncond,
                this->n_missed_uncond);
    ORCS_PRINTF("number of syscalls: %lu (missed: %lu)\n", this->n_syscalls,
                this->n_missed_syscalls);
    ORCS_PRINTF("number of calls: %lu (missed: %lu)\n", this->n_calls,
                this->n_missed_calls);
}
