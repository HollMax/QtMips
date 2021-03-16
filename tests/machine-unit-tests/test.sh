#!/usr/bin/env bash
TEST_NAME="machine-unit-test"

# Load common test shell functions
. "$(dirname "$0")/../test_lib.sh"

# Build tests binary
qtmips_make sub-qtmips_machine-tests
# Build test data
mips_make_test

cd "$TEST_DIR"

# Run unit tests
qtmips_run qtmips_machine/tests/tst_machine
