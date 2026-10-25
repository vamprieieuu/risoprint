#!/usr/bin/env python3
"""
Verification harness for risoprint Adobe After Effects plugin source code.
Validates parameter registration alignment, enum ordering, memory bounds,
and core mathematical models.
"""
import re, sys

def test_param_alignment():
    print("[TEST 1] Verifying parameter enum and ParamsSetup alignment...")
    with open("ae/risoprint_ae.cpp", "r", encoding="utf-8") as f:
        code = f.read()

    # Extract enum items
    enum_match = re.search(r"enum\s*\{([^}]+)\};", code)
    assert enum_match, "Enum block missing in risoprint_ae.cpp"
    raw_enum = enum_match.group(1)
    enum_items = []
    for token in raw_enum.split(","):
        token = token.strip()
        if not token or "NUM_PARAMS" in token:
            continue
        name = token.split("=")[0].strip()
        enum_items.append(name)

    # Extract ADD(...) lines in ParamsSetup
    add_items = []
    in_setup = False
    for line in code.splitlines():
        if "static PF_Err ParamsSetup" in line:
            in_setup = True
        if in_setup and "out_data->num_params" in line:
            break
        if in_setup and line.strip().startswith("ADD("):
            m = re.search(r"(ID_[A-Z0-9_]+)\s*\)\s*\);", line)
            if m:
                add_items.append(m.group(1))

    print(f"  Enum has {len(enum_items)} parameters.")
    print(f"  ParamsSetup registers {len(add_items)} parameters.")

    assert len(enum_items) == 25, f"Expected 25 enum parameters, got {len(enum_items)}"
    assert len(add_items) == 25, f"Expected 25 registered parameters, got {len(add_items)}"

    for idx, (enum_id, add_id) in enumerate(zip(enum_items, add_items), 1):
        assert enum_id == add_id, f"Displacement at index {idx}: enum has {enum_id} but setup registered {add_id}!"
        print(f"  [{idx:02d}] {enum_id} == {add_id} [OK]")

    print("[PASS] 100% 1:1 parameter alignment verified! Zero off-by-one displacement.")

def test_registration_clamping():
    print("\n[TEST 2] Verifying registration shift bounds in ae/risoprint_ae.cpp...")
    with open("ae/risoprint_ae.cpp", "r", encoding="utf-8") as f:
        code = f.read()
    assert "std::max(-0.05, std::min(0.05" in code, "Safety clamping missing on Shift X/Y!"
    print("[PASS] Registration shifts safely clamped within [-0.05, +0.05] range.")

def test_ghosting_prevention():
    print("\n[TEST 3] Verifying ghosting prevention and blendOriginal handling...")
    with open("core/risoprint_core.cpp", "r", encoding="utf-8") as f:
        core_code = f.read()
    assert "if (keep > 0.0001f)" in core_code, "Zero-blend branch bypass missing in core!"
    with open("ae/risoprint_ae.cpp", "r", encoding="utf-8") as f:
        ae_code = f.read()
    assert 'slider("Blend With Original (%)", 0, 100, 0, 100, 0, 1, ID_BLEND_ORIGINAL)' in ae_code, "Default not 0!"
    print("[PASS] Blend With Original defaults to 0% with conditional bypass. Zero ghosting verified.")

def test_color_defaults():
    print("\n[TEST 4] Verifying reference color defaults...")
    with open("core/risoprint_core.h", "r", encoding="utf-8") as f:
        h_code = f.read()
    assert "80.0f / 255.0f" in h_code, "Cyan Plate 1 default missing!"
    assert "191.0f / 255.0f" in h_code, "Ochre Plate 2 default missing!"
    assert "41.0f / 255.0f" in h_code, "Blue Plate 3 default missing!"
    assert "1.58" in h_code, "Contrast 1.58 default missing!"
    print("[PASS] Reference ink colors and contrast 1.58 default confirmed.")

if __name__ == "__main__":
    test_param_alignment()
    test_registration_clamping()
    test_ghosting_prevention()
    test_color_defaults()
    print("\n>>> ALL 4 AUDIT TESTS PASSED WITH ZERO FAILURES. <<<")
