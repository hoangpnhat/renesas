# Multi-Agent Mode — Quick Start

**RICA v1.2.0** · 5 minutes · screenshots show the actual UI

Multi-Agent is a new mode in v1.2.0. Use it when one task naturally splits into several jobs (research + implement, or A + B + C, or review from multiple angles). For one focused job, Agent mode is faster.

---

## When to use it

Pick **Multi-Agent** over Chat or Agent when:

- **Code review across angles** — *"Review src/payment/ for security, performance, and code quality"* → 3 reviewers in parallel, each from a different angle
- **Find then fix** — *"The login test is failing — find why and fix it"* → Researcher finds the cause, Implementer fixes it, Tester verifies
- **Multiple independent jobs in one go** — *"Update the README, add tests for billing.ts, and clean up unused imports in auth/"* → 3 agents working at the same time
- **Bug investigation** — *"Users report logout sometimes hangs — find the root cause"* → Analyst reproduces, traces, and reports
- **Production readiness check** — *"Is this module ready to ship?"* → multiple Reviewers + an Implementer to fix anything critical

Stick with **Agent** when the task is one focused job: *"add a logout button to Header.tsx"*, *"refactor this function"*, *"run the tests"*.

Stick with **Chat** for questions that don't need file access: *"explain what useMemo does"*, *"how do I write a regex for X"*.

---

## 1. Switch to Multi-Agent

Click the mode selector at the bottom of the input area, choose **Multi-Agent**.

![Mode selector dropdown showing Chat, Agent, and Multi-Agent options](images/mode-selector-dropdown.png)

---

## 2. Pick how agents should work

A second dropdown appears next to the mode chip. Leave on **Auto** unless you have a reason — RICA picks the right pattern from your prompt.

![Workflow pattern dropdown showing Auto, Sequential, Parallel, Iterative, Collaborative](images/auto-sequential-selector-dropdown.png)

| Pattern | Use when |
|---|---|
| **Auto** | You're not sure — RICA decides |
| **Sequential** | Later steps need earlier results |
| **Parallel** | Independent jobs, run at once |
| **Iterative** | Fix → verify → fix loop |
| **Collaborative** | Multiple reviewers on the same code |

---

## 3. Send a specific task

Be specific about scope. *"Read src/auth/ and list any security issues"* works. *"Look at the codebase"* doesn't.

If you want multiple things, say so explicitly: *"Review auth.ts AND update README.md AND add tests for billing.ts"* — RICA splits it into 3 parallel agents.

---

## 4. Watch the agents work

The Multi-Agent panel shows every agent live — what they're doing, how many tokens they've used, how many steps they've taken.

![Three Reviewer agents running in parallel under one Orchestrator, with token counters](images/multiagent-graph-tree.png)

Robots animate while running. When all are done, the orchestrator writes a single synthesis above the panel.

---

## 5. Click an agent to see what it did

Each completed agent has a **Task** and **Result** card you can click. They open the full markdown in your editor — every step, every tool call, every finding.

![Agent task and steps log opened in a markdown editor](images/agent-task-result-md-view.png)

---

## Two things to know

### Your model locks after the first message

Once you send the first prompt in a Multi-Agent session, the model is locked for that whole chat (the **🔒** icon shows up on the model chip). To switch models, start a new chat.

![Composer chips showing Multi-agent, Auto, and a locked Claude Opus 4.7 model](images/model-lock-after-first-message.png)

### Track your usage

Click the token counter (top-right) to see exactly what you've spent.

![Session Token Usage panel showing 8,147 total tokens with input/output breakdown](images/session-token-usage-panel.png)

For limits across all models, the Session Usage modal shows what's blocked and what's available.

![Session Usage modal listing current model and other available models with usage bars](images/session-usage-panel.png)

---

## If something goes wrong

- **Stuck for >90 seconds** — click cancel, the watchdog will clean up
- **"Token budget reached"** on an agent — normal, ask the orchestrator to continue
- **Model rate-limited** — wait for the countdown, or start a new chat with another model
- **Need to file a bug** — save the session log (the `↓ save log` button next to the session ID) and send it to the RICA team

![Session Token Usage header with the save log button visible](images/session-log-debugging.png)

Logs are at `~/.rica-multiagent-logs/<timestamp>_<sessionId>.json`.

---

## Want more detail?

This is the quick start. The full reference (all five patterns, all six agent roles, prompt-writing tips, every troubleshooting case) is in **[RICA_Multi_Agent_Reference.md](RICA_Multi_Agent_Reference.md)**.

For technical/implementation detail: **[RICA_Multi_Agent_Technical_Document.md](RICA_Multi_Agent_Technical_Document.md)**.

---

**Known limitation in v1.2.0:** on WSL2, terminal commands may be generated as Windows cmd.exe syntax instead of bash. Workaround: run the commands manually in your WSL terminal, or use the extension on plain Windows. Tracked for the next release.
