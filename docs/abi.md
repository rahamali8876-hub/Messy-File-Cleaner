🧠 Step 0 — Understand the Mission

Your platform_api is:

system  →  platform

It is the binary boundary between:

Infrastructure layer

OS implementation layer

If tomorrow you:

Replace Windows backend

Add POSIX backend

Ship platform as DLL

Add plugin OS backends

Your ABI must NOT break.

So we design for:

Stability

Versioning

Forward compatibility

Backward compatibility

Extensibility

Zero struct layout breakage

🏛 Step 1 — Freeze Design Philosophy

A long-term ABI must follow:

1️⃣ C only

No C++.
No inline functions.
No templates.
No compiler extensions.

Why?

C has the most stable cross-compiler ABI.

2️⃣ Opaque Everything

Never expose internal structs.

Bad:

typedef struct {
    HANDLE h;
    int flags;
} cleaner_file;

Good:

typedef struct cleaner_file cleaner_file;

Struct defined privately in .c.

3️⃣ No Struct Growth in the Middle

Struct layout must never change order.

You can only:

Append fields at the end

Or use reserved padding

🏗 Step 2 — Design the ABI Structure

This is your ABI nucleus.

# ifndef CLEANER_PLATFORM_API_H
# define CLEANER_PLATFORM_API_H

# include <stdint.h>

# ifdef __cplusplus
extern "C" {
# endif

# define CLEANER_PLATFORM_ABI_VERSION 1

typedef struct cleaner_platform_api cleaner_platform_api;

typedef struct cleaner_platform_api {
    /*===== ABI Identity =====*/

    uint32_t abi_version;        /* must equal CLEANER_PLATFORM_ABI_VERSION */
    uint32_t struct_size;        /* sizeof(cleaner_platform_api) */

    /* ===== Filesystem ===== */

    int (*fs_mkdir)(const char* path);
    int (*fs_remove)(const char* path);
    int (*fs_rename)(const char* oldp, const char* newp);

    /* ===== Threads ===== */

    int (*thread_create)(void* (*fn)(void*), void* arg);
    void (*thread_sleep_ms)(uint32_t ms);

    /* ===== Time ===== */

    uint64_t (*time_now_ms)(void);

    /* ===== Reserved for Future Expansion ===== */

    void* reserved[16];

} cleaner_platform_api;

/* Single entrypoint */
const cleaner_platform_api* cleaner_platform_get_api(void);

# ifdef __cplusplus
}
# endif

# endif

This is now a real ABI contract.

🧱 Step 3 — Why Each Field Exists
abi_version

Prevents loading incompatible ABI.

System layer checks:

if (api->abi_version != CLEANER_PLATFORM_ABI_VERSION)
    abort();
struct_size

Allows forward compatibility.

If v2 adds fields at the end:

System compiled with v1 will still work
Because it checks:

if (api->struct_size < sizeof(cleaner_platform_api_v1))
    abort();

This is how large systems evolve safely.

reserved[16]

Future-proof expansion space.

Never remove.
Never reorder.
Never reuse until needed.

🖥 Step 4 — Implement platform_win.c Correctly
# include "cleaner/platform/platform_api.h"
# include <windows.h>

static int win_fs_mkdir(const char* path)
{
    return CreateDirectoryA(path, NULL) ? 0 : -1;
}

static int win_fs_remove(const char* path)
{
    return DeleteFileA(path) ? 0 : -1;
}

static int win_fs_rename(const char*oldp, const char* newp)
{
    return MoveFileA(oldp, newp) ? 0 : -1;
}

static uint64_t win_time_now_ms(void)
{
    return GetTickCount64();
}

/*Static immutable ABI table*/
static const cleaner_platform_api g_api = {
    .abi_version = CLEANER_PLATFORM_ABI_VERSION,
    .struct_size = sizeof(cleaner_platform_api),

    .fs_mkdir = win_fs_mkdir,
    .fs_remove = win_fs_remove,
    .fs_rename = win_fs_rename,

    .thread_create = NULL,
    .thread_sleep_ms = NULL,

    .time_now_ms = win_time_now_ms,

    .reserved = {0}
};

const cleaner_platform_api* cleaner_platform_get_api(void)
{
    return &g_api;
}

Notice:

static const

No global mutable state

No exported struct directly

Only exported getter

This is ABI-clean.

🔐 Step 5 — How System Layer Uses It Safely
const cleaner_platform_api* api = cleaner_platform_get_api();

if (!api ||
    api->abi_version != CLEANER_PLATFORM_ABI_VERSION ||
    api->struct_size < sizeof(cleaner_platform_api))
{
    abort();
}

api->fs_mkdir("test");

Now you have runtime ABI validation.

Enterprise-grade move.

🧨 Step 6 — What You Must NEVER Do

❌ Reorder struct fields
❌ Remove fields
❌ Change function signatures
❌ Change calling convention
❌ Add fields in the middle
❌ Expose Windows types in header
❌ Inline function bodies in header

📈 Step 7 — How To Evolve ABI in v2

Add at END:

/* new in v2 */
int (*fs_copy)(const char* src, const char* dst);

Increase:

# define CLEANER_PLATFORM_ABI_VERSION 2

Old system code:

Will detect mismatch and refuse to run

OR

You support backward compatibility by:

Keeping version 1 table too

This is how serious systems evolve.

🏢 Real World Parallels

This pattern is used by:

Microsoft Windows COM

The Khronos Group Vulkan loader

Mozilla Foundation Firefox plugin API

Oracle Corporation JVM native interface

All use:

Struct tables

Version fields

Size fields

Reserved space

🧠 Step 8 — Extra Principal-Level Hardening
Add Feature Flags
uint64_t feature_flags;

Example:

bit 0 = supports_symlink
bit 1 = supports_atomic_rename

Now system can branch safely.

Add Capability Query Function
int (*query_capability)(int cap_id);

Even more flexible.

🏆 Step 9 — File Placement (Architectural Correctness)

platform_api.h must live in:

include/cleaner/platform/

Why?

Because:

It is public contract between layers

It is NOT internal

It is NOT domain

platform_win.c lives in:

src/platform/

Never inside internal/.

Because:

internal = private implementation helpers
platform_api = cross-layer contract

🎯 Final Mental Model

Your platform_api is now:

A binary constitution

A frozen contract

A versioned capability table

Runtime validated

Forward compatible

Backward detectable

Enterprise stable

🧠 Ultimate Principle

If API is what developers promise,

ABI is what compiled reality enforces.

You just designed a production-grade ABI.
