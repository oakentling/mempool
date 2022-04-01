# Copyright 2021 ETH Zurich and University of Bologna.
# Licensed under the Apache License, Version 2.0, see LICENSE for details.
# SPDX-License-Identifier: Apache-2.0

# Author: Samuel Riedel, ETH Zurich
#         Matheus Cavalcante, ETH Zurich

######################
##  MemPool flavor  ##
######################

# Choose a MemPool flavor, either "minpool" or "mempool".
# Check the README for more details
ifndef config
  ifdef MEMPOOL_CONFIGURATION
    config := $(MEMPOOL_CONFIGURATION)
  else
    # Default configuration, if neither `config` nor `MEMPOOL_CONFIGURATION` was found
    config := mempool
  endif
endif
include $(MEMPOOL_DIR)/config/$(config).mk

#############################
##  Address configuration  ##
#############################

# Boot address (in hex)
boot_addr ?= A0000000

# L2 memory configuration (in hex)
l2_base  ?= 80000000
l2_size  ?= 10000
l2_banks ?= 4

# Size of sequential memory per core (in bytes)
# (must be a power of two)
seq_mem_size ?= 2048

# Size of stack in sequential memory per core (in bytes)
# (must not be larger than the sequential memory - queue)
stack_size ?= 1024

#############################
##  Xqueues configuration  ##
#############################

# Size of queue in each memory bank (in words)
# (assume banking factor of 4)
queue_size ?= 4

################################
##  Optional functionalities  ##
################################

# Enable the XpulpIMG extension
xpulpimg ?= 1
