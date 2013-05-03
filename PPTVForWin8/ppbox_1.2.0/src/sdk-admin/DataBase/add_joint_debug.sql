USE	 `ppbox`;

DROP TABLE IF EXISTS `t_joint_debug`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `t_joint_debug` (
	`id` int(10) NOT NULL AUTO_INCREMENT,
	`engineer_id` int(10) NOT NULL DEFAULT '0',
	`user_id` int(10) unsigned NOT NULL DEFAULT '0',
	`description` varchar(255) NOT NULL DEFAULT '',
	PRIMARY KEY (`id`),
	CONSTRAINT `t_engineer_id` FOREIGN KEY (`engineer_id`) REFERENCES `t_engineer` (`id`),
	CONSTRAINT `t_user_id` FOREIGN KEY (`user_id`) REFERENCES `t_user` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
SET character_set_client = @saved_cs_client;

