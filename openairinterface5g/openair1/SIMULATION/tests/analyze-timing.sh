#!/bin/bash
# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# This bash script builds an awk script. The awk script tries to match
# (provided) patterns and compares them against given thresholds condition
# (e.g., threshold is "< 10").

function die() { echo $@ 1>&2; exit 1; }

# Print every line, because the logs are piped into this script, but a user is
# typically also interested into the raw logs.
SCRIPT='
  { print $0 }
'

NUM=0
# for each pair of <pattern>/<condition>, add corresponding rules.
while [ $# -gt 0 ]; do
  [ $# -ne 1 ] || die "unmatched <pattern>/<condition>"
  PATTERN=${1}
  COND=${2}
  shift 2

  # Add a rule that searches for a PATTERN + number, and checks against
  # CONDition. If the condition does not hold, it is counted as a failure. In
  # both cases, the result is logged in an array to output at the end of the
  # script.
  #
  # To search for the number, first substr() returns what has been matched
  # (PATTERN + number), and sub() deletes PATTERN and whitespace, resulting in
  # the number (measurement). To force number into a numeric context, we add 0,
  # as awk would otherwise compare a string to a number (example:
  #   gawk 'BEGIN { meas = "117"; if (meas > 96) { } else { print "fail";}}'
  # printfs "fail").
  #
  # Example: pattern "PHY proc tx", condition "< 200"
  # The awk script tries to match every line for "PHY proc tx _NUMBER_" (where
  # number is a decimal), and compares _NUMBER_ against condition "< 200",
  # i.e., "_NUMBER_ < 200".
  # If the condition holds, will set "CHECK PHY proc tx _NUMBER_ < 200 SUCCESS".
  # If the condition fails, will set "CHECK PHY proc tx _NUMBER_ < 200 FAIL".
  SCRIPT+='
  match($0, /'${PATTERN}' +([0-9]+(\.[0-9]+)?)/) {
    meas = substr($0, RSTART, RLENGTH);
    sub(/'${PATTERN}' +/, "", meas);
    if ((meas+0) '${COND}') {
      r = "SUCCESS";
    } else {
      r = "FAIL";
    }
    RESULTS['${NUM}']=sprintf("CHECK %-40s %10.2f %-10s %s", "'${PATTERN}'", meas, "'${COND}'", r);
  }
'

  # Generate an additional rule for the end of the logs: if the pattern is not
  # found, it is counted as a failure and logged appropriately.
  # Following the above example "PHY proc tx" and "< 200": if such log is not
  # found, will set "CHECK PHY proc tx < 200 NOTFOUND"
  SCRIPT+='
  END {
    if (!RESULTS['${NUM}']) {
      RESULTS['${NUM}']=sprintf("CHECK %-35s          %-7s NOTFOUND", "'${PATTERN}'", "'${COND}'");
    }
  }
'

  let NUM=${NUM}+1
done

# After passing through all logs, print all results, and exit with return code
# (0 on success, i.e..all conditions checked, otherwise 1 on failure).
SCRIPT+='
  END {
    RC = 0;
    for (i = 0; i < '${NUM}'; ++i) {
      print RESULTS[i]
      if (!(RESULTS[i] ~ /SUCCESS$/))
        RC = 1;
    }
    exit RC
  }
'

# Read from separate file descriptor 3: this allows us to pipe the actual log
# to analyze into awk (by using the script like so: analyze-timing.sh < log)
awk -f /dev/fd/3 3<<< ${SCRIPT}
