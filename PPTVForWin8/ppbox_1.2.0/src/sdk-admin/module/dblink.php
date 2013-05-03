<?php

class dblink
{
    function __construct()
    {
    }
    
    function connect($host, $user, $pass, $dbname, $port = 3306)
    {
        $this->link_ = mysql_connect($host, $user, $pass, $port);
        if (!$this->link_) {
            return FALSE;
        }
        mysql_query("SET NAMES 'gbk'");
        mysql_query("SET TIME_ZONE = '+8:00'");
        return mysql_select_db($dbname, $this->link_);
    }
    
    function close()
    {
        return mysql_close($this->link_);
    }
    
    function query($sql)
    {
        return mysql_query($sql, $this->link_);
    }
    
    function num_rows($result)
    {
        return mysql_num_rows($result);
    }
    
    function affected_rows()
    {
        return mysql_affected_rows();
    }
    
    function fetch_object($result_set)
    {
        return mysql_fetch_object($result_set);
    }
    
    function free_result($result)
    {
        return mysql_free_result($result);
    }
    
    function error()
    {
        return mysql_error($this->link_);
    }
    
    private $link_;
};

?>
