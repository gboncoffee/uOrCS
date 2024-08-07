#ifndef UORCS_BTB_PACKAGE_HPP_
#define UORCS_BTB_PACKAGE_HPP_

#include <cmath>
#include <cstdint>

#include "opcode_package.hpp"
#include "simulator.hpp"

namespace btb {

const int BLOCK_SIZE = 12;
const int LATENCY_UNCOND = 12;
const int LATENCY_COND = 512;

const int BUFFER_BITS = 10;
const int BUFFER_SIZE = 1 << BUFFER_BITS;

}  // namespace btb

typedef struct {
    bool valid;
    uint64_t address;
    uint64_t last_access;
    bool is_conditional;
    bool is_taken;
} btb_entry_t;

typedef btb_entry_t btb_block_t[btb::BLOCK_SIZE];

class btb_t {
   private:
    btb_block_t buffer[btb::BUFFER_SIZE];

    uint64_t n_uncond;
    uint64_t n_syscalls;
    uint64_t n_calls;
    uint64_t n_missed_uncond;
    uint64_t n_missed_syscalls;
    uint64_t n_missed_calls;

    uint64_t n_cond_taken;
    uint64_t n_cond_not_taken;
    uint64_t n_cond_taken_missed;
    uint64_t n_cond_not_taken_missed;
    uint64_t n_cond_taken_predicted;
    uint64_t n_cond_not_taken_predicted;

    uint64_t last_address;
    uint64_t last_size;
    btb_entry_t *last_entry;

    void increase_stat(branch_t type);
    void increase_uncond_missed(branch_t type);
    void set_uncond(uint64_t address, btb_entry_t *entry);
    int check_uncond(opcode_package_t *instruction, btb_entry_t *entry);
    void prepare_cond(opcode_package_t *instruction, btb_entry_t *entry);
    void increase_cond_stats(bool is_taken, bool hit, bool predicted_taken);
    int check_last(opcode_package_t *instruction);
    btb_entry_t *get_entry(uint64_t address, uint64_t cicle);

   public:
    btb_t();
    void statistics();
    // Returns the latency.
    int check_instruction(opcode_package_t *instruction, uint64_t cicle);
    inline uint64_t get_total_accesses() {
        return this->n_calls + this->n_syscalls + this->n_uncond +
               this->n_cond_taken + this->n_cond_not_taken;
    }
};

#endif  // UORCS_BTB_PACKAGE_HPP_
