-- MySQL dump 10.13  Distrib 5.1.41, for debian-linux-gnu (i486)
--
-- Host: localhost    Database: ppbox
-- ------------------------------------------------------
-- Server version	5.1.41-3ubuntu12.10

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `t_accessory_type`
--

DROP TABLE IF EXISTS `t_accessory_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_accessory_type` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_accessory_type`
--

LOCK TABLES `t_accessory_type` WRITE;
/*!40000 ALTER TABLE `t_accessory_type` DISABLE KEYS */;
INSERT INTO `t_accessory_type` VALUES (1,'鼠标'),(2,'键盘'),(3,'电源'),(4,'遥控器'),(5,'串口线');
/*!40000 ALTER TABLE `t_accessory_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_application`
--

DROP TABLE IF EXISTS `t_application`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_application` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `equipment_id` int(10) NOT NULL DEFAULT '0',
  `applicant` int(10) NOT NULL DEFAULT '0',
  `status` varchar(255) NOT NULL DEFAULT '',
  `application_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `confirm_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`),
  KEY `t_equipment_id` (`equipment_id`),
  KEY `t_applicant` (`applicant`),
  CONSTRAINT `t_equipment_id` FOREIGN KEY (`equipment_id`) REFERENCES `t_equipment` (`id`),
  CONSTRAINT `t_applicant` FOREIGN KEY (`applicant`) REFERENCES `t_engineer` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_application`
--

LOCK TABLES `t_application` WRITE;
/*!40000 ALTER TABLE `t_application` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_application` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_branch`
--

DROP TABLE IF EXISTS `t_branch`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_branch` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `product_id` int(10) NOT NULL DEFAULT '0',
  `description` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `t_product_id` (`product_id`),
  CONSTRAINT `t_product_id` FOREIGN KEY (`product_id`) REFERENCES `t_product` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_branch`
--

LOCK TABLES `t_branch` WRITE;
/*!40000 ALTER TABLE `t_branch` DISABLE KEYS */;
INSERT INTO `t_branch` VALUES (2,'1.0.0',1,'STB版本发布，支持play'),(3,'1.1.0',1,'支持ts flv索引文件'),(4,'1.2.0',1,'支持广告插播，新demer框架'),(5,'0.5.3',1,'早期STB版本,不带peer');
/*!40000 ALTER TABLE `t_branch` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_combination_strategy`
--

DROP TABLE IF EXISTS `t_combination_strategy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_combination_strategy` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `comb_id` int(10) NOT NULL,
  `strategy_id` int(10) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `comb_strategy_id` (`strategy_id`),
  KEY `comb_id` (`comb_id`),
  CONSTRAINT `comb_id` FOREIGN KEY (`comb_id`) REFERENCES `t_strategy` (`id`),
  CONSTRAINT `comb_strategy_id` FOREIGN KEY (`strategy_id`) REFERENCES `t_strategy` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=180 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_combination_strategy`
--

LOCK TABLES `t_combination_strategy` WRITE;
/*!40000 ALTER TABLE `t_combination_strategy` DISABLE KEYS */;
INSERT INTO `t_combination_strategy` VALUES (1,81,65),(2,81,66),(3,81,63),(4,81,64),(5,81,61),(6,81,62),(7,81,60),(8,82,79),(9,82,81),(10,82,1),(11,82,67),(12,82,73),(13,82,72),(14,82,76),(15,83,80),(16,83,81),(17,83,1),(18,83,67),(19,83,73),(20,83,76),(21,83,72),(22,84,78),(23,84,81),(24,84,1),(25,84,67),(26,84,73),(27,84,72),(28,84,75),(29,85,78),(30,85,81),(31,85,1),(32,85,69),(33,85,74),(34,85,72),(35,85,76),(36,86,78),(37,86,81),(38,86,1),(39,86,69),(40,86,74),(41,86,72),(42,86,75),(43,87,78),(44,87,81),(45,87,1),(46,87,69),(47,87,74),(48,87,72),(49,87,75),(50,87,71),(51,87,76),(162,88,71),(163,88,69),(164,88,72),(165,88,74),(166,88,75),(167,88,76),(168,90,78),(169,90,1),(170,90,69),(171,90,72),(172,90,74),(173,90,76),(174,89,78),(175,89,1),(176,89,69),(177,89,72),(178,89,74),(179,89,75);
/*!40000 ALTER TABLE `t_combination_strategy` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_company`
--

DROP TABLE IF EXISTS `t_company`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_company` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `type_id` int(10) NOT NULL DEFAULT '0',
  `address` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`),
  KEY `comp_type_id` (`type_id`),
  CONSTRAINT `comp_type_id` FOREIGN KEY (`type_id`) REFERENCES `t_company_type` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=41 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_company`
--

LOCK TABLES `t_company` WRITE;
/*!40000 ALTER TABLE `t_company` DISABLE KEYS */;
INSERT INTO `t_company` VALUES (1,'PPTV',4,'上海浦东新区碧波路690号张江微电子港5号楼5楼\n'),(2,'华数',5,'杭州\n'),(3,'TCL',6,' \n'),(4,'海尔',6,' \n'),(5,'大连天维',6,' \n'),(6,'碧维视',6,' \n'),(7,'新利实业',6,' \n'),(8,'联想',6,' \n'),(9,'精伦',6,' \n'),(10,'长虹',6,'绵阳\n'),(11,'松下',6,' \n'),(12,'LG',6,' \n'),(13,'海信',6,' \n'),(14,'杰科',6,' \n'),(15,'迈乐',6,' \n'),(17,'康佳',6,' \n'),(18,'创维',6,' \n'),(19,'集成视讯',6,' \n'),(20,'CNTV',5,'北京'),(21,'茁壮',2,'深圳'),(22,'瑞芯',1,'福州'),(23,'IM科技',6,''),(24,'全智',1,''),(25,'富士通',1,''),(26,'MStar',1,''),(27,'三星',6,''),(28,'Motorola',6,''),(29,'HTC',6,''),(30,'万利达',6,''),(31,'正路航',6,'深圳'),(32,'MIC',6,''),(33,'MFLD',6,''),(34,'中兴',6,''),(36,'华为',6,''),(37,'飞思卡尔',1,''),(38,'海思',1,''),(39,'skyvia',6,''),(40,'巍岭数码',6,'上海');
/*!40000 ALTER TABLE `t_company` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_company_type`
--

DROP TABLE IF EXISTS `t_company_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_company_type` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_company_type`
--

LOCK TABLES `t_company_type` WRITE;
/*!40000 ALTER TABLE `t_company_type` DISABLE KEYS */;
INSERT INTO `t_company_type` VALUES (1,'芯片厂商'),(2,'中间件厂商'),(3,'内容提供商'),(4,'服务提供商'),(5,'牌照方'),(6,'设备生产商');
/*!40000 ALTER TABLE `t_company_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_engineer`
--

DROP TABLE IF EXISTS `t_engineer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_engineer` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `company_id` int(10) NOT NULL DEFAULT '0',
  `type_id` int(10) NOT NULL DEFAULT '0',
  `telephone` varchar(40) NOT NULL,
  `email` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `eng_name` (`name`),
  KEY `engin_type_id` (`type_id`),
  KEY `eng_company_id` (`company_id`),
  CONSTRAINT `engin_type_id` FOREIGN KEY (`type_id`) REFERENCES `t_staff_type` (`id`),
  CONSTRAINT `eng_company_id` FOREIGN KEY (`company_id`) REFERENCES `t_company` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=83 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_engineer`
--

LOCK TABLES `t_engineer` WRITE;
/*!40000 ALTER TABLE `t_engineer` DISABLE KEYS */;
INSERT INTO `t_engineer` VALUES (1,'胡佳文',3,3,'15012652208 | 0755-33312872','hujiawen@tcl.com\n'),(2,'陈澄',3,3,'15989868769 | 0755-33312872','chencheng@tcl.com\n'),(3,'费流波',3,3,'13632829142 | 0755-33313237','feilb@tcl.com\n'),(4,'傅政',2,3,'','\n'),(5,'刘波',2,3,'','\n'),(6,'聂小燕',21,3,'','\n'),(7,'李晓冬',10,3,'15881649919','xiaodong.li@changhong.com\n'),(8,'王必娇',10,3,'18608168751','bijiao.wang@changhong.com\n'),(9,'廖福成',10,3,'18981180988 | 0816-2410187','fucheng.liao@changhong.com\n'),(10,'洪亮',12,3,'13911971631 | 010-64390099-5174','liang.hong@lge.com\n'),(11,'黄荣国',12,3,'18611445336','rongguo.huang@lge.com\n'),(12,'任伟',12,3,'13911971675','wei.ren@lge.com \n'),(13,'黄俊杰',4,3,' 0532-88936442','junjie_huang@haier.com\n'),(14,'袁斌 ',4,3,'18669700829','yuanbin@haiersoft.com\n'),(15,'耿晓慧',4,3,'18669787839','gengxh@haiersoft.com\n'),(16,'顾微微',4,3,'15954804270','guww@haiersoft.com\n'),(17,'崔永祯',11,3,'13801297252','cuiyongzhen@cn.panasonic.com\n'),(18,'李勇',11,3,'0411-84768550-2018','liyong02@cn.panasonic.com\n'),(19,'李继勇',11,3,'0411-84790599-2568','jyli@chmavc.panasonic.com.cn\n'),(20,'武玉朋',13,3,'18669712870','wuyupeng@hisense.com\n'),(21,'董波',13,3,'','dongbo@hisense.com\n'),(22,'姜恒工',13,3,'18678465973','\n'),(23,'李鹏',14,3,'','lip@giec.cn\n'),(24,'李永雄',14,3,'','liyx@giec.cn\n'),(25,'李国柱',15,3,'13823136211','\n'),(26,'高飞',15,3,'13925238431 | 0755- 28425502','gaofei@huawei.com\n'),(27,'肖凌',15,3,'18971388099 | 027-67845148-806','lingx@mail.hust.edu.cn\n'),(28,'郑昊',2,3,'','\n'),(29,'刘波',2,3,'','\n'),(30,'何万江',2,3,'','\n'),(31,'李孝锐',2,3,'','\n'),(32,'隗立涛',8,3,'18610542565 | 010-58866203','weilitao@ismartv.cn | weilt@lenovo.com\n'),(33,'邓铁铖',8,3,'13439868879 | 010-62667626-210','dengtiecheng@ismartv.cn\n'),(34,'李沛鸿',20,3,'','iptv_lipeihong@126.com\n'),(35,'沈伟',20,3,'18601264228','shenwei@staff.cntv.cn | iptv_shenwei@126.com\n'),(36,'王焕煜',20,3,'13001180811','wanghuanyu@ysten.com\n'),(37,'张正阳',4,3,'15964201697 | 0532-88038142','zhangzy@haiersoft.com\n'),(38,'尚衍筠 ',4,3,'13370853036','shangyj@haiersoft.com\n'),(39,'刘文飞',3,3,'','lwf@tcl.com\n'),(40,'赵亮',3,3,'18666293096 | 0755-33312102','zhaoliang@tcl.com\n'),(41,'高帆',3,3,'13691852304','gaofan@tcl.com\n'),(42,'程利根',3,3,'','chenglg@tcl.com\n'),(43,'吴东阳',5,3,'13478713431 | 0411-84542900-615','wudongyang@5itvi.com\n'),(44,'龙志华',6,3,'13543299529 | 0755-86329087','lion.long@zhdsoft.com\n'),(45,'曾行安',7,3,'0755-84711789-822','zengxingan@solid.com.cn\n'),(46,'马洪生',8,3,'13911122467 | 010-58861802','mahs1@lenovo.com\n'),(47,'张晋青',8,3,'18910860598','zhangjq1@lenovo.com\n'),(48,'万杰',9,3,'18627792648 | 027-87921111-2432','wanjie@routon.com | wanjie@jinglun.com.cn\n'),(49,'杨木叶',9,3,'027-87921111-2434','yangmuye@routon.com\n'),(50,'王晓星',3,3,'15013869528 | 0755-33311383','wangxiaoxing@huan.tv\n'),(51,'袁戬',3,3,'13632511141 | 0755-36645351','yjian@tcl.com\n'),(52,'王海兵 ',10,3,'13881130958','wanghb@changhong.com\n'),(53,'王晓星',10,3,'15013869528 | 0755-33311383','wangxiaoxing@huan.tv\n'),(54,'杨毅',10,3,'15983678216 | 0816-2417632','yangyi@changhong.com\n'),(55,'缪鸿生',10,3,'0816-2417020','hongsheng.miao@changhong.com\n'),(56,'毕端',10,3,'15882848471 | 0816-2417020','duan.bi@changhong.com\n'),(57,'谢仁斌',18,3,'13510349280','scorpio.xie@coocaa.com\n'),(58,'王海亮',13,3,'13573229852 | 0532-80874508','wanghailiang@hisense.com\n'),(59,'王端方',13,3,'13656422534 | 0532-80876929','wangduanfang@hisense.com\n'),(60,'王传吉',13,3,'15905428783 | 0532-80876925','wangchuanji@hisense.com\n'),(61,'李莉',13,3,'13153265443','lili8@hisense.com\n'),(62,'杨珍玺',13,3,'18653231410 | 13701335860 | 0532-8087694','wangchuanji@hisense.com\n'),(63,'万承龙',17,3,'13530657579 | 0755-26608866-6267','wanchenglong@konka.com\n'),(64,'赵文强',4,3,'0532-88936443','zhaowenqiang@haier.com\n'),(65,'李鹏',4,3,'0532-88038141','lipeng@haiersoft.com\n'),(68,'吴刚锋',15,3,'15818576207 | 0755-86363247','wugf@mele.cn\n'),(69,'张奎',15,3,'1358175597','zhangkui@mele.cn\n'),(70,'冯强周',15,3,'13434780029','fengqz@mele.cn\n'),(71,'黄雪梅',15,3,'13418665905 | 0755-86363200-8510','huangxm@mele.cn\n'),(72,'柯卯明',21,3,'',''),(73,'何昆懋',1,1,'13666236095','darrenhe@pptv.com'),(74,'敬徳天',40,1,'18980808712','dtjing @vlctech.com '),(75,'吕华洋',9,1,'027-87921111-2443','www.routon.com'),(76,'童佳',9,1,'027-87921111-2426 ','tongjia@jinglun.com.cn'),(77,'原建业',8,1,'','yuanjianye@routon.com'),(78,'王宗锋',9,4,'027-87921111-3316',''),(79,'朱春清',9,1,'027-87921111-2412','zhuchunqing@routon.com'),(80,'侯丁丁',11,3,'',''),(81,'左腾',2,1,'15858265329','zuoteng@wasu.com.cn'),(82,'方亮',1,3,'','');
/*!40000 ALTER TABLE `t_engineer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_equipment`
--

DROP TABLE IF EXISTS `t_equipment`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_equipment` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `company_id` int(10) NOT NULL DEFAULT '0',
  `chip_company_id` int(10) NOT NULL DEFAULT '0',
  `platform_id` int(10) NOT NULL DEFAULT '0',
  `player` varchar(255) NOT NULL DEFAULT '',
  `owner` int(10) unsigned NOT NULL DEFAULT '0',
  `description` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `company_id` (`company_id`),
  KEY `chip_company_id` (`chip_company_id`),
  KEY `platform_id` (`platform_id`),
  KEY `equip_owner` (`owner`),
  CONSTRAINT `chip_company_id` FOREIGN KEY (`chip_company_id`) REFERENCES `t_company` (`id`),
  CONSTRAINT `company_id` FOREIGN KEY (`company_id`) REFERENCES `t_company` (`id`),
  CONSTRAINT `equip_owner` FOREIGN KEY (`owner`) REFERENCES `t_user` (`id`),
  CONSTRAINT `platform_id` FOREIGN KEY (`platform_id`) REFERENCES `t_platform` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=45 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_equipment`
--

LOCK TABLES `t_equipment` WRITE;
/*!40000 ALTER TABLE `t_equipment` DISABLE KEYS */;
INSERT INTO `t_equipment` VALUES (1,'IMT-TV-BOX',23,22,5,'',2,'12伏直流电源，大号SD卡插槽。'),(2,'海信SmartTV',13,1,5,'AmlogicPlayer',2,'芯片厂商未知。'),(3,'迈乐客厅电脑1（卓影A1000）',15,24,5,'',2,'完整包装盒'),(4,'迈乐客厅电脑2（卓影A1000）',15,24,5,'',2,'完整包装盒，附件除电源和遥控器外，还有串口线一条。'),(5,'康佳（Mstar）',17,26,5,'',2,'分上下两层，体型较大。'),(6,'联想乐ipad',8,8,5,'HelixPlayer',2,''),(7,'Zpad(万利达)',30,30,5,'',2,'客户'),(8,'SamSung(GT-P1000)',27,27,5,'AwesomePlayer',2,'测试'),(9,'SamSung(GT-P7510)',27,27,5,'stagefright/Jackson/1.3.67',2,'测试'),(10,'e路航',31,31,5,'',2,''),(11,'MIC(gs700b)',32,32,5,'',2,''),(12,'MIC(gs700b)',32,32,5,'',2,''),(13,'MFLD(x86)',33,33,5,'Stagefright/1.1',2,'测试'),(14,'ZTE PAD',34,34,5,'',2,''),(15,'SamSung(i9000)(2.3小米)',27,27,5,'StagefrightPlayer',2,'宋巍'),(16,'SamSung(i9000)',27,27,5,'AwesomePlayer',2,'测试'),(17,'SamSung(GT-I9000)',27,27,5,'PVPlayer',2,'测试'),(18,'SamSung(GT-I9220)',27,27,5,'',2,'借用'),(19,'SamSung(GT-I9100)',27,27,5,'',2,'陈志东'),(20,'SamSung(白色)',27,27,5,'PVPlayer',2,'吴季丹'),(21,'Motorola(XT800)',28,28,5,'',2,'陈志东'),(22,'Motorola(ME722)',28,28,5,'PVPlayer',2,'测试'),(23,'Motorola(ME722)(2.3)',28,28,5,'StagefrightPlayer',2,'测试'),(24,'Motorola(XT702)',28,28,5,'',2,'柏启亮'),(25,'HTC(g6)',29,29,5,'',2,'左宗泽'),(26,'HTC(g8)',29,29,7,'',2,'陈相礼'),(27,'HTC(g7)',29,29,5,'StagefrightPlayer',2,'汪奕菲'),(28,'HTC(g15)',29,29,5,'',2,'余鹏'),(29,'HTC(Desire)',29,29,5,'StagefrightPlayer',2,'单总'),(30,'HTC(Sense)(Z710t)',29,29,5,'',2,'测试'),(31,'HTC(Sense)(A8180)',29,29,5,'',2,'无线部'),(32,'HTC(Desire)(2.2)',29,29,5,'',2,'沈吟曦'),(33,'HTC(Sense)白色',29,29,5,'AriMXPlayer',2,'测试'),(34,'华为PHONE',36,36,5,'',2,'测试'),(35,'ZTE-U(V880)',34,34,5,'',2,'项东涛'),(36,'Lenovo(3GW101)',8,8,5,'',2,''),(37,'LG(2.2)',12,12,5,'OMXPlayer',2,'测试'),(38,'LG(2.3.4小米) ',12,12,5,'',2,'陈冬'),(39,'LG(p990)',12,12,5,'',2,'郭天佑'),(40,'长虹(海思)TV',10,38,5,'AmlogicPlayer',2,''),(41,'联想TV',8,8,5,'HelixPlayer',2,''),(42,'飞思卡尔TV',37,37,5,'StagefrightPlayer',2,''),(43,'skyvia BOX',39,39,5,'',2,''),(44,'集成视讯BOX',19,22,5,'',2,'');
/*!40000 ALTER TABLE `t_equipment` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_equipment_accessory`
--

DROP TABLE IF EXISTS `t_equipment_accessory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_equipment_accessory` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `equipment_id` int(10) NOT NULL DEFAULT '0',
  `accessory_id` int(10) NOT NULL DEFAULT '0',
  `description` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `eq_ac_equipment_id` (`equipment_id`),
  KEY `eq_ac_accessory_id` (`accessory_id`),
  CONSTRAINT `eq_ac_accessory_id` FOREIGN KEY (`accessory_id`) REFERENCES `t_accessory_type` (`id`),
  CONSTRAINT `eq_ac_equipment_id` FOREIGN KEY (`equipment_id`) REFERENCES `t_equipment` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_equipment_accessory`
--

LOCK TABLES `t_equipment_accessory` WRITE;
/*!40000 ALTER TABLE `t_equipment_accessory` DISABLE KEYS */;
INSERT INTO `t_equipment_accessory` VALUES (1,1,3,'电源型号：LD-12020A'),(2,1,4,'体感遥控器'),(3,2,3,'型号：BJF-DY209B'),(4,3,3,'型号：ADS-18C-06 05012GPCN'),(5,3,4,'体型较大，中部具有圆形按键。'),(6,4,4,'体型较大，中部具有圆形按键。'),(7,4,3,'型号：ADS-18C-06 05012GPCN'),(8,4,5,'');
/*!40000 ALTER TABLE `t_equipment_accessory` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_equipment_type`
--

DROP TABLE IF EXISTS `t_equipment_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_equipment_type` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_equipment_type`
--

LOCK TABLES `t_equipment_type` WRITE;
/*!40000 ALTER TABLE `t_equipment_type` DISABLE KEYS */;
INSERT INTO `t_equipment_type` VALUES (1,' 电视机'),(2,'机顶盒'),(3,'平板电脑'),(4,'电话'),(5,'电脑');
/*!40000 ALTER TABLE `t_equipment_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_format`
--

DROP TABLE IF EXISTS `t_format`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_format` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_format`
--

LOCK TABLES `t_format` WRITE;
/*!40000 ALTER TABLE `t_format` DISABLE KEYS */;
INSERT INTO `t_format` VALUES (1,'[RAW]'),(2,'FLV'),(3,'TS'),(4,'MKV'),(5,'ASF');
/*!40000 ALTER TABLE `t_format` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_grant`
--

DROP TABLE IF EXISTS `t_grant`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_grant` (
  `role` int(10) unsigned NOT NULL DEFAULT '0',
  `target` int(10) unsigned NOT NULL DEFAULT '0',
  `operation` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`role`,`target`,`operation`),
  KEY `FK_t_grant_target` (`target`),
  KEY `FK_t_grant_operation` (`operation`),
  CONSTRAINT `FK_t_grant_target` FOREIGN KEY (`target`) REFERENCES `t_target` (`id`),
  CONSTRAINT `FK_t_grant_operation` FOREIGN KEY (`operation`) REFERENCES `t_operation` (`id`),
  CONSTRAINT `FK_t_grant_role` FOREIGN KEY (`role`) REFERENCES `t_role` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_grant`
--

LOCK TABLES `t_grant` WRITE;
/*!40000 ALTER TABLE `t_grant` DISABLE KEYS */;
INSERT INTO `t_grant` VALUES (1,1,4),(2,1,1),(3,1,1),(4,1,1),(1,2,4),(2,2,1),(3,2,1),(4,2,1),(1,3,4),(2,3,1),(3,3,1),(4,3,1),(1,4,4),(2,4,1),(3,4,1),(4,4,1),(1,5,4),(2,5,1),(3,5,1),(4,5,1),(1,6,4),(2,6,1),(3,6,1),(4,6,1),(1,7,4),(2,7,1),(3,7,1),(4,7,1),(1,8,4),(2,8,3),(3,8,1),(4,8,1),(1,9,4),(2,9,1),(3,9,1),(4,9,1),(1,10,4),(2,10,4),(3,10,1),(4,10,1),(1,11,4),(2,11,1),(3,11,1),(4,11,1),(1,12,4),(2,12,4),(3,12,1),(4,12,1),(1,13,4),(2,13,4),(3,13,1),(4,13,1),(1,14,4),(2,14,4),(3,14,1),(4,14,1),(1,15,4),(2,15,4),(3,15,1),(4,15,1),(1,16,4),(2,16,1),(3,16,1),(4,16,1),(1,17,4),(2,17,1),(3,17,1),(4,17,1),(1,18,4),(2,18,1),(3,18,1),(4,18,1),(1,19,4),(2,19,1),(3,19,1),(4,19,1),(1,20,4),(2,20,1),(3,20,1),(4,20,1),(1,21,4),(2,21,4),(3,21,4),(4,21,1),(1,22,4),(2,22,1),(3,22,1),(4,22,1),(1,23,4),(2,23,4),(3,23,1),(4,23,1),(1,24,4),(2,24,1),(3,24,4),(4,24,1),(1,25,4),(2,25,1),(3,25,4),(4,25,1),(1,27,4),(2,27,1),(3,27,1),(4,27,1),(1,31,4),(2,31,1),(3,31,1),(4,31,1),(1,33,4),(2,33,4),(3,33,1),(4,33,1),(1,34,4),(2,34,1),(3,34,4),(4,34,1),(1,35,4),(2,35,4),(3,35,1),(4,35,1),(1,36,4),(2,36,4),(3,36,1),(4,36,1);
/*!40000 ALTER TABLE `t_grant` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_joint_debug`
--

DROP TABLE IF EXISTS `t_joint_debug`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_joint_debug` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `track_id` int(10) NOT NULL DEFAULT '0',
  `engineer_id` int(10) NOT NULL DEFAULT '0',
  `user_id` int(10) unsigned NOT NULL DEFAULT '0',
  `description` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `t_engineer_id` (`engineer_id`),
  KEY `t_user_id` (`user_id`),
  CONSTRAINT `t_engineer_id` FOREIGN KEY (`engineer_id`) REFERENCES `t_engineer` (`id`),
  CONSTRAINT `t_user_id` FOREIGN KEY (`user_id`) REFERENCES `t_user` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_joint_debug`
--

LOCK TABLES `t_joint_debug` WRITE;
/*!40000 ALTER TABLE `t_joint_debug` DISABLE KEYS */;
INSERT INTO `t_joint_debug` VALUES (1,0,74,2,'上海环境拖动不了一直卡住'),(2,0,75,2,'在线播放失败'),(3,0,76,2,'一台测试机器上出现一个播放三分钟后就退出的问题'),(4,0,77,6,'trident平台下pptv播放接口库'),(5,0,76,2,'名侦探柯南 第八集 不能播'),(6,0,9,2,' 1）1d7c1WCcaMih2c7j4a6ZncXX3s%2fQ2JY%3d无法播放；         2）以后遇到ppbox_open失败的情况，是否应该执行ppbox_close。');
/*!40000 ALTER TABLE `t_joint_debug` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_operation`
--

DROP TABLE IF EXISTS `t_operation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_operation` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `op` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(45) NOT NULL DEFAULT '',
  `desc` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_operation`
--

LOCK TABLES `t_operation` WRITE;
/*!40000 ALTER TABLE `t_operation` DISABLE KEYS */;
INSERT INTO `t_operation` VALUES (1,8,'查看',''),(2,12,'修改',''),(3,11,'增删',''),(4,15,'完全','');
/*!40000 ALTER TABLE `t_operation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_platform`
--

DROP TABLE IF EXISTS `t_platform`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_platform` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `float_type` int(10) NOT NULL DEFAULT '0',
  `byte_order` int(10) NOT NULL DEFAULT '0',
  `tool_chain` varchar(255) NOT NULL DEFAULT '',
  `description` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `plat_name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=52 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_platform`
--

LOCK TABLES `t_platform` WRITE;
/*!40000 ALTER TABLE `t_platform` DISABLE KEYS */;
INSERT INTO `t_platform` VALUES (1,'actions-6007',0,0,'gcc43\n',''),(2,'actions-6017',0,0,'gcc44\n',''),(3,'ali-3000',0,0,'gcc44\n',''),(4,'arm-926ejs',0,0,'gcc43\n',''),(5,'armandroid-r4',0,0,'gcc44\n',''),(7,'broadcom-7405',0,0,'gcc42\n',''),(8,'c2-1100',0,0,'',''),(9,'c2-1200',0,0,'',''),(10,'ce-3100',0,0,'gcc41\n',''),(11,'ios-armv6',0,0,'gcc42\n',''),(12,'ios-armv7',0,0,'gcc42\n',''),(13,'ipp-freebsd',0,0,'gcc42\n',''),(14,'jz-4755',0,0,'gcc41\n',''),(15,'linux-x64',0,0,'gcc44\n',''),(16,'linux-x86',0,0,'gcc44\n',''),(17,'linux-x86s',0,0,'gcc41\n',''),(18,'marvell-88DE3010',0,0,'gcc41\n',''),(19,'marvell-2007q3',0,0,'gcc41\n',''),(20,'mips-android',0,0,'gcc44\n',''),(21,'mipsandroid-r5b',0,0,'gcc44\n',''),(22,'mips-nt72557',0,0,'gcc43\n',''),(23,'mstar-6a80x',0,0,'gcc45\n',''),(24,'mstar-6i78',0,0,'gcc43\n',''),(25,'mstar-6i98x',0,0,'gcc43\n',''),(26,'mstar-6i98x-float',1,0,'gcc43',''),(27,'mstar-saturn7',0,0,'gcc43\n',''),(28,'mtk-1630',0,0,'gcc44\n',''),(29,'mtk-1655',0,0,'gcc44\n',''),(30,'mtk-5301e',0,0,'gcc44\n',''),(31,'mtk-5325',0,0,'gcc44\n',''),(32,'mtk-5501',0,0,'gcc44\n',''),(33,'mtk-8555',0,0,'gcc44\n',''),(34,'mtk-8580',0,0,'gcc45\n',''),(35,'nxp-8935',0,0,'gcc42\n',''),(36,'plm-3000gb',0,1,'gcc43\n',''),(37,'plm-3000ub',0,1,'gcc42\n',''),(38,'plm-3020gb',0,1,'gcc43\n',''),(39,'plm-3020gl',0,0,'gcc43\n',''),(40,'realtek-1073',0,0,'gcc34\n',''),(41,'realtek-1185',0,0,'gcc43\n',''),(42,'realtek-1185h',0,0,'gcc43\n',''),(43,'sigma-8655',0,0,'gcc43\n',''),(44,'st-7105',0,0,'gcc42\n',''),(45,'tc-8900',0,0,'gcc43\n',''),(46,'trident',0,0,'gcc44\n',''),(47,'trident-h1',0,0,'gcc44\n',''),(48,'win32-i586',0,0,'mingw32\n',''),(49,'arm-hisiv200',0,0,'gcc44',''),(50,'android-86',0,0,'gcc44',''),(51,'trident-pnx8473',0,0,'gcc44','');
/*!40000 ALTER TABLE `t_platform` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_problem`
--

DROP TABLE IF EXISTS `t_problem`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_problem` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `description` varchar(255) NOT NULL DEFAULT '',
  `bug_id` int(10) NOT NULL DEFAULT '0',
  `problem_type` int(10) NOT NULL DEFAULT '0',
  `specificmatters` varchar(255) NOT NULL DEFAULT '',
  `project_id` int(10) NOT NULL DEFAULT '0',
  `begin_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `owner_id` int(10) NOT NULL DEFAULT '0',
  `engineer_id` int(10) NOT NULL DEFAULT '0',
  `solution` int(10) NOT NULL DEFAULT '0',
  `progress` int(10) NOT NULL DEFAULT '0',
  `complete_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `instruction` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `problem_project_id` (`project_id`),
  KEY `problem_owner_id` (`owner_id`),
  KEY `problem_engineer_id` (`engineer_id`),
  CONSTRAINT `problem_engineer_id` FOREIGN KEY (`engineer_id`) REFERENCES `t_engineer` (`id`),
  CONSTRAINT `problem_owner_id` FOREIGN KEY (`owner_id`) REFERENCES `t_engineer` (`id`),
  CONSTRAINT `problem_project_id` FOREIGN KEY (`project_id`) REFERENCES `t_project` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_problem`
--

LOCK TABLES `t_problem` WRITE;
/*!40000 ALTER TABLE `t_problem` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_problem` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_problem_type`
--

DROP TABLE IF EXISTS `t_problem_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_problem_type` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL DEFAULT '',
  `description` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_problem_type`
--

LOCK TABLES `t_problem_type` WRITE;
/*!40000 ALTER TABLE `t_problem_type` DISABLE KEYS */;
INSERT INTO `t_problem_type` VALUES (1,'授权/认证',''),(2,'授权/踢人',''),(3,'网络/用户环境/域名服务',''),(4,'网络/用户环境/接入设备',''),(5,'网络/用户环境/防火墙',''),(6,'后台/跳转',''),(7,'后台/索引',''),(8,'后台/数据服务',''),(9,'代码/SDK BUG',''),(10,'代码/内核',''),(11,'代码/第三方集成',''),(12,'集成/配置','第三方集成设备环境的配置问题'),(13,'集成/对接','第三方对接接口不兼容');
/*!40000 ALTER TABLE `t_problem_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_product`
--

DROP TABLE IF EXISTS `t_product`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_product` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `prod_name` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_product`
--

LOCK TABLES `t_product` WRITE;
/*!40000 ALTER TABLE `t_product` DISABLE KEYS */;
INSERT INTO `t_product` VALUES (2,'多屏互动SDK'),(1,'流媒体SDK');
/*!40000 ALTER TABLE `t_product` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_project`
--

DROP TABLE IF EXISTS `t_project`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_project` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `priority` int(10) NOT NULL DEFAULT '1',
  `begin_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `platform_id` int(10) NOT NULL DEFAULT '0',
  `description` varchar(255) NOT NULL DEFAULT '',
  `progress` int(10) NOT NULL DEFAULT '0',
  `result` int(10) NOT NULL DEFAULT '0',
  `complete_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `strategy_id` int(10) NOT NULL DEFAULT '0',
  `product_id` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `pro_name` (`name`),
  KEY `pro_platform_id` (`platform_id`),
  KEY `pro_strategy_id` (`strategy_id`),
  KEY `pro_product_id` (`product_id`),
  CONSTRAINT `pro_platform_id` FOREIGN KEY (`platform_id`) REFERENCES `t_platform` (`id`),
  CONSTRAINT `pro_product_id` FOREIGN KEY (`product_id`) REFERENCES `t_product` (`id`),
  CONSTRAINT `pro_strategy_id` FOREIGN KEY (`strategy_id`) REFERENCES `t_strategy` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=48 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_project`
--

LOCK TABLES `t_project` WRITE;
/*!40000 ALTER TABLE `t_project` DISABLE KEYS */;
INSERT INTO `t_project` VALUES (1,'TCL & MTK 5301E',0,'2011-11-11 16:00:00',30,'经过华数傅政确认该项目已经转成用原力的P2P服务',0,0,'2012-02-05 16:00:00',85,1),(2,'Hisence & MTK 5501',0,'2011-11-20 16:00:00',32,'华数项目，因没有MTK芯片厂商支持，项目暂时搁置',0,0,'2011-12-20 16:00:00',85,1),(3,'长虹 & Mstar 6i48',0,'2012-01-05 16:00:00',1,'华数项目，长虹 机器升级改造',0,0,'0000-00-00 00:00:00',85,1),(4,'LG & Mstar saturn 7',0,'2011-11-17 16:00:00',27,'华数项目',0,0,'0000-00-00 00:00:00',85,1),(5,'海尔 & Mstar 6i98x',0,'2011-12-20 16:00:00',25,'华数项目，chunked方式',0,0,'0000-00-00 00:00:00',85,1),(6,'松下 & LD4',0,'2012-01-28 16:00:00',1,'华数项目，多进程',0,0,'0000-00-00 00:00:00',87,1),(7,'松下 & Pro4',0,'2012-01-28 16:00:00',1,'华数项目，多进程',0,0,'0000-00-00 00:00:00',87,1),(8,'Hisence & Mstar 6A801',0,'2012-02-09 16:00:00',5,'华数项目',0,0,'0000-00-00 00:00:00',85,1),(9,'杰科 & Realtek 1185',0,'2012-03-12 16:00:00',41,'',0,0,'2012-02-29 16:00:00',85,1),(10,'迈乐 & 全智A10',0,'0000-00-00 00:00:00',5,'华数项目，还未采用SDK方式',0,0,'0000-00-00 00:00:00',85,1),(11,'视云 & Android TV',0,'2011-11-30 16:00:00',5,'4月份K91的ICS 8月份S系列 提供新的P2P版本',0,0,'2011-12-29 16:00:00',85,1),(12,'TCL & Win7',0,'2012-01-09 16:00:00',48,'flv格式播放',0,0,'2012-01-09 16:00:00',85,1),(13,'大连天维 & Win7',0,'2012-01-09 16:00:00',48,'flv播放，该项目商务没有反馈合作情况',0,0,'2012-01-09 16:00:00',85,1),(14,'碧维视 & Marvell',0,'2012-02-02 16:00:00',1,'暂停',0,0,'2012-02-02 16:00:00',1,1),(15,'新利实业 & Realtek 1186',0,'2012-01-29 16:00:00',1,'海外项目 暂停',0,0,'2012-01-31 16:00:00',1,1),(16,'联想 & Win7',0,'2012-02-19 16:00:00',48,'flv播放',0,0,'2012-02-19 16:00:00',85,1),(17,'精伦 & Trident',0,'2012-01-31 16:00:00',51,'',0,0,'2012-02-01 16:00:00',87,1),(18,'TCL&Hisi 3726(arm)',0,'0000-00-00 00:00:00',5,'目前还没有才发版本到TCL\r\n',0,0,'0000-00-00 00:00:00',1,1),(19,'TCL&Mstar99(mips)',0,'0000-00-00 00:00:00',5,'目前还没有才发版本到TCL\r\n',0,0,'0000-00-00 00:00:00',1,1),(20,'TCL&MTK(arm)',0,'0000-00-00 00:00:00',5,'目前还没有才发版本到TCL\r\n',0,0,'0000-00-00 00:00:00',1,1),(21,'TCL&Mstar X(arm)',0,'0000-00-00 00:00:00',5,'目前还没有才发版本到TCL\r\n',0,0,'0000-00-00 00:00:00',1,1),(22,'长虹&Hisi 3726(arm)',0,'0000-00-00 00:00:00',5,'\r\n',0,0,'0000-00-00 00:00:00',1,1),(23,'长虹&MTK 5502(arm)',0,'0000-00-00 00:00:00',5,'\r\n',0,0,'0000-00-00 00:00:00',1,1),(24,'长虹&MTK 5326(arm)',0,'0000-00-00 00:00:00',5,'\r\n',0,0,'0000-00-00 00:00:00',1,1),(25,'创维&Amlogic(arm)',0,'0000-00-00 00:00:00',5,'\r\n',0,0,'0000-00-00 00:00:00',1,1),(26,'创维&Realtek(mips)',0,'0000-00-00 00:00:00',20,'\r\n',0,0,'0000-00-00 00:00:00',1,1),(27,'创维&Mstar(mips)',0,'0000-00-00 00:00:00',20,'\r\n',0,0,'0000-00-00 00:00:00',1,1),(28,'海信&Mstar(mips)&K560(801)',0,'0000-00-00 00:00:00',20,'\r\n',0,0,'0000-00-00 00:00:00',1,1),(29,'海信&MTK(arm)',0,'0000-00-00 00:00:00',5,'\r\n',0,0,'0000-00-00 00:00:00',1,1),(30,'康佳&Mstar(mips)',0,'0000-00-00 00:00:00',20,'11号才发版本到万承龙\r\n',0,0,'0000-00-00 00:00:00',1,1),(31,'海尔&Andriod',0,'0000-00-00 00:00:00',5,'目前商务陆卫确认还没有测试\r\n',0,0,'0000-00-00 00:00:00',1,1),(32,'集成视讯&RK2918(arm)',0,'0000-00-00 00:00:00',5,'',0,0,'0000-00-00 00:00:00',85,1),(33,'迈乐&全智A10',0,'0000-00-00 00:00:00',1,'',0,0,'0000-00-00 00:00:00',85,1),(35,'松下华录',0,'0000-00-00 00:00:00',13,'',0,0,'2012-02-28 16:00:00',1,1),(36,'长虹&NXP',0,'0000-00-00 00:00:00',35,'华数项目',0,0,'0000-00-00 00:00:00',85,1),(39,'精伦 & Hisi v200',0,'2012-03-30 16:00:00',49,'',0,0,'2012-04-05 16:00:00',87,1),(40,'联想 & Android-86',0,'2012-03-31 16:00:00',50,'',0,0,'2012-04-29 16:00:00',84,1),(42,'PPTV&ios-armv6',0,'2012-04-05 16:00:00',11,'给公司iphone客户端提供m3u8点直播服务',0,0,'2012-04-29 16:00:00',83,1),(43,'PPTV&ios-armv7',0,'2012-04-05 16:00:00',12,'给公司iphone客户端提供m3u8点直播服务',0,0,'2012-04-29 16:00:00',83,1),(45,'PPTV&ios-armv7',0,'2012-04-05 16:00:00',12,'给公司ipad客户端提供m3u8点直播服务',0,0,'2012-04-29 16:00:00',82,1),(46,'PPTV&armandroid-r4',0,'2012-04-05 16:00:00',5,'给公司android phone/pad提供rtsp点直播服务',0,0,'2012-04-29 16:00:00',84,1),(47,'大连天维&Win7',1,'2012-04-05 16:00:00',48,'',0,0,'2012-04-05 16:00:00',85,1);
/*!40000 ALTER TABLE `t_project` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_project_company`
--

DROP TABLE IF EXISTS `t_project_company`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_project_company` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `project_id` int(10) NOT NULL DEFAULT '0',
  `company_id` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `t_project_id` (`project_id`),
  KEY `t_company_id` (`company_id`),
  CONSTRAINT `t_company_id` FOREIGN KEY (`company_id`) REFERENCES `t_company` (`id`),
  CONSTRAINT `t_project_id` FOREIGN KEY (`project_id`) REFERENCES `t_project` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_project_company`
--

LOCK TABLES `t_project_company` WRITE;
/*!40000 ALTER TABLE `t_project_company` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_project_company` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_project_engineer`
--

DROP TABLE IF EXISTS `t_project_engineer`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_project_engineer` (
  `project_id` int(10) NOT NULL DEFAULT '0',
  `engineer_id` int(10) NOT NULL DEFAULT '0',
  `role` varchar(255) NOT NULL DEFAULT '',
  KEY `pro_eng_project_id` (`project_id`),
  KEY `pro_eng_engineer_id` (`engineer_id`),
  CONSTRAINT `pro_eng_project_id` FOREIGN KEY (`project_id`) REFERENCES `t_project` (`id`),
  CONSTRAINT `pro_eng_engineer_id` FOREIGN KEY (`engineer_id`) REFERENCES `t_engineer` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_project_engineer`
--

LOCK TABLES `t_project_engineer` WRITE;
/*!40000 ALTER TABLE `t_project_engineer` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_project_engineer` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_protocol`
--

DROP TABLE IF EXISTS `t_protocol`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_protocol` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_protocol`
--

LOCK TABLES `t_protocol` WRITE;
/*!40000 ALTER TABLE `t_protocol` DISABLE KEYS */;
INSERT INTO `t_protocol` VALUES (1,'RTSP'),(2,'HTTP'),(3,'M3U8');
/*!40000 ALTER TABLE `t_protocol` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_roadmap`
--

DROP TABLE IF EXISTS `t_roadmap`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_roadmap` (
  `project_id` int(10) NOT NULL DEFAULT '0',
  `stage` int(10) NOT NULL DEFAULT '0',
  `content` varchar(255) NOT NULL DEFAULT '',
  `plan_timeline` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `actual_timeline` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `description` varchar(255) NOT NULL DEFAULT '',
  KEY `road_project_id` (`project_id`),
  CONSTRAINT `road_project_id` FOREIGN KEY (`project_id`) REFERENCES `t_project` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_roadmap`
--

LOCK TABLES `t_roadmap` WRITE;
/*!40000 ALTER TABLE `t_roadmap` DISABLE KEYS */;
/*!40000 ALTER TABLE `t_roadmap` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_role`
--

DROP TABLE IF EXISTS `t_role`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_role` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL DEFAULT '',
  `desc` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_role`
--

LOCK TABLES `t_role` WRITE;
/*!40000 ALTER TABLE `t_role` DISABLE KEYS */;
INSERT INTO `t_role` VALUES (1,'管理员','系统管理，拥有所有权限'),(2,'研发人员','拥有项目管理所有权限'),(3,'设备管理员','拥有设备管理的所有权限'),(4,'一般人员','拥有查看权限');
/*!40000 ALTER TABLE `t_role` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_staff_type`
--

DROP TABLE IF EXISTS `t_staff_type`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_staff_type` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_staff_type`
--

LOCK TABLES `t_staff_type` WRITE;
/*!40000 ALTER TABLE `t_staff_type` DISABLE KEYS */;
INSERT INTO `t_staff_type` VALUES (1,'研发'),(2,'测试'),(3,'产品'),(4,'市场'),(5,'商务');
/*!40000 ALTER TABLE `t_staff_type` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_strategy`
--

DROP TABLE IF EXISTS `t_strategy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_strategy` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `product_id` int(10) NOT NULL DEFAULT '0',
  `description` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=91 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_strategy`
--

LOCK TABLES `t_strategy` WRITE;
/*!40000 ALTER TABLE `t_strategy` DISABLE KEYS */;
INSERT INTO `t_strategy` VALUES (1,'no_live',1,'去除live模块'),(60,'libsalive',1,'alive打成静态库\r\n'),(61,'libslive',1,'live打成静态库\r\n'),(62,'libslive_worker',1,'live_worker打成静态库\r\n'),(63,'libspeer',1,'peer打成静态库\r\n'),(64,'libspeer_worker',1,'peer_worker打成静态库\r\n'),(65,'libsvod',1,'vod打成静态库\r\n'),(66,'libsvod_worker',1,'vod_worker打成静态库\r\n'),(67,'no_cert',1,'去除认证模块\r\n'),(68,'no_dac',1,'去除DAC模块\r\n'),(69,'no_demux_live2',1,'不支持2代直播\r\n'),(70,'no_demux_vod',1,'不支持点播\r\n'),(71,'no_mux',1,'去除mux封闭模块\r\n'),(72,'no_down',1,'去除download模块\r\n'),(73,'no_vod',1,'去除vod模块\r\n'),(74,'no_peer',1,'去除peer模块\r\n'),(75,'no_httpd',1,'去除http协议支持\r\n'),(76,'no_rtspd',1,'去除rtsp协议支持\r\n'),(77,'http_chunked',1,'http采用chunked方式传输\r\n'),(78,'ppbox_url',1,'box版本域名管理\r\n'),(79,'ipad_url',1,'ipad版本域名管理\r\n'),(80,'iphone_url',1,'iphone版本域名管理\r\n'),(81,'onelib',1,'单进程版本的整合\r\n'),(82,'ipad',1,'公司内部ipad发布策略 静态库\r\n'),(83,'iphone',1,'公司内部iphone发布策略 静态库\r\n'),(84,'android',1,'公司内部android发布策略\r\n'),(85,'httpd_mini',1,'给客户的http点播版本\r\n'),(86,'rtspd_mini',1,'给客户的rtsp点播版本\r\n'),(87,'api_mini',1,'给客户的api接口点播版本\r\n'),(88,'api',1,'api接口多进程版本'),(89,'rtspd',1,'rtsp协议多进程版本'),(90,'httpd',1,'http协议的多进程版本');
/*!40000 ALTER TABLE `t_strategy` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_target`
--

DROP TABLE IF EXISTS `t_target`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_target` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL DEFAULT '',
  `desc` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=37 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_target`
--

LOCK TABLES `t_target` WRITE;
/*!40000 ALTER TABLE `t_target` DISABLE KEYS */;
INSERT INTO `t_target` VALUES (1,'t_target','对象'),(2,'t_format','视频格式'),(3,'t_operation','操作'),(4,'t_role','角色'),(5,'t_grant','角色授权'),(6,'t_user','用户'),(7,'t_company','公司'),(8,'t_engineer','人员'),(9,'t_platform','平台'),(10,'t_problem','问题'),(11,'t_product','产品'),(12,'t_project','项目'),(13,'t_project_engineer','项目参与工程师'),(14,'t_roadmap','项目日程'),(15,'t_version','发布包'),(16,'t_strategy','策略'),(17,'t_combination_strategy','组合信息'),(18,'t_company_type','公司类型'),(19,'t_staff_type','人员类型'),(20,'t_accessory_type','附件类型'),(21,'t_equipment','设备'),(22,'t_protocol','协议'),(23,'t_test','测试'),(24,'t_equipment_accessory','附件'),(25,'t_application','设备申请'),(27,'t_project_company','项目参与公司'),(31,'t_problem_type','问题类型'),(33,'t_user_bug','用户故障'),(34,'t_equipment_type','设备类型'),(35,'t_joint_debug','联调问题'),(36,'t_branch','分支');
/*!40000 ALTER TABLE `t_target` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_test`
--

DROP TABLE IF EXISTS `t_test`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_test` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `equipment_id` int(10) NOT NULL DEFAULT '0',
  `version` varchar(255) NOT NULL DEFAULT '',
  `protocol_id` int(10) NOT NULL DEFAULT '0',
  `format_id` int(10) NOT NULL DEFAULT '0',
  `result` int(10) NOT NULL DEFAULT '0',
  `description` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `test_equipment_id` (`equipment_id`),
  KEY `test_protocol_id` (`protocol_id`),
  KEY `test_format_id` (`format_id`),
  CONSTRAINT `test_equipment_id` FOREIGN KEY (`equipment_id`) REFERENCES `t_equipment` (`id`),
  CONSTRAINT `test_format_id` FOREIGN KEY (`format_id`) REFERENCES `t_format` (`id`),
  CONSTRAINT `test_protocol_id` FOREIGN KEY (`protocol_id`) REFERENCES `t_protocol` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=72 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_test`
--

LOCK TABLES `t_test` WRITE;
/*!40000 ALTER TABLE `t_test` DISABLE KEYS */;
INSERT INTO `t_test` VALUES (1,6,'2.2',1,1,0,''),(2,6,'2.2',2,3,2,'\r\n'),(3,6,'2.2',2,2,2,'一直在loading'),(4,7,'2.2',1,1,0,'直播音视频不同步'),(5,7,'2.2',2,3,0,'之前不能播放，可以选择的播放器不一样'),(6,8,'2.3',1,1,0,'三星需要时间戳连续，之前视频慢是cts不是严格升序'),(7,8,'2.3',2,3,1,'视频有点慢的感觉'),(8,8,'2.3',2,2,2,''),(9,9,'3.1',1,1,2,'达尔文服务器能播，SDK服务器单音视频可以播，组合不能播'),(10,10,'2.2.1',1,1,0,'一代直播没有声音'),(11,10,'2.2.1',2,3,2,'连请求消息都没有，可能不支持http'),(12,11,'2.2.1',1,1,1,'有声音，没有图像'),(13,13,'4.0.3',1,1,0,'有时候没有声音'),(14,13,'4.0.4',2,3,0,''),(15,14,'',1,1,1,'只能播放Low Profile'),(16,15,'2.3(小米)',1,1,0,''),(17,15,'2.3(小米)',2,3,0,''),(18,15,'2.3(小米)',2,2,2,'一直在loading'),(19,16,'2.3',1,1,0,'三星需要时间戳连续，之前视频慢是cts不是严格升序'),(20,17,'2.3.3',1,1,0,''),(21,17,'2.3.4',2,3,0,''),(22,18,'2.3.5',1,1,0,'三星需要时间戳连续，之前视频慢是cts不是严格升序'),(23,19,'2.3.7(小米)',1,1,0,'之前没有刷小米，不能播放，刷了小米可以播放'),(24,20,'2.2.1',1,1,1,'有声音，无图像，Baseline可以播放 之前不能播放是SDP项目顺序问题'),(25,21,'2.1',1,1,1,'有声音，无视频，H264 Profile不支持'),(26,22,'2.2',1,1,0,''),(27,22,'2.2',2,3,2,''),(28,22,'2.2',2,2,2,''),(29,23,'2.3',1,1,0,'增加了SPS、PPS，就可以播放了'),(30,24,'2.2.1',1,1,1,'有声音，无图像，Baseline可以播放'),(31,25,'',1,1,1,'只能播放Baseline'),(32,26,'2.2',1,1,1,'有声音，无视频，H264 Profile不支持'),(33,27,'2.3(小米)',1,1,0,'小米之前不能播放，是因为没有发送RTCP Sender Report'),(34,28,'2.3.3',1,1,0,''),(35,28,'2.3.3',1,3,2,''),(36,29,'2.3.7(小米)',1,1,0,''),(37,30,'',1,1,0,'需要音频timestamp的scale大一些，直播之前是1000不能播放'),(38,31,'2.2',1,1,1,'只能播放Baseline'),(39,32,'2.2',1,1,1,'有声音，无图像，Baseline可以播放'),(40,33,'2.3.5',1,1,1,'有声音，无图像，Baseline可以播放 之前不能播放是SDP项目顺序问题'),(41,34,'',1,1,1,'有绿屏、花屏'),(42,35,'2.2.2',1,1,1,'点播不能播放（有声音），一代直播可以播，二代直播不能播'),(43,37,'2.2',1,1,0,''),(44,37,'2.2',1,3,2,''),(45,38,'2.3.4(小米)',1,1,0,''),(46,39,'2.3(小米)',1,1,0,''),(47,2,'2.2',1,1,0,'\r\n'),(48,2,'2.2',2,3,0,'\r\n'),(49,2,'2.2',2,2,2,'好像有几帧图像显示'),(50,40,'2.2',1,1,1,'有图像，无声音'),(51,40,'2.2',2,3,0,'\r\n'),(52,40,'2.2',2,2,0,'\r\n'),(53,5,'',1,1,2,'据Mstar说：有RTSP协议，但是不支持H.264,AAC'),(54,5,'',1,3,2,'\r\n'),(55,42,'',1,1,0,'\r\n'),(56,41,'2.2',1,1,0,'拖动有问题，拖动后不继续播放'),(57,41,'2.2',2,3,1,'需要Content-Length'),(58,43,'2.2',1,1,2,'无声音图像，客户说只支持MS RTSP'),(59,44,'',1,1,0,'\r\n'),(60,44,'',1,3,2,'\r\n'),(61,44,'',2,3,0,'不能拖动'),(62,3,'',1,1,1,'不能拖动'),(64,6,'2.2',3,3,2,''),(65,8,'2.3',3,3,2,''),(66,15,'2.3小米',3,3,2,''),(67,22,'2.2',3,3,2,''),(68,2,'2.2',3,3,0,''),(69,40,'2.2',3,3,2,''),(70,41,'2.2',3,3,0,'需要Chunked-Encoding'),(71,3,'',2,3,1,'不能拖动');
/*!40000 ALTER TABLE `t_test` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_user`
--

DROP TABLE IF EXISTS `t_user`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_user` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `role` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(45) NOT NULL DEFAULT '',
  `pass` varchar(32) NOT NULL DEFAULT '' COMMENT 'MD5',
  `t_add` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `t_online` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`),
  UNIQUE KEY `Index_name` (`name`),
  KEY `FK_t_user_role` (`role`),
  CONSTRAINT `FK_t_user_role` FOREIGN KEY (`role`) REFERENCES `t_role` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_user`
--

LOCK TABLES `t_user` WRITE;
/*!40000 ALTER TABLE `t_user` DISABLE KEYS */;
INSERT INTO `t_user` VALUES (1,1,'root','d41d8cd98f00b204e9800998ecf8427e','2009-03-31 16:00:00','2012-04-06 08:01:37'),(2,1,'guochunmao','e10adc3949ba59abbe56e057f20f883e','2012-03-26 13:17:32','0000-00-00 00:00:00'),(3,1,'fangliang','e10adc3949ba59abbe56e057f20f883e','2012-03-26 13:17:45','2012-04-06 06:00:31'),(4,2,'xiangdongtao','e10adc3949ba59abbe56e057f20f883e','2012-03-26 13:17:59','0000-00-00 00:00:00'),(5,2,'zhangkuangzheng','e10adc3949ba59abbe56e057f20f883e','2012-03-26 13:18:13','2012-04-06 05:37:17'),(6,2,'chenxiangli','e10adc3949ba59abbe56e057f20f883e','2012-03-26 13:18:24','2012-04-06 05:24:46'),(7,2,'hekunmao','e10adc3949ba59abbe56e057f20f883e','2012-03-26 13:18:33','2012-04-01 11:16:02'),(8,4,'guest','d41d8cd98f00b204e9800998ecf8427e','2012-03-26 13:18:42','2012-03-27 02:00:12');
/*!40000 ALTER TABLE `t_user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_user_bug`
--

DROP TABLE IF EXISTS `t_user_bug`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_user_bug` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `track_id` int(10) NOT NULL DEFAULT '0',
  `engineer_id` int(10) NOT NULL DEFAULT '0',
  `name` varchar(45) NOT NULL DEFAULT '',
  `telephone` varchar(45) NOT NULL DEFAULT '',
  `address` varchar(100) NOT NULL DEFAULT '',
  `buy_date` varchar(45) NOT NULL DEFAULT '',
  `version` varchar(45) NOT NULL DEFAULT '',
  `serial_number` varchar(100) NOT NULL DEFAULT '',
  `net_service` varchar(45) NOT NULL DEFAULT '',
  `H3_ways` varchar(100) NOT NULL DEFAULT '',
  `scope` varchar(255) NOT NULL DEFAULT '',
  `frequency` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `t_track_id` (`track_id`)
) ENGINE=InnoDB AUTO_INCREMENT=24 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_user_bug`
--

LOCK TABLES `t_user_bug` WRITE;
/*!40000 ALTER TABLE `t_user_bug` DISABLE KEYS */;
INSERT INTO `t_user_bug` VALUES (2,0,1,'王先生','13387870818','大连','','','','铁通4M','','只能看新闻，其他的电影和电视剧都是在缓冲到80%就停止了，不能播放','用户在早上和深夜使用仍然没有得到有效的改善'),(3,0,78,'林先生','13596215549 ','上海','201112 ','V04.10 ','','电信20M','光猫-路由器-H3（有线）','观看“在线影视”里面的片子时，刚开始播放是正常的，播放 一段时间后就出会卡（卡死，不动）','近2、3天出现此问题'),(4,0,82,'','','大连','','','04:20:9a:05:31:af ','铁通','',' jump服务器获取不到数据',''),(5,0,81,'王女士','15541117371 ','大连','','','','','','松下电视用户播放卡',''),(6,0,82,'华数用户','','北京','','','','','','http://118.186.142.71:8888/0/%a1%be%bb%aa%ca%fd%a1%bf201202071627572793_340719.mp4?key=4f758c89209ae5ea858f7f6a964d29a4 不能下载数据，http://123.126.34.220:8888/0/%a1%be%bb%aa%ca%fd%a1%bf2012010515460710709_327260.mp4?key=e7d37c5e71c7dc71e32db5f5b9d93caf ，能下载数据',''),(8,0,81,'陶先生','041162942595','大连','','','','铁通4M','','点击影片缓冲一直为零',''),(9,0,81,'吕女士','13332129811','辽宁抚顺','','','','铁通4M','','用户表示之前能正常播放，近期进入华数时，观看里边所有影视剧都是缓冲为零。',''),(10,0,81,'松下用户张女士','','上海','','','','','','点播部分电视剧中的个别集数不能观看，悬崖 的 第一集 和 第十集 ，不能看',''),(11,0,80,'松下用户林先生','','贵州','','','','','','《机械师》与《狂暴飞车》播放故障',''),(12,0,81,'江先生','18650383185','福州','','','04209A07A601','电信2M','','音乐里的MV无法播放，一点击就显示本机ID',''),(13,0,80,'杨先生','18987867369','昆明','','','04209A05DC67','电信4M','',' 观看所用影片时都很卡，看一会儿缓冲一会儿，高清的就更卡了，其他的如新闻等可以正常观看，网络环境正常',''),(14,0,78,'文先生','15508173322 ','南充','20121208 ','V04.09','','网通2M','','华数打开时提示数据超时，（互联网影视在线）点击影片缓冲后提示数据超时','从本月26日中午开始出现此状况，27日晚6点到9点都是这个状况，9点半后恢复正常。在这期间用电脑观看正常，26日之前是正常的。 '),(15,0,78,'毕滔伦','15810004947   ','北京','','','','联通2M ADSL,','',',猎魔教士缓冲到50%，开始没声音，重启后缓冲。 其他片子正常。',''),(16,0,48,'','','浙江','','','','电信','','视频数据获取不到',''),(17,0,48,'王先生','13901024377 ','北京','','','','联通','','大部分影片都不能缓冲完成，基本都是在80%左右停止。',''),(18,0,79,'','','武汉','','','','电信','','在线影片比较卡，片名《伤城》',''),(19,0,82,'','','四川','','','','','','ping 不通 dt.api.pplive.com、dc.api.pplive.com、c.api.pplive.com 这三个域名',''),(22,0,2,'飞','','','','','','','','','');
/*!40000 ALTER TABLE `t_user_bug` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `t_version`
--

DROP TABLE IF EXISTS `t_version`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_version` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `description` varchar(255) NOT NULL DEFAULT '',
  `project_id` int(10) NOT NULL DEFAULT '0',
  `branch_id` int(10) NOT NULL DEFAULT '0',
  `engineer_id` int(10) unsigned NOT NULL DEFAULT '0',
  `publish_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`),
  KEY `ver_name` (`name`),
  KEY `ver_project_id` (`project_id`),
  KEY `ver_engineer_id` (`engineer_id`),
  CONSTRAINT `ver_engineer_id` FOREIGN KEY (`engineer_id`) REFERENCES `t_user` (`id`),
  CONSTRAINT `ver_project_id` FOREIGN KEY (`project_id`) REFERENCES `t_project` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `t_version`
--

LOCK TABLES `t_version` WRITE;
/*!40000 ALTER TABLE `t_version` DISABLE KEYS */;
INSERT INTO `t_version` VALUES (1,'ppbox-android-x86-android-release-1.0.0.3163.tar.gz','联想pad(intel芯片)上发布pptv客户端',40,2,5,'2012-04-03 16:00:00'),(2,'P2P_TCL_MTK5301E_0.5.3_20111125','',1,5,3,'2011-11-24 16:00:00'),(3,'P2P_Hisence_MTK5501_0.5.3_20111206','',2,5,3,'2011-12-05 16:00:00'),(4,'P2P_Changhong_Mstar6i48_0.5.3_20120109','',3,5,3,'2012-01-08 16:00:00'),(5,'P2P_Changhong_NXP_1.0.0_20120319','',36,2,3,'2012-03-18 16:00:00'),(6,'P2P_LG_MstarSaturn7_0.5.3_20111209','',4,5,3,'2011-12-08 16:00:00'),(7,'P2P_Haier_Mstar6i98x_0.5.3_20120110','',5,5,3,'2012-01-09 16:00:00'),(8,'P2P_Panasonic_LD4_0.5.3_20120131','',6,5,3,'2012-01-30 16:00:00'),(9,'P2P_Panasonic_PRO4_0.5.3_20120131','',7,5,3,'2012-01-30 16:00:00'),(11,'P2P_Hisence_Mstar6a80x_0.5.3_20120215','',8,5,3,'2012-02-14 16:00:00'),(12,'P2P_Giec_Realtek1185_1.0.0_20120313','',9,2,3,'2012-03-12 16:00:00'),(13,'P2P_Haier_Mstar6i98x_hard_hi_0.5.3_20120110.zip','',5,5,3,'2012-01-09 16:00:00'),(14,'ppbox-win32-i586-httpd_mini-release-0.5.3.2238.tar.gz','',16,5,3,'2012-04-05 16:00:00'),(15,'ppbox-win32-i586-httpd_mini-release-0.5.3.2238.tar.gz','',12,5,3,'2012-04-05 16:00:00'),(16,'ppbox-win32-i586-httpd_mini-release-0.5.3.2238.tar.gz','',47,5,3,'2012-04-05 16:00:00'),(17,'ppbox-marvell-88DE3010-biweisi_hi-0.5.3.2400.zip','',14,5,3,'2012-04-05 16:00:00'),(18,'ppbox-realtek-1185-hard_hi-0.5.3.2393.zip','',15,5,3,'2012-04-05 16:00:00'),(19,'P2P_JingLun_TridentPnx8473_0.5.3_20120222','',17,5,3,'2012-04-05 16:00:00');
/*!40000 ALTER TABLE `t_version` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2012-04-06 16:08:01
