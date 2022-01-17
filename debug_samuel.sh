#!/bin/bash

tmux kill-session -t foo
tmux new -s foo '/home/garthu/Documentos/toolchain/qemu/bin/qemu-system-aarch64 -M raspi3 -cpu cortex-a53 -smp 4 -gdb tcp::1236 -S -m 1G -serial null -serial mon:stdio -nographic -no-reboot -device loader,file=img/hello.img,addr=0x000800000,force-raw=on -kernel img/hello.bin | tee img/hello.out &
konsole -e /home/garthu/Documentos/toolchain/gdb/bin/aarch64-elf-gdb --tui -ex "target remote:1236" -ex "set confirm off" \
-ex "add-symbol-file /home/garthu/Documentos/P1/INE5424/app/hello/hello" \
-ex "add-symbol-file /home/garthu/Documentos/P1/INE5424/img/setup_raspberry_pi3" \
-ex "add-symbol-file /home/garthu/Documentos/P1/INE5424/img/system_raspberry_pi3" \
-ex "add-symbol-file /home/garthu/Documentos/P1/INE5424/img/init_raspberry_pi3" \
-ex "b EPOS::S::Setup::build_lm" \
-ex "c" \
-ex "layout split"'


#-ex "b *0x0080000" \
#EPOS::S::Setup::Setup