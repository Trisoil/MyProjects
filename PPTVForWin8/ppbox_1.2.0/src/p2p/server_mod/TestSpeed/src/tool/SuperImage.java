package tool;

import com.sun.image.codec.jpeg.JPEGCodec;
import com.sun.image.codec.jpeg.JPEGEncodeParam;
import com.sun.image.codec.jpeg.JPEGImageEncoder;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Created by IntelliJ IDEA.
 *
 * @author 徐剑波 Xu Jianbo sword20@263.net
 *         Date: 2004-6-8
 *         Time: 15:51:05
 *         Location: Shanghai
 */

public class SuperImage {
    public static final double blurLevel1 = 0.13d;
    public static final double blurLevel2 = 0.25d;
    public static final double blurLevel3 = 0.38d;

    public SuperImage() {

    }


    /**
     * 生成新的图片文件
     *
     * @param path             文件所在的路径，可以为空，这时sourceImageName和destImageName就必须是带路径的完整文件名
     *                         如果path不为空，则sourceImageName和destImageName必须只是文件名(不带路径），这时目标文件只能存储在
     *                         源文件同一个目录下
     * @param sourceImageName  源文件名称 格式见上面的说明
     * @param destImageName    目标文件名称 格式见上面的说明
     * @param quality          生成的图片的压缩质量
     * @param destImageWidth   生成的图片的宽度
     * @param destImageHeight  生成的图片的高度，0：表示按照宽度缩放 -1：表示destImageWidth
     *                         是Max(宽,高)，看图片长宽比例而定  >0：正常缩放
     * @param fillSpacingColor 若生成的图片比原图片小时在四周填充的颜色
     * @param needBlurImage    是否柔化缩小的图像
     * @param blurTempImgWidth 柔化过程中过渡文件的宽度（高度） 0表示不生成过渡图片
     *                         它的大小决定了柔化过程所需的时间快慢，数量越大，时间越长，生成的图像越清晰平滑
     * @param blurImageName    柔化图像过程中的临时图像文件名
     * @param delBlurImage     是否删除blurImageName
     */
    public static void generateImage(String path, String sourceImageName, String destImageName, int quality,
                                     int destImageWidth, int destImageHeight, Color fillSpacingColor,
                                     boolean needBlurImage, int blurTempImgWidth, String blurImageName,
                                     boolean delBlurImage) {
        try {
            String sourceImage = "";
            String destImage = "";
            String blurImage = "";
            String sourcePath = "";
            String destPath = "";
            String tempBlurImageName = blurImageName;
            if((blurImageName == null) || blurImageName.equals("")) tempBlurImageName = destImageName;
            if((path == null) || (path.length() == 0)) {
                sourceImageName = sourceImageName.replace('\\', '/').replace('/', File.separatorChar);
                destImageName = destImageName.replace('\\', '/').replace('/', File.separatorChar);
                tempBlurImageName = tempBlurImageName.replace('\\', '/').replace('/', File.separatorChar);
                sourceImage = sourceImageName;
                destImage = destImageName;
                blurImage = tempBlurImageName;
                sourcePath = sourceImageName.substring(0, sourceImageName.lastIndexOf(File.separatorChar));
                sourceImageName = sourceImageName.substring(sourceImageName.lastIndexOf(File.separatorChar) + 1,
                        sourceImageName.length());
                destPath = destImageName.substring(0, destImageName.lastIndexOf(File.separatorChar));
                destImageName = destImageName.substring(destImageName.lastIndexOf(File.separatorChar) + 1,
                        destImageName.length());
            } else {
                //如果路径最后面没有加"/"或"\"，则自动给它加上
                path = path.replace('\\', '/').replace('/', File.separatorChar);
                if(!path.substring(path.length() - 1, path.length()).equals(String.valueOf(File.separatorChar)))
                    path += File.separatorChar;
                sourcePath = path;
                destPath = path;
                //得到完整的文件名
                sourceImage = path + sourceImageName;
                destImage = path + destImageName;
                blurImage = path + tempBlurImageName;
            }

            //System.out.println("-------开始生成图片" + destImage);

            Image imageSource = (new ImageIcon(sourceImage)).getImage();

            //计算缩小比例
            int sourceWidth = imageSource.getWidth(null);
            int sourceHeight = imageSource.getHeight(null);
            double rateW = (double) destImageWidth / (double) sourceWidth;
            double rateH = 0;
            double rate = rateW;
            if(destImageHeight > 0) {
                rateH = (double) destImageHeight / (double) sourceHeight;
                rate = Math.min(rate, rateH);
            } else if(destImageHeight == -1) {
                rateH = (double) destImageWidth / (double) sourceHeight;
                rate = Math.min(rate, rateH);
                destImageWidth = (int) (rate * sourceWidth);
                destImageHeight = (int) (rate * sourceHeight);
            }

            //System.out.println("源图片名称："+sourceImageName);
            //System.out.println("原始比率："+rate);

            //如果原文件的宽度*3还是小于原文件的高度,那么重新计算比例
            //if(sourceWidth * 3 < sourceHeight)
            //    rate = (double)destImageWidth / (double)sourceHeight;

            //得到新的图片宽高
            int destWidthCal = (int) (rate * (double) sourceWidth);
            int destHeightCal = (int) (rate * (double) sourceHeight);
            //System.out.println("原图片："+sourceWidth+" "+sourceHeight);
            int availableImageWidth = destWidthCal;
            int availableImageHeight = destHeightCal;
            int destWidth = Math.max(destWidthCal, destImageWidth);
            int destHeight = Math.max(destHeightCal, destImageHeight);
            if(rate > 1) {
                if(destWidth > sourceWidth)
                    availableImageWidth = sourceWidth;
                if(destHeight > sourceHeight)
                    availableImageHeight = sourceHeight;
                if(destImageHeight <= 0)
                    destHeight = sourceHeight;
            }
            //System.out.println("新图片："+destWidth+" "+destHeight);
            //System.out.println("有效图片："+availableImageWidth+" "+availableImageHeight);

            //图片的偏移量
            int shiftX = 0, shiftY = 0;
            //如果宽高的缩放比率不一致，即将会出现空白部分
            if((availableImageWidth < destWidth) || (availableImageHeight < destHeight)) {
                shiftX = (destWidth - availableImageWidth) / 2;
                shiftY = (destHeight - availableImageHeight) / 2;
            }

            boolean doBlur = false;
            //生成图像的柔化处理
            if((needBlurImage) && (destWidth <= 200) && (destHeight <= 200) &&
                    (sourceWidth > 300) &&
                    ((sourceWidth > destWidth) || (sourceHeight > destHeight))) {
                //先对原图像进行平滑处理
                //System.out.println("图片柔化界限："+blurTempImgWidth);
                /*0627不管怎样，都不要用ImageIO.write的方式生成柔化图片，这样的方式在处理某些图片时会出现BUG
                即对ImageIO.write生成的柔化图片进行柔化时速度会超级慢，可能时ImageIO.write后没有及时释放柔化
                图片的缘故！所以这里改成不管怎样都用generateImage来生成柔化图片
                0703＝＝发现用FileCopy和Rotate的方式也会出现同样的问题，只好还是用SuperImage的Generate来
                生成我们要柔化的图像了
                */
                if((blurTempImgWidth > 0) && ((sourceWidth > blurTempImgWidth) ||
                        (sourceHeight > blurTempImgWidth))) {
                    generateImage(null, sourceImage, blurImage, quality, blurTempImgWidth, -1, fillSpacingColor, false, 0, null, false);
                    //System.out.println("-------用SuperImage生成了柔化图片" + blurImageName);
                } else {
                    //System.out.println("=拷贝生成了柔化图片"+blurImageName);
                    //SuperImage.rotateImage(null,sourceImage,blurImage,quality,0,false,fillSpacingColor);
                    //System.out.println("=旋转生成了柔化图片"+blurImageName);
                    generateImage(null, sourceImage, blurImage, quality, sourceWidth, 0, fillSpacingColor, false, 0, null, false);
                    //System.out.println("-------还是用SuperImage生成了柔化图片" + blurImageName);
                    //BufferedImage bufTemp=ImageIO.read(new File(sourceImage));
                    //ImageIO.write(bufTemp,"jpg",new File(blurImage));
                }
                //System.out.println("生成了柔化图片"+blurImage);
                //if(true) return;

                int blur = 0;
                if(rate < blurLevel1)
                    blur = 7;
                else if(rate < blurLevel2)
                    blur = 5;
                else if(rate < blurLevel3) blur = 3;
                //System.out.println("-------缩放比率：" + rate);
                //System.out.println("-------柔化程度：" + blur);
                //System.out.println("========================");
                //System.out.println("开始柔化图片");
                imageSource = null;
                smoothImage(null, blurImage, 0, 0, blur);
                //System.out.println("-------图片柔化完成");
                //imageSource =  (new ImageIcon(blurImage)).getImage();
                imageSource = ImageIO.read(new File(blurImage));
                //System.out.println("-------重新装载了柔化过的图片");
                if(delBlurImage) deleteFile(blurImage);
                doBlur = true;
            }

            //用新的宽高创建一个BufferedImage对象
            BufferedImage bufImageDest = new BufferedImage(destWidth, destHeight, 1);

            //把背景设置为指定颜色
            int[] aRGB = new int[bufImageDest.getWidth() * bufImageDest.getHeight()];
            for(int i = 0; i < aRGB.length; i++) aRGB[i] = fillSpacingColor.getRGB();
            bufImageDest.setRGB(0, 0, bufImageDest.getWidth(), bufImageDest.getHeight(), aRGB, 0, 1);
            //System.out.println("设置背景完成");

            //再创建一个仿射变形对象,目前没有用到此对象
            AffineTransform atf = new AffineTransform();
            //如果图片是按照指定宽来进行缩放的（即未指定高或者宽缩放比率和高缩放比率一致）
            if((availableImageWidth >= destWidth) && (availableImageHeight >= destHeight))
                atf.scale(rate, rate);//缩小到新图片尺寸

            //用BufferedImage对象来创建一个Graphics2D对象
            Graphics2D graphic2D = bufImageDest.createGraphics();

            Color bcolor = new Color(255, 255, 255);
            graphic2D.setBackground(bcolor);

            //把图像装入graphic2D中
            graphic2D.drawImage(imageSource, shiftX, shiftY, availableImageWidth, availableImageHeight, bcolor, null);
            //graphic2D.drawImage(imageSource,atf,null);
            graphic2D.dispose();
            //System.out.println("图像装入graphic2D中完成");

            //创建目标文件的输出流
            OutputStream outstream = new FileOutputStream(destImage);
            //System.out.println("目标文件的输出流被创建");

            //JPEG图像编码器
            JPEGImageEncoder jpegencoder = JPEGCodec.createJPEGEncoder(outstream);
            //JPEG图像编码参数
            JPEGEncodeParam jpegencoderparam = jpegencoder.getDefaultJPEGEncodeParam(bufImageDest);
            quality = Math.max(0, Math.min(quality, 100));
            jpegencoderparam.setQuality((float) quality / 100F, false);
            jpegencoder.setJPEGEncodeParam(jpegencoderparam);
            //JPEG编码器对目标文件对象进行编码
            jpegencoder.encode(bufImageDest);
            //System.out.println("JPEG编码器对目标文件对象进行编码");

            //OutputStream关闭，结束文件输出
            outstream.close();
            //下面这句很重要，时因为有时我们需要删除源文件，但是如果前面没有进行过blur的话。
            //sourceIamge会被imageSource对象占住一定时间，这时外面的程序就删除不了源文件，
            //因此这里将imageSource重新指向到另外一个文件，以便外面的程序对源文件进行删除
            if(!doBlur) imageSource = (new ImageIcon(destImage)).getImage();
            imageSource = null;
            //System.out.println("-------成功生成了图片" + destImageName);
        } catch(IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * 生成新图片文件 异构方法
     * 为原网站旧程序提供的接口
     *
     * @param sourceImage    ：源图片文件（完整物理路径和文件名）
     * @param destImage      : 目标图片文件（完整物理路径和文件名）
     * @param quality        : 生成的图片的压缩质量
     * @param destImageWidth :生成的图片的宽度
     */
    public static void generateImage(String sourceImage, String destImage, int quality,
                                     int destImageWidth) {
        generateImage(null, sourceImage, destImage, quality, destImageWidth, 0, new Color(255, 255, 255), false, 0, null, false);
    }


    /**
     * 对图像进行旋转变换  徐剑波 2006.06.09
     *
     * @param path               文件所在的路径，可以为空，这时sourceImageName和destImageName就必须是带路径的完整文件名
     *                           如果path不为空，则sourceImageName和destImageName必须只是文件名(不带路径），这时目标文件只能存储在
     *                           源文件同一个目录下
     * @param sourceImageName    源文件名称 格式见上面的说明
     * @param destImageName      目标文件名称 格式见上面的说明
     * @param quality            生成新的图片的压缩质量
     * @param degree             旋转度数 0-360，超过的话系统会自动转换到0-360之间
     * @param remainGIF2JPGImage 因为如果源文件不是jpg图片，则系统会先生成一个jpg图片，然后再来旋转
     *                           此参数规定完成后是否删除掉系统临时生成的jpg图片
     * @param fillBGColor        旋转后留下的四周空白处的填充色
     */
    public static void rotateImage(String path, String sourceImageName, String destImageName, int quality,
                                   int degree, boolean remainGIF2JPGImage, Color fillBGColor) {
        try {
            //转换度数
            while((degree < 0) || (degree >= 360)) {
                if(degree < 0) degree = 360 + degree;
                if(degree >= 360) degree = degree - 360;
            }
            //if(degree==0) return;

            String sourceImage = "";
            String destImage = "";
            String sourcePath = "";
            String destPath = "";
            if((path == null) || (path.length() == 0)) {
                sourceImage = sourceImageName;
                destImage = destImageName;
                sourcePath = sourceImageName.substring(0, sourceImageName.lastIndexOf(File.separatorChar));
                sourceImageName = sourceImageName.substring(sourceImageName.lastIndexOf(File.separatorChar) + 1,
                        sourceImageName.length());
                destPath = destImageName.substring(0, destImageName.lastIndexOf(File.separatorChar));
                destImageName = destImageName.substring(destImageName.lastIndexOf(File.separatorChar) + 1,
                        destImageName.length());
            } else {
                //如果路径最后面没有加"/"或"\"，则自动给它加上
                path = path.replace('\\', '/').replace('/', File.separatorChar);
                if(!path.substring(path.length() - 1, path.length()).equals(String.valueOf(File.separatorChar)))
                    path += File.separatorChar;
                sourcePath = path;
                destPath = path;
                //得到完整的文件名
                sourceImage = path + sourceImageName;
                destImage = path + destImageName;
            }
            BufferedImage bufImageBefore = ImageIO.read(new File(sourceImage));
            String tempJPGFile = "";
            //如果图片不是JPG格式的，则需要生成一个临时的JPG文件
            if(!getFileExtName(sourceImageName).equalsIgnoreCase("jpg")) {
                tempJPGFile = sourcePath + getFileName(sourceImageName) + ".jpg";
                SuperImage.generateImage(null, sourceImage, tempJPGFile, 100, bufImageBefore.getWidth(), 0, new Color(255, 255, 255), false, 0, null, false);
                bufImageBefore = ImageIO.read(new File(tempJPGFile));
            }

            //开始旋转并输出
            AffineTransform af = AffineTransform.getRotateInstance(Math.toRadians(degree));

            double dx = 0;
            double dy = 0;
            double degreerad = Math.toRadians(degree);
            int imageW = bufImageBefore.getWidth();
            int imageH = bufImageBefore.getHeight();
            if(degree <= 90) {
                dx = imageH * Math.sin(degreerad) * Math.cos(degreerad);
                dy = -imageH * Math.sin(degreerad) * Math.sin(degreerad);
            } else if(degree <= 180) {
                dx = -imageW * Math.cos(degreerad) * Math.cos(degreerad);
                dy = -(imageH -
                        imageW * Math.sin(degreerad) * Math.cos(degreerad));
            } else if(degree <= 270) {
                dx = -(imageW + imageH * Math.sin(degreerad) * Math.cos(degreerad));
                dy = -imageH * Math.cos(degreerad) * Math.cos(degreerad);
            } else if(degree <= 360) {
                dx = -imageW * Math.sin(degreerad) * Math.sin(degreerad);
                dy = -imageW * Math.sin(degreerad) * Math.cos(degreerad);
            }
            af.translate(dx, dy);

            AffineTransformOp afop = new AffineTransformOp(af, null);
            BufferedImage bufImageAfter = afop.filter(bufImageBefore, null);

            //如果不是90度旋转，则把空白部分设置为指定的填充色
            if((degree % 90) != 0) {
                for(int i = 0; i < bufImageAfter.getWidth(); i++) {
                    for(int j = 0; j < bufImageAfter.getHeight(); j++) {
                        if(!inImageZone(i, j, imageW, imageH, degree))
                            bufImageAfter.setRGB(i, j, fillBGColor.getRGB());
                    }
                }
            }

            ImageIO.write(bufImageAfter, "jpg", new File(destImage));

            //最后如果需要删除该产生的临时文件
            if(!remainGIF2JPGImage && !getFileExtName(sourceImageName).equalsIgnoreCase("jpg")) {
                int i = 0;
                while(!deleteFile(tempJPGFile) && (i < 1000)) {
                    i++;
                    //System.out.println("-------第" + i + "次删除文件" + sourceImageName);
                }
            }
        } catch(IOException e) {
            e.printStackTrace();
        }
    }


    /**
     * 判断坐标轴上的点是否在旋转后的图片内
     *
     * @param x      X轴坐标
     * @param y      Y轴坐标
     * @param w      原图片（旋转之前）的宽度
     * @param h      原图片（旋转之前）的高度
     * @param degree 原图片被旋转的度数
     * @return 在图像内返回true，不在图像内返回false
     */
    public static boolean inImageZone(int x, int y, int w, int h, int degree) {
        if((degree % 90) == 0) return true;
        y = y * -1;
        double rad = Math.toRadians(degree);
        int W = w;
        int H = h;
        if(degree > 270) {
            W = h;
            H = w;
            rad = Math.toRadians(degree - 270);
        } else if(degree > 180) {
            rad = Math.toRadians(degree - 180);
        } else if(degree > 90) {
            W = h;
            H = w;
            rad = Math.toRadians(degree - 90);
        }

        double x0 = H * Math.sin(rad);
        double y0 = 0;
        double x1 = H * Math.sin(rad) + W * Math.cos(rad);
        double y1 = -W * Math.sin(rad);
        double x2 = W * Math.cos(rad);
        double y2 = -(H * Math.cos(rad) + W * Math.sin(rad));
        double x3 = 0;
        double y3 = -H * Math.cos(rad);
        if((x < x0) && (y > (((x - x0) / (x3 - x0)) * (y3 - y0) + y0))) return false;
        if((x > x0) && (y > ((((x - x0) / (x1 - x0)) * (y1 - y0) + y0)))) return false;
        if((x < x2) && (y < ((((x - x2) / (x3 - x2)) * (y3 - y2) + y2)))) return false;
        if((x > x2) && (y < ((((x - x1) / (x2 - x1)) * (y2 - y1) + y1)))) return false;

        return true;
    }


    /**
     * 对图像进行平滑(柔化)处理
     *
     * @param destPath      目标图像路径
     * @param destImageName 目标图像
     * @param aX            有效图像X坐标
     * @param aY            有效图像Y坐标
     * @param blur          柔化程度
     */
    public static void smoothImage(String destPath, String destImageName, int aX, int aY, int blur) {
        try {
            if((blur <= 0) || (blur > 7)) return;
            if(blur < 4) {
                blur = 3;
            } else if(blur < 6) {
                blur = 5;
            } else if(blur < 8) {
                blur = 7;
            }
            String destImage = destImageName;
            if((destPath != null) && (!destPath.equals(""))) {
                destPath = destPath.replace('\\', '/').replace('/', File.separatorChar);
                if(!destPath.substring(destPath.length() - 1, destPath.length()).equals(String.valueOf(File.separatorChar)))
                    destPath += File.separatorChar;
                destImage = destPath + destImageName;
            }


            Color c = null;
            //System.out.println("柔化图片开始");
            BufferedImage bufImage = ImageIO.read(new File(destImage));
            int aW = bufImage.getWidth();
            int aH = bufImage.getHeight();
            //System.out.println("目标文件读取到源文件完成");
            //BufferedImage bufImageOut=bufImage;
            BufferedImage bufImageOut = ImageIO.read(new File(destImage));
            //BufferedImage bufImageOut=new BufferedImage(aW,aH,1);
            //System.out.println("目标文件读取到输出文件完成");
            //System.out.println("目标文件宽"+aW+" 高"+aH);

            //for(int i=aX+(blur-2);i<aW-aX-(Math.ceil(blur/2)) ;i++){
            //    for(int j=aY+(blur-2);j<aH-aY-(Math.ceil(blur/2)) ;j++){
            for(int i = aX + (blur - 2); i < aW - aX - (blur - 2); i++) {
                for(int j = aY + (blur - 2); j < aH - aY - (blur - 2); j++) {
                    int red = 0;
                    int green = 0;
                    int blue = 0;
                    for(int m = -(int) (Math.floor(blur / 2)); m <= Math.floor(blur / 2); m++) {
                        for(int n = -(int) (Math.floor(blur / 2)); n <= Math.floor(blur / 2); n++) {
                            c = new Color(bufImage.getRGB(i + m, j + n));
                            red += c.getRed();
                            green += c.getGreen();
                            blue += c.getBlue();
                        }
                    }
                    c = new Color(red / (blur * blur), green / (blur * blur), blue / (blur * blur));

                    //if((bufImage.getRGB(i,j)-c.getRGB())>10)
                    bufImageOut.setRGB(i, j, c.getRGB());
                }
                //if(i% 50==0)
                //System.out.println("Smooth柔化水平像素"+i+"完成");
            }

            //System.out.println("图像柔化完成，开始重新写入目标文件");
            ImageIO.write(bufImageOut, "jpg", new File(destImage));
            //System.out.println("重新写入目标文件完成");
        } catch(IOException e) {
            e.printStackTrace();
        }
    }


    /**
     * 返回图片文件的宽和高
     */
    public static int[] getImageWidthHeight(String path, String sourceImageName) {
        int[] aIntImage = {-1, -1};
        String sourceImage = "";
        String sourcePath = "";
        if((path == null) || (path.length() == 0)) {
            sourceImage = sourceImageName;
            sourcePath = sourceImageName.substring(0, sourceImageName.lastIndexOf(File.separatorChar));
            sourceImageName = sourceImageName.substring(sourceImageName.lastIndexOf(File.separatorChar) + 1,
                    sourceImageName.length());
        } else {
            //如果路径最后面没有加"/"或"\"，则自动给它加上
            path = path.replace('\\', '/').replace('/', File.separatorChar);
            if(!path.substring(path.length() - 1, path.length()).equals(String.valueOf(File.separatorChar)))
                path += File.separatorChar;
            sourcePath = path;
            //得到完整的文件名
            sourceImage = path + sourceImageName;
        }
        try {
            BufferedImage bufImageSource = ImageIO.read(new File(sourceImage));
            aIntImage[0] = bufImageSource.getWidth();
            aIntImage[1] = bufImageSource.getHeight();
            bufImageSource = null;
        } catch(IOException e) {
            e.printStackTrace();
        }
        return aIntImage;
    }


    /**
     * 判断文件是否需要柔化
     *
     * @return 柔化指数
     */
    public static int getImageBlurValue(int sourceWidth, int sourceHeight, int destWidth, int destHeight) {
        int blur = 0;
        if((destWidth <= 200) && (destHeight <= 200) && (sourceWidth > 300) &&
                ((sourceWidth > destWidth) || (sourceHeight > destHeight))) {
            //计算缩小比率
            double rateW = (double) destWidth / (double) sourceWidth;
            double rateH = 0;
            double rate = rateW;
            if(destHeight > 0) {
                rateH = (double) destHeight / (double) sourceHeight;
                rate = Math.min(rate, rateH);
            } else if(destHeight == -1) {
                rateH = (double) sourceWidth / (double) destHeight;
                rate = Math.min(rate, rateH);
            }
            if(rate < blurLevel1)
                blur = 7;
            else if(rate < blurLevel2)
                blur = 5;
            else if(rate < blurLevel3) blur = 3;
        }
        //System.out.println("得到柔化值："+blur);
        return blur;
    }


    /**
     * 生成柔化图片
     */
    public static void generateBlurImage(String path, String sourceImageName, int sourceWidth, int sourceHeight,
                                         String blurImageName, int blurImageWidth, int quality,
                                         Color fillSpacingColor) throws IOException {

        String sourceImage = "";
        String blurImage = "";
        String sourcePath = "";
        String destPath = "";
        if((path == null) || (path.length() == 0)) {
            sourceImage = sourceImageName;
            blurImage = blurImageName;
            sourcePath = sourceImageName.substring(0, sourceImageName.lastIndexOf(File.separatorChar));
            sourceImageName = sourceImageName.substring(sourceImageName.lastIndexOf(File.separatorChar) + 1,
                    sourceImageName.length());
            destPath = blurImageName.substring(0, blurImageName.lastIndexOf(File.separatorChar));
            blurImageName = blurImageName.substring(blurImageName.lastIndexOf(File.separatorChar) + 1,
                    blurImageName.length());
        } else {
            //如果路径最后面没有加"/"或"\"，则自动给它加上
            path = path.replace('\\', '/').replace('/', File.separatorChar);
            if(!path.substring(path.length() - 1, path.length()).equals(String.valueOf(File.separatorChar)))
                path += File.separatorChar;
            sourcePath = path;
            destPath = path;
            //得到完整的文件名
            sourceImage = path + sourceImageName;
            blurImage = path + blurImageName;
        }

        if((sourceWidth > blurImageWidth) || (sourceHeight > blurImageWidth)) {
            //System.out.println("源图片比柔化图片大，重新生成柔化图片");
            generateImage(null, sourceImage, blurImage, 100, blurImageWidth, -1, fillSpacingColor, false, 0, null, false);
        } else {
            BufferedImage bufTemp = null;
            //System.out.println("源图片比指定柔化图片小，直接复制成柔化图片");
            generateImage(null, sourceImage, blurImage, 100, sourceWidth, 0, fillSpacingColor, false, 0, null, false);
            //bufTemp = ImageIO.read(new File(sourceImage));
            //ImageIO.write(bufTemp,"jpg",new File(blurImage));
            //bufTemp=null;
        }

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

    public static void main(String[] args) throws IOException {
        Color c = new Color(-1);

        for(int i = 0; i < 8; i++) {
            //generateImage("d:/pic",i+".jpg","T"+i+".jpg",100,132,99,c,true,800);
        }

        //SuperImage.rotateImage("f:/pic", "2.jpg", "R2.jpg", 100, 0, false, c);
        //SuperImage.generateImage("f:/pic", "R2.jpg", "D2.jpg", 85, 132, 99, c, true, 800, "blur2.jpg", false);

        //System.out.println(getFileSize("d:/pic","040701130833928038.jpg"));
        //generateImage("f:/pic","1.jpg","T1.jpg",100,132,99,c,true,750,"blur1.jpg",false);
        //generateImage("f:/pic","1.jpg","T1.jpg",100,600,0,c,false,750,null,false);
        //generateBlurImage("f:/pic","1.jpg",800,600,"T1.jpg",1000,100,c);
        //smoothImage("f:/pic","T1.jpg",0,0,5);
        //smoothImage("d:/pic/T0.jpg",0,0,3);
        //rotateImage("d:/pic/1","0.jpg","00.jpg",100,60,false,c);
        //System.out.println(getImageWidthHeight("d:/pic","2.jpg")[0]);
        //System.out.println(getImageWidthHeight("d:/pic","2.jpg")[1]);


        for(int i = 0; i < 5; i++) {
            //generateImage("d:/pic/","5.jpg","T5"+i+".jpg",100,60*(i+1),0,c,true);
        }
        /*
                int k=0;
                for(int i=0;i<=360 ;i+=15){
                    SuperImage.rotateImage("d:/pic","00.jpg",i+".jpg",100,i,false,new Color(255,k,k));
                    k+=10;
                    System.out.println(i);
                }
        */
        //SuperImage.rotateImage("d:/pic","0.jpg","zz.jpg",100,30,false);
        //SuperImage.rotateImage("d:/pic","z.jpg","zz1.jpg",100,45,false);
        //SuperImage.rotateImage("d:/pic","z.jpg","zz2.jpg",100,60,false);
    }
}

