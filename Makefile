BUILD_DIR = build

.PHONY: all build run clean

all: build

build:
	@cmake -S . -B $(BUILD_DIR) -G "Ninja" > /dev/null 2>&1
	@ninja -C $(BUILD_DIR)
	@mkdir -p logs
	@> logs/vehicle_log.txt
	@> logs/snapshots.txt

run: build
	@mkdir -p logs
	@mkdir -p tests
	@cd "$(CURDIR)" && ./$(BUILD_DIR)/vehicle_monitor

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf logs