BUILD_DIR = build

.PHONY: all build run clean

all: build

build:
	@cmake -S . -B $(BUILD_DIR) -G "Ninja" > /dev/null 2>&1
	@ninja -C $(BUILD_DIR)

run: build
	@mkdir -p logs
	@./$(BUILD_DIR)/vehicle_monitor

clean:
	@rm -rf $(BUILD_DIR)
