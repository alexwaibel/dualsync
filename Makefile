DOCKER ?= docker

DSI_IMAGE := dualsync-dsi-dev
DSI_DOCKERFILE := containers/dsi/Dockerfile
TOOLS_IMAGE := dualsync-tools-dev
TOOLS_DOCKERFILE := containers/tools/Dockerfile
DOCKER_USER := --user $(shell id -u):$(shell id -g)
DOCKER_WORKSPACE := -v $(CURDIR):/work -w /work
DSI_BUILD_DIR := build/obj/dsi
DSI_DIST_DIR := build/dist/dsi
CORE_TEST_BUILD_DIR := build/obj/tests
C_SOURCES := $(shell find apps core -type f \( -name '*.c' -o -name '*.h' \) | sort)

.PHONY: all check clean dsi dsi-image format format-check lint test tools-image

all: dsi

check: format-check lint test dsi

dsi: dsi-image
	@mkdir -p $(DSI_BUILD_DIR) $(DSI_DIST_DIR)
	$(DOCKER) run --rm $(DOCKER_USER) $(DOCKER_WORKSPACE) \
		$(DSI_IMAGE) \
		make -C $(DSI_BUILD_DIR) -f ../../../apps/dsi/Makefile
	cp $(DSI_BUILD_DIR)/dualsync-dsi.nds \
		$(DSI_DIST_DIR)/dualsync-dsi.nds

dsi-image:
	$(DOCKER) build -f $(DSI_DOCKERFILE) -t $(DSI_IMAGE) .

format: tools-image
	$(DOCKER) run --rm $(DOCKER_USER) $(DOCKER_WORKSPACE) \
		$(TOOLS_IMAGE) \
		clang-format -i $(C_SOURCES)

format-check: tools-image
	$(DOCKER) run --rm $(DOCKER_USER) $(DOCKER_WORKSPACE) \
		$(TOOLS_IMAGE) \
		clang-format --dry-run --Werror $(C_SOURCES)

lint: tools-image
	$(DOCKER) run --rm $(DOCKER_USER) $(DOCKER_WORKSPACE) \
		$(TOOLS_IMAGE) \
		cppcheck --enable=warning,style,performance,portability \
			--error-exitcode=1 --inline-suppr \
			--std=c11 --suppress=missingIncludeSystem \
			-Icore/include apps/dsi/source core/source core/tests

test: tools-image
	@mkdir -p $(CORE_TEST_BUILD_DIR)
	$(DOCKER) run --rm $(DOCKER_USER) $(DOCKER_WORKSPACE) \
		$(TOOLS_IMAGE) \
		make -C $(CORE_TEST_BUILD_DIR) -f ../../../core/tests/Makefile \
			CC=clang

tools-image:
	$(DOCKER) build -f $(TOOLS_DOCKERFILE) -t $(TOOLS_IMAGE) .

clean:
	rm -rf build
