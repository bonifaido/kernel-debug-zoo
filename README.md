# kernel-debug-zoo

This kernel module intentionally breaks the kernel to show its debug capabilities.

There is also a [MARP](https://marp.app) based [presentation](./kernel-debug-zoo.md) about the features inside this repo.

## Usage

Clone this repo and start a distro with nice debug kernel packages, something like CentOS for example:

```bash
limactl start --name debug template://centos-stream-10 --set '.mounts[0].writable=true' --tty=false --vm-type vz
```

Setup the VM:

```bash
limactl shell debug
sudo dnf update
sudo dnf install kernel-debug kernel-debug-devel

# Check the current debug kernel version, and set it as default:

CURRENT_DEBUG_KERNEL=$(ls -1 /boot/vmlinuz-*+debug | head -1)

sudo grubby --set-default ${CURRENT_DEBUG_KERNEL}
sudo grubby --update-kernel=${CURRENT_DEBUG_KERNEL} --args kmemleak=on kasan=on kasan.multi_shot=1
sudo reboot
```


```bash
make

make insmod[-whatever-scenario]

make logs
```
