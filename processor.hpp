#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "btb.hpp"
#include "predictor.hpp"

class processor_t {
   public:
    btb_t *btb;
    predictor_t *predictor;
    // =========================================================================
    // Methods.
    // =========================================================================
    processor_t();
    void allocate();
    void clock();
    void statistics();
};

#endif  // PROCESSOR_HPP
