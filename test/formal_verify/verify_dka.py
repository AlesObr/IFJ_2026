#!/usr/bin/env python3
"""
Validátor DKA, simulátor a generátor pro IFJ26 Scanner.
Nástroj ověřuje determinismus, detekuje nedosažitelné stavy a slepé uličky (dead-ends),
simuluje běh automatu nad konkrétními řetězci, generuje minimální vzorky
a exportuje graf do formátu DOT (Graphviz).
"""

from collections import deque
from dataclasses import dataclass
import sys


@dataclass(frozen=True)
class Transition:
    from_state: str
    symbol_class: str  # např. 'digit', 'alpha', '_', '/', '*', 'other'
    to_state: str


class DFAValidator:

    def __init__(
        self,
        states: set[str],
        alphabet: set[str],
        transitions: list[Transition],
        start_state: str,
        final_states: dict[str, str],
        sink_states: set[str] | None = None,
    ):
        self.states = states
        self.alphabet = alphabet
        self.transitions = transitions
        self.start_state = start_state
        self.final_states = final_states  # Stav -> Typ tokenu
        self.sink_states = sink_states or set()  # Záměrné chybové/záchytné stavy

    def verify_determinism(self) -> list[str]:
        """Ověří, že z každého stavu vede pro danou třídu znaků max 1 přechod."""
        seen = {}
        errors = []
        for t in self.transitions:
            key = (t.from_state, t.symbol_class)
            if key in seen:
                errors.append(
                    f"Nedeterminismus! Ze stavu '{t.from_state}' pro třídu "
                    f"'{t.symbol_class}' vede přechod do '{seen[key]}' i '{t.to_state}'."
                )
            seen[key] = t.to_state
        return errors

    def find_unreachable_states(self) -> set[str]:
        """Detekuje stavy, do kterých se nelze dostat z počátečního stavu."""
        reachable = {self.start_state}
        queue = deque([self.start_state])

        adj = {s: [] for s in self.states}
        for t in self.transitions:
            adj.setdefault(t.from_state, []).append(t.to_state)

        while queue:
            curr = queue.popleft()
            for nxt in adj.get(curr, []):
                if nxt not in reachable and nxt in self.states:
                    reachable.add(nxt)
                    queue.append(nxt)

        return self.states - reachable

    def find_dead_end_states(self) -> set[str]:
        """Detekuje stavy, ze kterých nelze dosáhnout žádného koncového stavu (mimo definované sink stavy)."""
        can_reach_final = set(self.final_states.keys())
        queue = deque(list(self.final_states.keys()))

        # Inverzní seznam sousedů: to_state -> [from_state]
        reverse_adj = {s: [] for s in self.states}
        for t in self.transitions:
            reverse_adj.setdefault(t.to_state, []).append(t.from_state)

        while queue:
            curr = queue.popleft()
            for prev in reverse_adj.get(curr, []):
                if prev not in can_reach_final and prev in self.states:
                    can_reach_final.add(prev)
                    queue.append(prev)

        # Stavy, ze kterých se nelze dostat do finále a nejsou to explicitní chybové sinky
        return (self.states - can_reach_final) - self.sink_states

    def _char_to_class(self, ch: str) -> str:
        """Mapování konkrétního znaku na obecnou třídu symbolů."""
        if ch.isdigit():
            return "digit"
        if ch.isalpha():
            return "alpha"
        return ch

    def simulate(self, input_string: str) -> tuple[bool, str | None, str]:
        """Simuluje průchod automatu zadaným řetězcem znaků.

        Vrací trojici: (akceptováno, typ_tokenu, koncový_stav_nebo_popis_chyby).
        """
        curr = self.start_state
        for i, ch in enumerate(input_string):
            sym_cls = self._char_to_class(ch)
            nxt = None
            fallback = None

            # Hledání přechodu: přednost má přesný znak, pak třída znaků, nakonec 'other'
            for t in self.transitions:
                if t.from_state == curr:
                    if t.symbol_class == ch:
                        nxt = t.to_state
                        break
                    elif t.symbol_class == sym_cls:
                        nxt = t.to_state
                    elif t.symbol_class == "other" and not fallback:
                        fallback = t.to_state

            target = nxt or fallback
            if not target:
                return (
                    False,
                    None,
                    f"Zaseknuto ve stavu '{curr}' na znaku '{ch}' (pozice {i})",
                )
            curr = target

        if curr in self.final_states:
            return (True, self.final_states[curr], curr)
        return (False, None, f"Řetězec skončil v nekoncovém stavu '{curr}'")

    def generate_positive_samples(self) -> list[tuple[str, str]]:
        """Prohledáváním stavového prostoru vygeneruje nejkratší řetězec pro každý koncový stav."""
        paths = {self.start_state: ""}
        queue = deque([self.start_state])
        samples = []
        found_tokens = set()

        sample_char = {
            "digit": "5",
            "alpha": "a",
            "_": "_",
            "/": "/",
            "*": "*",
            '"': '"',
            ".": ".",
            "e": "e",
            "+": "+",
            "-": "-",
            "other": "#",
        }

        while queue:
            curr = queue.popleft()
            curr_path = paths[curr]

            if curr in self.final_states and self.final_states[curr] not in found_tokens:
                samples.append((curr_path, self.final_states[curr]))
                found_tokens.add(self.final_states[curr])

            for t in self.transitions:
                if t.from_state == curr and t.to_state not in paths:
                    char_repr = sample_char.get(t.symbol_class, t.symbol_class)
                    paths[t.to_state] = curr_path + char_repr
                    queue.append(t.to_state)

        return samples

    def to_graphviz(self) -> str:
        """Vygeneruje kompletní DOT kód pro vizualizaci přes Graphviz."""
        lines = [
            "digraph DFA {",
            "    rankdir=LR;",
            '    node [shape=circle, fontname="Helvetica"];',
            '    edge [fontname="Helvetica", fontsize=10];',
            "",
            "    // Počáteční stav",
            '    __start [shape=point, width=0.2, label=""];',
            f'    __start -> "{self.start_state}";',
            "",
            "    // Koncové stavy",
        ]

        for s, token in self.final_states.items():
            lines.append(f'    "{s}" [shape=doublecircle, label="{s}\\n({token})"];')

        if self.sink_states:
            lines.append("\n    // Chybové / Sink stavy")
            for s in self.sink_states:
                lines.append(f'    "{s}" [color=red, fontcolor=red];')

        lines.append("\n    // Přechody")
        for t in self.transitions:
            lines.append(f'    "{t.from_state}" -> "{t.to_state}" [label="{t.symbol_class}"];')

        lines.append("}")
        return "\n".join(lines)


# ==============================================================================
# Ukázka použití: Podautomat pro čísla (Int / Double)
# ==============================================================================
if __name__ == "__main__":
    states = {
        "START",
        "NUM_ZERO",
        "NUM_INT",
        "NUM_INT_USCORE",
        "NUM_DOT",
        "NUM_DOUBLE",
        "NUM_DOUBLE_USCORE",
        "NUM_EXP_START",
        "NUM_EXP_SIGN",
        "NUM_EXP_DIGITS",
        "SINK_ERROR",
    }

    alphabet = {"digit", "_", ".", "e", "E", "+", "-", "other"}

    transitions = [
        # Celá čísla
        Transition("START", "0", "NUM_ZERO"),
        Transition("START", "digit", "NUM_INT"),
        Transition("NUM_INT", "digit", "NUM_INT"),
        Transition("NUM_INT", "_", "NUM_INT_USCORE"),
        Transition("NUM_INT_USCORE", "digit", "NUM_INT"),
        # Desetinná čísla
        Transition("NUM_INT", ".", "NUM_DOT"),
        Transition("NUM_ZERO", ".", "NUM_DOT"),
        Transition("NUM_DOT", "digit", "NUM_DOUBLE"),
        Transition("NUM_DOUBLE", "digit", "NUM_DOUBLE"),
        Transition("NUM_DOUBLE", "_", "NUM_DOUBLE_USCORE"),
        Transition("NUM_DOUBLE_USCORE", "digit", "NUM_DOUBLE"),
        # Exponent
        Transition("NUM_INT", "e", "NUM_EXP_START"),
        Transition("NUM_DOUBLE", "e", "NUM_EXP_START"),
        Transition("NUM_EXP_START", "+", "NUM_EXP_SIGN"),
        Transition("NUM_EXP_START", "-", "NUM_EXP_SIGN"),
        Transition("NUM_EXP_START", "digit", "NUM_EXP_DIGITS"),
        Transition("NUM_EXP_SIGN", "digit", "NUM_EXP_DIGITS"),
        Transition("NUM_EXP_DIGITS", "digit", "NUM_EXP_DIGITS"),
        # Příklad přechodu do chybového stavu
        Transition("NUM_INT_USCORE", "other", "SINK_ERROR"),
    ]

    final_states = {
        "NUM_ZERO": "TOKEN_LIT_INT",
        "NUM_INT": "TOKEN_LIT_INT",
        "NUM_DOUBLE": "TOKEN_LIT_DOUBLE",
        "NUM_EXP_DIGITS": "TOKEN_LIT_DOUBLE",
    }

    sink_states = {"SINK_ERROR"}

    validator = DFAValidator(
        states=states,
        alphabet=alphabet,
        transitions=transitions,
        start_state="START",
        final_states=final_states,
        sink_states=sink_states,
    )

    print("=== KONTROLA STRUKTURY AUTOMATU ===")
    errs = validator.verify_determinism()
    unreachable = validator.find_unreachable_states()
    dead_ends = validator.find_dead_end_states()

    print(f"Chyby determinismu: {errs if errs else 'Žádné'}")
    print(f"Nedosažitelné stavy: {unreachable if unreachable else 'Žádné'}")
    print(f"Slepé uličky (Dead-ends): {dead_ends if dead_ends else 'Žádné'}")

    print("\n=== SIMULACE BĚHU ===")
    test_inputs = ["123", "0", "1_000", "3.14", "1e-5", "10_", "1.e2"]
    for inp in test_inputs:
        ok, tok, detail = validator.simulate(inp)
        status = f"PŘIJATO ({tok})" if ok else f"ODMÍTNUTO ({detail})"
        print(f"  '{inp:<8}' -> {status}")

    print("\n=== MINIMÁLNÍ TESTOVACÍ VZORKY ===")
    for sample, tok in validator.generate_positive_samples():
        print(f"  Token {tok:<18} generován vstupem: '{sample}'")

    # Pro uložení DOT souboru odkomentuj:
    # with open("dfa.dot", "w") as f:
    #     f.write(validator.to_graphviz())