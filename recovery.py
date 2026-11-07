from pathlib import Path
import re
import sys

BASE_DIR = Path(__file__).resolve().parent
DUMP_FILE = BASE_DIR / "project_dump.txt"
OUTPUT_DIR = BASE_DIR / "recovered_project"

# Matches headers such as:
# FILE: ./src\game.c
HEADER_RE = re.compile(r"^FILE:\s*(.+?)\s*$")


def relative_path_from_header(header):
    """Convert a dump path into a safe relative Windows path."""

    cleaned = header.strip().strip('"').replace("\\", "/")

    # Remove the leading ./ if present.
    if cleaned.startswith("./"):
        cleaned = cleaned[2:]

    relative = Path(*cleaned.split("/"))

    # Prevent writing outside the recovery directory.
    if relative.is_absolute() or ".." in relative.parts:
        raise ValueError(f"Unsafe file path: {header}")

    return relative


def main():
    if not DUMP_FILE.is_file():
        print(f"ERROR: Cannot find {DUMP_FILE}")
        return 1

    dump_text = DUMP_FILE.read_text(
        encoding="utf-8-sig",
        errors="replace"
    )

    files = []
    current_path = None
    current_content = []

    for line_number, line in enumerate(
        dump_text.splitlines(keepends=True), start=1
    ):
        stripped = line.strip()

        # Ignore separator lines made entirely of '='.
        if stripped and set(stripped) == {"="}:
            continue

        match = HEADER_RE.match(stripped)

        if match:
            # Save the previous file before starting the next one.
            if current_path is not None:
                files.append((current_path, "".join(current_content)))

            try:
                current_path = relative_path_from_header(match.group(1))
            except ValueError as error:
                print(f"ERROR on line {line_number}: {error}")
                return 1

            current_content = []

        elif current_path is not None:
            current_content.append(line)

    # Save the final file.
    if current_path is not None:
        files.append((current_path, "".join(current_content)))

    if not files:
        print("ERROR: No files found in project_dump.txt")
        return 1

    # Create the recovered project.
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    for relative_path, content in files:
        destination = OUTPUT_DIR / relative_path

        destination.parent.mkdir(parents=True, exist_ok=True)

        destination.write_text(
            content,
            encoding="utf-8",
            newline=""
        )

        print(f"Recovered: {relative_path}")

    print("\n" + "=" * 50)
    print(f"Successfully recovered {len(files)} files.")
    print(f"Location: {OUTPUT_DIR}")
    print("=" * 50)

    return 0


if __name__ == "__main__":
    sys.exit(main())