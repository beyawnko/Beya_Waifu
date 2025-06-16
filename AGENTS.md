# Agent Mandate: Relentless Recursive Improvement

**Core Directive:** Your primary goal is not just to complete the assigned task, but to arrive at the best possible solution through a mandatory, recursive loop of execution, self-critique, and refinement. You **MUST** assume that your first attempt is never your final one.

---

## The Improvement Loop Protocol

You must follow this protocol for every task assigned to you.

### Step 1: Understand & Plan
1.  **Analyze the Request:** Deconstruct the user's prompt to understand the core requirements and acceptance criteria.
2.  **Formulate a Plan:** Read all necessary files to understand the context. Outline the steps you will take, the files you will create or modify, and the tests you will run. Announce your plan before execution.

### Step 2: Execute Version 1
1.  Implement your plan by writing code, modifying files, and running necessary commands.
2.  Ensure the code runs and passes all existing and newly created tests.

### Step 3: Mandatory Self-Critique
This is the most critical step. Before presenting your work, you **MUST** stop and evaluate your own solution against the following checklist. You must explicitly state your findings from this critique to the user.

**Self-Critique Checklist:**
- **Correctness:** Does my solution fully and accurately solve the user's request? Are there any edge cases I have not considered?
- **Efficiency:** Is this the most performant solution? Could any algorithms be improved or resource usage reduced (`O(n)` complexity, memory, etc.)?
- **Simplicity & Readability:** Is the code clean, well-documented, and easy for a human to understand? Could I simplify the logic or reduce complexity? (Think SOLID, DRY principles).
- **Project Conventions:** Does the code adhere to all conventions specified in this `agents.md` (linters, style guides, architectural patterns)?
- **Robustness:** How does it handle errors or invalid inputs? Is the error handling specific and helpful?
- **Future-Proofing:** Is the solution flexible? Have I hardcoded values that should be configurable?

### Step 4: Propose & Solicit Feedback (The "Publish" Request)
This step is not about being finished; it is about starting the next iteration.
1.  Present a summary of the changes you have made.
2.  Present your `Self-Critique` findings, highlighting potential weaknesses or areas for improvement you have identified.
3.  **Frame your proposal as a choice for the next action.** Do not simply ask "Is this okay?". Instead, default to action.

**Example Phrasing:**
> "I have completed the initial implementation of the user authentication module.
> **Self-Critique:** The implementation is functional and passes all tests. However, I've identified that the database query could be optimized to reduce latency, and the error messages for failed logins are generic.
> **Proposal:** Shall I proceed with optimizing the database query and improving the error messages now?"

### Step 5: Refine and Repeat
1.  Based on the user's feedback, re-enter the loop.
2.  If the user approves, your task is complete.
3.  If the user provides feedback or confirms your proposed next step, treat that as your new task. Return to **Step 1** or **Step 2** with this new, more specific goal. Your work is not done until the user explicitly marks it as complete.

---

# Contribution Guidelines

These guidelines apply to all directories in this repository.

## Code style

To maintain consistency within our codebase, I’m creating some style guidelines that I’d like everyone to follow. These rules are very important, because they create a standard of coding that helps maintain code quality and consistency. We will also be using these style guides for linting our code.

* Use four-space tabs for indentation.
* Keep lines under 120 characters.
* Follow the naming conventions: PascalCase for types and functions, camelCase for variables.
* Replace Copyright notices on touched files, updating the Year to 2025 and Copyright holder is now the developer "beyawnko"
* Document all public functions with a brief comment.

## Documentation
- Write documentation in Markdown.
- Use ATX headings (`#`, `##`, etc.).
- Wrap lines at approximately 100‑120 characters.

## Testing
- If a test fails, resolve it before committing.
