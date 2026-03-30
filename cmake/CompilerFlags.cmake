# =============================================================================
# Wendfyr — Compiler Warnings Configuration
# =============================================================================


add_library(wf_warnings INTERFACE)

target_compile_options(wf_warnings INTERFACE

    # ── MSVC (Visual Studio) ─────────────────────────────────────────────
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4         # Warning level 4 (highest practical level)
                    # Catches: unused variables, signed/unsigned mismatch,
                    # narrowing conversions, unreachable code, and more.

        /w14242     # Possible loss of data in conversion (int → short)
        /w14254     # Operator precedence issues (a + b & c)
        /w14263     # Member function hides base class virtual function
        /w14265     # Class has virtual functions but no virtual destructor
        /w14287     # Unsigned/negative constant mismatch
        /w14296     # Expression is always true/false
        /w14311     # Pointer truncation (64-bit → 32-bit)
        /w14545     # Expression before comma has no effect
        /w14546     # Function call before comma has no effect
        /w14547     # Operator before comma has no effect
        /w14549     # Operator before comma has no effect (variant)
        /w14555     # Expression has no effect, side effect expected
        /w14619     # Unknown pragma warning
        /w14640     # Thread-unsafe static local initialization
        /w14826     # Signed/unsigned conversion with data loss
        /w14905     # Wide string literal cast
        /w14906     # String literal cast
        /w14928     # Illegal copy-initialization (multiple conversions)

        /permissive- # Strict standards conformance — disables MSVC
                     # "extensions" that break on other compilers.
                     # Without this, code compiles on MSVC but fails
                     # on GCC/Clang. Essential for cross-platform teams.

        /WX         # Treat warnings as errors. No warning goes unfixed.
    >

    # ── GCC and Clang ────────────────────────────────────────────────────
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Wall           # "All" common warnings (misleading name — it's
                        # not actually all of them, hence the extras below)

        -Wextra         # Even more warnings beyond -Wall

        -Wpedantic      # Strict ISO C++ compliance — rejects compiler
                        # extensions. Ensures our code is portable across
                        # GCC, Clang, and MSVC.

        -Wshadow        # A local variable shadows an outer variable.
                        # Example: void foo(int x) { int x = 5; }
                        # This is a common source of subtle bugs.

        -Wnon-virtual-dtor  # A class with virtual functions but no
                             # virtual destructor. Critical for our
                             # interface classes (ICommand, etc.) —
                             # without virtual dtor, deleting through a
                             # base pointer causes undefined behavior.

        -Wold-style-cast    # C-style casts like (int)x instead of
                            # static_cast<int>(x). C++ casts are safer
                            # because they're explicit about intent
                            # and searchable in code.

        -Wcast-align    # Casting to a type with stricter alignment.
                        # Can cause crashes on ARM architectures.

        -Wunused        # Any unused variable, function, or parameter.
                        # Dead code is confusing and should be removed.

        -Woverloaded-virtual  # A derived class function hides a base
                              # class virtual function (different signature).
                              # Violates LISKOV SUBSTITUTION — the derived
                              # class is no longer substitutable for the base.

        -Wconversion    # Implicit type conversions that may lose data.
                        # Example: int x = 3.14; (double → int silently)
                        # Forces you to be explicit about your intent.

        -Wsign-conversion   # Implicit signed ↔ unsigned conversions.
                            # These are a notorious source of bugs:
                            # if (unsigned_val < -1) is always false.

        -Wnull-dereference  # Warn when compiler detects a null pointer
                            # dereference path. Catches crashes early.

        -Wdouble-promotion  # Float silently promoted to double in
                            # arithmetic. Matters for performance-critical
                            # code and embedded systems.

        -Wformat=2      # Stricter printf/scanf format string checking.
                        # Catches format string vulnerabilities.

        -Wimplicit-fallthrough  # Switch case falls through without a
                                # [[fallthrough]] attribute. Forces you
                                # to be explicit about intentional fallthrough.

        -Werror         # Treat all warnings as errors.
                        # In a 50-person team, this is non-negotiable.
                        # Without it, warnings accumulate until nobody
                        # reads them, and real bugs hide in the noise.
    >

    # ── GCC-only extras ──────────────────────────────────────────────────
    $<$<CXX_COMPILER_ID:GNU>:
        -Wmisleading-indentation  # Indentation suggests a block, but
                                  # there's no block. The "goto fail"
                                  # Apple SSL bug was exactly this.

        -Wduplicated-cond    # if (a) ... else if (a) — the second
                             # condition is identical, probably a bug.

        -Wduplicated-branches  # if (a) { x; } else { x; } — both
                               # branches do the same thing.

        -Wlogical-op         # Suspicious use of logical operators.
                             # Example: (a && a) is always just (a).

        -Wuseless-cast       # Casting to the same type. Pointless and
                             # clutters the code.
    >
)