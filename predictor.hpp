#ifndef UORCS_PREDICTOR_HPP_
#define UORCS_PREDICTOR_HPP_

#include <sys/types.h>

#include <cstdint>

#include "opcode_package.hpp"

namespace counter {

const int BITS = 2;
const int MAX = (1 << BITS) - 1;
const int MIDDLE_TRUE = (1 << BITS / 2);

typedef uint8_t counter;

inline bool is_true(counter counter) { return counter >= MIDDLE_TRUE; }

inline counter increase(counter counter) { return counter + (counter < MAX); }

inline counter decrease(counter counter) { return counter - (counter > 0); }

}  // namespace counter

namespace predictor {

const int TABLE_BITS = 11;
const int TABLE_SIZE = 1 << TABLE_BITS;
const int LATENCY = 512;

}  // namespace predictor

class predictor_t {
   private:
    uint64_t history;
    counter::counter gshare[predictor::TABLE_SIZE];
    counter::counter bimodal[predictor::TABLE_SIZE];
    counter::counter use_gshare;

    uint64_t last_address;
    uint64_t last_size;
	uint16_t last_gshare_index;
	uint16_t last_bimodal_index;
    bool last_gshare_is_taken;
    bool last_bimodal_is_taken;
    bool last_use_gshare;
    bool last_is_valid;

    uint64_t n_taked;
    uint64_t n_not_taked;
    uint64_t n_used_gshare;
    uint64_t n_gshare_predicted;
    uint64_t n_bimodal_predicted;
    uint64_t n_gshare_predicted_on_bimodal_error;
    uint64_t n_bimodal_predicted_on_gshare_error;

    int check_last(opcode_package_t *instruction);
    void prepare(opcode_package_t *instruction);

   public:
    predictor_t();
    void statistics();
    // Returns the latency.
    int check_instruction(opcode_package_t *instruction);
};

#endif  // UORCS_PREDICTOR_HPP_
