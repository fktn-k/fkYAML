#!/usr/bin/env python3

from __future__ import annotations

import argparse
import pathlib
import re
import subprocess
import sys
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Tuple


FAILED_TEST_LINE_RE = re.compile(r"^\s*\d+/\d+ Test\s+#(?P<ctest_id>\d+):\s+(?P<name>\S+)\s+.*\*\*\*Failed")
SUMMARY_LINE_RE = re.compile(r"^\s*(?P<ctest_id>\d+)\s+-\s+(?P<name>\S+)\s+\(Failed\)$")
LABEL_RE = re.compile(r"^label:\s+(?P<label>.+)$")
TEST_CASE_RE = re.compile(r"^TEST CASE:\s+(?P<name>\S+)\s*$")
ERROR_RE = re.compile(r"^(?P<message>.+(?:ERROR|FATAL ERROR):.+)$")
INPUT_FORMAT_RE = re.compile(r"logged:\s+Input format:\s+(?P<value>.+)$")


@dataclass
class FailureRecord:
    ctest_id: str
    name: str
    label: str = ""
    doctest_case: str = ""
    error_message: str = ""
    input_format: str = ""
    input_file: str = ""
    input_data: str = ""

    @property
    def case_id(self) -> str:
        prefix = "yaml_test_suite_"
        if self.name.startswith(prefix):
            suffix = self.name[len(prefix):]
            if suffix.endswith("_YAML"):
                return suffix[:-5]
            if suffix.endswith("_JSON"):
                return suffix[:-5]
            return suffix
        return self.name

    @property
    def format_name(self) -> str:
        if self.name.endswith("_YAML"):
            return "YAML"
        if self.name.endswith("_JSON"):
            return "JSON"

        value = self.input_format.strip()
        if value in {"0", "YAML"}:
            return "YAML"
        if value in {"1", "JSON"}:
            return "JSON"
        return value or "unknown"


def build_argument_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Run the yaml_test_suite ctest target and print a Markdown table of failing cases."
    )
    parser.add_argument(
        "--workspace",
        default=".",
        help="Path to the repository root. Default: current directory.",
    )
    parser.add_argument(
        "--test-dir",
        default="build_yaml_test_suite",
        help="ctest --test-dir value. Default: build_yaml_test_suite.",
    )
    parser.add_argument(
        "--regex",
        default="yaml_test_suite_",
        help="ctest -R regex. Default: yaml_test_suite_.",
    )
    parser.add_argument(
        "--log-file",
        help="Read an existing ctest log instead of running ctest.",
    )
    parser.add_argument(
        "--write-log",
        help="Write captured ctest output to this file when ctest is executed.",
    )
    parser.add_argument(
        "--output",
        help="Write the Markdown report to this file. Default: stdout.",
    )
    parser.add_argument(
        "--yaml-test-suite-root",
        help=(
            "Path to yaml-test-suite source root (directory containing case subdirectories)."
            "If ommitted, common build locations are auto-detected."
        ),
    )
    return parser


def run_ctest(workspace: pathlib.Path, test_dir: str, regex: str) -> str:
    command = [
        "ctest",
        "--test-dir",
        test_dir,
        "--output-on-failure",
        "-R",
        regex,
    ]
    completed = subprocess.run(
        command,
        cwd=str(workspace),
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        encoding="utf-8",
    )
    return completed.stdout


def parse_failures(lines: Iterable[str]) -> List[FailureRecord]:
    failures: List[FailureRecord] = []
    current: Optional[FailureRecord] = None

    for raw_line in lines:
        line = raw_line.rstrip("\n")

        failed_match = FAILED_TEST_LINE_RE.match(line)
        if failed_match:
            current = FailureRecord(failed_match.group("ctest_id"), failed_match.group("name"))
            failures.append(current)
            continue

        summary_match = SUMMARY_LINE_RE.match(line)
        if summary_match:
            if not any(
                failure.ctest_id == summary_match.group("ctest_id") and failure.name == summary_match.group("name")
                for failure in failures
            ):
                failures.append(FailureRecord(summary_match.group("ctest_id"), summary_match.group("name")))
            continue

        if current is None:
            continue

        label_match = LABEL_RE.match(line)
        if label_match:
            current.label = label_match.group("label")
            continue

        stripped = line.strip()
        test_case_match = TEST_CASE_RE.match(stripped)
        if test_case_match:
            current.doctest_case = test_case_match.group("name")
            continue

        error_match = ERROR_RE.match(stripped)
        if error_match and not current.error_message:
            current.error_message = strip_path_prefixes(normalize_whitespace(error_match.group("message")))
            continue

        input_format_match = INPUT_FORMAT_RE.search(stripped)
        if input_format_match:
            current.input_format = input_format_match.group("value")
            continue

    return failures


def detect_yaml_test_suite_root(workspace: pathlib.Path, test_dir: str) -> Optional[pathlib.Path]:
    test_dir_path = pathlib.Path(test_dir)
    candidate_dirs = [
        workspace / test_dir_path / "tests" / "yaml_test_suite_runner" / "yaml-test-suite-src",
        workspace / test_dir_path / "_deps" / "yaml-test-suite-src",
    ]

    for candidate in candidate_dirs:
        if candidate.exists() and candidate.is_dir():
            return candidate
    return None


def build_input_file_index(yaml_test_suite_root: pathlib.Path) -> Dict[Tuple[str, str], pathlib.Path]:
    index: Dict[Tuple[str, str], pathlib.Path] = {}
    file_patterns = (("YAML", "in.yaml"), ("JSON", "in.json"))

    for format_name, file_name in file_patterns:
        for input_file in yaml_test_suite_root.rglob(file_name):
            relative_case_dir = input_file.parent.relative_to(yaml_test_suite_root).as_posix()
            case_id = relative_case_dir.replace("/", "-")
            index[(case_id, format_name)] = input_file

    return index


def attach_input_data_to_failures(
    failures: List[FailureRecord],
    yaml_test_suite_root: Optional[pathlib.Path],
) -> None:
    if yaml_test_suite_root is None:
        return

    index = build_input_file_index(yaml_test_suite_root)

    for failure in failures:
        key = (failure.case_id, failure.format_name)
        input_file_path = index.get(key)
        if input_file_path is None:
            continue

        failure.input_file = input_file_path.relative_to(yaml_test_suite_root).as_posix()
        failure.input_data = input_file_path.read_text(encoding="utf-8", errors="replace")


def normalize_whitespace(text: str) -> str:
    return " ".join(text.split())


def strip_path_prefixes(text: str) -> str:
    # Keep only the basename when file paths appear in error messages.
    with_line = re.compile(r"(?P<path>(?:[A-Za-z]:)?(?:[\\/][^\\/:\s]+)+[\\/](?P<file>[^\\/:\s]+))(?=:\d+)")
    without_line = re.compile(r"(?P<path>(?:[A-Za-z]:)?(?:[\\/][^\\/:\s]+)+[\\/](?P<file>[^\\/:\s]+))")

    text = with_line.sub(lambda match: match.group("file"), text)
    text = without_line.sub(lambda match: match.group("file"), text)
    return text


def escape_markdown(text: str) -> str:
    return text.replace("|", "\\|")


def anchor_id_for_failure(failure: FailureRecord) -> str:
    case_token = re.sub(r"[^a-z0-9-]", "-", failure.case_id.lower())
    format_token = re.sub(r"[^a-z0-9-]", "-", failure.format_name.lower())
    case_token = re.sub(r"-+", "-", case_token).strip("-") or "unknown"
    format_token = re.sub(r"-+", "-", format_token).strip("-") or "unknown"
    return f"input-data-{case_token}-{format_token}"


def fenced_code_block_language(format_name: str) -> str:
    if format_name == "YAML":
        return "yaml"
    if format_name == "JSON":
        return "json"
    return "text"


def render_report(failures: List[FailureRecord], command_description: str) -> str:
    lines = [
        "# yaml_test_suite Failure Report",
        "",
        f"Command: `{command_description}`",
        "",
        f"Failed cases: {len(failures)}",
        "",
    ]

    if not failures:
        lines.append("No failed cases were found.")
        lines.append("")
        return "\n".join(lines)

    lines.extend(
        [
            "| case id | format | input file | label | failure |",
            "| --- | --- | --- | --- | --- |",
        ]
    )

    for failure in failures:
        anchor_id = anchor_id_for_failure(failure)
        input_file_text = escape_markdown(failure.input_file or "(not found)")
        input_file_cell = f"[{input_file_text}](#{anchor_id})"
        lines.append(
            "| {case_id} | {format_name} | {input_file} | {label} | {error_message} |".format(
                case_id=escape_markdown(failure.case_id),
                format_name=escape_markdown(failure.format_name),
                input_file=input_file_cell,
                label=escape_markdown(failure.label or "-"),
                error_message=escape_markdown(failure.error_message or "(summary only)"),
            )
        )

    lines.extend(
        [
            "",
            "## Input Data",
            "",
        ]
    )

    for failure in failures:
        anchor_id = anchor_id_for_failure(failure)
        lines.append(f"<a id=\"{anchor_id}\"></a>")
        lines.append(f"### {failure.case_id} ({failure.format_name})")
        lines.append("[Back to failed cases table](#yaml_test_suite-failure-report)")
        if failure.input_file:
            lines.append(f"Input file: `{failure.input_file}`")
        else:
            lines.append("Input file: (not found)")

        if failure.input_data:
            lines.append("")
            lines.append(f"```{fenced_code_block_language(failure.format_name)}")
            lines.append(failure.input_data)
            lines.append("```")
        else:
            lines.append("")
            lines.append("```")
            lines.append("(empty input)")
            lines.append("```")

        lines.append("")
        lines.append("Error:")
        lines.append("```")
        lines.append(f"Error: {escape_markdown(failure.error_message or '(summary only)')}")
        lines.append("```")
        lines.append("")

    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = build_argument_parser()
    args = parser.parse_args()

    workspace = pathlib.Path(args.workspace).resolve()
    if args.log_file:
        log_path = pathlib.Path(args.log_file)
        output = log_path.read_text(encoding="utf-8")
        command_description = f"log file: {log_path}"
    else:
        output = run_ctest(workspace, args.test_dir, args.regex)
        command_description = f"ctest --test-dir {args.test_dir} --output-on-failure -R '{args.regex}'"
        if args.write_log:
            pathlib.Path(args.write_log).write_text(output, encoding="utf-8")

    failures = parse_failures(output.splitlines())

    if args.yaml_test_suite_root:
        yaml_test_suite_root = pathlib.Path(args.yaml_test_suite_root)
        if not yaml_test_suite_root.is_absolute():
            yaml_test_suite_root = workspace / yaml_test_suite_root
        yaml_test_suite_root = yaml_test_suite_root.resolve()
    else:
        yaml_test_suite_root = detect_yaml_test_suite_root(workspace, args.test_dir)

    attach_input_data_to_failures(failures, yaml_test_suite_root)

    report = render_report(failures, command_description)

    if args.output:
        pathlib.Path(args.output).write_text(report, encoding="utf-8")
    else:
        sys.stdout.write(report)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
