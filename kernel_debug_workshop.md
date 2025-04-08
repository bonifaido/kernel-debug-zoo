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

# 🐧 Kernel Debugging Features
### Brown Bag Workshop Edition
#### Nandor Kracser

---

## 🔍 What This Workshop Covers

- Why kernel debugging features matter
- Core tools: `lockdep`, `KASAN`, `KMEMLEAK`, `SLUB` poisoning
- How to trigger them with demo modules
- Real logs, real bugs, no hand-waving

---

## 🔐 `lockdep`  
### Lock Dependency Validator

- Detects incorrect lock ordering
- Flags possible deadlocks
- Works best when locks are used inconsistently

```
WARNING: possible circular locking dependency detected
```

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
```
kasan=on kasan.multi_shot=1
```

⚙️ Needs: `CONFIG_KASAN=y`

---

## 🧼 `KMEMLEAK`  
### Memory Leak Detector

- Scans memory for unreachable allocations
- Reports leaks via:

```
/sys/kernel/debug/kmemleak
```

🛠 Boot param:
```
kmemleak=on
```

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
```
slub_debug=PU
```

Other flags: `F`, `Z`, `U`, `A`

---

## 💣 Page Poisoning

- Catches access to freed pages
- Poisons memory (e.g., with `0xaa`)
- Triggers Oops on access

🛠 Boot param:
```
page_poison=1
```

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

- Run `dmesg -w` while testing
- Use `pr_info()` or `trace_printk()`
- Clean up modules: `rmmod`
- Trigger bugs safely: prefer VMs

---

## 🎓 Final Words

You’re not breaking the kernel.  
You're making it **talk**.

Now let's debug something **for real**.

---

## 💬 Questions?

Let’s go break stuff. 😈
