# Security Policy

## Supported Versions

| Version  | Supported          |
|----------| -------------------|
| develop  | :white_check_mark: |

## Reporting a Vulnerability

We strongly encourage you to report security vulnerabilities first to our
private security mailing list, wg-security@durantaproject.org, before disclosing
them in any public forum.

This is a private mailing list to which the members of Duranta's security team
are subscribed, and reports sent to it are treated as top priority.

- Email subject: [SEC-VUL-DURANTA-OAI]: "Mention the affected functionality
  here".
- Commit: it should be the latest commit, or a very recent one, as of writing
  the bug report.
- Issue description (please do not write long paragraphs).
- Affected code functionality and file paths.
- How did you discover the problem, and how can it be reproduced?
- Acknowledge AI tools: please mention and explain whether you have used any AI
  tool to generate this report or to find the vulnerability. If you used one to
  find the vulnerability, please see the next section. The tool should be
  reported as `TOOL-NAME: LLM-MODEL-VERSION`, for example
  `Claude: Claude Opus 5`.
- Details of the hardware used: CPU, RAM, hard disk, kernel command-line
  parameters, operating system name, and kernel version.
- Do not attach any files, just an extract of the logs if needed. We may request
  the full logs later.
- We encourage you to share a tentative patch if you have one.

### Use of AI to find a vulnerability

If you used AI tools to find issues or vulnerabilities, you must treat the
finding as public, because multiple researchers using the same or different AI
tools can find the same issue. You can open an [issue in the
repository](https://github.com/duranta-project/openairinterface5g/issues). In
this case, please follow the format below to open the issue:

- Issue title: [AI-SEC-VUL-DURANTA-OAI]: "Mention the affected functionality
  here".
- Commit: it should be the latest commit, or a very recent one, as of writing
  the bug report.
- Issue description (please do not write long paragraphs).
- Affected code functionality and file paths.
- Acknowledge AI tools: please mention and explain whether you have used any AI
  tool to find the vulnerability. It should be reported as
  `TOOL-NAME: LLM-MODEL-VERSION`, for example `Claude: Claude Opus 5`.
- Details of the hardware used: CPU, RAM, hard disk, kernel command-line
  parameters, operating system name, and kernel version.
- Do not attach any files, just an extract of the logs if needed. We may request
  the full logs later.
- Do not mention how to reproduce the problem in the issue description. The
  security team will contact you about it.
- AI tools are good at fixing vulnerabilities; you can ask your AI tool to
  provide a patch.

### Scope

The Duranta/openairinterface5g repository contains software for 4G/5G RAN, UE,
protocol stacks, radio drivers, simulators, deployment artifacts, and CI/release
tooling. 4G development in the repository is frozen, and we will only address
security issues related to the 5G stack.

Security reports are in scope when they affect the confidentiality, integrity,
or availability of a supported Duranta/OAI component running in a documented or
reasonably expected deployment.

In-scope examples include:

- Memory corruption, crashes, or denial of service triggered by malformed RF,
  RRC, NAS, NGAP, X2AP, F1AP, E1AP, E2AP, GTP-U, FAPI/nFAPI, or O-RAN fronthaul
  messages.
- Authentication, ciphering, integrity, or session/state-machine bypasses in RAN
  or UE procedures.
- Unauthorized packet injection, traffic interception, subscriber information
  exposure, or leakage of credentials, keys, IMSI/SUPI-like identifiers, logs,
  or user-plane data.
- Remote code execution, privilege escalation, arbitrary file access, or command
  execution through configuration parsing, protocol handling, logging, web
  utilities, tools, or deployment artifacts.
- Container, Helm, OpenShift, Docker, or default-configuration issues that
  expose sensitive services, secrets, or unsafe privileges in documented
  deployments.
- CI, build, signing, or release-pipeline issues only when they could compromise
  official Duranta/OAI release artifacts, published images, or trusted source
  distribution.

Out-of-scope examples include:

- Performance issues without a security impact.
- Bugs requiring local admin/root access with no privilege boundary crossed.
- Reports against unsupported forks, private deployments, local lab
  misconfiguration, or modified code not present in this repository.
- Issues coming from third-party projects used for Duranta/OAI testing.
- Radio jamming, RF interference, spectrum misuse, or attacks that only depend
  on physical-layer disruption without exploiting Duranta/OAI software behavior.
- Denial of service in experimental, test-only, or simulator-only code unless it
  demonstrates a realistic impact on supported deployments.
- Issues only affecting contributor CI jobs, temporary development artifacts, or
  untrusted test images, unless they can affect official releases.

### Disclosure

The project aims to acknowledge all contributors for valid reports of security
issues. Each security issue sent via the mailing list will, after review and if
accepted, be posted as a draft GitHub security advisory. GitHub will provide the
CVE ID once the advisory is published and accepted. Reporters will be credited
by name or GitHub handle in the advisory. Disclosure will typically be made at
or shortly after the release of the fix.

The security team will decide whether a report meets the requirements for a
GitHub advisory and CVE ID on a case-by-case basis.

Some reports may lead to changes in the Duranta/OAI codebase even if they do not
result in an associated advisory. Examples of reports that may fall into this
category include (but are not limited to):

- Reports of issues in unstable functionality or incomplete features.
- Reports of issues where there is no evidence that a recent tag of Duranta/OAI
  has been affected.

In such cases, the project aims to credit reporters with an acknowledgement in
the relevant fix commit via a `Reported-by:` trailer in the commit message.

**NOTE**: The Duranta project manages CVEs only via the [GitHub security
advisory database](https://github.com/advisories). If you have posted the CVE at
[CVE ORG](https://www.cve.org/ReportRequest/ReportRequestForNonCNAs) or a
similar organization, please provide the identifier. We request that you update
the CVE description when the project maintainers provide a fix or reject the
CVE.

#### Timeline

After receiving the report, the team will validate the security issue and will
respond to the reporter within 10 days. Once the issue is fixed and the report
meets the requirements for a GitHub advisory, it will be published with the fix
within 90 days.
