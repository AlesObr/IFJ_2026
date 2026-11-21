#!/usr/bin/env python3
"""Writes the hand-written semantic test cases (one rule of the specification per file)."""
from pathlib import Path

OUT = Path(__file__).resolve().parent / "cases"

# name -> (spec reference / description, top-level definitions, body of main)
# Main is "@main def main(): Unit =" with the body indented by two spaces; None = no main.
CASES = {
    # ------------------------------------------------------------------ valid
    "ok_minimal": ("2: nejmensi program", "", 'print("ok")'),
    "ok_decl_int_to_double": ("1.3: implicitni konverze Int -> Double v deklaraci", "",
                              "val x: Double = 1\nprint(x)"),
    "ok_param_int_to_double": ("1.3: konverze Int -> Double u argumentu",
                               "def half(x: Double): Double = x / 2\n", "val h = half(3)\nprint(h)"),
    "ok_return_int_to_double": ("1.3: konverze Int -> Double u navratove hodnoty",
                                "def one(): Double = 1\n", "val x = one()\nprint(x)"),
    "ok_assign_int_to_double_var": ("1.3: konverze pri prirazeni", "", "var d = 1.5\nd = 2\nprint(d)"),
    "ok_mixed_arith": ("3: Int a Double v aritmetice", "", "val x = 1 + 2.5\nval y = 2 * 0.5 - 1\nprint(x)\nprint(y)"),
    "ok_mixed_compare": ("3: relacni operatory nad Int a Double", "",
                         'if 1 < 2.5 then\n  print("a")\nelse\n  print("b")'),
    "ok_mixed_equality": ("3: == nad dvema ciselnymi hodnotami", "",
                          'if 1 == 1.0 then\n  print("a")\nelse\n  print("b")'),
    "ok_string_compare": ("3: lexikograficke porovnani retezcu", "",
                          'if "a" < "b" then\n  print("a")\nelse\n  print("b")'),
    "ok_string_equality": ("3: == nad retezci", "",
                           'val s = "x"\nif s != "y" then\n  print(s)\nelse\n  ()'),
    "ok_concat": ("3: konkatenace dvou retezcu", "", 'val s = "a" + "b"\nprint(s)'),
    "ok_unary_minus": ("3: unarni minus nad Int i Double", "", "val a = -5\nval b = -2.5\nprint(a)\nprint(b)"),
    "ok_shadowing_inner_block": ("2.2: prekryti promenne v podbloku", "",
                                 'val x = 1\nif x < 2 then\n  val x = "inner"\n  print(x)\nelse\n  ()\nprint(x)'),
    "ok_shadowing_in_loop": ("2.2: definice v tele cyklu (kazda iterace nova)", "",
                             "var i = 0\nwhile i < 3 do\n  val sq = i * i\n  print(sq)\n  i = i + 1"),
    "ok_same_name_in_sibling_blocks": ("2.2: stejne jmeno ve dvou vetvich", "",
                                       "if 1 < 2 then\n  val t = 1\n  print(t)\nelse\n  val t = 2\n  print(t)"),
    "ok_forward_call": ("2.3: volani pred lexikalni definici", "",
                        "val v = later(2)\nprint(v)"),
    "ok_mutual_recursion": ("2.3: vzajemna rekurze s explicitnimi typy",
                            "def isEven(n: Int): Int =\n  if n == 0 then\n    return 1\n  else\n    val m = n - 1\n    return isOdd(m)\n\n"
                            "def isOdd(n: Int): Int =\n  if n == 0 then\n    return 0\n  else\n    val m = n - 1\n    return isEven(m)\n",
                            "val r = isEven(4)\nprint(r)"),
    "ok_inferred_return_type": ("2.3: odvozeny navratovy typ", "def five() = 5\n", "val x: Int = five()\nprint(x)"),
    "ok_inferred_unit_return": ("2.3: odvozeny Unit (blok konci prirazenim)",
                                "def setter() =\n  var a = 1\n  a = 2\n", "setter()"),
    "ok_unit_variable": ("1.3: Unit jako typ promenne", "", "val u: Unit = ()\nprint(u)"),
    "ok_unit_parameter": ("1.3: Unit jako typ parametru", "def show(u: Unit): Unit = print(u)\n", "show(())"),
    "ok_if_last_all_return": ("2.3 pravidlo 3: if na konci tela, vsechny vetve konci return",
                              "def sign(x: Int): Int =\n  if x < 0 then\n    return 0\n  else\n    return 1\n",
                              "val s = sign(5)\nprint(s)"),
    "ok_if_last_unit_explicit": ("2.3 pravidlo 3: if na konci tela funkce s typem Unit",
                                 'def say(x: Int): Unit =\n  if x < 0 then\n    print("neg")\n  else\n    print("pos")\n',
                                 "say(1)"),
    "ok_if_last_inferred_unit": ("2.3 pravidlo 3: if na konci, typ neuveden, vetve typu Unit",
                                 'def say(x: Int) =\n  if x < 0 then\n    print("neg")\n  else\n    print("pos")\n',
                                 "say(1)"),
    "ok_unit_function_discards_value": ("2.3 pravidlo 4: hodnota v Unit funkci se zahodi",
                                        "def ignore(): Unit = 5\n", "ignore()"),
    "ok_unit_function_return_value": ("2.3 pravidlo 4: i return v Unit funkci se zahodi",
                                      "def ignore(): Unit =\n  return 5\n", "ignore()"),
    "ok_bare_return_unit": ("2.3 pravidlo 1: samostatny return ve funkci s Unit",
                            'def early(x: Int): Unit =\n  if x < 0 then\n    return\n  else\n    print("x")\n  print("end")\n',
                            "early(1)"),
    "ok_return_in_loop": ("2.3 pravidlo 1: return z cyklu",
                          "def first(): Int =\n  var i = 0\n  while i < 10 do\n    if i == 3 then\n      return i\n    else\n      i = i + 1\n  0\n",
                          "val f = first()\nprint(f)"),
    "ok_call_as_statement": ("2.3: volani jako samostatny prikaz (vysledek se zahodi)",
                             "def value(): Int = 3\n", "value()"),
    "ok_builtins": ("4: vsechny vestavene funkce se spravnymi typy", "",
                    'val line = readLine()\nval i = readInt()\nval d = readDouble()\nval s = toStr(d)\n'
                    'val t = toInt(d)\nval n = strLength(s)\nval sub = subStr("abcd", 1, 3)\nval o = ord(sub, 0)\n'
                    'val c = chr(o)\nprint(line)\nprint(i)\nprint(t)\nprint(n)\nprint(c)'),
    "ok_toint_from_int": ("4 + 1.3: toInt s argumentem Int (implicitni konverze)", "", "val t = toInt(3)\nprint(t)"),
    "ok_print_any": ("4: print s libovolnym typem", "", 'print(1)\nprint(2.5)\nprint("s")\nprint(())'),
    "ok_tostr_any": ("4: toStr s libovolnym typem", "",
                     'val a = toStr(1)\nval b = toStr("s")\nval c = toStr(())\nprint(a)\nprint(b)\nprint(c)'),
    "ok_main_custom_name": ("2: vstupni funkce s libovolnym jmenem", None,
                            '@main def start(): Unit =\n  print("start")'),
    "ok_main_annotation_own_line": ("2: anotace @main na samostatnem radku", None,
                                    '@main\ndef start(): Unit =\n  print("start")'),
    "ok_typed_declarations": ("2.2: deklarace s uvedenym typem", "",
                              'val a: Int = 1\nval b: Double = 2.5\nval c: String = "c"\nvar d: Int = a\nd = 3\nprint(d)'),
    "ok_var_reassign": ("2.2: opakovane prirazeni do var", "", "var x = 1\nx = 2\nx = x + 1\nprint(x)"),
    "ok_funexp_nested_calls": ("5.1 FUNEXP: volani ve vyrazech a vnorena volani",
                               "def inc(x: Int): Int = x + 1\n",
                               'val r = 2 * inc(inc(1)) + strLength(subStr("abcd", 1, 3))\nprint(toStr(r))'),

    # ------------------------------------------------------------------ 3
    "e3_undefined_variable": ("6/2.2: pouziti nedefinovane promenne", "", "print(missing)"),
    "e3_use_before_definition": ("2.2: pouziti promenne pred definici", "", "print(x)\nval x = 1"),
    "e3_out_of_scope_branch": ("2.2: promenna z vetve if mimo jeji rozsah", "",
                               "if 1 < 2 then\n  val y = 1\n  print(y)\nelse\n  ()\nprint(y)"),
    "e3_out_of_scope_loop": ("2.2: promenna z tela cyklu mimo cyklus", "",
                             "var i = 0\nwhile i < 1 do\n  val inner = i\n  i = i + 1\nprint(inner)"),
    "e3_self_reference_init": ("2.2: inicializace odkazuje na sebe sama (jeste nedefinovana)", "",
                               "val x = x + 1\nprint(x)"),
    "e3_undefined_function": ("6: volani nedefinovane funkce", "", "foo(1)"),
    "e3_other_function_param": ("2.3: parametr jine funkce neni viditelny",
                                "def f(secret: Int): Int = secret\n", "print(secret)"),
    "e3_missing_main": ("2: chybi funkce s anotaci @main", None, 'def notMain(): Unit =\n  print("x")'),
    "e3_assign_undefined": ("2.2: prirazeni do nedefinovane promenne (3 ma prednost pred 7)", "",
                            "missing = 1"),

    # ------------------------------------------------------------------ 4
    "e4_redefine_variable": ("2.2: opakovana definice ve stejnem bloku", "", "val x = 1\nval x = 2\nprint(x)"),
    "e4_redefine_var_as_val": ("2.2: var i val stejneho jmena ve stejnem bloku", "", "var x = 1\nval x = 2\nprint(x)"),
    "e4_redefine_function": ("2.3: dve definice stejne funkce",
                             "def twice(x: Int): Int = x * 2\n\ndef twice(x: Int): Int = x + x\n", "print(twice(1))"),
    "e4_overload": ("2.3: pretezovani neni podporovano",
                    "def show(x: Int): Unit = print(x)\n\ndef show(x: String): Unit = print(x)\n", "show(1)"),
    "e4_two_mains": ("2: vice vstupnich funkci", "@main def other(): Unit =\n  print(1)\n", "print(2)"),
    "e4_duplicate_parameter": ("2.3: dva parametry stejneho jmena", "def f(a: Int, a: Int): Int = a\n", "print(f(1, 2))"),

    # ------------------------------------------------------------------ 5
    "e5_too_many_arguments": ("2.3: prilis mnoho argumentu", "def one(x: Int): Int = x\n", "val r = one(1, 2)\nprint(r)"),
    "e5_too_few_arguments": ("2.3: prilis malo argumentu", "def two(x: Int, y: Int): Int = x + y\n",
                             "val r = two(1)\nprint(r)"),
    "e5_wrong_argument_type": ("2.3: nekompatibilni typ argumentu", "def one(x: Int): Int = x\n",
                               'val r = one("s")\nprint(r)'),
    "e5_double_for_int_parameter": ("1.3: Double na miste Int parametru (opacna konverze neni)",
                                    "def one(x: Int): Int = x\n", "val r = one(1.5)\nprint(r)"),
    "e5_builtin_wrong_type": ("4: spatny typ argumentu vestavene funkce", "", "val n = strLength(5)\nprint(n)"),
    "e5_builtin_wrong_count": ("4: spatny pocet argumentu vestavene funkce", "", "print(1, 2)"),
    "e5_toint_string": ("4: toInt se String argumentem", "", 'val t = toInt("1")\nprint(t)'),
    "e5_substr_wrong_type": ("4: subStr s Double indexem", "", 'val s = subStr("abc", 0.5, 2)\nprint(s)'),
    "e5_main_with_parameter": ("2: vstupni funkce s parametrem", None, "@main def main(x: Int): Unit =\n  print(x)"),

    # ------------------------------------------------------------------ 6
    "e6_decl_string_to_int": ("2.2: inicializace nekompatibilnim typem", "", 'val x: Int = "a"\nprint(x)'),
    "e6_decl_double_to_int": ("1.3: Double do Int (opacna konverze neni)", "", "val x: Int = 1.5\nprint(x)"),
    "e6_assign_string_to_int": ("2.2: prirazeni jineho typu", "", 'var x = 1\nx = "a"\nprint(x)'),
    "e6_assign_double_to_int": ("1.3: prirazeni Double do Int promenne", "", "var x = 1\nx = 2.5\nprint(x)"),
    "e6_assign_unit_to_int": ("2.2: prirazeni Unit do Int promenne", "", "var x = 1\nx = ()\nprint(x)"),
    "e6_add_int_string": ("3: aritmetika nad Int a String", "", 'val x = 1 + "a"\nprint(x)'),
    "e6_concat_auto_conversion": ("3: konkatenace bez toStr (automaticka konverze neni)", "",
                                  'val x = "a" + 1\nprint(x)'),
    "e6_sub_strings": ("3: odcitani retezcu", "", 'val x = "a" - "b"\nprint(x)'),
    "e6_unit_arithmetic": ("3: aritmetika s Unit", "", "val x = () + 1\nprint(x)"),
    "e6_compare_string_int": ("3: relace String a Int", "", 'if "a" < 1 then\n  print(1)\nelse\n  print(2)'),
    "e6_equality_different_types": ("3: == nad ruznymi necislenymi typy", "",
                                    'if "a" == 1 then\n  print(1)\nelse\n  print(2)'),
    "e6_unary_minus_string": ("3: unarni minus nad String", "", 'val x = -"a"\nprint(x)'),
    "e6_condition_not_relational": ("2.4: podminka neni relacni vyraz", "", "if 1 then\n  print(1)\nelse\n  print(2)"),
    "e6_while_condition_string": ("2.5: podminka cyklu typu String", "", 'while "a" do\n  print(1)'),
    "e6_return_type_mismatch": ("2.3 pravidlo 4: vysledek nekompatibilni s navratovym typem",
                                'def name(): Int = "a"\n', "print(name())"),
    "e6_return_double_from_int": ("2.3: Double vraceny z Int funkce", "def half(): Int = 1.5\n", "print(half())"),
    "e6_return_stmt_mismatch": ("2.3 pravidlo 1: return s nekompatibilni hodnotou",
                                'def name(): Int =\n  return "a"\n', "print(name())"),
    "e6_return_needs_explicit_type": ("2.3: return v tele vyzaduje explicitni navratovy typ",
                                      "def f(x: Int) =\n  return x\n", "print(f(1))"),
    "e6_recursion_needs_explicit_type": ("2.3: rekurze vyzaduje explicitni navratovy typ",
                                         "def down(n: Int) =\n  if n < 1 then\n    print(n)\n  else\n    val m = n - 1\n    down(m)\n",
                                         "down(3)"),
    "e6_if_last_not_all_return": ("2.3 pravidlo 3: if na konci Int funkce, ne vsechny vetve konci return",
                                  'def f(x: Int): Int =\n  if x < 0 then\n    return 0\n  else\n    print("x")\n',
                                  "print(f(1))"),
    "e6_block_ends_with_decl_non_unit": ("2.3 pravidlo 2: blok konci definici (vysledek ()), typ Int",
                                         "def f(): Int =\n  val x = 1\n", "print(f())"),
    "e6_main_return_type": ("2: vstupni funkce s jinym navratovym typem nez Unit", None,
                            "@main def main(): Int =\n  print(1)\n  1"),
    "e6_param_double_into_int_decl": ("1.3: Double parametr do Int promenne",
                                      "def f(d: Double): Int =\n  val x: Int = d\n  x\n", "print(f(1.5))"),
    "e6_call_result_into_wrong_type": ("2.2: vysledek volani nekompatibilni s typem promenne", "",
                                       "val n: Int = readLine()\nprint(n)"),

    # ------------------------------------------------------------------ 7
    "e7_assign_val": ("2.2: prirazeni do nemodifikovatelne promenne", "", "val x = 1\nx = 2\nprint(x)"),
    "e7_assign_parameter": ("2.3: prirazeni do formalniho parametru", "def f(p: Int): Int =\n  p = 2\n  p\n",
                            "print(f(1))"),
    "e7_assign_outer_val_in_block": ("2.2: prirazeni do val nadrazeneho bloku", "",
                                     "val x = 1\nif x < 2 then\n  x = 3\nelse\n  ()\nprint(x)"),
    "e7_assign_val_in_loop": ("2.2: prirazeni do val v cyklu", "",
                              "val limit = 3\nvar i = 0\nwhile i < limit do\n  limit = 2\n  i = i + 1"),
}

HEADER = "// {code}: {desc}\n// Generovano: test/semantic_test (rucni testy pravidel zadani)\nimport ifj26.*\n\n"

# later() pro ok_forward_call je definovana az za main
TRAILER = {"ok_forward_call": "\ndef later(x: Int): Int = x * 2\n"}


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    for old in OUT.glob("*.scala"):
        old.unlink()
    for name, (desc, defs, body) in CASES.items():
        code = "OK (0)" if name.startswith("ok_") else f"chyba {name[1:name.index('_')]}"
        text = HEADER.format(code=code, desc=desc)
        if defs is None:
            text += body + "\n"
        else:
            text += defs + ("\n" if defs else "")
            text += "@main def main(): Unit =\n" + "".join("  " + line + "\n" for line in body.split("\n"))
        text += TRAILER.get(name, "")
        (OUT / f"{name}.scala").write_text(text, encoding="ascii")
    print(f"{len(CASES)} testu v {OUT}")


if __name__ == "__main__":
    main()
