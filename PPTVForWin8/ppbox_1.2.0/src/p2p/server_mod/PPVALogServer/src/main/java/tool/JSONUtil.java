package tool;

import org.json.JSONArray;

import java.util.List;
import java.util.LinkedList;


/**
 * Created by IntelliJ IDEA.
 * User: Administrator
 * Date: 2009-12-14
 * Time: 14:58:23
 * To change this template use File | Settings | File Templates.
 */
public class JSONUtil {
    public static int[] formJsonArrayToInt(JSONArray array) throws Exception{
        if(array==null) return null;
        int[] javaArray=new int[array.length()];
        for(int i=0;i<array.length();i++){
             javaArray[i]=array.getInt(i);
        }
        return  javaArray;
    }

    public static long[] formJsonArrayToLong(JSONArray array) throws Exception{
        if(array==null) return null;
        long[] javaArray=new long[array.length()];
        for(int i=0;i<array.length();i++){
             javaArray[i]=array.getLong(i);
        }
        return  javaArray;
    }

    public static float[] formJsonArrayToFloat(JSONArray array) throws Exception{
        if(array==null) return null;
        float[] javaArray=new float[array.length()];
        for(int i=0;i<array.length();i++){
             javaArray[i]=(float)array.getDouble(i);
        }
        return  javaArray;
    }

    public static double[] formJsonArrayToDouble(JSONArray array) throws Exception{
        if(array==null) return null;
        double[] javaArray=new double[array.length()];
        for(int i=0;i<array.length();i++){
             javaArray[i]=array.getDouble(i);
        }
        return  javaArray;
    }

     public static boolean[] formJsonArrayToBoolean(JSONArray array) throws Exception{
        if(array==null) return null;
        boolean[] javaArray=new boolean[array.length()];
        for(int i=0;i<array.length();i++){
             javaArray[i]=array.getBoolean(i);
        }
        return  javaArray;
    }

    public static String[] formJsonArrayToString(JSONArray array) throws Exception{
        if(array==null) return null;
        String[] javaArray=new String[array.length()];
        for(int i=0;i<array.length();i++){
             javaArray[i]=array.getString(i);
        }
        return  javaArray;
    }

    public static List<Integer> formJsonArrayToIntList(JSONArray array) throws Exception{
        int[] intArray=formJsonArrayToInt(array);
        if(intArray==null) return null;
        List<Integer> intList=new LinkedList<Integer>();
        for(int i=0;i<intArray.length;i++){
              intList.add(intArray[i]);
        }
        return intList;
    }

    public static List<Long> formJsonArrayToLongList(JSONArray array) throws Exception{
        long[] longArray=formJsonArrayToLong(array);
        if(longArray==null) return null;
        List<Long> longList=new LinkedList<Long>();
        for(int i=0;i<longArray.length;i++){
              longList.add(longArray[i]);
        }
        return longList;
    }

     public static List<Float> formJsonArrayToFloatList(JSONArray array) throws Exception{
        float[] floatArray=formJsonArrayToFloat(array);
        if(floatArray==null) return null;
        List<Float> floatList=new LinkedList<Float>();
        for(int i=0;i<floatArray.length;i++){
              floatList.add(floatArray[i]);
        }
        return floatList;
    }

     public static List<Double> formJsonArrayToDoubleList(JSONArray array) throws Exception{
        double[] doubleArray=formJsonArrayToDouble(array);
        if(doubleArray==null) return null;
        List<Double> doubleList=new LinkedList<Double>();
        for(int i=0;i<doubleArray.length;i++){
              doubleList.add(doubleArray[i]);
        }
        return doubleList;
    }

     public static List<Boolean> formJsonArrayToBooleanList(JSONArray array) throws Exception{
        boolean[] booleanArray=formJsonArrayToBoolean(array);
        if(booleanArray==null) return null;
        List<Boolean> booleanList=new LinkedList<Boolean>();
        for(int i=0;i<booleanArray.length;i++){
              booleanList.add(booleanArray[i]);
        }
        return booleanList;
    }



    public static void main(String[] args) throws Exception {
        List list=new LinkedList();
        int[] ints=new int[50];
        for(int i=0;i<50;i++){
           ints[i]=i;
        }
        for(int i=0;i<100;i++){
           if(i%2==0) list.add(ints);
            else  list.add(i);
        }
    }




}
