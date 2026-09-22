#!/usr/bin/env python3
import subprocess
import sys
from verify_dka import DFAValidator, Transition # import tvého modulu

def check_scala_literal(literal: str) -> tuple[bool, str]:
    """
    Ověří platnost literálu pomocí Scala 3 kompilátoru.
    Vrací (is_valid, typ_nebo_chyba).
    """
    # Zabalíme testovaný literál do minimálního platného Scala programu
    # Používáme scala-cli (nebo scalac)
    code = f'println({{ val _x = {literal}; _x.getClass.getSimpleName }})'
    
    try:
        proc = subprocess.run(
            ["scala-cli", "eval", "--suppress-experimental-feature-warning", code],
            capture_output=True,
            text=True,
            timeout=5
        )
        if proc.returncode == 0:
            # Scala výraz přijala
            scala_type = proc.stdout.strip()
            return True, scala_type
        else:
            # Scala vyhodila syntaktickou/typovou chybu
            return False, proc.stderr.strip()
    except subprocess.TimeoutExpired:
        return False, "Timeout"
    except FileNotFoundError:
        print("Chyba: 'scala-cli' není nainstalován v PATH!", file=sys.stderr)
        sys.exit(1)

def run_differential_tests(validator: DFAValidator, test_cases: list[str]):
    print(f"{'Vstup':<18} | {'Náš DKA':<20} | {'Scala 3':<20} | {'Výsledek':<10}")
    print("-" * 75)

    mismatches = 0
    for literal in test_cases:
        # 1. Spuštění našeho DKA / simulátoru
        dka_ok, dka_tok, dka_detail = validator.simulate(literal)
        
        # 2. Spuštění referenční Scaly
        scala_ok, scala_info = check_scala_literal(literal)
        
        # Srovnání rozhodnutí: Shodují se v platnosti?
        match = (dka_ok == scala_ok)
        
        # Zvláštní pravidlo IFJ26: Scala může přijímat věci, které IFJ26 nepovoluje (např. sufix 'L' nebo 1.),
        # ale cokoliv co IFJ26 přijme, MUSÍ přijmout i Scala.
        status_str = "OK" if match else "NESHODA!"
        if not match:
            mismatches += 1

        dka_res = dka_tok if dka_ok else "REJECT"
        scala_res = "ACCEPT" if scala_ok else "REJECT"

        print(f"{literal:<18} | {dka_res:<20} | {scala_res:<20} | {status_str:<10}")

    print("-" * 75)
    if mismatches == 0:
        print("Všechny testy v naprosté shodě s referenční Scalou!")
    else:
        print(f"Pozor: Nalezeno {mismatches} neshod mezi DKA a Scalou!")

if __name__ == "__main__":
    # Importuj nebo inicializuj svůj validator
    # ...
    # validator = ...
    
    test_suite = [
        # Běžné validní případy
        "0", "42", "1_000_000", "3.1415", "0.5", "1e-5", "2.5e+10",
        # Hraniční chyby
        "0123",      # Zakázané úvodní nuly v celých číslech
        "1__000",    # Dvojité podtržítko
        "100_",      # Podtržítko na konci
        "1.",        # Tečka bez desetinné části
        ".5",        # IFJ26 vs Scala chování
        "1e",        # Exponent bez číslic
        "1e+",       # Exponent bez číslic za znaménkem
        "_123",      # Identifikátor, nikoliv číslo
    ]
    
    # run_differential_tests(validator, test_suite)