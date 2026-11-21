//> using scala 3
//> using dep org.scala-lang::scala3-compiler:3.3.3

import dotty.tools.dotc.core.Contexts.*
import dotty.tools.dotc.parsing.Scanners.Scanner
import dotty.tools.dotc.util.SourceFile
import dotty.tools.dotc.reporting.StoreReporter
import java.nio.file.{Files, Paths}
import scala.io.Source

object ScalaLexerOracle:
  def main(args: Array[String]): Unit =
    val inputJson = Source.stdin.mkString.trim
    val paths = inputJson
      .stripPrefix("[").stripSuffix("]")
      .split(",")
      .map(_.trim.stripPrefix("\"").stripSuffix("\""))
      .filter(_.nonEmpty)

    val results = paths.map { p =>
      val pathObj = Paths.get(p)
      val content = Files.readString(pathObj)

      val reporter = new StoreReporter(null)
      val baseCtx = (new ContextBase).initialCtx
      val ctx = baseCtx.fresh.setReporter(reporter)

      val src = SourceFile.virtual(p, content)
      val scanner = new Scanner(src)(using ctx)

      while scanner.token != dotty.tools.dotc.parsing.Tokens.EOF do
        scanner.nextToken()

      val hasErr = reporter.hasErrors
      val errMsg = if hasErr then
        reporter.allErrors.map(_.message).mkString("; ")
          .replace("\\", "\\\\")
          .replace("\"", "\\\"")
          .replace("\n", " ")
          .replace("\r", " ")
      else ""

      s"""{"path": "$p", "valid": ${!hasErr}, "error": "$errMsg"}"""
    }

    println("[" + results.mkString(",") + "]")