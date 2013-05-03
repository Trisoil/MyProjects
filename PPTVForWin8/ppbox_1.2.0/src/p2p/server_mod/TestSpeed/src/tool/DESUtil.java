package tool;

import org.apache.commons.codec.binary.Base64;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.SecretKeySpec;
import javax.crypto.spec.DESKeySpec;
import javax.crypto.spec.IvParameterSpec;
import java.security.Security;
import java.security.SecureRandom;
import java.util.List;
import java.util.ArrayList;

/**
 * Created by IntelliJ IDEA.
 *
 * @author jianbo.xu
 * @version 1.0
 *          Date: 2005-11-24
 *          Time: 14:06:26
 */
public class DESUtil {
    public static int _DES = 1;
    public static int _DESede = 2;
    public static int _Blowfish = 3;

    private Cipher p_Cipher;
    private SecretKey p_Key;
    private String p_Algorithm;

    private void selectAlgorithm(int al) {
        switch(al) {
            default:
            case 1:
                this.p_Algorithm = "DESUtil";
                break;
            case 2:
                this.p_Algorithm = "DESede";
                break;
            case 3:
                this.p_Algorithm = "Blowfish";
                break;
        }
    }

    public DESUtil(int algorithm) throws Exception {
        this.selectAlgorithm(algorithm);
        Security.addProvider(new com.sun.crypto.provider.SunJCE());
        this.p_Cipher = Cipher.getInstance(this.p_Algorithm);
    }

    public byte[] getKey() {
        return this.checkKey().getEncoded();
    }

    private SecretKey checkKey() {
        try {
            if(this.p_Key == null) {
                KeyGenerator keygen = KeyGenerator.getInstance(this.p_Algorithm);
                this.p_Key = keygen.generateKey();
            }
        } catch(Exception nsae) {
        }
        return this.p_Key;
    }

    public void setKey(byte[] enckey) {
        this.p_Key = new SecretKeySpec(enckey, this.p_Algorithm);
    }

    public byte[] encode(byte[] data) throws Exception {
        //this.p_Cipher.init(Cipher.ENCRYPT_MODE, this.checkKey());
        this.p_Cipher.init(Cipher.ENCRYPT_MODE, this.p_Key);
        return this.p_Cipher.doFinal(data);
    }

    public byte[] decode(byte[] encdata, byte[] enckey) throws Exception {
        this.setKey(enckey);
        this.p_Cipher.init(Cipher.DECRYPT_MODE, this.p_Key);
        return this.p_Cipher.doFinal(encdata);
    }

    public static String byte2hex(byte[] b) {
        String hs = "";
        String stmp = "";
        for(int i = 0; i < b.length; i++) {
            stmp = Integer.toHexString(b[i] & 0xFF);
            if(stmp.length() == 1) {
                hs += "0" + stmp;
            } else {
                hs += stmp;
            }
        }
        return hs.toUpperCase();
    }

    public static byte[] hex2byte(String hex) throws IllegalArgumentException {
        if(hex.length() % 2 != 0) {
            throw new IllegalArgumentException();
        }
        char[] arr = hex.toCharArray();
        byte[] b = new byte[hex.length() / 2];
        for(int i = 0, j = 0, l = hex.length(); i < l; i++, j++) {
            String swap = "" + arr[i++] + arr[i];
            int byteint = Integer.parseInt(swap, 16) & 0xFF;
            b[j] = new Integer(byteint).byteValue();
        }
        return b;
    }

    public static String encodeString(String txt, String key) {
        try {
            DESUtil desUtil = new DESUtil(DESUtil._DESede);// 声明DES
            byte enc[];
            desUtil.setKey(Base64.decodeBase64(key.getBytes()));
            enc = desUtil.encode(txt.getBytes());
            return new String(Base64.encodeBase64(enc));
        } catch(Exception e) {
            return "";
        }
    }

    public static byte[] encodeByteString(byte[] b, String key) {
        try {
            DESUtil desUtil = new DESUtil(DESUtil._DESede);// 声明DES
            byte enc[];
            desUtil.setKey(Base64.decodeBase64(key.getBytes()));
            enc = desUtil.encode(b);
            return Base64.encodeBase64(enc);
        } catch(Exception e) {
            return null;
        }
    }

    public static String decodeString(String txt, String key) {
        try {
            DESUtil desUtil = new DESUtil(DESUtil._DESede);// 声明DES
            byte dec[];
            byte buf[] = Base64.decodeBase64(txt.getBytes());
            dec = desUtil.decode(buf, Base64.decodeBase64(key.getBytes()));
            return new String(dec);
        } catch(Exception e) {
            return "";
        }
    }

    public static byte[] decodeByteString(byte[] b, String key) {
        try {
            DESUtil desUtil = new DESUtil(DESUtil._DESede);// 声明DES
            byte dec[];
            byte buf[] = Base64.decodeBase64(b);
            return desUtil.decode(buf, Base64.decodeBase64(key.getBytes()));
        } catch(Exception e) {
            return null;
        }
    }

    public static String generateKey() {
        try {
            DESUtil desUtil = new DESUtil(DESUtil._DESede);// 声明DES
            byte[] key = desUtil.getKey();
            return new String(Base64.encodeBase64(key));
        } catch(Exception e) {
            return "";
        }
    }

    public static void main(String[] args) throws Exception {
        DESUtil desUtil = new DESUtil(DESUtil._DESede);// 声明DES

        SecureRandom sr = new SecureRandom();
        KeyGenerator kg = KeyGenerator.getInstance ("DES" );
        kg.init (sr);
        SecretKey skey = kg.generateKey();
        byte[] b1= skey.getEncoded();
        String hex1=desUtil.byte2hex(b1);
        //System.out.println("16进制密码："+hex1);

        hex1="3139373630353138";
        System.out.println("字符串密钥："+new String(desUtil.hex2byte(hex1)));
        System.out.println("16进制密钥："+hex1);

        //还原密码为byte[]
        byte[] keyb=desUtil.hex2byte(hex1);
        DESKeySpec dks = new DESKeySpec (keyb);
        SecretKeyFactory keyFactory = SecretKeyFactory.getInstance("DES" );
        SecretKey sskey = keyFactory.generateSecret( dks );
        Cipher cipher = Cipher.getInstance( "DES/CBC/PKCS5Padding" );
        IvParameterSpec iv = new IvParameterSpec(keyb);
        cipher.init( Cipher.ENCRYPT_MODE, sskey, iv );
        String mystr="abcd1234";
        System.out.println("原始字符串："+mystr);
        String mystrhex=desUtil.byte2hex(mystr.getBytes());
        System.out.println("原始字符创16进制："+mystrhex);
        byte[] data = mystr.getBytes();
        byte[] encryptedClassData = cipher.doFinal(data );
        String enhex=desUtil.byte2hex(encryptedClassData);
        System.out.println("加密后的16进制："+enhex);

        //String mykeyhex="3139373630353138";
        //System.out.println("解密密钥："+mykeyhex);
        byte[] mykey=desUtil.hex2byte(hex1);
        DESKeySpec mydks = new DESKeySpec (mykey);
        SecretKeyFactory keyFactory2 = SecretKeyFactory.getInstance("DES" );
        //SecretKey sskey2 = keyFactory2.generateSecret( mydks );
        SecretKey sskey2 = new SecretKeySpec(keyb,"DES");
        Cipher cipher2 = Cipher.getInstance( "DES/CBC/Nopadding" );
        IvParameterSpec iv2 = new IvParameterSpec(keyb);
        cipher2.init( Cipher.DECRYPT_MODE, sskey2, iv2 );

        String enhex2="46FA32B6CAEA7F3B".toUpperCase();
        System.out.println("需要解密的密文："+enhex2);

        byte[] bdata=desUtil.hex2byte(enhex);
        //byte[] bdata=desUtil.hex2byte(enhex2);

        byte[] bmystr = cipher2.doFinal( bdata );
        String mystr2=new String(bmystr);
        System.out.println("解密后的字符串："+mystr2);



        String str = "EGPKDCLGMKOKHPDLNHLONIFDMGOBOKONEPJBDHMLLICJEJCJAPAENDMLAHJEGFFBCIMCKLJALIMOGIFK";
        //String str="GFCLNMEDMIHOIGFHIJEHDINFAFGAHMBAGAGKLMOLGGAPKLDDAPCPOHKNBAOPBFLJFJHFNJMHLFKGCIFIFENKDIJNCHDCLHDBHOOILGHGKFGOBLHEOHLIMGKJPAJEILHPNONACELBAGMAMNCAMKFLNCIGFIFGCPKB";
        System.out.println("原始密文："+str);
        byte r=0x41;
        byte[] bstr=str.getBytes();
        byte[] bstr2=new byte[bstr.length];
        byte[] bstr3=new byte[bstr.length/2];
        for(int i = 0,j=0,k=bstr.length; i < k; i++,j++) {
            int _c1=bstr[i]-r;
            bstr2[i]=new Integer(_c1).byteValue();
            i++;
            int _c2=bstr[i]-r;
            bstr2[i]=new Integer(_c2).byteValue();
            byte[] _b=desUtil.hex2byte(Integer.toHexString(_c1)+Integer.toHexString(_c2));
            bstr3[j]=_b[0];
        }
        String str2Hex=desUtil.byte2hex(bstr2);
        System.out.println("步骤1每个数据减去0x41："+str2Hex);
        String str3Hex=desUtil.byte2hex(bstr3);
        System.out.println("步骤2将相邻两个数据合并："+str3Hex);
        List<String> list=new ArrayList<String>();
        int pos=0;
        while(pos<str3Hex.length()){
            list.add(str3Hex.substring(pos,pos+16));
            pos+=16;
        }
        System.out.println("步骤3将数据每8个字节分组：");
        for(String a:list) {
            byte[] br = cipher2.doFinal(desUtil.hex2byte(a));
            String brstr=new String(br).trim();
            System.out.println(a+"==>"+desUtil.byte2hex(br)+"==>"+brstr);
        }
        

    }

}
