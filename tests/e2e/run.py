#!/usr/bin/env python3
import argparse, difflib, subprocess, sys
from pathlib import Path

def color(s,c):
    if not sys.stdout.isatty(): return s
    m={"red":"\x1b[31m","green":"\x1b[32m","yellow":"\x1b[33m","dim":"\x1b[2m","reset":"\x1b[0m"}
    return f"{m[c]}{s}{m['reset']}"

def normalize(t:str)->str: return t.replace("\r","")

def resolve_from_cwd(p:str|Path)->Path:
    p=Path(p)
    return (Path.cwd()/p).resolve() if not p.is_absolute() else p.resolve()

def default_dirs(script_dir:Path):
    return script_dir/"tests", script_dir/"keys"

def main():
    script_dir = Path(__file__).resolve().parent
    d_tests, d_keys = default_dirs(script_dir)

    ap = argparse.ArgumentParser(description="Run BIN < test.txt and compare stdout with key file.")
    ap.add_argument("-b","--bin", default=str(script_dir/"bin"),
                    help=f"Path to executable (default: {script_dir/'bin'})")
    ap.add_argument("-t","--test-dir", default=str(d_tests),
                    help=f"Directory with input .txt (default: {d_tests})")
    ap.add_argument("-k","--key-dir", default=str(d_keys),
                    help=f"Directory with expected .txt (default: {d_keys})")
    args = ap.parse_args()

    # Make paths absolute FROM CWD
    bin_path = resolve_from_cwd(args.bin)
    test_dir = resolve_from_cwd(args.test_dir)
    key_dir  = resolve_from_cwd(args.key_dir)

    # Validate strictly: --bin must be an existing file
    if not bin_path.exists():
        print(color("ERROR: --bin does not exist: ","red")+str(bin_path))
        sys.exit(2)
    if not bin_path.is_file():
        print(color("ERROR: --bin is not a file: ","red")+str(bin_path))
        sys.exit(2)

    tests = sorted(p for p in test_dir.glob("*.txt"))
    if not test_dir.is_dir():
        print(color("ERROR: Test dir not found: ","red")+str(test_dir)); sys.exit(2)
    if not key_dir.is_dir():
        print(color("ERROR: Key dir not found: ","red")+str(key_dir)); sys.exit(2)
    if not tests:
        print(color(f"No test files in {test_dir}","yellow")); sys.exit(2)

    print(color(f"BIN: {bin_path}", "dim"))
    print(color(f"TEST_DIR: {test_dir}", "dim"))
    print(color(f"KEY_DIR: {key_dir}", "dim"))
    print()

    passed=failed=0
    for tf in tests:
        name = tf.name
        expf = key_dir/name
        if not expf.exists():
            print(f"[{color('FAIL','red')}] {name}  {color(f'(missing expected: {expf})','dim')}")
            failed+=1; continue

        with tf.open("rb") as fin:
            try:
                proc = subprocess.run(
                    [str(bin_path)],          # run directly, no shell
                    input=fin.read(),
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    check=False,
                )
            except OSError as e:
                print(f"[{color('FAIL','red')}] {name}")
                print("  "+color("Execution error:","yellow"), e)
                failed+=1; continue

        status_ok = (proc.returncode == 0)
        stderr_ok = (len(proc.stderr) == 0)
        got = proc.stdout.decode("utf-8", errors="replace")
        exp = expf.read_text(encoding="utf-8", errors="ignore")

        got_n, exp_n = normalize(got), normalize(exp)
        stdout_ok = (got_n == exp_n)

        if stdout_ok and stderr_ok and status_ok:
            print(f"[{color('PASS','green')}] {name}")
            passed+=1
        else:
            print(f"[{color('FAIL','red')}] {name}")
            if not status_ok:
                print(f"  {color('Exit status:','yellow')} {proc.returncode}")
            if not stderr_ok:
                err = "\n".join(proc.stderr.decode(errors="replace").splitlines()[:5])
                print(f"  {color('Stderr (first 5 lines):','yellow')}\n    "+err.replace("\n","\n    "))
            if not stdout_ok:
                diff = difflib.unified_diff(
                    exp_n.splitlines(keepends=True),
                    got_n.splitlines(keepends=True),
                    fromfile=f"expected/{name}", tofile=f"actual/{name}", n=3
                )
                for i,line in enumerate(diff):
                    if i>=50: print("    ..."); break
                    sys.stdout.write("    "+line)
                if sys.stdout.tell() == 0: pass
            failed+=1

    print(f"\nTotal: {passed+failed}  {color('pass:','green')} {passed}  {color('fail:','red')} {failed}")
    sys.exit(0 if failed==0 else 1)

if __name__ == "__main__":
    main()
