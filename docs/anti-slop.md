# Anti-Slop Rules

All agents must follow these rules in every output: code comments, commit messages, bug analyses, log explanations, PR descriptions, chat responses, and documentation.

Sources:
- [Cursor unslop](https://github.com/cursor/plugins/tree/main/pstack/skills/unslop)
- [No AI Slop](https://github.com/petergyang/no-ai-slop)
- [Impeccable](https://github.com/pbakaus/impeccable) (design slop, relevant when working on Qt UI)

## Banned words and phrases

Delete on sight. Replace with the plain word.

**Banned vocabulary:** delve, foster, leverage, utilize, facilitate, empower, streamline, robust, cutting-edge, paradigm shift, game changer, this is huge, this changes everything, tapestry, realm, beacon, multifaceted, meticulous, intricate, paramount, transformative, elevate, embark, supercharge, harness, ever-evolving, additionally, crucial, enduring, enhance, garner, interplay, landscape (abstract), pivotal, showcase, testament, underscore, vibrant, substrate, wedge, vector (meaning "method"), locus, vantage, nexus, primitive (as noun), modality, scaffolding (metaphorical), bedrock, gold-plating, evacuate (for moving code), endgame, north star, flywheel.

**Fancy ways to say "is":** "serves as", "stands as", "boasts", "features". Say "is" or "has".

**Empty adverbs:** just, literally, honestly, simply, actually, truly, fundamentally, importantly, crucially, inherently, inevitably. Cut unless they carry real emphasis.

**Empty phrases:** it's worth noting, it's important to note, at the end of the day, when it comes to, at its core, in today's world, in the age of, in the world of, the reality is, the truth is, in terms of, with regard to, in order to, going forward, let's dive in, here's the thing, let me be clear, the uncomfortable truth is.

**Chatbot phrases:** "I hope this helps!", "Let me know if...", "Of course!", "Certainly!", "Great question!", "Found the smoking gun!", "You're absolutely right!" Remove.

## Patterns to cut

**Binary contrasts.** "It's not X. It's Y." / "The question isn't X, it's Y." State Y directly.

**Throat-clearing openers.** "Here's the thing," "Here's what I mean," cut them and state the point.

**Faux-insight setups.** "What nobody tells you," "The part everyone misses," "Here's what most people get wrong." Cut the setup, make the claim stand alone.

**Superficial -ing phrases.** "highlighting...", "ensuring...", "reflecting...", "showcasing...", "fostering..." Delete or expand with real content.

**Vague attributions.** "Experts believe", "Industry reports suggest", "Some critics argue". Name the source or delete.

**Importance puffery.** "Stands as a testament," "marks a pivotal moment," "plays a vital role," "solidifies its position." State the fact, let the reader judge.

**Weasel attribution.** "Experts agree," "studies show," "widely regarded as." Name the source or cut.

**Synonym cycling.** Using protagonist, main character, central figure, hero in the same paragraph. Pick one, repeat it.

**Colon reveals.** Noun phrase, colon, dramatic reveal. Use colons for lists, not fake drama.

**Fake-profound endings.** "The future isn't coming. It's already here." Delete, end on the last concrete point.

**Summary-recap endings.** "In conclusion," "Ultimately," "Overall," restating what was just said. End on the last concrete point or next action.

**Dramatic fragments.** "X. And Y. And Z." or "That's it. That's the whole thing." Use complete sentences.

**Negative listing.** "Not a X. Not a Y. A Z." Just say Z.

**Rhetorical setups.** "What if I told you...", "Think about it:", self-answered "Question? Answer." pairs. Drop and make the point.

## Formatting rules

- **No em dashes.** Use periods or commas. Never em dashes, en dashes as dashes, or parentheses as crutches.
- **No colon overuse.** Colons before lists are fine. Mid-sentence connector colons are not.
- **No boldface overuse.** Don't bold every noun or acronym.
- **No inline-header lists** where the bold label restates the line: "**Performance:** Performance improved..." Convert to prose.
- **Sentence case headings.** Not Title Case.
- **No decorative emojis** in headings or bullets.
- **Straight quotes.** Not curly.

## Writing rules

- **Active voice.** "The compiler validates queries" not "queries are validated." Passive only when the actor is unknown.
- **Cut adverbs or use a stronger verb.** "runs quickly" → "is fast" or the number. "significantly improves" → the measured delta.
- **One idea per sentence.** If the reader has to backtrack, split it.
- **Rule of three:** Don't force ideas into groups of three. Use the natural number.
- **Filler phrases:** "In order to" → "To". "Due to the fact that" → "Because". Delete "It is important to note that".
- **Over-compression:** Dropped articles, verbless fragments, symbol-speak. Write whole sentences. "Parser rejects bad date → exit 2" becomes "The parser rejects a bad date and exits with code 2."
- **Mannered prose:** Metaphor where a literal phrase exists. "A dial worth turning" → "a parameter worth varying." Say what you mean.
- **Say what it does, not how it feels.** "the database stays close at hand" → "`.toSQL()` returns the exact string sent to the database." If a sentence could appear unchanged in another project's docs, it says nothing about this one. Cut it.
- **Portability test:** If a sentence could move unchanged to another person, company, or product, it is filler. Replace with a fact, example, or mechanism specific to this subject.

## For code specifically

- Comments should explain **why**, not **what**. If the code is unclear, fix the code.
- Don't write comments that restate the code: `// increment i` above `i++` is slop.
- Don't pad commit messages with filler. Lead with what changed and why.
- Bug analyses: lead with the root cause, then evidence. Don't bury the answer.

## Design (when working on Qt UI)

When working on the emulator's Qt frontend, also follow [Impeccable](https://impeccable.style) principles:
- No Inter-everywhere, no purple-to-blue gradients, no cards-in-cards, no gray text on colored backgrounds, no rounded-square icon tiles above every heading.
- Run `npx impeccable detect` on UI changes when possible.
- See [impeccable.style/docs](https://impeccable.style/docs) for the full playbook.
