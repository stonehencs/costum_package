# costum_package
costum app and modification for breadbee buildroot.
# check if .img file contains update before writing to chip
first use binwalk to detect sizes
binwalk file.img

carve out:
dd if=nor-16.img of=a.bin bs=1 skip=<offset> count=<img size>
then unsquashfs a.bin

putback:
mksquashfs <dir name> newrootfs.bin -comp xz
dd if=newrootfs.bin of=<out file.bin> bs=1 seek=<offset> count=<img size> conv=notrunc 


