## Command Execution Guidelines

### Shell Preference
- **PREFER Bash over PowerShell** for all commands unless Windows-specific operations are required
- Bash commands have better permission allowlist matching and cross-platform consistency

### Working Directory
- The working directory is always set to the project root: `C:\work\modular-keypads\firmware`
- **NEVER** chain commands with `cd` prefix (e.g., `cd C:\work\modular-keypads\firmware; platformio run`)
- **ALWAYS** execute commands directly without `cd`: `platformio run -e s3-module`

**Reason:** Chaining with `cd` changes the command structure and prevents matching permission allowlist rules, causing unnecessary permission prompts.

### Build & Test Commands
- Execute `platformio run` and `platformio test` directly from the root directory using PowerShell
- Use patterns that match existing permission allowlist entries:
  - ✅ `platformio run -e s3-module`
  - ✅ `platformio test -e test`
  - ❌ `cd C:\work\modular-keypads\firmware; platformio run -e s3-module`
  - ❌ Piping/redirects with these commands (e.g., `2>&1 | tail`) - wildcards don't match shell operators
- There are no tests for the `s3-module` environment, only the `test` environment. Only use `s3-module` with `run` to confirm compilation success.