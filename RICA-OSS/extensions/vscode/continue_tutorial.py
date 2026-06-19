"""                              ____  ___ ______ ___
                                  / __ \/  _/ ____//   |
                                 / /_/ // // /    / /| |
                                / _, _// // /___ / ___ |
                               /_/ |_/___/\____//_/  |_|

                          Renesas Internal Coding AI — Autocomplete, Edit, Chat, and Multi-Agent tutorial
"""


# —————————————————————————————————————————————     Autocomplete     —————————————————————————————————————————————— #
#                            Autocomplete provides inline code suggestions as you type.

# 1. Place cursor after `sorting_algorithm:` below and press [Enter]
# 2. Press [Tab] to accept the Autocomplete suggestion

# Basic assertion for sorting_algorithm:

# —————————————————————————————————————————————————     Edit      ————————————————————————————————————————————————— #
#                   Edit is a convenient way to make quick changes to specific code and files.

# 1. Highlight the code below
# 2. Press [Cmd/Ctrl + I] to Edit
# 3. Try asking RICA to "make this more readable"
def sorting_algorithm(x):
    for i in range(len(x)):
        for j in range(len(x) - 1):
            if x[j] > x[j + 1]:
                x[j], x[j + 1] = x[j + 1], x[j]
    return x

# —————————————————————————————————————————————————     Chat      ————————————————————————————————————————————————— #
#                    Chat makes it easy to ask for help from an LLM without needing to leave the IDE.

# 1. Highlight the code below
# 2. Press [Cmd/Ctrl + L] to add to Chat
# 3. Try asking RICA "what sorting algorithm is this?"
def sorting_algorithm2(x):
    for i in range(len(x)):
        for j in range(len(x) - 1):
            if x[j] > x[j + 1]:
                x[j], x[j + 1] = x[j + 1], x[j]
    return x

# ———————————————————————————————————————————————     Multi-Agent      ———————————————————————————————————————————— #
#         Multi-Agent mode lets the orchestrator spawn specialised sub-agents (Researcher, Reviewer, Implementer,
#         Tester, Analyst, Planner) in parallel for tasks that touch multiple files or require independent review.
#         Each sub-agent runs its own tool loop autonomously and reports back to the orchestrator.

# 1. Switch from "Chat" to "Multi-Agent" mode using the dropdown above the input box
#    (only available when a Tier-S model — Claude Opus 4.x or Claude 4.5 Sonnet — is selected)
# 2. Try asking: "audit this file for code quality, security issues, and best practices"
# 3. Watch the orchestrator spawn 2–3 Reviewer sub-agents in parallel via the bobbing-robots view


  # ——————————————————      Renesas-internal docs: contact the RICA team for the latest URL      ——————————————————— #
