# Sequel Pro dump
# Version 2492
# http://code.google.com/p/sequel-pro
#
# Host: 127.0.0.1 (MySQL 5.1.44)
# Database: PPVALogSys
# Generation Time: 2011-07-04 17:08:25 +0800
# ************************************************************

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

/*CREATE DATABASE `PPVALogSys` DEFAULT CHARACTER SET `utf8` */;

# Dump of table sys_module
# ------------------------------------------------------------

DROP TABLE IF EXISTS `sys_module`;

CREATE TABLE `sys_module` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `ModuleID` varchar(10) NOT NULL,
  `ModuleName` varchar(20) NOT NULL,
  `ParentID` varchar(10) NOT NULL,
  `Link` varchar(255) DEFAULT NULL,
  `LinkClass` varchar(20) DEFAULT NULL,
  `Status` int(2) NOT NULL DEFAULT '0',
  `Memo` varchar(50) DEFAULT NULL,
  `CreateTime` datetime DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=214 DEFAULT CHARSET=utf8;

LOCK TABLES `sys_module` WRITE;
/*!40000 ALTER TABLE `sys_module` DISABLE KEYS */;
INSERT INTO `sys_module` (`ID`,`ModuleID`,`ModuleName`,`ParentID`,`Link`,`LinkClass`,`Status`,`Memo`,`CreateTime`)
VALUES
	(17,'X','系统设置','0','','link_menutitle',0,'','2010-03-10 15:45:43'),
	(28,'X-W','系统角色设置','X','sys_role.jsp','',1,'','2010-07-02 16:38:22'),
	(29,'X-V','系统用户设置','X','sys_user.jsp','',0,'','2010-07-02 16:37:14'),
	(30,'X-X','系统模块管理','X','sys_module.jsp','',1,'','2010-07-02 15:30:09'),
	(192,'A','日志管理','0','','link_menutitle',0,'','2011-04-13 19:22:29'),
	(213,'X-L','系统日志查看','X','sys_log.jsp','',0,'','2010-07-02 15:28:58'),
	(200,'A-A','查看日志列表','A','loglist.jsp','',0,'','2011-04-14 11:31:52');

/*!40000 ALTER TABLE `sys_module` ENABLE KEYS */;
UNLOCK TABLES;


# Dump of table sys_role
# ------------------------------------------------------------

DROP TABLE IF EXISTS `sys_role`;

CREATE TABLE `sys_role` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `RoleName` varchar(20) NOT NULL,
  `Permission` text,
  `CreateTime` datetime DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

LOCK TABLES `sys_role` WRITE;
/*!40000 ALTER TABLE `sys_role` DISABLE KEYS */;
INSERT INTO `sys_role` (`ID`,`RoleName`,`Permission`,`CreateTime`)
VALUES
	(1,'超级管理员','A-A,X-L,X-V,X-W,X-X','2006-06-21 14:35:32'),
	(6,'后台管理员','A-A,X-W','2008-01-16 16:56:58'),
	(7,'后台用户','A-A,X-L,X-V,X-W,X-X','2008-02-27 17:06:18');

/*!40000 ALTER TABLE `sys_role` ENABLE KEYS */;
UNLOCK TABLES;


# Dump of table sys_user
# ------------------------------------------------------------

DROP TABLE IF EXISTS `sys_user`;

CREATE TABLE `sys_user` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `UserName` varchar(30) NOT NULL,
  `Password` varchar(255) DEFAULT NULL,
  `RoleID` int(11) NOT NULL DEFAULT '0',
  `Permission` text,
  `Memo` varchar(255) DEFAULT NULL,
  `CreateTime` datetime DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=18 DEFAULT CHARSET=utf8;

LOCK TABLES `sys_user` WRITE;
/*!40000 ALTER TABLE `sys_user` DISABLE KEYS */;
INSERT INTO `sys_user` (`ID`,`UserName`,`Password`,`RoleID`,`Permission`,`Memo`,`CreateTime`)
VALUES
	(1,'admin','01EgJhCVRAMlBiZu1URIPA==',1,'A-A,X-L,X-V,X-W,X-X',NULL,'2006-12-06 13:01:02'),
	(12,'zxw','tzwSvStnTlg=',6,'A-A,X-W',NULL,'2010-04-17 04:42:39'),
	(16,'service','U+puZLSnf/A=',7,'A-A,X-L,X-V',NULL,'2011-04-29 17:32:57'),
	(17,'zhang','U+puZLSnf/A=',1,'A-A,X-L,X-V,X-W,X-X',NULL,'2011-06-22 14:32:29');

/*!40000 ALTER TABLE `sys_user` ENABLE KEYS */;
UNLOCK TABLES;





/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
