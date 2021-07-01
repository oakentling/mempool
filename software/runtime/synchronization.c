// Copyright 2021 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

// Author: Samuel Riedel, ETH Zurich

#include <stdbool.h>
#include <stdint.h>

#include "runtime.h"
#include "synchronization.h"

uint32_t volatile barrier __attribute__((section(".l1")));
uint32_t volatile barrier_iteration __attribute__((section(".l1")));
uint32_t volatile barrier_init __attribute__((section(".l2"))) = 0;
uint32_t volatile barrier_init_gomp __attribute__((section(".l2"))) = 0;

void mempool_barrier_init(uint32_t core_id) {
  if (core_id == 0) {
    // Initialize the barrier
    barrier = 0;
    wake_up_all();
    mempool_wfi();
  } else {
    mempool_wfi();
  }
}

void mempool_barrier(uint32_t num_cores) {
  // Increment the barrier counter
  if ((num_cores - 1) == __atomic_fetch_add(&barrier, 1, __ATOMIC_RELAXED)) {
    __atomic_store_n(&barrier, 0, __ATOMIC_RELAXED);
    __sync_synchronize(); // Full memory barrier
    wake_up_all();
  }
  // Some threads have not reached the barrier --> Let's wait
  // Clear the wake-up trigger for the last core reaching the barrier as well
  mempool_wfi();
}

void mempool_barrier_gomp(uint32_t core_id, uint32_t num_cores) {
  if (barrier_init_gomp == 0){
    if (core_id == 0) {
      barrier = 0;
      barrier_iteration = 0;
      barrier_init = 1;
      barrier_init_gomp = 1;
    } else {
      while (!barrier_init)
	mempool_wait(2 * num_cores);
    }
  }
  // Remember previous iteration
  uint32_t iteration_old = barrier_iteration;

  // Increment the barrier counter
  if ((num_cores - 1) == __atomic_fetch_add(&barrier, 1, __ATOMIC_SEQ_CST)) {
    barrier = 0;
    __atomic_fetch_add(&barrier_iteration, 1, __ATOMIC_SEQ_CST);
  } else {
    // Some threads have not reached the barrier --> Let's wait
    while (iteration_old == barrier_iteration)
      mempool_wait(num_cores*2);
  }
}
