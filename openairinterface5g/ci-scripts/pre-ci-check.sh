#!/bin/bash
# SPDX-License-Identifier: LicenseRef-CSSL-1.0

function usage {
    echo "OAI GitLab MR validation script (Signed-off-by and merge commits)"
    echo ""
    echo "Usage:"
    echo "------"
    echo "    $0 -s <source-branch> -t <target-branch>"
    echo ""
    echo "Options:"
    echo "--------"
    echo "    -s"
    echo "    The source branch of the merge request. Default value is current Git Branch (HEAD)"
    echo ""
    echo "    -t"
    echo "    The target branch of the merge request. Default value is develop"
    echo ""
    echo "    -h"
    echo "    Print this help message."
    echo ""
}

# Parse arguments properly
SOURCE_BRANCH=$(git rev-parse --abbrev-ref HEAD)
TARGET_BRANCH="origin/develop"

while getopts ":s:t:h" opt; do
    case "$opt" in
        s)
            SOURCE_BRANCH="$OPTARG"
            ;;
        t)
            TARGET_BRANCH="$OPTARG"
            ;;
        h)
            usage
            exit 0
            ;;
        :)
            echo "Error: Option -$OPTARG requires a value."
            echo ""
            usage
            exit 2
            ;;
        \?)
            echo "Error: Invalid option -$OPTARG"
            echo ""
            usage
            exit 2
            ;;
    esac
done

# For fork PRs the head commit is often not in the workspace; fetch it so the
# rev-list ranges below resolve and forks are checked like in-repo branches.
if ! git rev-parse --verify --quiet "${SOURCE_BRANCH}^{commit}" >/dev/null; then
    git fetch --no-tags --quiet origin "$SOURCE_BRANCH" || true
fi

# ----------------------------
# Merged commits
# ----------------------------
if [[ "$SOURCE_BRANCH" =~ ^[0-9a-f]{40}$ ]]; then
  # note: if no branch could be found, it will result in "" and git rev-list
  # will use the commit ID. Exclude "HEAD detached at", then use first branch
  # name.
  BRANCH_NAME=$(git branch -a --points-at $SOURCE_BRANCH --format='%(refname:short)' | grep -v detached | head -n1)
  echo "SHA recognized in $SOURCE_BRANCH, using \"$BRANCH_NAME\" as branch name"
else
  BRANCH_NAME="$SOURCE_BRANCH"
fi
if [[ ! "$BRANCH_NAME" =~ ^(origin/)?integration_[0-9]{4}_w[0-9]{2}$ ]]; then
    mergeCommits=$(git rev-list --merges --abbrev-commit "$TARGET_BRANCH".."$SOURCE_BRANCH")
    if [[ -n "$mergeCommits" ]]; then
        message="Error: Following merge commits are found in the source branch history. Please rebase your branch.\n\n"
        message+="$(echo "$mergeCommits" | paste -sd ',' | sed 's/,/, /g')\n"
        echo -e "$message"
        exit 3
    fi
fi

# ----------------------------
# Check unsigned commits
# ----------------------------
unsignedCommits=$(
    for c in $(git rev-list "$TARGET_BRANCH".."$SOURCE_BRANCH" --no-merges); do
        if ! git log -1 --format=%B "$c" | grep -q "Signed-off-by:"; then
            git log -1 --format='%h' "$c"
        fi
    done | paste -sd ',' | sed 's/,/, /g'
)

# ----------------------------
# Report unsigned commits
# ----------------------------
message=""

if [ -n "$unsignedCommits" ]; then
    message="The following commit(s) are missing a Signed-off-by:\n\n$unsignedCommits\n\n"
    message+="Please use 'git commit -s' to sign your commits.\n"
    message+="For detailed instructions, refer to the CONTRIBUTING file at the root of this repository."
    echo -e "$message"
    exit 1
else
    message="All commits are signed off using 'git commit -s'."
    echo -e "$message"
    exit 0
fi
