"""Run byte-exact compression regressions in disposable working directories."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


def run_case(executable, case, timeout):
    repo = Path(__file__).resolve().parents[1]
    with tempfile.TemporaryDirectory(prefix="huffman-") as directory:
        work = Path(directory)
        if case.isdigit():
            fixture = repo / "test" / case
            target = work / "test" / case
            target.mkdir(parents=True)
            shutil.copyfile(fixture / "in.txt", target / "in.txt")
            commands = (fixture / "test.txt").read_bytes() + b"\n4\n"
            original, restored = target / "in.txt", target / "decom.txt"
        else:
            data = {"empty": b"", "single": b"a" * 4096,
                    "binary": bytes(range(256)) * 16}[case]
            original, restored = work / "in.bin", work / "out.bin"
            original.write_bytes(data)
            commands = b"1\nin.bin\ncompressed.bin\n2\ncompressed.bin\nout.bin\n4\n"
        result = subprocess.run([str(executable)], input=commands, cwd=work,
                                stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                timeout=timeout)
        if result.returncode:
            raise RuntimeError("program exited with {}\n{}".format(
                result.returncode, result.stdout.decode(errors="replace")[-4000:]))
        if not restored.is_file():
            raise RuntimeError("decompressed output was not created")
        if original.read_bytes() != restored.read_bytes():
            raise RuntimeError("decompressed bytes differ from input")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--executable", required=True, type=Path)
    parser.add_argument("--case", choices=[str(i) for i in range(1, 7)] +
                        ["empty", "single", "binary"])
    parser.add_argument("--timeout", type=float, default=60)
    args = parser.parse_args()
    executable = args.executable.resolve(strict=True)
    failures = 0
    for case in [args.case] if args.case else [str(i) for i in range(1, 7)] + ["empty", "single", "binary"]:
        try:
            run_case(executable, case, args.timeout)
            print("PASS", case)
        except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
            failures += 1
            print("FAIL", case, error)
    return int(failures != 0)


if __name__ == "__main__":
    raise SystemExit(main())
