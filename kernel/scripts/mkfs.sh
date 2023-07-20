#!/bin/bash

# Build directory structure;
BUILD_DIR=$PWD/build
FS=$BUILD_DIR/fs

mkdir -p $FS

# Place empty image;
cp $PWD/scripts/empty_fs.img $BUILD_DIR/fs.img

# Attach to loopback device;
LOOPBACK_DEVICE=$(losetup --partscan --show --find $BUILD_DIR/fs.img)
LOOPBACK_DEVICE_PART="${LOOPBACK_DEVICE}p1"

# Copy the kernel image to it;
mount $LOOPBACK_DEVICE_PART $BUILD_DIR/fs

mkdir $BUILD_DIR/fs/boot
cp $BUILD_DIR/kernel.elf $BUILD_DIR/fs/boot/kernel.elf
cp $PWD/scripts/boot.scr $BUILD_DIR/fs/boot.scr

echo "Wait for FS to process"
sleep 1s

umount $BUILD_DIR/fs

# Detach;
losetup -d $LOOPBACK_DEVICE

# Change permission;
chmod 466 $BUILD_DIR/fs.img