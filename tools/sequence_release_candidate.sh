#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# Validate and package sequencing-v1.3 for a local FM-1 installation.
# This script never modifies the connected device or publishes a release.
set -eu
cd "$(dirname "$0")/.."

VERSION=1.3-rc1
PACKAGE="build/niagara-drift-$VERSION.fwsc"
SITE=build/sequence-rc-site
JIELI_TOOLCHAIN="${JIELI_TOOLCHAIN:-$HOME/.jieli/toolchain}"
AC79_SDK="${AC79_SDK:-$HOME/fw-AC79_AIoT_SDK}"
export JIELI_TOOLCHAIN AC79_SDK

case "$(git branch --show-current)" in
    main|sequencing-v1.3) ;;
    *) echo "FAIL: build the v1.3 candidate from main or sequencing-v1.3" >&2; exit 1 ;;
esac
[ -z "$(git status --porcelain --untracked-files=no)" ] || {
    echo "FAIL: tracked source changes are not committed" >&2
    exit 1
}
[ -x "$JIELI_TOOLCHAIN/pi32v2/bin/clang" ] || {
    echo "FAIL: missing JieLi toolchain at $JIELI_TOOLCHAIN" >&2
    exit 1
}
[ -f "$AC79_SDK/cpu/wl82/tools/uboot.boot" ] || {
    echo "FAIL: missing AC79 SDK at $AC79_SDK" >&2
    exit 1
}
command -v emcc >/dev/null || {
    echo "FAIL: activate emsdk first: source ~/emsdk/emsdk_env.sh" >&2
    exit 1
}
command -v node >/dev/null || {
    echo "FAIL: Node.js required for browser emulator test" >&2
    exit 1
}

echo "Source: $(git rev-parse HEAD)"
echo "1/5  Building physical FM-1 firmware ..."
# A failed build must never leave a previously generated package eligible
# for the subsequent tests or installation.
rm -f "$PACKAGE" build/omni.fwsc
./build.sh --release "$VERSION"
[ -s "$PACKAGE" ] || {
    echo "FAIL: firmware package missing or empty" >&2
    exit 1
}
# The OTA/loader tests look for the conventional build/omni.fwsc name.
cp "$PACKAGE" build/omni.fwsc

echo "2/5  Checking the package identity and loader ..."
python3 - "$PACKAGE" <<'PY'
from pathlib import Path
import hashlib
import sys
sys.path.insert(0, "tools")
from fm1_install import product_of
p = Path(sys.argv[1])
raw = p.read_bytes()
identity = product_of(raw)
assert identity == "FM-1_8010300", f"unexpected FM-1 identity: {identity}"
assert b"FELUCCA-LOADER-1" in raw, "missing update loader"
assert raw == Path("build/omni.fwsc").read_bytes(), "OTA test image differs from release"
print(f"  verified {identity}, {len(raw)} bytes")
print("  SHA-256:", hashlib.sha256(raw).hexdigest())
PY

echo "3/5  Running the complete host, OTA, emulator and scenario tests ..."
CC="${CC:-cc -D_DEFAULT_SOURCE -Wl,--no-as-needed -lm}" sh tests/run_tests.sh

echo "4/5  Preparing a versioned local emulator and USB-MIDI installer ..."
OM_VERSION="$VERSION" sh web/emu/build.sh
python3 tools/make_pages.py "$PACKAGE" "$VERSION" "$SITE"

echo "5/5  Release candidate ready. Device has NOT been flashed."
echo "  Package: $PACKAGE"
echo "  Local installer: $SITE/install/index.html"
echo "  Serve it with:"
echo "      python3 -m http.server 8765 --directory $SITE"
echo "  Then open http://localhost:8765/install/ in Windows Chrome or Edge."
echo "  Test on the physical FM-1 before merging PR #2 into main."
