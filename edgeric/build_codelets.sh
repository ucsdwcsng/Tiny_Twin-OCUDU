#!/usr/bin/env bash
# Build the EdgeRIC codelets (codelets/edgeric: dl_sched.o, dl_mcs.o, their protobuf serializers and the ctypes
# modules the jrtc apps import) inside Microsoft's srs-jbpf-sdk image, like scout-jbpf's jrtc-apps/codelets/make.sh
# with SRS_JBPF_DOCKER=1 and USE_JRTC=1. The sources, Makefiles and apps are verbatim copies from scout-jbpf
# (commit cfd2a6e); see codelets/ and apps/.
#
# Usage: ./build_codelets.sh [make target...]        (e.g. "clean")
#   SDK_IMAGE  SDK image (default ghcr.io/microsoft/jrtc-apps/srs-jbpf-sdk:srsran25.10-latest)
#
# A passing build prints "Program terminates within N instructions" for each codelet (verifier).

set -euo pipefail

SDK_IMAGE="${SDK_IMAGE:-ghcr.io/microsoft/jrtc-apps/srs-jbpf-sdk:srsran25.10-latest}"
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"

# Run make as root like scout (the SDK's nanopb generator writes inside the image), then hand the build outputs back
# to the calling user.
docker run --rm \
  -e USE_JRTC=1 \
  -v "$HERE/codelets:/codelet" \
  --entrypoint /bin/bash \
  "$SDK_IMAGE" \
  -c 'make -C /codelet/edgeric "$@"; rc=$?; chown -R '"$(id -u):$(id -g)"' /codelet/edgeric; exit $rc' make "$@"
