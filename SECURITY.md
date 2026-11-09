# Security Policy

## Supported Versions

ForgeUI actively supports and maintains security fixes on the following branches:

| Version / Branch  | Supported          |
| ----------------- | ------------------ |
| `main`            | :white_check_mark: |
| Releases (latest) | :white_check_mark: |

---

## Reporting a Vulnerability

The ForgeUI team takes security, memory safety, and robustness seriously. If you discover a security vulnerability—such as an out-of-bounds read/write, memory corruption in the arena allocator or font decompression, buffer overruns, or unexpected host state poisoning—please follow these steps:

1. **Do not disclose the issue publicly** in GitHub Issues, Discussions, or social media.
2. **Submit a Private Vulnerability Report**:
   - Use the **GitHub Security Advisory** feature: navigate to [TimeEngineOrg/ForgeUI Security Advisories](https://github.com/TimeEngineOrg/ForgeUI/security/advisories) and click **Report a vulnerability**.
   - Alternatively, email the maintainer directly at `shankharajdatta2004@gmail.com` with the subject line `[ForgeUI Security Vulnerability]`.
3. **Include the following information in your report**:
   - A clear description of the vulnerability and its potential impact.
   - Minimal, reproducible C++ code example or input sequence demonstrating the issue.
   - Target operating system, compiler, architecture, and graphics backend.
   - Any suggested remediations or patches if available.

---

## Response & Disclosure Process

- **Acknowledgment**: We strive to acknowledge vulnerability reports within 48 to 72 hours.
- **Investigation & Remediation**: We will investigate, reproduce, and prepare an isolated patch in a private fork.
- **Coordination & Release**: Once verified, a security patch will be merged, and a CVE / GitHub Advisory published alongside credit to the finder (unless anonymity is requested).

Thank you for helping keep ForgeUI and its downstream applications secure.
