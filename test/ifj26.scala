package ifj26

object `package`:
  def print(value: Any): Unit = Predef.print(value)
  def readLine(): String = scala.io.StdIn.readLine()
  def readInt(): Int =
    try scala.io.StdIn.readInt() catch case _: Exception => 0
  def readDouble(): Double =
    try scala.io.StdIn.readDouble() catch case _: Exception => 0.0
  def toStr(value: Any): String = value.toString
  def toInt(value: Double): Int = value.toInt
  def strLength(value: String): Int = value.length
  def subStr(value: String, start: Int, end: Int): String =
    value.substring(start, end)
  def ord(value: String, index: Int): Int =
    if index < 0 || index >= value.length then -1 else value.charAt(index).toInt
  def chr(code: Int): String = code.toChar.toString