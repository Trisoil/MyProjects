USE  `ppbox`;

DROP TABLE IF EXISTS `t_internal_engineer`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `t_internal_engineer` (
	`id` int(10) NOT NULL AUTO_INCREMENT,
	`name` varchar(255) NOT NULL DEFAULT '',
	`company_id` int(10) NOT NULL DEFAULT '0',
	`type_id` int(10) NOT NULL DEFAULT '0',
	`telephone` varchar(40) NOT NULL DEFAULT '',
	`email` varchar(255) NOT NULL DEFAULT '',
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
SET character_set_client = @saved_cs_client;
