#!/bin/sh
# Integration checks against a temporary directory, independent of the host's files.
set -eu

project=$(CDPATH= cd "$(dirname "$0")/.." && pwd)
fixture=$(mktemp -d)
trap 'rm -rf "$fixture"' EXIT HUP INT TERM

mkdir "$fixture/sub"
printf 'a' > "$fixture/a"
printf 'bbbb' > "$fixture/b"
printf 'secret' > "$fixture/.hidden"
newline_name=$(printf 'line\nbreak')
printf 'odd' > "$fixture/$newline_name"
printf 'child' > "$fixture/sub/child"
ln -s a "$fixture/link"
chmod +x "$fixture/a"
touch -t 202001010000 "$fixture/a"
touch -t 202201010000 "$fixture/b"

expect_line() {
    printf '%s\n' "$1" | grep -Fx -- "$2" >/dev/null || {
        printf 'Missing expected line: %s\n' "$2" >&2
        exit 1
    }
}

plain=$("$project/myls" "$fixture")
expect_line "$plain" a
expect_line "$plain" b
if printf '%s\n' "$plain" | grep -Fx .hidden >/dev/null; then exit 1; fi

almost=$("$project/myls" -A "$fixture")
expect_line "$almost" .hidden
if printf '%s\n' "$almost" | grep -Fx . >/dev/null; then exit 1; fi
expect_line "$("$project/myls" -q "$fixture")" 'line?break'
all=$("$project/myls" -a "$fixture")
expect_line "$all" .
expect_line "$all" ..

expect_line "$("$project/myls" -d "$fixture")" "$fixture"
classified=$("$project/myls" -F "$fixture")
expect_line "$classified" 'a*'
expect_line "$classified" 'link@'
expect_line "$classified" 'sub/'
"$project/myls" -l "$fixture" | grep 'link -> a' >/dev/null
"$project/myls" -n "$fixture" | grep -E '^[dl-][rwxstST-]{9} [0-9]+ [0-9]+ [0-9]+ ' >/dev/null
expect_line "$("$project/myls" -R "$fixture")" child

reverse=$("$project/myls" -r "$fixture")
expect_line "$reverse" b
first=$(printf '%s\n' "$reverse" | sed -n '1p')
[ "$first" = sub ]
sorted=$("$project/myls" -S "$fixture/a" "$fixture/b")
[ "$(printf '%s\n' "$sorted" | sed -n '1p')" = "$fixture/b" ]
timed=$("$project/myls" -t "$fixture/a" "$fixture/b")
[ "$(printf '%s\n' "$timed" | sed -n '1p')" = "$fixture/b" ]

block_env=$(BLOCKSIZE=1K "$project/myls" -s "$fixture/a")
block_k=$("$project/myls" -ks "$fixture/a")
[ "$block_env" = "$block_k" ]
[ "$("$project/myls" -hks "$fixture/a")" = "$block_k" ]
"$project/myls" -khs "$fixture/a" | grep -E '^[0-9.]+[BKMGTPE] ' >/dev/null
BLOCKSIZE=1G "$project/myls" -s "$fixture/a" >/dev/null
"$project/myls" -hls "$fixture/a" >/dev/null
"$project/myls" -i "$fixture/a" >/dev/null

if "$project/myls" "$fixture/missing" >/dev/null 2>&1; then
    echo 'Missing path unexpectedly succeeded' >&2
    exit 1
fi

echo 'All smoke tests passed.'
