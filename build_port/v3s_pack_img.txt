sudo mkfs.jffs2 -s 0x100 -e 0x10000 -p 0x1AF0000 -d ./buildroot-2018.08.2/output/images/rootfs -o jffs2.img && echo "done"

dd if=/dev/zero of=flashimg.bin bs=1M count=32 && echo "done"
#dd if=/dev/zero bs=1M count=32 | tr "\000" "\377" > flashimg.bin && echo "done"

dd if=u-boot-spi/u-boot-sunxi-with-spl.bin of=flashimg.bin bs=1K conv=notrunc && echo "done"

dd if=zero-4.13.y/arch/arm/boot/dts/sun8i-v3s-licheepi-zero-dock.dtb of=flashimg.bin bs=1K seek=1024  conv=notrunc && echo "done"

dd if=zero-4.13.y/arch/arm/boot/zImage of=flashimg.bin bs=1K seek=1088  conv=notrunc && echo "done"

dd if=jffs2.img of=flashimg.bin  bs=1K seek=5184 conv=notrunc && echo "done"