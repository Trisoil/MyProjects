USE	 `ppbox`;

DROP TABLE IF EXISTS `t_domain_stratyge`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `t_domain_stratyge` (
	`id` int(10) NOT NULL AUTO_INCREMENT,
	`name` varchar(255) NOT NULL DEFAULT '',
	`product_id` int(10) NOT NULL DEFAULT '0',
	`description` varchar(255) NOT NULL DEFAULT '',
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=gbk;
SET character_set_client = @saved_cs_client;

