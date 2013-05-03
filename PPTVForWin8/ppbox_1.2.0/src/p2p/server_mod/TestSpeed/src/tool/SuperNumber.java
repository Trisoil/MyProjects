package tool;

import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.text.DecimalFormat;

/**
 * Created by IntelliJ IDEA.
 * User: Xu Jianbo
 * Date: 2004-4-2
 * Time: 9:50:51
 * To change this template use File | Settings | File Templates.
 */

/* The format methods receive the value to be formatted (which can
 * be of any type) and a format descriptor: a string that contains
 * one or more of the following flags in any order and in any case:
 *
 * L:
 *   By default, all values are aligned right within their field
 *   width. If this flag is specified, left alignment is used instead.
 *   This flag has no effect if the field width is not specified.
 * C:
 *   By default, all values are aligned right within their field
 *   width. If this flag is specified, centre alignment is used instead.
 *   This flag has no effect if the field width is not specified.
 * X:
 *   By default, all numeric values are shown in the decimal system.
 *   If this flag is specified, hexadecimal is used instead (showing
 *   IEEE-754 for real's). This flag has no effect if the value is not
 *   numeric.
 * B:
 *   By default, all numeric values are shown in the decimal system.
 *   If this flag is specified, binary is used instead (showing
 *   IEEE-754 for real's). This flag has no effect if the value is not
 *   numeric.
 * ,:
 *   By default, all numeric, base-10 values are shown without a
 *   thousand-separator. If this flag is specified, a comma is inserted
 *   in the integer part of the number to separate thousands. This flag
 *   has no effect if the value is not numeric, is not in decimal, or if
 *   the scientific notation is used.
 * S:
 *   By default, all numeric, base-10 values are shown as an integer part
 *   followed by a mantissa, or fractional part. If this flag is specified,
 *   scientific notation is used: One digits (possibly preceded by a minus
 *   sign) followed by a decimal point, a mantissa, the letter 'E' and an
 *   exponent. This flag has no effect if the value is not numeric or is not
 *   in decimal.
 * Z:
 *   By default, all integer, base-10 values are shown with leading or
 *   trailing spaces to fill the specified field width. If this flag is
 *   specified, the field is filled with leading zeros instead. This flag is
 *   only meaningful if the value is a base-10 integer, the width is specified,
 *   and the thousand-separator flag is not specified.
 * w.d (two integers separated by a period):
 *   w is the desired width of the entire returned string, after formatting.
 *   If the formatted string is shorter than w, it will be padded by leading
 *   and/or trailing spaces (or some other fill character) depending on the
 *   requested alignment (left, right, or centre). d is the desired number of
 *   decimals and is meaningful only if the value is a base-10 real (in standard
 *   or scientific notation). Rounding to the specified number of decimals is
 *   done using conventional rules but the case of 5 is handled by rounding to
 *   the nearest even number (same as the rint method of the Math class). Note
 *   that you can specify only w (in that case don't include the period), or
 *   only d (in this last case do precede it by the period).
 */

public class SuperNumber {
    /**
     * 格式化为Money格式
     */
    public static String formatMoney(double money, boolean grouped) {
        DecimalFormat df = new DecimalFormat();
        df.setMinimumFractionDigits(2);
        df.setGroupingUsed(grouped);
        return df.format(money);
    }

    public static String formatMoney(float money, boolean grouped) {
        DecimalFormat df = new DecimalFormat();
        df.setMinimumFractionDigits(2);
        df.setGroupingUsed(grouped);
        return df.format(money);
    }

    public static String formatMoney(int money, boolean grouped) {
        DecimalFormat df = new DecimalFormat();
        df.setMinimumFractionDigits(2);
        df.setGroupingUsed(grouped);
        return df.format(money);
    }

    public static String formatMoney(double money) {
        return formatMoney(money, true);
    }

    public static String formatMoney(float money) {
        return formatMoney(money, true);
    }

    public static String formatMoney(int money) {
        return formatMoney(money, true);
    }

    /**
     * 将一个double类型根据指定精度返回成String类型（有逗号那种）
     *
     * @param n        原始的double数字
     * @param fd       小数点后面的精度
     * @param fillZero 表示小数点后面是否用0来填充
     */
    public static String formatDecimal(double n, int fd, boolean fillZero) {
        DecimalFormat df = new DecimalFormat();
        df.setMaximumFractionDigits(fd);
        String r = df.format(n);
        if(!fillZero) {
            double rd = Double.parseDouble(SuperString.replace(r, ",", ""));
            if(Math.floor(rd) == rd) r = df.format((int) Math.ceil(rd));
        }
        return r;
    }

    /**
     * 将一个double类型根据指定精度返回，仍然是double
     *
     * @param d  原始的double数字
     * @param fd 小数点后面的精度
     */
    public static double formatDouble(double d, int fd) {
        DecimalFormat df = new DecimalFormat();
        df.setMaximumFractionDigits(fd);
        String r = df.format(d);
        return Double.parseDouble(SuperString.replace(r, ",", ""));
    }

    public static float formatFloat(float d, int fd) {
        DecimalFormat df = new DecimalFormat();
        df.setMaximumFractionDigits(fd);
        String r = df.format(d);
        return Float.parseFloat(SuperString.replace(r, ",", ""));
    }

    /**
     * 将一个int类型根据指定精度返回，仍然是double
     * @param n 原始的double数字
     * @param fd 小数点后面的精度
     */
    /*
        public static String formatDecimal1(int n,int fd){
            DecimalFormat df=new DecimalFormat();
            df.setMaximumFractionDigits(fd);
            return df.format(n);
        }
    */


    /**
     * **********************************************************
     * Formats the passed value using the passed format descriptor
     * and returns the result as a string.
     *
     * @param value the value to be formatted.
     * @param fd    the format descriptor.
     * @return the formatted value as a string.
     *         ************************************************************
     */
    public static String format(byte value, String fd) {
        return formatInteger(value, fd, 8);
    }

    /**
     * **********************************************************
     * Formats the passed value using the passed format descriptor
     * and returns the result as a string.
     *
     * @param value the value to be formatted.
     * @param fd    the format descriptor.
     * @return the formatted value as a string.
     *         ************************************************************
     */
    public static String format(char value, String fd) {
        return formatInteger(value, fd, 16);
    }

    /**
     * **********************************************************
     * Formats the passed value using the passed format descriptor
     * and returns the result as a string.
     *
     * @param value the value to be formatted.
     * @param fd    the format descriptor.
     * @return the formatted value as a string.
     *         ************************************************************
     */
    public static String format(double value, String fd) {
        extractAttributes(fd);
        String s1;
        if(base == 'B') {
            s1 = Long.toBinaryString(Double.doubleToLongBits(value));
            s1 = repeat(64, '0') + s1;
            s1 = s1.substring(s1.length() - 64);
        } else if(base == 'X') {
            s1 = Long.toHexString(Double.doubleToLongBits(value));
            s1 = repeat(16, '0') + s1;
            s1 = s1.substring(s1.length() - 16);
        } else {
            pattern = decimals != -1 ? "." + repeat(decimals, '0') : ".#";
            if(scientific)
                pattern = "0" + pattern + "E0";
            else
                pattern = separator ? "#,##0" + pattern : "0" + pattern;
            s1 = (new DecimalFormat(pattern)).format(value);
        }
        return size(s1);
    }

    /**
     * **********************************************************
     * Formats the passed value using the passed format descriptor
     * and returns the result as a string.
     *
     * @param value the value to be formatted.
     * @param fd    the format descriptor.
     * @return the formatted value as a string.
     *         ************************************************************
     */
    public static String format(float value, String fd) {
        extractAttributes(fd);
        String s1;
        if(base == 'B') {
            s1 = Integer.toBinaryString(Float.floatToIntBits(value));
            s1 = repeat(32, '0') + s1;
            s1 = s1.substring(s1.length() - 32);
        } else if(base == 'X') {
            s1 = Integer.toHexString(Float.floatToIntBits(value));
            s1 = repeat(8, '0') + s1;
            s1 = s1.substring(s1.length() - 8);
        } else {
            pattern = decimals != -1 ? "." + repeat(decimals, '0') : ".#";
            if(scientific)
                pattern = "0" + pattern + "E0";
            else
                pattern = separator ? "#,##0" + pattern : "0" + pattern;
            s1 = (new DecimalFormat(pattern)).format(value);
        }
        return size(s1);
    }

    /**
     * **********************************************************
     * Formats the passed value using the passed format descriptor
     * and returns the result as a string.
     *
     * @param value the value to be formatted.
     * @param fd    the format descriptor.
     * @return the formatted value as a string.
     *         ************************************************************
     */
    public static String format(int value, String fd) {
        return formatInteger(value, fd, 32);
    }

    /**
     * **********************************************************
     * Formats the passed value using the passed format descriptor
     * and returns the result as a string.
     *
     * @param value the value to be formatted.
     * @param fd    the format descriptor.
     * @return the formatted value as a string.
     *         ************************************************************
     */
    public static String format(long value, String fd) {
        return formatInteger(value, fd, 64);
    }

    /**
     * **********************************************************
     * Formats the passed value using the passed format descriptor
     * and returns the result as a string.
     *
     * @param value the value to be formatted.
     * @param fd    the format descriptor.
     * @return the formatted value as a string.
     *         ************************************************************
     */
    public static String format(short value, String fd) {
        return formatInteger(value, fd, 16);
    }

    /* Formats the passed value using the passed format descriptor
     * and returns the result as a string.
     */
    private static String formatInteger(long l, String s, int i) {
        extractAttributes(s);
        String s1;
        if(base == 'B') {
            s1 = Long.toBinaryString(l);
            s1 = repeat(64, '0') + s1;
            s1 = s1.substring(s1.length() - i);
        } else if(base == 'X') {
            s1 = Long.toHexString(l);
            s1 = repeat(16, '0') + s1;
            s1 = s1.substring(s1.length() - i / 4);
        } else if(separator) {
            s1 = (new DecimalFormat("#,###")).format(l);
        } else {
            s1 = String.valueOf(l);
            if(zeroFill)
                s1 = repeat(width - s1.length(), '0') + s1;
        }
        return size(s1);
    }

    // Gets information from the passed format descriptor.
    private static void extractAttributes(String s) {
        s = s.toUpperCase();
        alignment = 'R';
        separator = false;
        base = 'D';
        scientific = false;
        zeroFill = false;
        width = -1;
        decimals = -1;
        int i = s.indexOf(76, 0);
        if(i > -1) {
            alignment = 'L';
            s = s.substring(0, i) + s.substring(i + 1);
        }
        i = s.indexOf(67, 0);
        if(i > -1) {
            alignment = 'C';
            s = s.substring(0, i) + s.substring(i + 1);
        }
        i = s.indexOf(44, 0);
        if(i > -1) {
            separator = true;
            pattern = pattern + ",###";
            s = s.substring(0, i) + s.substring(i + 1);
        }
        i = s.indexOf(88, 0);
        if(i > -1) {
            base = 'X';
            s = s.substring(0, i) + s.substring(i + 1);
        } else {
            i = s.indexOf(66, 0);
            if(i > -1) {
                base = 'B';
                s = s.substring(0, i) + s.substring(i + 1);
            }
        }
        i = s.indexOf(83, 0);
        if(i > -1) {
            scientific = true;
            s = s.substring(0, i) + s.substring(i + 1);
        }
        i = s.indexOf(90, 0);
        if(i > -1) {
            zeroFill = true;
            s = s.substring(0, i) + s.substring(i + 1);
        }
        i = s.indexOf(46, 0);
        if(i > -1) {
            decimals = Integer.parseInt(s.substring(i + 1));
            s = s.substring(0, i);
        }
        if(s.length() > 0)
            width = Integer.parseInt(s);
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptor. No trailing End-Of-Line
     * character is printed.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void print(byte value, String fd) {
        handle.print(format(value, fd));
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptor. No trailing End-Of-Line
     * character is printed.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void print(char value, String fd) {
        handle.print(format(value, fd));
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptor. No trailing End-Of-Line
     * character is printed.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void print(double value, String fd) {
        handle.print(format(value, fd));
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptor. No trailing End-Of-Line
     * character is printed.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void print(float value, String fd) {
        handle.print(format(value, fd));
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptor. No trailing End-Of-Line
     * character is printed.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void print(int value, String fd) {
        handle.print(format(value, fd));
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptor. No trailing End-Of-Line
     * character is printed.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void print(long value, String fd) {
        handle.print(format(value, fd));
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptor. No trailing End-Of-Line
     * character is printed.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void print(short value, String fd) {
        handle.print(format(value, fd));
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptorand followed by an End-Of-Line
     * marker.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void println(byte value, String fd) {
        print(value, fd);
        handle.print(EOL);
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptorand followed by an End-Of-Line
     * marker.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void println(char value, String fd) {
        print(value, fd);
        handle.print(EOL);
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptorand followed by an End-Of-Line
     * marker.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void println(double value, String fd) {
        print(value, fd);
        handle.print(EOL);
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptorand followed by an End-Of-Line
     * marker.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void println(float value, String fd) {
        print(value, fd);
        handle.print(EOL);
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptorand followed by an End-Of-Line
     * marker.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void println(int value, String fd) {
        print(value, fd);
        handle.print(EOL);
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptorand followed by an End-Of-Line
     * marker.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void println(long value, String fd) {
        print(value, fd);
        handle.print(EOL);
        handle.flush();
    }

    /**
     * **********************************************************
     * Output the passed value to the standard output device using
     * the passed format descriptorand followed by an End-Of-Line
     * marker.
     *
     * @param value the value to be printed.
     * @param fd    the format descriptor.
     *              ************************************************************
     */
    public static void println(short value, String fd) {
        print(value, fd);
        handle.print(EOL);
        handle.flush();
    }

    // Returns the string padding with 'fillChar'.
    private static String size(String s) {
        int i = width - s.length();
        if(alignment == 'R')
            return repeat(i, fillChar) + s;
        if(alignment == 'L')
            return s + repeat(i, fillChar);
        else
            return repeat(i / 2, fillChar) + s + repeat(i / 2 + i % 2, fillChar);
    }

    // Repeats the passed character 'times' times.
    public static String repeat(int times, char c) {
        String s = "";
        for(int i = 0; i < times; i++)
            s = s + c;
        return s;
    }

    public static float RoundNum(float f, int fractionDigits) {
        long d = 1;
        for(int i = 0; i < fractionDigits; i++) d *= 10;
        return Math.round(f * d) / Float.valueOf(d).floatValue();
    }

    private static final String EOL = System.getProperty("line.separator");
    private static PrintWriter handle = new PrintWriter(new OutputStreamWriter(System.out));
    public static char fillChar = ' ';
    private static char alignment;
    private static boolean separator;
    private static char base;
    private static boolean scientific;
    private static boolean zeroFill;
    private static int width;
    private static int decimals;
    private static String pattern;
}

