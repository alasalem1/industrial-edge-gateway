CC ?= gcc
CFLAGS ?= -Wall -Wextra -Werror -std=c11

ROOT := ..
ACQUISITION := $(ROOT)/meta-sentinel-edge/recipes-apps/industrial-data-acquisition/files
DIAGNOSTICS := $(ROOT)/sentinel-edge-ui/src/diagnostics
BUILD_DIR := /tmp/sentinel-edge-tests

.PHONY: all test clean

all: test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/test_diagnostic_engine: test_diagnostic_engine.c \
	$(DIAGNOSTICS)/diagnostic_engine.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(DIAGNOSTICS) $^ -o $@

$(BUILD_DIR)/test_sensor_backend_simulation: test_sensor_backend_simulation.c \
	$(ACQUISITION)/sensor_backend_simulation.c \
	$(DIAGNOSTICS)/diagnostic_engine.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L \
		-I$(ACQUISITION) -I$(DIAGNOSTICS) $^ -o $@

test: $(BUILD_DIR)/test_diagnostic_engine \
	$(BUILD_DIR)/test_sensor_backend_simulation
	@echo
	@echo "===== DIAGNOSTIC ENGINE TESTS ====="
	$(BUILD_DIR)/test_diagnostic_engine
	@echo
	@echo "===== SENSOR SIMULATION TESTS ====="
	$(BUILD_DIR)/test_sensor_backend_simulation

clean:
	rm -rf $(BUILD_DIR)
