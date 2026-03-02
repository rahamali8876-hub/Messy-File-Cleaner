🧱 Step 1 — Define ABI Philosophy

For a stable ABI:

1️⃣ No exposed structs with OS types
2️⃣ No inline functions in headers
3️⃣ No macro-based behavior
4️⃣ No struct size dependency across boundary
5️⃣ All ownership rules explicit
6️⃣ Only fixed-width types (stdint.h)
7️⃣ Versioned entry point


Clean Dependency Direction
Core → System → Platform ABI → OS implementation