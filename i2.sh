#-rtc base=localtime \
cd ~/build &&
  nvim ../main.c &&
  make &&
  mkdir -p ~/efidir/EFI/BOOT &&
  cp ~/build/BOOTX64.EFI ~/efidir/EFI/BOOT/ &&
  qemu-system-x86_64 \
    -bios /usr/share/ovmf/OVMF.fd \
    -drive file=fat:rw:${HOME}/efidir,format=raw \
    -serial stdio \
    -vga virtio \
    -audiodev pa,id=audio0 \
    -machine pcspk-audiodev=audio0
