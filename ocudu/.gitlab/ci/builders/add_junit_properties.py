#!/usr/bin/env python3

# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

"""
Add a <properties> block to every <testsuite> element in a CTest JUnit XML file.

Usage:
  add_junit_properties.py <xunit.xml> [--commit <hash>]

If --commit is omitted, the value is read from the OCUDU_COMMIT environment variable.
"""

import argparse
import os
import re


def main():
    """Add a <properties> block to every <testsuite> element in a CTest JUnit XML file."""
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("junit_xml", help="Path to the JUnit XML file to modify in-place")
    parser.add_argument(
        "--suite-name", default=os.environ.get("CI_JOB_NAME", ""), help="Override testsuite name attribute"
    )
    parser.add_argument("--commit", default=os.environ.get("OCUDU_COMMIT", ""), help="OCUDU commit hash")
    parser.add_argument("--test-commit", default=os.environ.get("CI_COMMIT_SHA", ""), help="CI commit hash")
    parser.add_argument("--url", default=os.environ.get("CI_JOB_URL", ""), help="CI job URL")
    args = parser.parse_args()

    with open(args.junit_xml, encoding="utf-8") as f:
        content = f.read()

    properties = [("ocudu_commit", args.commit), ("test_commit", args.test_commit), ("url", args.url)]
    prop_block = (
        "<properties>" + "".join(f'<property name="{k}" value="{v}"/>' for k, v in properties) + "</properties>"
    )

    def patch_testsuite(m):
        tag = m.group(0)
        if args.suite_name:
            tag = re.sub(r'name="[^"]*"', f'name="{args.suite_name}"', tag)
        return tag + prop_block

    content = re.sub(r"(<testsuite\b[^>]*>)", patch_testsuite, content)

    with open(args.junit_xml, "w", encoding="utf-8") as f:
        f.write(content)


if __name__ == "__main__":
    main()
