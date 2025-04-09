# kernel-debug-zoo

This kernel module intentionally breaks the kernel to show its debug capabilities.

## Usage

Clone this repo and start a distro with nice debug kernel packages, something like Rocky Linux for example:

```bash
limactl start --name rocky template://rocky --set '.mounts[0].writable=true' --tty=false
```

```bash
limactl shell rocky

make

make insmod[-whatever-scenario]

make logs
```