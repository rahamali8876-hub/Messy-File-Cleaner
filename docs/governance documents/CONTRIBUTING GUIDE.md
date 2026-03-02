# CONTRIBUTING GUIDE

    Welcome to the Cleaner project.

    This project enforces strict architectural boundaries. Every contributor must follow the layer rules defined in:

    * CORE_RULES.md
    * SYSTEM_RULES.md
    * PLATFORM_RULES.md
     ### Governance documents are located in docs.
            * CODE_REVIEW_CHECKLIST.md
            * ARCHITECTURE_DIAGRAM.md

    Failure to follow these rules will result in PR rejection.

---

# 1. Before Writing Code

    Ask yourself:

    * Is this domain logic? → Core
    * Is this orchestration? → System
    * Is this OS-specific? → Platform

    Do not start coding until placement is clear.

---

# 2. Pull Request Requirements

    Every PR must:

    * Clearly state which layer is modified
    * Justify architectural placement
    * Confirm dependency direction is preserved
    * Include relevant tests
    * Pass code review checklist

    PR description must include:

    "Layer: [Core/System/Platform]"
    "No architecture rules violated."

---

# 3. Prohibited Actions

    * Adding OS headers outside Platform
    * Adding platform includes inside Core
    * Moving business logic into Platform
    * Mixing scheduling policy with OS wrappers
    * Introducing circular dependencies

    These will be rejected without discussion.

---

# 4. Naming Conventions

    Platform functions must use prefix:

    cleaner_fs_*
    cleaner_thread_*
    cleaner_mutex_*
    cleaner_time_*

    System functions must use:

    threadpool_*
    logger_*
    metrics_*

    Core functions must represent domain behavior.

---

# 5. Testing Policy

    * Core must be unit-testable independently
    * Platform must compile for each supported OS
    * System must not depend on OS directly

---

# 6. Architectural Authority

    If a conflict arises between feature implementation and architecture rules:

    Architecture rules win.

---

# Final Statement

    This project values long-term maintainability over short-term convenience.

    Respect the layers.
    Protect the boundaries.
    Build systems that scale.
