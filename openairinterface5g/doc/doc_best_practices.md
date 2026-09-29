<!-- SPDX-License-Identifier: CC-BY-4.0 -->

# Documentation Best Practices

The following contribution guidelines ensure that all documentation in the
repository is clear, consistent, and readable. Contributors should follow them
to make it easier for other developers and users to understand and maintain the
documentation.

## Headings

- Only **one** top-level heading (`# H1`) should exist per document.
- Use subsequent heading levels (`## H2` to `###### H6`) for subheadings.
- Do **not** add trailing `#` symbols after headings.

**Examples**:

Incorrect:

```markdown
# Contribution Guidelines for Developers
# Workflow and Versioning #
## Manage your own branch
## Merge Requests
```

Correct:

```markdown
# Contribution Guidelines for Developers
## Workflow and Versioning
### Manage your own branch
### Merge Requests
```

## Quoting Texts

Use Markdown **blockquotes (`>`)** for notes, tips, or warnings.  Blockquotes
clearly separate important information from regular text, helping readers
notice critical instructions or cautionary notes.

**Examples**:

```markdown
> **Note:** Ensure all dependencies are installed before building.

> **Warning:** Do not run the build as sudo.
```

## Avoid HTML formatting

Do **not** use HTML tags like `<b>`, `<font>`, `<table>`, etc.  Use standard
Markdown syntax. HTML may not display correctly on all platforms such as
[GitLab](https://about.gitlab.com/), [GitHub](https://github.com/), etc.

**Examples**:

Correct (Markdown):

```markdown
**Bold text**
*Italic text*

![some image](./path/to/image.png)

| Feature       | Description                     |
|---------------|---------------------------------|
| H1 Heading    | Only one per document            |
| Blockquotes   | For notes and warnings           |
```

Incorrect (HTML):

```html
<b>Bold text</b>
<i>Italic text</i>

<img src="./path/to/image.png" alt="some image">

<table style="border-collapse: collapse; border: none;">
  <tr>
    <td>H1 Heading</td>
    <td>Only one per document</td>
  </tr>
  <tr>
    <td>Blockquotes</td>
    <td>For notes and warnings</td>
  </tr>
</table>

```

## Links and references

- Use Markdown link syntax `[Text](URL)` for all external URLs or references.
- Use `[Text](#section-anchor)` to link to sections within the same document.
- Use `![Alt text](URL_or_path)` to insert images, with descriptive alt text
  for accessibility. Do **not** use HTML `<figure>` or `<figcaption>` elements
      for images.
- Always verify that section anchors exist and check all links to prevent
  broken references.
- When adding, deleting, or renaming any documentation file, make sure to
  update any related references, links, and indexes in the repository including
  the [README.md](README.md).

**Examples**:

```markdown
- For more information, visit the [OAI
  website](http://www.openairinterface.org/).
- See [primer](http://google.github.io/googletest/primer.html) for a quick
  introduction.
- The Linux kernel has some [documentation on what a logical change
  is](https://www.kernel.org/doc/html/latest/process/submitting-patches.html#separate-your-changes).
- See [OAI CN5G configuration files](#22-oai-cn5g-configuration-files) for
  details.
- ![some image](./path/to/image.png)
```

## Inline code for technical elements

Enclose **variables, function names, filenames, commands, or any technical identifiers** in backticks (\`).
Inline code formatting improves readability, clearly differentiates technical terms from normal text, and prevents confusion between text and code.

**Examples**:

```markdown
- Run the command `./build_oai`.
- See the function `time_manager_start()`.
- On `x86_64` platform the CPU should support `avx2` instruction set.
- The A2 event can be disabled by setting `enable = 0`.
- The logs are stored in `gnb.log` file.
- The default value for User IMSI is `2089900007487`.
- Once installed you can use this configuration file for ptp4l
  (`/etc/ptp4l.conf`).
```

For **multi-line code blocks**, use triple backticks with a language identifier for syntax highlighting.

````bash
```bash
docker --version
docker images
docker ps -a
docker system prune -a
```
````

> **Note:** To make commands copy-friendly, **avoid using `$` signs** and use
> plain commands.

Also ensure:

- No extra whitespace before backticks.
- The appropriate language (e.g., `bash`, `python`, `yaml`, `json`, etc.) is
  specified for syntax highlighting.

## Collapsible Sections

You can create collapsible sections that expand when clicked — useful for
hiding long outputs, logs, or optional details.  This feature helps keep your
documentation concise while still providing extra information when needed.

**Example**:

````markdown
<details>
<summary>Expected Output</summary>

```bash
Docker version 28.0.4, build b8034c0
```

</details>
````

**Rendered Output**:

<details>
<summary>Expected Output</summary>

```bash
Docker version 28.0.4, build b8034c0
```

</details>

## Lists and Nested Lists Formatting

- Use `-` or `*` followed by a space for each bullet.
- Indent nested bullets with **4 spaces** under the parent bullet.
- You must leave a blank line before starting a list after a sentence or
  paragraph.
- No blank line is needed before a nested list under a parent bullet.

> This is because we use `mkdocs` to render documentation, which requires these
> rulee.

## Documentation commits

- For each logical change in a documentation file, create a separate,
  descriptive commit.
- Each commit should clearly describe what was changed and why, so the history
  is easy to read and review.
- Use a clear prefix, like `docs:`, to indicate the commit affects
  documentation.
- Reference related issues or MRs if applicable, to help trace changes.


**Example:**

```
docs: add 5G NSA and Faraday Cage testbench setup to Testbenches.md
```
