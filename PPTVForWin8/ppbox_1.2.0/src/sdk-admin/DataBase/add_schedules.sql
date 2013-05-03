USE	 `ppbox`;

DROP TABLE IF EXISTS `t_schedules`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `t_schedules` (
	`id` int(10) NOT NULL AUTO_INCREMENT,
	`name` varchar(50) NOT NULL DEFAULT '',
	`time` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
	`content` varchar(255) NOT NULL DEFAULT '',
	`workload` int(10) NOT NULL DEFAULT '0',
	`quality` int(10) NOT NULL DEFAULT '0',
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
SET character_set_client = @saved_cs_client;

