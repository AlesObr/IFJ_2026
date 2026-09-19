#!/usr/bin/env python3
"""
Validátor DKA a generátor testovacích řetězců pro Scanner.
Note: Tento kód byl vytvořen pro účely testování a validace deterministického konečného automatu (DKA) používaného ve skeneru.
Byl vygenerován pomocí nástrojů velkých jazykových modelů(Gemini 3.8 flash) a je určen k ověření determinismu, detekci nedosažitelných stavů a generování minimálních testovacích vzorků pro každý koncový stav automatu.
"""


from collections import deque
from dataclasses import dataclass
import sys


@dataclass(frozen=True)
class Transition:
  from_state: str
  symbol_class: str  # např. 'digit', 'alpha', '_', '/', '*'
  to_state: str


class DFAValidator:

  def __init__(
      self,
      states: set[str],
      alphabet: set[str],
      transitions: list[Transition],
      start_state: str,
      final_states: dict[str, str],
  ):
    self.states = states
    self.alphabet = alphabet
    self.transitions = transitions
    self.start_state = start_state
    self.final_states = (
        final_states  # Stav -> Typ tokenu (např. 'STATE_INT': 'TOKEN_INT')
    )

  def verify_determinism(self) -> list[str]:
    """Ověří, že z každého stavu vede pro danou třídu znaků max 1 přechod."""
    seen = {}
    errors = []
    for t in self.transitions:
      key = (t.from_state, t.symbol_class)
      if key in seen:
        errors.append(
            f"Nedeterminismus! Ze stavu '{t.from_state}' pro třídu"
            f" '{t.symbol_class}' vede přechod do '{seen[key]}' i"
            f" '{t.to_state}'."
        )
      seen[key] = t.to_state
    return errors

  def find_unreachable_states(self) -> set[str]:
    """Detekuje mrtvé stavy v diagramu."""
    reachable = {self.start_state}
    queue = deque([self.start_state])

    adj = {s: [] for s in self.states}
    for t in self.transitions:
      adj[t.from_state].append(t.to_state)

    while queue:
      curr = queue.popleft()
      for nxt in adj.get(curr, []):
        if nxt not in reachable:
          reachable.add(nxt)
          queue.append(nxt)

    return self.states - reachable

  def generate_positive_samples(self) -> list[tuple[str, str]]:
    """Prohledáváním stavového prostoru vygeneruje nejkratší řetězec pro každý koncový stav."""
    paths = {self.start_state: ""}
    queue = deque([self.start_state])
    samples = []
    found_tokens = set()

    # Reprezentant pro každou třídu symbolů
    sample_char = {
        "digit": "5",
        "alpha": "a",
        "_": "_",
        "slash": "/",
        "star": "*",
        "quote": '"',
    }

    while queue:
      curr = queue.popleft()
      curr_path = paths[curr]

      if curr in self.final_states and self.final_states[curr] not in found_tokens:
        samples.append((curr_path, self.final_states[curr]))
        found_tokens.add(self.final_states[curr])

      for t in self.transitions:
        if t.from_state == curr and t.to_state not in paths:
          paths[t.to_state] = curr_path + sample_char.get(t.symbol_class, "?")
          queue.append(t.to_state)

    return samples


# --- Ukázka definice automatu pro scanner (čísla, identifikátory, komentáře) ---
states = {
    "START",
    "INT",
    "ID",
    "SLASH",
    "LINE_COMMENT",
    "BLOCK_COMMENT",
    "BLOCK_STAR",
}
alphabet = {"digit", "alpha", "_", "slash", "star"}
transitions = [
    Transition("START", "digit", "INT"),
    Transition("INT", "digit", "INT"),
    Transition("START", "alpha", "ID"),
    Transition("START", "_", "ID"),
    Transition("ID", "alpha", "ID"),
    Transition("ID", "digit", "ID"),
    Transition("START", "slash", "SLASH"),
    Transition("SLASH", "slash", "LINE_COMMENT"),
    Transition("SLASH", "star", "BLOCK_COMMENT"),
    Transition("BLOCK_COMMENT", "star", "BLOCK_STAR"),
    Transition("BLOCK_STAR", "slash", "START"),  # Blokový komentář skončil
]
final_states = {"INT": "TOKEN_INT", "ID": "TOKEN_IDENTIFIER", "SLASH": "TOKEN_DIV"}

dfa = DFAValidator(states, alphabet, transitions, "START", final_states)

# Validace
errors = dfa.verify_determinism()
unreachable = dfa.find_unreachable_states()

if errors:
  print("Chyby determinismu:", errors)
if unreachable:
  print("Nedosažitelné stavy v DKA:", unreachable)

print("Automaticky vygenerované minimální vzorky z DKA:")
for string_sample, token_type in dfa.generate_positive_samples():
  print(f"  Token {token_type:<16} reprezentován řetězcem: '{string_sample}'")