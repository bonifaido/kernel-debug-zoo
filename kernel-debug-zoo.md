---
marp: true
theme: default
paginate: true
style: |
  section {
    font-family: monospace;
    background: white;
    color: black;
  }
---

# 🐧 Kernel Debug Zoo
### Workshop Edition
#### Nandor Kracser

https://github.com/bonifaido/kernel-debug-zoo

---

## 🧠 What Is Kernel Debugging?

- Debugging the **Linux kernel** is very different from userspace:
  - No full stack traces by default
  - Panics can crash the system
  - Subtle bugs (e.g., race conditions) hard to reproduce
- Needs special tooling and awareness

🎯 Goal: Make the kernel **observable** and **breakable**, safely

---

## 🔍 What This Workshop Covers

- Why kernel debugging features matter
- Core tools: `lockdep`, `KASAN`, `KMEMLEAK`, `SLUB`, poisoning
- How to trigger them with demo modules
- Real logs, real bugs, no hand-waving

---

## 🔐 `lockdep`  
### Lock Dependency Validator

- Detects incorrect lock ordering
- Flags possible deadlocks
- Works best when locks are used inconsistently

WARNING: possible circular locking dependency detected

Always show details


✅ Usually enabled  
🛠 Boot param: `lockdep`

---

## 🔥 `KASAN`  
### Kernel Address Sanitizer

- Detects:
  - Use-after-free
  - Out-of-bounds accesses
- Reports with stack trace

🛠 Boot params:
kasan=on kasan.multi_shot=1

Always show details


⚙️ Needs: `CONFIG_KASAN=y`

---

## 🧼 `KMEMLEAK`  
### Memory Leak Detector

- Scans memory for unreachable allocations
- Reports leaks via:

/sys/kernel/debug/kmemleak

Always show details


🛠 Boot param:
kmemleak=on

Always show details


Enable with: `CONFIG_DEBUG_KMEMLEAK`

---

## 🧨 `SLUB` Debugging  
### Heap Hardening

- Detects:
  - Redzone overflows
  - Double frees
  - Use-after-free
- Works with `kmalloc()`

🛠 Boot param:
slub_debug=PU

Always show details


Other flags: `F`, `Z`, `U`, `A`

---

## 💣 Page Poisoning

- Catches access to freed pages
- Poisons memory (e.g., with `0xaa`)
- Triggers Oops on access

🛠 Boot param:
page_poison=1

Always show details


Needs: `CONFIG_PAGE_POISONING`

---

## ⚔️ Race Condition Demo

- 2 threads update shared var without lock
- Random outputs = race
- `dmesg` might not show it unless it breaks something
- Add manual checks or log divergence

---

## ✅ Summary Table

| Feature      | Catches            | Boot Param        | Kernel Config             |
|--------------|--------------------|--------------------|----------------------------|
| `lockdep`    | Deadlocks          | `lockdep`          | `CONFIG_LOCKDEP`          |
| `KASAN`      | UAF, OOB           | `kasan=on`         | `CONFIG_KASAN`            |
| `KMEMLEAK`   | Leaks              | `kmemleak=on`      | `CONFIG_DEBUG_KMEMLEAK`   |
| `SLUB`       | Heap overflows     | `slub_debug=PU`    | `CONFIG_SLUB_DEBUG_ON`    |
| Page Poison  | Freed-page access  | `page_poison=1`    | `CONFIG_PAGE_POISONING`   |

---

## 🧪 Tips for Demos

- Trigger bugs safely: prefer VMs
- Run `dmesg -w` while testing
- Use `pr_info()` or `trace_printk()`
- Clean up modules: `rmmod`


---

## 🛠 Enabling These Features

### Option 1: Use a Prebuilt Debug Kernel

- Fedora/CentOS:
sudo dnf install kernel-debug sudo grubby --set-default /boot/vmlinuz-<debug-version>

Always show details

- Comes with most debug configs: `KASAN`, `lockdep`, `SLUB_DEBUG`, etc.

---

### Option 2: Build a Custom Kernel

- Set config options in `make menuconfig`:
- `CONFIG_KASAN=y`
- `CONFIG_DEBUG_KMEMLEAK=y`
- `CONFIG_LOCKDEP_SUPPORT=y`
- `CONFIG_SLUB_DEBUG_ON=y`
- `CONFIG_PAGE_POISONING=y`
- Add boot params in GRUB:
kasan=on slub_debug=PU kmemleak=on page_poison=1

Always show details


---

## 🦀 Can Rust Help?

### Yes — a lot.

- Rust prevents **entire classes** of memory bugs:
- Use-after-free
- Buffer overflows
- Null pointer deref
- Enforced at compile-time

---

### 🔐 Rust + Kernel = Safer Drivers

- Rust kernel modules have:
- **Ownership model** for memory safety
- Safe concurrency via `Send` / `Sync`
- No `unsafe` code unless explicitly declared

💡 Still early-stage, but promising for:
- Drivers
- Filesystems
- Net modules

---

## Rust vs C (for Kernel Safety)

| Bug Type           | C Kernel | Rust Kernel |
|--------------------|----------|-------------|
| Use-after-free     | ✅       | 🚫 (safe by default) |
| Buffer overflow    | ✅       | 🚫          |
| Null deref         | ✅       | 🚫          |
| Data races         | ✅       | 🚫 (unless `unsafe`) |

---

## 🎓 Final Words

You’re not breaking the kernel.  
You're making it **talk**.

Now let's debug something **for real**.

---

## 💬 Questions?

Let’s go break stuff. 😈
