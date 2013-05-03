package tool;

import java.math.BigDecimal;

/**
 * Created by IntelliJ IDEA.
 *
 * @author Xu Jianbo
 * @version 1.0
 *          Date: 2008-11-21
 *          Time: 17:01:40
 */
public class SuperMath {

    /*double的浮点计算*/
    //加
    public static double add(double d1, double d2) {
        return new BigDecimal(Double.toString(d1)).add(new BigDecimal(Double.toString(d2))).doubleValue();
    }

    public static double adds(double[] ds) {
        double d = 0.00d;
        for(double _d : ds) d = add(d, _d);
        return d;
    }

    //减法
    public static double sub(double d1, double d2) {
        return new BigDecimal(Double.toString(d1)).subtract(new BigDecimal(Double.toString(d2))).doubleValue();
    }

    public static double subs(double[] ds) {
        double d = 0.00d;
        if(ds.length <= 0) return d;
        if(ds.length >= 1) d = ds[0];
        for(int i = 1; i < ds.length; i++) d = sub(d, ds[i]);
        return d;
    }

    //乘法
    public static double mul(double d1, double d2) {
        return new BigDecimal(Double.toString(d1)).multiply(new BigDecimal(Double.toString(d2))).doubleValue();
    }

    public static double mul(double d1, double d2, int scale) {
        BigDecimal dr = new BigDecimal(Double.toString(d1)).multiply(new BigDecimal(Double.toString(d2)));
        return dr.setScale(scale, BigDecimal.ROUND_HALF_UP).doubleValue();
    }

    //除法
    public static double div(double d1, double d2, int scale) {
        if(scale < 0) scale = 0;
        return new BigDecimal(Double.toString(d1)).divide(new BigDecimal(Double.toString(d2)), scale,
                BigDecimal.ROUND_HALF_UP).doubleValue();
    }

    public static double round(double d, int scale) {
        return new BigDecimal(Double.toString(d)).setScale(scale, BigDecimal.ROUND_HALF_UP).doubleValue();
    }

    public static double roundMoney(double d) {
        return round(d, 2);
    }

    public static float toFloat(double d) {
        return new BigDecimal(Double.toString(d)).floatValue();
    }

    public static double toDouble(float f) {
        return new BigDecimal(Float.toString(f)).floatValue();
    }

    public static double getDouble(float f) {
        return roundMoney(toDouble(f));
    }

    public static float getFloat(double d) {
        return new BigDecimal(Double.toString(d)).setScale(2, BigDecimal.ROUND_HALF_UP).floatValue();
    }

    public static void main(String[] args) {
        System.out.println(subs(new double[]{adds(new double[]{2.55, 3.45}), 2.34}));
    }

}
