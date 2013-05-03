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
) ENGINE=InnoDB AUTO_INCREMENT=52 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
) ENGINE=InnoDB AUTO_INCREMENT=40 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
) ENGINE=InnoDB AUTO_INCREMENT=74 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
) ENGINE=InnoDB AUTO_INCREMENT=49 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
  `solution` varchar(255) NOT NULL DEFAULT '',
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
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
-- Table structure for table `t_project`
--

DROP TABLE IF EXISTS `t_project`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_project` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL DEFAULT '',
  `begin_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `platform_id` int(10) NOT NULL DEFAULT '0',
  `description` varchar(255) NOT NULL DEFAULT '',
  `progress` int(10) NOT NULL DEFAULT '0',
  `complete_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `strategy_id` int(10) NOT NULL DEFAULT '0',
  `product_id` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `pro_name` (`name`),
  KEY `pro_platform_id` (`platform_id`),
  KEY `pro_strategy_id` (`strategy_id`),
  KEY `pro_product_id` (`product_id`),
  CONSTRAINT `pro_platform_id` FOREIGN KEY (`platform_id`) REFERENCES `t_platform` (`id`),
  CONSTRAINT `pro_strategy_id` FOREIGN KEY (`strategy_id`) REFERENCES `t_strategy` (`id`),
  CONSTRAINT `pro_product_id` FOREIGN KEY (`product_id`) REFERENCES `t_product` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=36 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
) ENGINE=InnoDB AUTO_INCREMENT=88 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
) ENGINE=InnoDB AUTO_INCREMENT=35 DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
-- Table structure for table `t_user_bug`
--

DROP TABLE IF EXISTS `t_user_bug`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_user_bug` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `track_id` int(10) NOT NULL DEFAULT '0',
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
  KEY `t_track_id` (`track_id`),
  CONSTRAINT `t_track_id` FOREIGN KEY (`track_id`) REFERENCES `t_problem` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;

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
  `engineer_id` int(10) unsigned NOT NULL DEFAULT '0',
  `publish_time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`id`),
  KEY `ver_name` (`name`),
  KEY `ver_project_id` (`project_id`),
  KEY `ver_engineer_id` (`engineer_id`),
  CONSTRAINT `ver_project_id` FOREIGN KEY (`project_id`) REFERENCES `t_project` (`id`),
  CONSTRAINT `ver_engineer_id` FOREIGN KEY (`engineer_id`) REFERENCES `t_user` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2012-04-01 16:28:34
