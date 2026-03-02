# CODE REVIEW CHECKLIST

This checklist is mandatory for every pull request.

If any rule fails, the PR must not be merged.

---

# 1. LAYER VALIDATION

## Core Layer

    * [ ] No OS headers included
    * [ ] No platform headers included
    * [ ] No system headers included
    * [ ] No filesystem operations
    * [ ] No threading primitives
    * [ ] Deterministic logic only
    * [ ] Unit tests added or updated

    If any item fails → REJECT PR.

---

## System Layer

    * [ ] No OS headers included
    * [ ] No direct OS API calls
    * [ ] Only uses platform abstractions
    * [ ] No business/domain logic
    * [ ] Clear orchestration responsibility
    * [ ] Thread safety handled via platform APIs

    If system code includes windows.h or similar → REJECT IMMEDIATELY.

---

## Platform Layer

    * [ ] Contains OS interaction only
    * [ ] No business logic
    * [ ] No scheduling policy
    * [ ] No metrics or logging policy
    * [ ] Interface is OS-agnostic
    * [ ] No Core or System dependency

    If platform depends on higher layer → REJECT.

---

# 2. DEPENDENCY DIRECTION

    Validate dependency graph:

    Core → (nothing)
    System → Core, Platform
    Platform → OS

    No reverse dependency allowed.

---

# 3. THREAD SAFETY

    * [ ] Shared state protected
    * [ ] No data races
    * [ ] Mutex usage via platform abstraction
    * [ ] No global mutable state in Core

---

# 4. API DESIGN

    * [ ] Headers are minimal
    * [ ] No OS types exposed in public headers
    * [ ] Function names follow cleaner_* naming convention
    * [ ] Clear ownership rules for memory

---

# 5. TESTING

    * [ ] Core fully unit-testable
    * [ ] Platform functions tested per OS
    * [ ] No hidden side effects

---

# FINAL RULE

    If architecture boundaries are violated, functionality does not matter.
    Architecture correctness has higher priority than feature completion.
