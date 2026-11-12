//> using scala 3.8.4
//> using dep org.scala-lang::scala3-compiler:3.8.4

// Reference AST for the IFJ26 parser: parses the source with the Scala compiler's parser
// and prints the tree in the ast_dump_sexpr format (src/parser/ast.c). The tree shape is
// therefore defined by an independent compiler, not by the parser under test. Supports the
// base language only: on any other construct or a parse error no .ast file is written and
// the program exits with code 1.
//
// Usage:  scala run --server=false test/syntax_test/ScalaAstOracle.scala -- [--check] file.scala...
// Output: file.ast next to each input (not with --check) and one line per input:
//   OK <file>            parsed and rendered
//   SYNTAX <file>: ...   the Scala parser reports a syntax (or lexical) error
//   CHYBA <file>: ...    valid Scala, but a construct outside the base language

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
class SyntaxError(message: String) extends Unsupported(message)

object ScalaAstOracle:
  /** Matches the C "%g" format (6 significant digits, no trailing zeros). */
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
    // IFJ26 treats a negative literal as unary minus applied to the positive part
    if negative then s"(- $rendered)" else rendered

  private def typeName(tree: Tree): String = tree match
    case Ident(name) => name.toString
    case other if other.isEmpty => "Undef"
    case other => throw Unsupported(s"typ $other")

  /** Branch or function body: always (block ...), even for a single statement. */
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
    // The Scala parser already evaluates a literal with a d/D suffix to a constant, including
    // its sign; IFJ26 treats the minus as unary, as for the other numeric literals
    case Literal(Constant(value: Double)) =>
      if value < 0 || (value == 0 && 1 / value < 0) then s"(- ${formatG(-value)})" else formatG(value)
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
      throw SyntaxError(reporter.removeBufferedMessages.map(d => s"${d.pos.line + 1}:${d.pos.column + 1} ${d.message}").mkString("; ").replace("\n", " "))
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
    val checkOnly = args.contains("--check")
    for arg <- args if arg != "--check" do
      val path = Paths.get(arg)
      try
        val rendered = render(path)
        if !checkOnly then
          val target = path.resolveSibling(path.getFileName.toString.stripSuffix(".scala") + ".ast")
          Files.writeString(target, rendered + "\n")
        println(s"OK    $arg")
      catch
        case error: SyntaxError =>
          failed = true
          println(s"SYNTAX $arg: ${error.getMessage}")
        case error: Unsupported =>
          failed = true
          println(s"CHYBA $arg: ${error.getMessage}")
        // The Scala parser can crash on malformed input, e.g. with a NullPointerException
        case error: RuntimeException =>
          failed = true
          println(s"SYNTAX $arg: parser Scaly spadl: $error")
    if failed then sys.exit(1)
