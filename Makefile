DOCKER ?= docker

DSI_IMAGE := dualsync-dsi-dev
DSI_DOCKERFILE := containers/dsi/Dockerfile
CORE_TEST_IMAGE := gcc:14-bookworm
DOCKER_USER := --user $(shell id -u):$(shell id -g)
DOCKER_WORKSPACE := -v $(CURDIR):/work -w /work
DSI_BUILD_DIR := build/obj/dsi
DSI_DIST_DIR := build/dist/dsi
CORE_TEST_BUILD_DIR := build/obj/tests

.PHONY: all dsi dsi-image test clean

all: dsi

dsi: dsi-image
	@mkdir -p $(DSI_BUILD_DIR) $(DSI_DIST_DIR)
	$(DOCKER) run --rm $(DOCKER_USER) $(DOCKER_WORKSPACE) \
		$(DSI_IMAGE) \
		make -C $(DSI_BUILD_DIR) -f ../../../apps/dsi/Makefile
	cp $(DSI_BUILD_DIR)/dualsync-dsi.nds \
		$(DSI_DIST_DIR)/dualsync-dsi.nds

dsi-image:
	$(DOCKER) build -f $(DSI_DOCKERFILE) -t $(DSI_IMAGE) .

test:
	@mkdir -p $(CORE_TEST_BUILD_DIR)
	$(DOCKER) run --rm $(DOCKER_USER) $(DOCKER_WORKSPACE) \
		$(CORE_TEST_IMAGE) \
		make -C $(CORE_TEST_BUILD_DIR) -f ../../../core/tests/Makefile

clean:
	rm -rf build
