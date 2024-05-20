#include "predictor.hpp"

#include <cstdio>

#include "opcode_package.hpp"
#include "simulator.hpp"

predictor_t::predictor_t() {
    for (auto i = 0; i < predictor::TABLE_SIZE; ++i) {
        this->bimodal[i] = counter::MIDDLE_TRUE;
        this->gshare[i] = counter::MIDDLE_TRUE;
    }
    this->use_gshare = counter::MIDDLE_TRUE;
    this->last_is_valid = false;

	this->history = 0xffff;
    this->n_taked = 0;
    this->n_not_taked = 0;
    this->n_used_gshare = 0;
    this->n_gshare_predicted = 0;
    this->n_bimodal_predicted = 0;
    this->n_gshare_predicted_on_bimodal_error = 0;
    this->n_bimodal_predicted_on_gshare_error = 0;
}

void predictor_t::prepare(opcode_package_t *instruction) {
    this->last_is_valid = true;
    // To explain this, I'll copy the comment in predictor.hpp:34:
    //
    // > And here's why BUFFER_SIZE shall be a power of 2.
    // > This gets exactly the number of bits we want from the address.
    // >
    // > Example:
    // > If our BUFFER_BITS is 10, BUFFER_SIZE is 1024 (0b10000000000). So it -1
    // > is 0b01111111111, so, with an AND, it's exactly the lower 10 bits of
    // > the address.
    uint16_t gshare_idx = (instruction->opcode_address ^ this->history) &
                          (predictor::TABLE_SIZE - 1);
    uint16_t bimodal_idx =
        instruction->opcode_address & (predictor::TABLE_SIZE - 1);
    this->last_gshare_is_taken = counter::is_true(this->gshare[gshare_idx]);
    this->last_bimodal_is_taken = counter::is_true(this->bimodal[bimodal_idx]);
    this->last_use_gshare = counter::is_true(this->use_gshare);
    this->last_address = instruction->opcode_address;
    this->last_size = instruction->opcode_size;
    this->last_gshare_index = gshare_idx;
    this->last_bimodal_index = bimodal_idx;
}

int predictor_t::check_last(opcode_package_t *instruction) {
    if (!this->last_is_valid) return 0;

    int latency = 0;

    bool taked =
        instruction->opcode_address != this->last_address + this->last_size;

    bool gshare_correct = this->last_gshare_is_taken == taked;
    bool bimodal_correct = this->last_bimodal_is_taken == taked;

    if (this->last_use_gshare) {
        this->n_used_gshare++;
        if (gshare_correct) {
            this->n_gshare_predicted++;
        } else {
            latency = predictor::LATENCY;
            if (bimodal_correct) {
                this->n_bimodal_predicted_on_gshare_error++;
                this->use_gshare = counter::decrease(this->use_gshare);
            }
        }
    } else {
        if (bimodal_correct) {
            this->n_bimodal_predicted++;
        } else {
            latency = predictor::LATENCY;
            if (gshare_correct) {
                this->n_gshare_predicted_on_bimodal_error++;
                this->use_gshare = counter::increase(this->use_gshare);
            }
        }
    }

    if (taked) {
        this->n_taked++;
        this->gshare[this->last_gshare_index] =
            counter::increase(this->gshare[this->last_gshare_index]);
        this->bimodal[this->last_bimodal_index] =
            counter::increase(this->bimodal[this->last_bimodal_index]);
    } else {
        this->n_not_taked++;
        this->gshare[this->last_gshare_index] =
            counter::decrease(this->gshare[this->last_gshare_index]);
        this->bimodal[this->last_bimodal_index] =
            counter::decrease(this->bimodal[this->last_bimodal_index]);
    }

    this->history = (history << 1) | taked;

    return latency;
}

int predictor_t::check_instruction(opcode_package_t *instruction) {
    int latency = this->check_last(instruction);
    this->last_is_valid = false;

    if (instruction->opcode_operation == INSTRUCTION_OPERATION_BRANCH &&
        instruction->branch_type == BRANCH_COND) {
        this->prepare(instruction);
    }

    return latency;
}

void predictor_t::statistics() {
    ORCS_PRINTF("######################################################\n");
    ORCS_PRINTF("predictor_t\n");
    ORCS_PRINTF("total predictor accesses: %lu\n",
                this->n_taked + this->n_not_taked);
    ORCS_PRINTF("number of gshare uses: %lu\n", this->n_used_gshare);
    ORCS_PRINTF("number of gshare correct predictions: %lu\n",
                this->n_gshare_predicted);
    ORCS_PRINTF("number of bimodal correct predictions: %lu\n",
                this->n_bimodal_predicted);
    ORCS_PRINTF("number of gshare predictions on bimodal error: %lu\n",
                this->n_gshare_predicted_on_bimodal_error);
    ORCS_PRINTF("number of bimodal predictions on gshare error: %lu\n",
                this->n_bimodal_predicted_on_gshare_error);
}
