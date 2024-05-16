#ifndef UORCS_BTB_PACKAGE_HPP_
#define UORCS_BTB_PACKAGE_HPP_

#include <cmath>
#include <cstdint>

#include "opcode_package.hpp"

namespace btb {

const int BLOCK_SIZE = 12;
const int LATENCY_UNCOND = 12;

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
    void set_uncond(uint64_t address, btb_entry_t *entry);
    bool fetch_uncond(uint64_t address, btb_entry_t *entry);
    btb_entry_t *get_entry(uint64_t address);

   public:
    btb_t();
    void statistics();
    // Returns the latency.
    int check_instruction(opcode_package_t *instruction, uint64_t cicle);
};

#endif  // UORCS_BTB_PACKAGE_HPP_
