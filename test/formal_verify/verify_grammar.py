#!/usr/bin/env python3
"""Formální validátor bezkontextových gramatik pro IFJ26.

Počítá FIRST/FOLLOW, detekuje levou rekurzi a hlásí LL(1) konflikty.
Note: Tento kód byl vytvořen za užití nástrojů velkých jazykových modelů (Gemini 3.8 flash) a je určen k ověření formální správnosti bezkontextových gramatik používaných v parseru IFJ26.
"""

from collections import defaultdict
import sys


class GrammarVerifier:

  def __init__(self, rules: dict[str, list[list[str]]], start_symbol: str):
    self.rules = rules  # Neterminál -> list alternativ (listů tokenů)
    self.start = start_symbol
    self.nonterminals = set(rules.keys())
    self.terminals = set()
    for alts in rules.values():
      for alt in alts:
        for sym in alt:
          if sym not in self.nonterminals and sym != "EPSILON":
            self.terminals.add(sym)

    self.first: dict[str, set[str]] = defaultdict(set)
    self.follow: dict[str, set[str]] = defaultdict(set)

  def compute_first(self):
    for t in self.terminals:
      self.first[t] = {t}
    self.first["EPSILON"] = {"EPSILON"}

    changed = True
    while changed:
      changed = False
      for nt, alts in self.rules.items():
        for alt in alts:
          # alt je např. ['ID', 'ASSIGN', 'Expr']
          trailer = set()
          all_epsilon = True
          for sym in alt:
            sym_first = self.first[sym]
            trailer |= sym_first - {"EPSILON"}
            if "EPSILON" not in sym_first:
              all_epsilon = False
              break
          if all_epsilon:
            trailer.add("EPSILON")

          if not trailer.issubset(self.first[nt]):
            self.first[nt] |= trailer
            changed = True

  def compute_follow(self):
    self.follow[self.start].add("$")  # EOF marker

    changed = True
    while changed:
      changed = False
      for nt, alts in self.rules.items():
        for alt in alts:
          trailer = set(self.follow[nt])
          for sym in reversed(alt):
            if sym in self.nonterminals:
              if not trailer.issubset(self.follow[sym]):
                self.follow[sym] |= trailer
                changed = True
              if "EPSILON" in self.first[sym]:
                trailer = trailer | (self.first[sym] - {"EPSILON"})
              else:
                trailer = set(self.first[sym])
            else:
              trailer = set(self.first[sym])

  def check_left_recursion(self) -> list[str]:
    errors = []
    for nt, alts in self.rules.items():
      for alt in alts:
        if alt and alt[0] == nt:
          errors.append(
              f"Kritická přímá levá rekurze: {nt} -> {' '.join(alt)}"
          )
    return errors

  def verify_ll1(self) -> list[str]:
    self.compute_first()
    self.compute_follow()

    errors = []
    for nt, alts in self.rules.items():
      for i in range(len(alts)):
        for j in range(i + 1, len(alts)):
          alt1, alt2 = alts[i], alts[j]

          # FIRST pro alternativy
          f1 = self._first_of_seq(alt1)
          f2 = self._first_of_seq(alt2)

          inter = f1 & f2
          if inter:
            errors.append(
                f"LL(1) FIRST/FIRST konflikt v '{nt}': pravidla {alt1} a {alt2}"
                f" sdílí FIRST={inter}"
            )

          if "EPSILON" in f1:
            inter_follow = (self.follow[nt] | {"$"}) & f2
            if inter_follow:
              errors.append(
                  f"LL(1) FIRST/FOLLOW konflikt v '{nt}': eps-pravidlo {alt1}"
                  f" koliduje s FOLLOW({nt})={inter_follow}"
              )
          if "EPSILON" in f2:
            inter_follow = (self.follow[nt] | {"$"}) & f1
            if inter_follow:
              errors.append(
                  f"LL(1) FIRST/FOLLOW konflikt v '{nt}': eps-pravidlo {alt2}"
                  f" koliduje s FOLLOW({nt})={inter_follow}"
              )
    return errors

  def _first_of_seq(self, seq: list[str]) -> set[str]:
    res = set()
    all_eps = True
    for s in seq:
      res |= self.first[s] - {"EPSILON"}
      if "EPSILON" not in self.first[s]:
        all_eps = False
        break
    if all_eps:
      res.add("EPSILON")
    return res


# --- Příklad gramatiky IFJ (Parser těla a přiřazení) ---
grammar = {
    "Program": [["StatementList"]],
    "StatementList": [["Statement", "StatementList"], ["EPSILON"]],
    "Statement": [["VAR", "ID", "ASSIGN", "Expr", "SEMICOLON"]],
    "Expr": [["ID"], ["INT_LIT"]],
}

verifier = GrammarVerifier(grammar, "Program")
rec_errs = verifier.check_left_recursion()
ll1_errs = verifier.verify_ll1()

if not rec_errs and not ll1_errs:
  print("✓ Gramatika je 100% LL(1) validní bez rekurzí.")
else:
  for e in rec_errs + ll1_errs:
    print(f"✗ {e}")