//> using scala 3.8.4
//> using dep org.scala-lang::scala3-compiler:3.8.4

// Referencni AST pro parser IFJ26: zdrojak rozparsuje parser prekladace Scala a strom
// vypise ve formatu ast_dump_sexpr (src/parser/ast.c). Tvar stromu tak neurcuje testovany
// parser, ale nezavisly prekladac. Podporuje jen zakladni zadani: na jine konstrukci
// nebo chybe parsovani soubor .ast nevznikne a program skonci s kodem 1.
//
// Pouziti: scala run --server=false test/syntax_test/ScalaAstOracle.scala -- soubor.scala...
// Vystup:  soubor.ast vedle kazdeho vstupu

import dotty.tools.dotc.ast.untpd.*
import dotty.tools.dotc.core.Constants.Constant
import dotty.tools.dotc.core.Contexts.*
import dotty.tools.dotc.core.Flags
import dotty.tools.dotc.parsing.Parsers.Parser
import dotty.tools.dotc.reporting.StoreReporter
import dotty.tools.dotc.util.SourceFile
import java.math.{BigDecimal as JBigDecimal, MathContext, RoundingMode}
import java.nio.file.{Files, Path, Paths}

class Unsupported(message: String) extends Exception(message)

object ScalaAstOracle:
  /** Odpovida formatu "%g" v C (6 platnych cislic, bez koncovych nul). */
  private def formatG(value: Double): String =
    if value == 0.0 then return "0"
    val rounded = new JBigDecimal(value).round(new MathContext(6, RoundingMode.HALF_EVEN))
    val exponent = rounded.precision - rounded.scale - 1
    if exponent < -4 || exponent >= 6 then
      val mantissa = rounded.movePointLeft(exponent).stripTrailingZeros.toPlainString
      val sign = if exponent < 0 then "-" else "+"
      f"${mantissa}e$sign${math.abs(exponent)}%02d"
    else rounded.stripTrailingZeros.toPlainString

  private def number(digits: String, kind: NumberKind): String =
    val text = digits.replace("_", "").stripSuffix("d").stripSuffix("D")
    val (negative, magnitude) =
      if text.startsWith("-") then (true, text.drop(1)) else (false, text)
    val rendered = kind match
      case NumberKind.Whole(10) => BigInt(magnitude).toString
      case NumberKind.Decimal | NumberKind.Floating => formatG(magnitude.toDouble)
      case other => throw Unsupported(s"ciselny literal $other")
    // IFJ26 chape zaporny literal jako unarni minus a kladnou cast
    if negative then s"(- $rendered)" else rendered

  private def typeName(tree: Tree): String = tree match
    case Ident(name) => name.toString
    case other if other.isEmpty => "Undef"
    case other => throw Unsupported(s"typ $other")

  /** Telo vetve nebo funkce: vzdy jako (block ...), i pro jediny prikaz. */
  private def block(tree: Tree)(using Context): String = tree match
    case Block(stats, last) =>
      val items = if last.isEmpty then stats else stats :+ last
      items.map(expr).mkString("(block ", " ", ")")
    case single => s"(block ${expr(single)})"

  private def expr(tree: Tree)(using Context): String = tree match
    case Parens(inner) => expr(inner)
    case Tuple(Nil) => "null"
    case Literal(Constant(())) => "null"
    case Literal(Constant(text: String)) => "\"" + text + "\""
    // Literal s priponou d/D uz parser Scaly vyhodnoti na konstantu
    case Literal(Constant(value: Double)) => formatG(value)
    case Number(digits, kind) => number(digits, kind)
    case Ident(name) => name.toString
    case InfixOp(left, Ident(op), right) => s"(${op.toString} ${expr(left)} ${expr(right)})"
    case PrefixOp(Ident(op), operand) => s"(${op.toString} ${expr(operand)})"
    case Assign(target, value) => s"(= ${expr(target)} ${expr(value)})"
    case Apply(Ident(name), args) =>
      val params = args.map(arg => " " + expr(arg)).mkString
      s"(call ${name.toString} (params$params))"
    case If(cond, thenp, elsep) =>
      if elsep.isEmpty then throw Unsupported("if bez else (rozsireni BOOLTHEN)")
      s"(if ${expr(cond)} ${block(thenp)} (else ${block(elsep)}))"
    case WhileDo(cond, body) => s"(while ${expr(cond)} ${block(body)})"
    case Return(value, _) => if value.isEmpty then "(return)" else s"(return ${expr(value)})"
    case vd: ValDef =>
      val keyword = if vd.mods.is(Flags.Mutable) then "var" else "const"
      s"($keyword ${vd.name}:${typeName(vd.tpt)} ${expr(vd.rhs)})"
    case other => throw Unsupported(s"konstrukce ${other.getClass.getSimpleName}: $other")

  private def function(tree: DefDef)(using Context): String =
    val windows = tree.paramss.map { clause =>
      val params = clause.map {
        case param: ValDef => s" (param ${param.name}:${typeName(param.tpt)})"
        case other => throw Unsupported(s"parametr $other")
      }
      s" (params${params.mkString}))".dropRight(1)
    }
    s"(def ${tree.name}${windows.mkString}:${typeName(tree.tpt)} ${block(tree.rhs)})"

  def render(path: Path)(using Context): String =
    val reporter = new StoreReporter(null)
    given Context = summon[Context].fresh.setReporter(reporter)
    val tree = new Parser(SourceFile.virtual(path.toString, Files.readString(path))).parse()
    if reporter.hasErrors then
      throw Unsupported(reporter.removeBufferedMessages.map(_.message).mkString("; "))
    val definitions = tree match
      case PackageDef(_, stats) =>
        stats.flatMap {
          case _: Import => None
          case fn: DefDef => Some(function(fn))
          case other => throw Unsupported(s"definice $other")
        }
      case other => throw Unsupported(s"koren $other")
    definitions.mkString("(block ", " ", ")")

  def main(args: Array[String]): Unit =
    val base = new ContextBase
    given Context = base.initialCtx
    var failed = false
    for arg <- args do
      val path = Paths.get(arg)
      try
        val target = path.resolveSibling(path.getFileName.toString.stripSuffix(".scala") + ".ast")
        Files.writeString(target, render(path) + "\n")
        println(s"OK    $arg")
      catch
        case error: Unsupported =>
          failed = true
          println(s"CHYBA $arg: ${error.getMessage}")
        // Parser Scaly umi na poskozenem vstupu spadnout, napr. s NullPointerException
        case error: RuntimeException =>
          failed = true
          println(s"CHYBA $arg: parser Scaly spadl: $error")
    if failed then sys.exit(1)
