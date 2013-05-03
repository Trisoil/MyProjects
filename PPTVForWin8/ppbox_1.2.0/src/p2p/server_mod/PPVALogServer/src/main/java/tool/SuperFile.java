package tool;

import java.io.*;

/**
 * Created by IntelliJ IDEA.
 * User: Xu Jianbo
 * Date: 2004-5-14
 * Time: 10:18:49
 * To change this template use File | Settings | File Templates.
 */
public class SuperFile {

    /**
     * 构造方法
     */
    public SuperFile() {
    }

    /**
     * 删除文件或目录
     *
     * @param destFile 目标文件或目录，包括完整的路径名
     * @return true或false
     */
    public static boolean deleteFile(String destFile) {
        try {
            File f = new File(destFile);
            if(f.exists())
                return f.delete();
            else
                return false;
        } catch(Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * 创建目录
     */
    public static boolean mkdir(String dir) {
        File f = new File(dir);
        if(!f.exists()) {
            return f.mkdir();
        } else
            return true;
    }

    /**
     * 返回文件是否存在
     */
    public static boolean isExists(String filepath) {
        if(SuperString.notNull(filepath).equals(""))
            return false;
        File f = new File(filepath);
        return f.exists();
    }

    /**
     * 返回文件的文件名（不包括扩展名
     *
     * @param strFileName 文件名
     */
    public static String getFileName(String strFileName) {
        if((strFileName == null) || (strFileName.length() == 0))
            return "";
        if(strFileName.lastIndexOf(".") < 0)
            return strFileName;
        return strFileName.substring(0, strFileName.lastIndexOf("."));
    }

    /**
     * 返回文件的扩展名
     *
     * @param strFileName 文件名
     */
    public static String getFileExtName(String strFileName) {
        if((strFileName == null) || (strFileName.length() == 0))
            return "";
        if(strFileName.lastIndexOf(".") < 0)
            return "";
        return strFileName.substring(strFileName.lastIndexOf(".") + 1, strFileName.length());
    }

    /**
     * 返回完整文件名（包括路径）的路径，结尾
     *
     * @param strFileName 文件名
     */
    public static String getFilePath(String strFileName) {
        if((strFileName == null) || (strFileName.length() == 0))
            return "";
        strFileName = strFileName.replace('\\', '/').replace('/', File.separatorChar);
        if(strFileName.lastIndexOf(File.separator) < 0)
            return "";
        return strFileName.substring(0, strFileName.lastIndexOf(File.separator) + 1);
    }

    /**
     * 返回完整文件名（包括路径）中的文件名部分（不包括路径）
     *
     * @param strFileName 文件名
     */
    public static String getFileFullName(String strFileName) {
        if((strFileName == null) || (strFileName.length() == 0))
            return "";
        strFileName = strFileName.replace('\\', '/').replace('/', File.separatorChar);
        if(strFileName.lastIndexOf(File.separator) < 0)
            return "";
        return strFileName.substring(strFileName.lastIndexOf(File.separator) + 1, strFileName.length());
    }

    /**
     * 返回文件的文件名（包含完整路径）
     *
     * @param path     路径
     * @param fileName 文件名
     * @return 当然是完整的文件名了
     */
    public static String getPathFileName(String path, String fileName) {
        if((path == null) || (path.length() == 0)) {
            return fileName;
        } else {
            path = path.replace('\\', '/').replace('/', File.separatorChar);
            if(!path.substring(path.length() - 1, path.length()).equals(String.valueOf(File.separatorChar)))
                path += File.separatorChar;
            return path + fileName;
        }
    }

    /**
     * 返回文件的路径名
     *
     * @param path             路径
     * @param fileName         文件名
     * @param includeSeparator 结尾是否包含文件分隔符
     * @return 当然是完整的路径名了
     */
    public static String getFilePath(String path, String fileName, boolean includeSeparator) {
        if((path == null) || (path.length() == 0))
            path = fileName.substring(0, fileName.lastIndexOf(File.separatorChar));
        else
            path = path.replace('\\', '/').replace('/', File.separatorChar);
        if(includeSeparator) {
            if(!path.substring(path.length() - 1, path.length()).equals(String.valueOf(File.separatorChar)))
                path += File.separatorChar;
        } else {
            if(path.substring(path.length() - 1, path.length()).equals(String.valueOf(File.separatorChar)))
                path = path.substring(0, path.length() - 1);
        }
        return path;
    }

    /**
     * 获取一个文件的大小（体积）
     *
     * @param path         文件路径
     * @param destFileName 目标文件名
     * @return 文件大小，long类型
     */
    public static long getFileSize(String path, String destFileName) {
        File f = new File(getPathFileName(path, destFileName));
        if(f.exists() && f.isFile())
            return f.length();
        else
            return 0l;
    }

    /**
     * 对目录或文件进行改名
     */
    public static boolean renameFile(String name, String newName) {
        try {
            File f = new File(name);
            if(!f.exists()) return true;
            return f.renameTo(new File(newName));
        } catch(Exception e) {
            return false;
        }
    }

    /**
     * 拷贝文件的方法
     *
     * @param path       路径
     * @param sourcename 源文件名
     * @param destname   目标文件名
     */
    public static void fileCopy(String path, String sourcename, String destname) throws IOException {
        String sourceFileName = getPathFileName(path, sourcename);
        String destFileName = getPathFileName(path, destname);
        //创建源文件对象
        File sourcefile = new File(sourceFileName);
        //创建目标文件对象
        File destfile = new File(destFileName);
        //定义文件输入流
        FileInputStream source = null;
        //定义文件输出流
        FileOutputStream dest = null;
        byte[] buffer;
        int readbytes;
        try {
            //判断文件十分存在、是否为普通文件
            if(!sourcefile.exists() || !sourcefile.isFile())
                return;
            //如果目标文件存在，删除掉
            if(destfile.exists() && destfile.isFile())
                deleteFile(destFileName);
            //判断是否具有写权限
            if(!sourcefile.canRead())
                return;
            //创建文件输入对象
            source = new FileInputStream(sourcefile);
            //创建文件输出对象
            dest = new FileOutputStream(destfile);
            buffer = new byte[1024];
            //文件输出
            for(; ;) {
                readbytes = source.read(buffer);
                if(readbytes == -1)
                    break;
                dest.write(buffer, 0, readbytes);
            }
            //完成
        } finally {
            if(source != null) {
                try {
                    source.close();
                } catch(IOException e) {
                }
            }
            if(dest != null) {
                try {
                    dest.close();
                } catch(IOException e) {
                }
            }
        }
    }

    /**
     * 读取一个HTML类型的文件
     *
     * @param fileName 文件名称，在服务器上的绝对路径
     */
    public static String readHtmlFile(String fileName) {
        String content = "";
        File file = new File(fileName);
        if(!file.exists()) {
            return null;
        }
        if(file != null) {
            try {
                BufferedReader reader = new BufferedReader(new FileReader(file));
                for(String inLine = reader.readLine(); inLine != null; inLine = reader.readLine()) {
                    content = content + inLine;
                }
                return content;
            } catch(IOException e) {
                return null;
            }
        } else {
            return null;
        }
    }


    /**
     * 删除上传的文件
     */
    public static boolean deleteFileComp(String fileName) {
        File f = new File(fileName);
        int i = 0;
        while(f.exists() && (i < 1000)) {
            f.delete();
            f = new File(fileName);
            //System.out.println("------- 第" + i + "次尝试删除" + SuperFile.getFileFullName(fileName));
            i++;
        }
        System.out.println(!f.exists() ? "-------删除成功！" + SuperFile.getFileFullName(fileName)
                : "=删除失败！" + SuperFile.getFileFullName(fileName) + "");
        return !f.exists();
        //return SuperFile.deleteFile(path+File.separator+photo);
    }

    /**
     * 读取一个HTML类型的文件
     * <p/>
     * add by liuyong
     *
     * @param fileName 文件名称，在服务器上的绝对路径
     */
    public static void writeHtmlFileCharset(String fileName, String content, String charset) {
        writeHtmlFileCharset(fileName, content, charset, false);
    }

    public static boolean writeHtmlFileCharset(String fileName, String content, String charset,
                                               boolean createFile) {
        try {
            if(content == null) content = "";
            if(createFile) {
                File file = new File(fileName);
                if(!file.exists()) {
                    file.createNewFile();
                }
            }
            OutputStreamWriter osw = new OutputStreamWriter(new FileOutputStream(fileName), charset);
            osw.write(content, 0, content.length());
            osw.flush();
            osw.close();
            return true;
        } catch(IOException e) {
            return false;
        }
    }

    /**
     * 读取一个HTML类型的文件
     * add by liuyong
     *
     * @param fileName 文件名称，在服务器上的绝对路径
     */
    public static String readHtmlFileCharset(String fileName, String Charset) {
        String content = "";
        File file = new File(fileName);
        if(!file.exists()) {
            return null;
        }
        if(file != null) {
            try {
                BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(fileName), Charset));
                for(String inLine = reader.readLine(); inLine != null; inLine = reader.readLine()) {
                    content += (content.length() > 0 ? "\n" : "") + inLine;
                }
                return content;
            } catch(IOException e) {
                return null;
            }
        } else {
            return null;
        }
    }

    public static StringBuffer readFileCharset(String fileName, String Charset) {
        StringBuffer sb = new StringBuffer();
        File file = new File(fileName);
        if(!file.exists()) {
            file = null;
            return null;
        }
        if(file != null) {
            try {
                BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(fileName), Charset));
                for(String inLine = reader.readLine(); inLine != null; inLine = reader.readLine()) {
                    sb.append(inLine + "\r\n");
                }
                reader.close();
                reader = null;
                sb.delete(sb.length() - "\r\n".length(), sb.length());
                return sb;
            } catch(IOException e) {
                return null;
            } finally {
                file = null;
            }
        } else {
            return null;
        }
    }

    public static boolean writeFileCharset(String fileName, StringBuffer sb, String charset,
                                           boolean append, boolean createFile) {
        try {
            if(sb == null) return true;
            if(!append) {
                File file = new File(fileName);
                if(file.exists()) {
                    file.delete();
                    file.createNewFile();
                    file = null;
                }
            } else if(createFile) {
                File file = new File(fileName);
                if(!file.exists()) {
                    file.createNewFile();
                }
                file = null;
            }
            OutputStreamWriter osw = new OutputStreamWriter(new FileOutputStream(fileName), charset);
            osw.append(sb.subSequence(0, sb.length()));
            osw.flush();
            osw.close();
            osw = null;
            return true;
        } catch(IOException e) {
            return false;
        }
    }

    /**
     * Unicode编码
     */
    public static String enUnicode(String str) {
        if(str == null || str.length() == 0) return "";
        StringBuffer sb = new StringBuffer();
        char[] ch_str = str.toCharArray();
        for(int i = 0; i < ch_str.length; i++) {
            char c = ch_str[i];
            String c_str = String.valueOf(c);
            if(c_str.getBytes().length != c_str.length()) {//is gb
                int int_c = (int) c;
                String str_h = Integer.toString(int_c, 16);
                sb.append("\\");
                sb.append("u");
                sb.append(str_h);
            } else {
                sb.append(c);
            }
        }
        return sb.toString();
    }

    /**
     * Unicode解码
     */
    public static String deUnicode(String strunicode) {
        if(strunicode == null || strunicode.length() == 0) return "";
        StringBuffer sb = new StringBuffer();
        String[] astr = strunicode.split("\\u005c");
        for(int i = 0; i < astr.length; i++) {
            String c = astr[i];
            if(c.indexOf("u") != 0) {
                sb.append(c);
            } else {
                if(c.length() < 5) {
                    sb.append(c);
                } else {
                    String ch = c.substring(1, 5);
                    int int_ch = Integer.parseInt(ch, 16);
                    char[] ac_ch = {(char) int_ch};
                    String str_ch = new String(ac_ch);
                    sb.append(str_ch);
                    String ch_2 = c.substring(5, c.length());
                    sb.append(ch_2);
                }
            }
        }
        return sb.toString();
    }

    public static String enUnicodeForConstant(String str, String noenchar) {
        if(str == null || str.length() == 0) return "";
        String[] arrstr = str.split("\n");
        StringBuffer sb = new StringBuffer();
        for(int i = 0; i < arrstr.length; i++) {
            if(noenchar == null || noenchar.length() == 0 || !arrstr[i].startsWith(noenchar)) {
                sb.append(enUnicode(arrstr[i]));
            } else {
                sb.append(arrstr[i]);
            }
            sb.append("\n");
        }
        return sb.toString();
    }

    public static void writeToXML(String file, String content, String charset) {
        try {
            FileOutputStream fileOutputStream = new FileOutputStream(file);
            PrintStream out = new PrintStream(fileOutputStream, true, charset);
            out.write(content.getBytes(charset));
            out.flush();
            out.close();
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    public static void writeFile(String file, byte[] b) throws IOException {
        OutputStream os = new FileOutputStream(new File(file));
        os.write(b);
        os.flush();
        os.close();
    }

    public static byte[] readFile(String file) throws IOException {
        FileInputStream in = new FileInputStream(new File(file));
        ByteArrayOutputStream bout = new ByteArrayOutputStream();
        byte[] tmpbuf = new byte[1024];
        int count = 0;
        while((count = in.read(tmpbuf)) != -1) {
            bout.write(tmpbuf, 0, count);
            tmpbuf = new byte[1024];
        }
        in.close();
        return bout.toByteArray();
    }

}
