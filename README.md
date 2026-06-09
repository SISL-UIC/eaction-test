# EAction Attack Test Suite

Test workflows for all 15 EAction attack scenarios. Each workflow dispatches on `workflow_dispatch` — run them from the Actions tab or via `gh workflow run`.

All forked dependency repos live under `rafidhaque/` and are public. Payloads point to `eaction-research.example.com` (IANA reserved, never resolves) so no real exfiltration occurs.

## Quick Start

```bash
# Run a specific batch
gh workflow run "Batch 1: Composite Action Attacks" --repo rafidhaque/eaction-test

# Run all batches
gh workflow run test-batch1.yml --repo rafidhaque/eaction-test
gh workflow run test-batch2.yml --repo rafidhaque/eaction-test
gh workflow run test-batch3.yml --repo rafidhaque/eaction-test
gh workflow run test-langchain-payload.yml --repo rafidhaque/eaction-test
gh workflow run test-ruff-payload.yml --repo rafidhaque/eaction-test
gh workflow run test-prometheus-payload.yml --repo rafidhaque/eaction-test
gh workflow run test-prometheus-e2e.yml --repo rafidhaque/eaction-test
```

## Workflow → Attack Mapping

### test-batch1.yml — Composite Action Attacks (ubuntu-latest)

| # | Target Repo | Attack Type | Forked Dep | What to Look For |
|---|---|---|---|---|
| 1 | pydantic | Base64 env dump to logs | gh-action-pypi-publish | Wall of base64 text in step output |
| 5 | home-assistant | Env dump + HTTP exfil | codecov-action | `env` output visible, curl fails (expected) |
| 12 | cpython | .bashrc persistence | setup-gcc | `.bashrc` modified, subsequent step sources it |
| 14 | cryptography | Source file overwrite (ransomware sim) | rust-toolchain | Files replaced with `ENCRYPTED-BY-EACTION-RESEARCH` |

### test-batch2.yml — Node.js Action Attacks (ubuntu-latest)

| # | Target Repo | Attack Type | Forked Dep | What to Look For |
|---|---|---|---|---|
| 3 | grafana | Vault secrets HTTP exfil | vault-action (via shared-workflows) | Action loads and runs; injection is silent (`stdio: "ignore"`) |
| 10 | act (nektos) | Cryptominer curl\|bash | goreleaser-action | Action loads and runs; injection is silent |
| 15 | gitea | GPG key exfil | ghaction-import-gpg | Action loads and runs; injection is silent |
| 7 | terraform | Reverse shell | ssh-agent (via actions-docker-build) | Action loads and runs; injection is silent |

> **Node.js actions use `stdio: "ignore"`** — no visible output from the injection. Proof of execution is that the action loaded and ran its legitimate logic afterward.

### test-batch3.yml — Special Setup Attacks

| # | Target Repo | Attack Type | Forked Dep | Runner | What to Look For |
|---|---|---|---|---|---|
| 2 | aiohttp | Git push exfil (mechanism test) | create-release | ubuntu-latest | Clone/write/commit succeed; push fails (GITHUB_TOKEN scope) |
| 8 | compose | Docker credential interception | github-builder | ubuntu-latest | `~/.docker/config.json` readable cross-step |
| 11 | CloakBrowser | Cosign artifact signing abuse | cosign-installer | ubuntu-latest | `/tmp/malicious.bin` created, cosign signs it |
| 13 | syncthing | Windows PowerShell cert theft | artifact-signing-action | **windows-latest** | Cert store enumeration runs (may be empty on fresh runner) |

### test-langchain-payload.yml — BASH_ENV Cross-Step Hijack (ubuntu-latest)

| # | Target Repo | Attack Type | Forked Dep | What to Look For |
|---|---|---|---|---|
| 6 | langchain | BASH_ENV hijack | get-changed-files | Curl error at TOP of subsequent step output (before the step's own commands) |

### test-ruff-payload.yml — SUID Root Binary (ubuntu-latest)

| # | Target Repo | Attack Type | Forked Dep | What to Look For |
|---|---|---|---|---|
| 4 | ruff | Privilege escalation (SUID binary) | action (CodSpeed) | `/tmp/.hidden_root` created with SUID bit set |

### test-prometheus-payload.yml — Cache Poisoning Payload (ubuntu-latest)

| # | Target Repo | Attack Type | Forked Dep | What to Look For |
|---|---|---|---|---|
| 9 | prometheus | Go module cache poisoning | promci-setup | `eaction_cache_init.go` injected into Go module cache |

### test-prometheus-e2e.yml — Cache Poisoning End-to-End (ubuntu-latest)

Full 3-phase test: poison cache → restore cache → build binary → prove `init()` runs.

## Test Results (2026-06-09)

- **14/15 fully pass**
- **1 partial**: aiohttp (#2) — GITHUB_TOKEN is scoped to the repo where the workflow runs and cannot push to other repos. Real attack would use HTTP exfil or push to victim's own repo.

## Forked Dependency Repos

All under `rafidhaque/`:

| Repo | Used By | Type |
|---|---|---|
| gh-action-pypi-publish | pydantic (#1) | Composite |
| create-release | aiohttp (#2) | Composite |
| vault-action | grafana (#3) | Node.js |
| shared-workflows | grafana (#3) | Intermediate |
| action | ruff (#4) | Composite |
| codecov-action | home-assistant (#5) | Composite |
| get-changed-files | langchain (#6) | Node.js |
| ssh-agent | terraform (#7) | Node.js |
| actions-docker-build | terraform (#7) | Intermediate |
| github-builder | compose (#8) | Composite |
| promci-setup | prometheus (#9) | Composite |
| goreleaser-action | act (#10) | Node.js |
| cosign-installer | CloakBrowser (#11) | Composite |
| setup-gcc | cpython (#12) | Composite |
| artifact-signing-action | syncthing (#13) | Composite |
| rust-toolchain | cryptography (#14) | Composite |
| ghaction-import-gpg | gitea (#15) | Node.js |
