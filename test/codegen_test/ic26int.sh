#!/usr/bin/env bash
# Spousti referencni interpret ic26int (stejne argumenty, stdin i navratovy kod).
#
# Interpret je prelozeny proti glibc 2.38 a libstdc++ z GCC 13+. Na starsim systemu
# (napr. Debian 12 ve WSL) se nespusti, proto si skript jednou stahne knihovny z balicku
# Debianu 13 do build/ic26int-runtime a spusti interpret pres jejich zavadec. Do systemu
# se nic neinstaluje.
#
# Cesta k interpretu: promenna IC26INT_BIN, jinak ic26int-linux-x86_64 v koreni projektu.

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BIN="${IC26INT_BIN:-$ROOT/ic26int-linux-x86_64}"
RUNTIME="$ROOT/build/ic26int-runtime"
MIRROR="https://deb.debian.org/debian/pool/main"
PACKAGES=(
	"g/glibc/libc6_2.41-12+deb13u5_amd64.deb"
	"g/gcc-14/libstdc++6_14.2.0-19_amd64.deb"
	"g/gcc-14/libgcc-s1_14.2.0-19_amd64.deb"
)

if [ ! -x "$BIN" ]; then
	echo "ic26int.sh: interpret '$BIN' neexistuje nebo neni spustitelny" >&2
	exit 50
fi

# Novejsi system: interpret pobezi primo
if "$BIN" --help >/dev/null 2>&1; then
	exec "$BIN" "$@"
fi

# Debian 13 ma sloucene /usr: knihovny i zavadec jsou v usr/lib/x86_64-linux-gnu
LIBDIR="$RUNTIME/usr/lib/x86_64-linux-gnu"
LOADER="$LIBDIR/ld-linux-x86-64.so.2"
if [ ! -x "$LOADER" ]; then
	mkdir -p "$RUNTIME"
	# Zamek: runner muze spustit vic instanci najednou
	exec 9>"$RUNTIME/.lock"
	flock 9
	if [ ! -x "$LOADER" ]; then
		echo "ic26int.sh: stahuji knihovny glibc 2.41 a libstdc++ 14 do $RUNTIME (jednorazove)" >&2
		for package in "${PACKAGES[@]}"; do
			deb="$RUNTIME/$(basename "$package")"
			curl -fsSL "$MIRROR/$package" -o "$deb"
			dpkg-deb -x "$deb" "$RUNTIME"
			rm -f "$deb"
		done
	fi
	flock -u 9
fi

exec "$LOADER" --library-path "$LIBDIR" "$BIN" "$@"
