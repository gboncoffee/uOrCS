#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

class processor_t {
   public:
    // =========================================================================
    // Methods.
    // =========================================================================
    processor_t();
    void allocate();
    void clock();
    void statistics();
};

#endif  // PROCESSOR_HPP
