// Decompiled by DJ v2.9.9.61 Copyright 2000 Atanas Neshkov  Date: 2008-3-31 20:04:23
// Home Page : http://members.fortunecity.com/neshkov/dj.html  - Check often for new version!
// Decompiler options: packimports(3) 
// Source File Name:   CryptogramHelper.java

package tool.cookie;

import com.sun.crypto.provider.SunJCE;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.security.*;
import javax.crypto.Cipher;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.DESedeKeySpec;
import javax.crypto.spec.IvParameterSpec;

import org.bouncycastle.util.encoders.Base64;
import org.bouncycastle.util.encoders.Hex;

public class CryptogramHelper {

    public CryptogramHelper() {
    }

    public static byte[] Base64Encode(byte b[])
            throws Exception {
        return Base64.encode(b);
    }

    public static byte[] Base64Decode(byte b[])
            throws Exception {
        return Base64.decode(b);
    }

    public static byte[] Base64Decode(String s)
            throws Exception {
        return Base64.decode(s);
    }

    public static String URLEncode(String strToBeEncode)
            throws Exception {
        return URLEncoder.encode(strToBeEncode, CodingType);
    }

    public static String URLDecode(String strToBeDecode)
            throws Exception {
        return URLDecoder.decode(strToBeDecode, CodingType);
    }

    private static IvParameterSpec IvGenerator(byte b[])
            throws Exception {
        IvParameterSpec IV = new IvParameterSpec(b);
        return IV;
    }

    private static Key KeyGenerator(String KeyStr)
            throws Exception {
        byte input[] = Hex.decode(KeyStr);
        DESedeKeySpec KeySpec = new DESedeKeySpec(input);
        SecretKeyFactory KeyFactory = SecretKeyFactory.getInstance(KeyAlgorithm);
        return KeyFactory.generateSecret(KeySpec);
    }

    public static byte[] IVGenerator(String strIV)
            throws Exception {
        return Hex.decode(strIV);
    }

    public static String GenerateDigest(String strTobeDigest)
            throws Exception {
        byte input[] = strTobeDigest.getBytes(CodingType);
        byte output[] = (byte[]) null;
        MessageDigest DigestGenerator = MessageDigest.getInstance(DigestAlgorithm);
        DigestGenerator.update(input);
        output = DigestGenerator.digest();
        return new String(Base64Encode(output), CodingType);
    }

    public static String Encrypt(String strTobeEnCrypted, String strKey, byte byteIV[])
            throws Exception {
        byte input[] = strTobeEnCrypted.getBytes(CodingType);
        Key k = KeyGenerator(strKey);
        IvParameterSpec IVSpec = byteIV.length != 0 ? IvGenerator(byteIV) : IvGenerator(defaultIV);
        Cipher c = Cipher.getInstance(CryptAlgorithm);
        c.init(1, k, IVSpec);
        byte output[] = c.doFinal(input);
        return new String(Base64Encode(output), CodingType);
    }

    public static String Decrypt(String strTobeDeCrypted, String strKey, byte byteIV[])
            throws Exception {
        byte input[] = Base64Decode(strTobeDeCrypted);
        Key k = KeyGenerator(strKey);
        IvParameterSpec IVSpec = byteIV.length != 0 ? IvGenerator(byteIV) : IvGenerator(defaultIV);
        Cipher c = Cipher.getInstance(CryptAlgorithm);
        c.init(2, k, IVSpec);
        byte output[] = c.doFinal(input);
        return new String(output, CodingType);
    }

    public static byte[] HexStringToByteArray(String s) {
        byte buf[] = new byte[s.length() / 2];
        for(int i = 0; i < buf.length; i++)
            buf[i] = (byte) (chr2hex(s.substring(i * 2, i * 2 + 1)) * 16 + chr2hex(s.substring(i * 2 + 1, i * 2 + 1 + 1)));

        return buf;
    }

    private static byte chr2hex(String chr) {
        if("0".equals(chr))
            return 0;
        if("1".equals(chr))
            return 1;
        if("2".equals(chr))
            return 2;
        if("3".equals(chr))
            return 3;
        if("4".equals(chr))
            return 4;
        if("5".equals(chr))
            return 5;
        if("6".equals(chr))
            return 6;
        if("7".equals(chr))
            return 7;
        if("8".equals(chr))
            return 8;
        if("9".equals(chr))
            return 9;
        if("A".equals(chr))
            return 10;
        if("B".equals(chr))
            return 11;
        if("C".equals(chr))
            return 12;
        if("D".equals(chr))
            return 13;
        if("E".equals(chr))
            return 14;
        return ((byte) (!"F".equals(chr) ? 0 : 15));
    }

    private static String CodingType = "UTF-8";
    private static String DigestAlgorithm = "SHA1";
    private static String CryptAlgorithm = "DESede/CBC/PKCS5Padding";
    private static String KeyAlgorithm = "DESede";
    private static byte defaultIV[] = {
            1, 2, 3, 4, 5, 6, 7, 8
    };

    static {
        Security.addProvider(new SunJCE());
    }
}