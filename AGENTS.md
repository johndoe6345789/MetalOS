# Agents

This document describes the micro agents available in the MetalOS project and provides guidelines for creating and integrating agents within the Software Development Lifecycle (SDLC), incorporating industry-standard software engineering best practices.

## Software Development Lifecycle (SDLC)

Micro agents should align with the following SDLC phases:

1. **Requirements Gathering**
   - Automate verification of requirement specifications, traceability, and completeness checks.
2. **Design**
   - Generate or validate design artifacts (UML diagrams, interface contracts).
3. **Implementation**
   - Scaffold code templates, enforce style guides, run linters and formatters.
4. **Testing**
   - Execute unit, integration, and system tests; collect and report coverage metrics.
5. **Deployment**
   - Package builds, create images, and deploy to staging or production environments.
6. **Maintenance**
   - Monitor for dependency updates, security patches, and performance regressions.

## Software Engineering Best Practices

- **Version Control**: Use Git feature branches; agents should support branch checkout and tagging.
- **Code Quality**: Integrate static analysis tools (clang-tidy, flake8) and adhere to coding standards.
- **CI/CD Integration**: Automate build, test, and deployment pipelines; provide status badges and failure alerts.
- **Testing Strategy**: Encourage TDD/BDD; maintain high test coverage; isolate flaky tests.
- **Documentation**: Generate and validate README, API docs, and changelogs; ensure up-to-date references.
- **Security**: Automate vulnerability scanning (e.g., Snyk, Dependabot); enforce secret detection policies.
- **Performance Monitoring**: Collect metrics during test runs; alert on regressions.

## Directory Structure

By convention, all agent code resides under an `agents/` directory:

```
agents/
  ├── ExampleAgent/
  │     ├── main.py
  │     ├── README.md
  │     └── tests/
  └── AnotherAgent/
        └── ...
```

## Creating a New Agent

1. Create `agents/<AgentName>/`.
2. Add an entrypoint script (`main.py`, `agent.sh`) with `--help` output.
3. Include unit tests under `tests/` and CI workflow definitions.^
4. Document usage and dependencies in `agents/<AgentName>/README.md`.
5. Register the agent under **Available Agents** below.

## Available Agents

| Name         | Path                  | Description                               |
|--------------|-----------------------|-------------------------------------------|
| ExampleAgent | `agents/ExampleAgent` | Sample agent illustrating best practices. |
| BuildAgent   | `agents/BuildAgent`   | Automates the CMake build process and reports status. |
| TestAgent    | `agents/TestAgent`    | Runs project test suite and gathers results. |
| LintAgent    | `agents/LintAgent`    | Executes linters (clang-tidy, flake8) and formats code. |
| DocGenAgent  | `agents/DocGenAgent`  | Generates and validates project documentation. |
| DeployAgent  | `agents/DeployAgent`  | Packages and deploys builds to staging or production. |
| DepUpdateAgent | `agents/DepUpdateAgent` | Checks for and applies dependency updates. |
| RoadmapAgent    | `agents/RoadmapAgent`    | View or append items in docs/ROADMAP.md. |

---
*Maintain this document by updating **Available Agents**, SDLC alignment, and best practices guidelines as agents evolve.*
