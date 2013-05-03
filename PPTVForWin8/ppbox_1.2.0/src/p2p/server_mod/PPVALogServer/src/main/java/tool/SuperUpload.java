package tool;

import java.util.Random;

/**
 * Created by IntelliJ IDEA.
 * User: Xu Jianbo
 * Date: 2004-5-11
 * Time: 10:50:11
 * To change this template use File | Settings | File Templates.
 */
public class SuperUpload {
    public String websitePath;               //网站在服务器上的相对路径，以"/"开头
    public String savePath;                  //文件保存在服务器上的upfiles目录里的路径
    public String saveFileName;             //指定的文件保存名
    public String fileNameMode;             //文件名生成模式
    public String saveTableName;            //文件保存在数据库中的表名
    public String saveFieldName;            //文家保存在数据中的字段名
    public String imgPath;                   //图片显示区显示图片的路径,服务器上upfiles里的路径，如果为空，例如ad/
    public String imgFileName;              //图片显示区 显示图片的文件名
    public int imgWidth;                     //显示图片的宽度
    public int imgMaxWidth;                  //显示的图片的最大宽度，用来控制版面（暂未生效）
    public int imgHeight;                    //显示图片的高度
    public int imgBorder;                    //显示的图片的边框
    public String imgBorderColor;            //显示的图片的边框的颜色
    public String upFileInputName;          //文件名显示框的Name
    public String upFileInputTitle;         //文件名显示框的Title，用于客户端数据验证错误提示
    public String upFileName;                //文件名显示框中显示的内容（一般是文件名）
    public boolean showImgZone;             //是否显示图片显示区
    public boolean showUpFileInputZone;   //是否显示文件名显示框
    public int tableWidth;                  //整个上传表格的宽度
    public int upFileInputWidth;           //文件名显示框的宽度
    public int fileInputWidth;             //文件选取框的宽度
    public String allowedFileExt;           //允许上传文件的扩展名，逗号分隔
    public String deniedFileExt;            //限制上传文件的扩展名，逗号分隔
    public long maxFileSize;                //允许上传文件的最大长度
    public boolean generateJavascript;    //是否生成javascript函数部分

    public static String MODE_CREATE = "0"; //生成新的
    public static String MODE_SOURCE = "1"; //使用上传的文件名，但不能有中文

    public SuperUpload() {
        this.websitePath = "/";
        this.savePath = "";
        this.saveFileName = "";
        this.fileNameMode = MODE_CREATE;
        this.saveTableName = "";
        this.saveFieldName = "";
        this.imgPath = "";
        this.imgFileName = "";
        this.imgWidth = 0;
        this.imgMaxWidth = 0;
        this.imgHeight = 0;
        this.imgBorder = 0;
        this.imgBorderColor = "000000";
        this.upFileInputName = "upfile";
        this.upFileInputTitle = "上传文件";
        this.upFileName = "";
        this.showImgZone = true;
        this.showUpFileInputZone = true;
        this.tableWidth = 280;
        this.upFileInputWidth = 233;
        this.fileInputWidth = 200;
        this.allowedFileExt = "";
        this.deniedFileExt = "";
        this.maxFileSize = 10240000L;
        this.generateJavascript = true;
    }

    /*生成文件上传的显示界面*/
    public String generateFileUploadInterface() {

        String strFileUpload = "";
        String strImgWidthText = (imgWidth == 0) ? "" : " width=" + imgWidth;
        String strImgHeightText = (imgHeight == 0) ? "" : " height=" + imgHeight;
        String strImg = strImgWidthText + strImgHeightText;
        String strShowImgZone = showImgZone ? "Y" : "N";
        String strShowUpFileInputZone = showUpFileInputZone ? "Y" : "N";

        Random rnd = new Random();
        int randomNumber = 1000 + rnd.nextInt(9000);
        String upFileDivName = "upFileDiv_" + randomNumber;

        if(generateJavascript) {
            strFileUpload = strFileUpload + "\n\n<SCRIPT LANGUAGE=javascript>\n" +
                    "<!--\n" +
                    "//----------------以下函数用于文件上传中各种状态控制\n" +
                    "function setUpPic(upFileInput,upFileDivName,savePath,fn,iBorder,iBorderColor,strImg,imgMaxWidth){\n" +
                    "  if((fn!=\"\")&&(savePath!=\"\")){\n" +
                    "    if(document.all(upFileInput)!=null) document.all(upFileInput).value=fn;\n" +
                    "    if(document.all(upFileDivName)!=null)\n" +
                    "      document.all(upFileDivName).innerHTML=\"<img name='UpFilePic' id='UpFilePic' tagName='\"+imgMaxWidth+\"' \"+strImg+\" " +
                    "border='\"+iBorder+\"' src='\"+savePath+fn+\"' style='cursor:hand;border-color=#\"+iBorderColor+\"' " +
                    "onmousewheel='width+=(window.event.wheelDelta==120)?((width>20)?-20:-0):((width<1600)?+20:+0);return false;' " +
                    "onload='onloadPic(this);' onclick='window.open(\\\"\"+savePath+fn+\"\\\");' " +
                    ">\";\n" +
                    "  }\n" +
                    "}\n" +
                    "function upFileShowPic(savePath,upFileInput,upFileDivName,iBorder,iBorderColor,strImg,imgMaxWidth){\n" +
                    "  var obj=document.all(upFileInput);\n" +
                    "  if(obj!=null) {\n" +
                    "    if(obj.value==\"\"){\n" +
                    "      alert(obj.title+' 为空,不能显示!');\n" +
                    "      return false;\n" +
                    "    }\n" +
                    "    else\n" +
                    "        setUpPic(upFileInput,upFileDivName,savePath,obj.value,iBorder,iBorderColor,strImg,imgMaxWidth);\n" +
                    "  }\n" +
                    "  return true;\n" +
                    "}\n" +
                    "function onloadPic(obj){\n" +
                    "   var maxw=0;\n" +
                    "   try{\n" +
                    "       maxw=parseInt(obj.tagName);\n" +
                    "   }catch(e){maxw=0;}\n" +
                    "   if(maxw>0 && obj.width>0){\n" +
                    "       if(obj.width>maxw) obj.width=maxw;" +
                    "   }\n" +
                    "}\n" +
                    "function preViewPic(upFileDivName,fn,iBorder,iBorderColor,strImg,imgMaxWidth){\n" +
                    "  if(document.all(upFileDivName)!=null){\n" +
                    "    document.all(upFileDivName).innerHTML=\"<img name='UpFilePic' id='UpFilePic' tagName='\"+imgMaxWidth+\"' alt='预览状态，" +
                    "请点击上传保存此图片！' \"+strImg+\" border='\"+iBorder+\"' src='\"+fn+\"' style='border-color=#\"+iBorderColor+\"' " +
                    "onmousewheel='width+=(window.event.wheelDelta==120)?((width>20)?-20:-0):((width<1600)?+20:+0);return false;' " +
                    "onload='onloadPic(this);' " +
                    ">\";\n" +
                    "   }\n" +
                    "}\n" +
                    "//-->\n" +
                    "</SCRIPT>\n";
        }

        strFileUpload = strFileUpload + "      <table border=\"0\" cellspacing=\"1\" style=\"border-collapse: collapse\" " +
                "width=\"" + tableWidth + "\" name=\"tableUpFile\" cellpadding=\"0\">\n";
        if(showImgZone) {
            strFileUpload = strFileUpload + "        <tr>\n";
            strFileUpload = strFileUpload + "          <td width=\"100%\" nowrap id=\"" + upFileDivName + "\">";
            if(!imgFileName.equals("")) {
                strFileUpload = strFileUpload + "<img name=\"UpFilePic\" id=\"UpFilePic\" tagName=\"" + imgMaxWidth + "\" " +
                        strImg + " border=\"" + imgBorder + "\" src=\"" +
                        websitePath + imgPath + imgFileName + "\" style=\"cursor:hand;border-color=#" + imgBorderColor + "\" " +
                        "onmousewheel='width+=(window.event.wheelDelta==120)?((width>20)?-20:-0):((width<1600)?+20:+0);return false;' " +
                        "onload='onloadPic(this);' onclick='window.open(\"" + websitePath + imgPath + imgFileName + "\");' " +
                        ">";

            }
            strFileUpload = strFileUpload + "</td>\n";
            strFileUpload = strFileUpload + "        </tr>\n";
        }

        if(showUpFileInputZone) {
            strFileUpload = strFileUpload + "        <tr>\n";
            strFileUpload = strFileUpload + "          <td width=\"100%\" nowrap><input type=\"text\" " +
                    "name=\"" + upFileInputName + "\" id=\"" + upFileInputName + "\" size=\"30\" value=\"" +
                    upFileName + "\" style=\"background-color: #E6E6E6;width=" +
                    upFileInputWidth + "\" readonly title=\"" +
                    upFileInputTitle + "\">\n";
            if(showImgZone) {
                strFileUpload = strFileUpload + "<input type=\"button\" value=\"显示\" name=\"btnShow\" " +
                        "onclick=\"javascript:return upFileShowPic('" +
                        websitePath + imgPath + "','" +
                        upFileInputName + "','" + upFileDivName + "','" +
                        imgBorder + "','" + imgBorderColor + "','" + strImg + "'," + imgMaxWidth + ")\">\n";
            }
            strFileUpload = strFileUpload + "          </td>\n";
            strFileUpload = strFileUpload + "        </tr>\n";
        } else {
            strFileUpload = strFileUpload + "<input type=\"hidden\" name=\"" +
                    upFileInputName + "\" id=\"" + upFileInputName + "\" size=\"30\" value=\"" +
                    upFileName + "\" style=\"background-color: #E6E6E6;width=" +
                    upFileInputWidth + "\" title=\"" +
                    upFileInputTitle + "\">\n";
        }
        strFileUpload = strFileUpload + "        <tr>\n";
        strFileUpload = strFileUpload + "          <td width=\"100%\" nowrap>\n";
        strFileUpload = strFileUpload + "          <IFRAME src=\"" +
                websitePath + "commonupload.jsp?" +
                "websitePath=" + websitePath +
                "&savePath=" + savePath +
                "&saveFileName=" + saveFileName +
                "&fileNameMode=" + fileNameMode +
                "&saveTableName=" + saveTableName +
                "&saveFieldName=" + saveFieldName +
                "&imgPath=" + imgPath +
                "&imgBorder=" + imgBorder +
                "&imgBorderColor=" + imgBorderColor +
                "&strImg=" + strImg +
                "&imgMaxWidth=" + imgMaxWidth +
                "&upFileInputName=" + upFileInputName +
                "&upFileDivName=" + upFileDivName +
                "&showImgZone=" + strShowImgZone +
                "&showUpFileInputZone=" + strShowUpFileInputZone +
                "&fileInputWidth=" + fileInputWidth +
                "&allowedFileExt=" + allowedFileExt +
                "&deniedFileExt=" + deniedFileExt +
                "&maxFileSize=" + maxFileSize +
                "\" width=\"" + (fileInputWidth + 80) +
                "\" height=\"23\" marginwidth=\"1\" marginheight=\"1\" hspace=\"0\" " +
                "frameborder=\"0\" scrolling=\"no\" name=\"frameUpload\" align=\"middle\" " +
                "border=\"0\"></IFRAME></td>\n";
        strFileUpload = strFileUpload + "        </tr>\n";
        strFileUpload = strFileUpload + "      </table>\n";

        return strFileUpload;
    }

}
