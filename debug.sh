#!/bin/bash

tmux kill-session -t foo
tmux new -s foo '/home/josef/qemu6/qemu-6.1.0/build/aarch64-softmmu/qemu-system-aarch64 -M raspi3 -cpu cortex-a53 -smp 4 -gdb tcp::1236 -S -m 1G -serial null -serial mon:stdio -nographic -no-reboot -device loader,file=img/hello.img,addr=0x001000000,force-raw=on -kernel img/hello.bin | tee img/hello.out & 
konsole -e aarch64-elf-gdb -ex "target remote:1236" -ex "set confirm off" -ex "add-symbol-file /home/josef/Desktop/INE5424/app/hello/hello" \
-ex "b *0x0080000" \
-ex "c" \
-ex "layout split"'