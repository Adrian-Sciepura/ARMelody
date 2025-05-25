#!/bin/bash
qemu-aarch64 -cpu max ./build/app.elf $@
