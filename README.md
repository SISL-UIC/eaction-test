# EAction Attack Test Suite

Test workflows for all 15 EAction attack scenarios. Each workflow dispatches on `workflow_dispatch` — run them from the Actions tab or via `gh workflow run`.

All forked dependency repos live under `rafidhaque/` and are public. Payloads point to `eaction-research.example.com` (IANA reserved, never resolves) so no real exfiltration occurs.

## Quick Start

```bash
# Run a single attack
gh workflow run test-01-pydantic.yml --repo rafidhaque/eaction-test

# Run all 15
for f in test-{01,02,03,04,05,06,07,08,09,10,11,12,13,14,15}-*.yml; do
  gh workflow run "$f" --repo rafidhaque/eaction-test
done
```

## Workflow Files

| File | # | Target Repo | Attack Type | Forked Dep | Runner |
|---|---|---|---|---|---|
| test-01-pydantic.yml | 1 | pydantic | Base64 env dump to logs | gh-action-pypi-publish | ubuntu |
| test-02-aiohttp.yml | 2 | aiohttp | Git push exfil (mechanism) | create-release | ubuntu |
| test-03-grafana.yml | 3 | grafana | Vault secrets HTTP exfil | vault-action + shared-workflows | ubuntu |
| test-04-ruff.yml | 4 | ruff | SUID root binary | action (CodSpeed) | ubuntu |
| test-05-home-assistant.yml | 5 | home-assistant | Env dump + HTTP exfil | codecov-action | ubuntu |
| test-06-langchain.yml | 6 | langchain | BASH_ENV cross-step hijack | get-changed-files | ubuntu |
| test-07-terraform.yml | 7 | terraform | Reverse shell | ssh-agent + actions-docker-build | ubuntu |
| test-08-compose.yml | 8 | compose | Docker credential interception | github-builder | ubuntu |
| test-09-prometheus.yml | 9 | prometheus | Go module cache poisoning | promci-setup | ubuntu |
| test-09-prometheus-e2e.yml | 9 | prometheus | Cache poison end-to-end | promci-setup | ubuntu |
| test-10-act.yml | 10 | act (nektos) | Cryptominer curl\|bash | goreleaser-action | ubuntu |
| test-11-cloakbrowser.yml | 11 | CloakBrowser | Cosign artifact signing abuse | cosign-installer | ubuntu |
| test-12-cpython.yml | 12 | cpython | .bashrc persistence | setup-gcc | ubuntu |
| test-13-syncthing.yml | 13 | syncthing | Windows PowerShell cert theft | artifact-signing-action | **windows** |
| test-14-cryptography.yml | 14 | cryptography | Source file overwrite (ransomware) | rust-toolchain | ubuntu |
| test-15-gitea.yml | 15 | gitea | GPG key exfil | ghaction-import-gpg | ubuntu |

## What to Look For

**Composite actions** (#1, #2, #5, #8, #9, #11, #12, #13, #14): Injection output is visible directly in step logs.

**Node.js actions** (#3, #4, #6, #7, #10, #15): Injection uses `stdio: "ignore"` — no visible output. Proof of execution is that the action loaded and ran its legitimate logic.

**BASH_ENV** (#6): Curl error appears at the TOP of subsequent step output, before the step's own commands.

## Test Results (2026-06-09)

- **14/15 fully pass**
- **1 partial**: aiohttp (#2) — GITHUB_TOKEN cannot push to repos outside the workflow's own repo.

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
