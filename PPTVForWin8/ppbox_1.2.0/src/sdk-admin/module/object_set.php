<?php

require_once("config.php");
require_once('dblink.php');
require_once('table.php');
require_once('convert.php');
require_once("session.php");

class object_set
{
    function get_items($param)
    {
        $this->open_db_table();
        $count = 0;
        $result = $this->table_->select('COUNT(*) as count', $param);
        if (is_bool($result) && $result == FALSE) {
            return array(success => false, message => $this->table_->error());
        } else {
            $count = $result[0]->count;
        }
        $result = $this->table_->select(NULL, $param);
        if (is_bool($result) && $result == FALSE) {
            return array(success => false, message => $this->table_->error());
        } else {
            return array(success => true, count => $count, data => $result);
        }
    }
    
    function count_items($filter)
    {
        $this->open_db_table();
        $result = $this->table_->select('COUNT(*) as count', $filter);
        if (is_bool($result) && $result == FALSE) {
            return array(success => false, message => $this->table_->error());
        } else {
            return array(success => true, count => $result[0]->count);
        }
    }
    
    function add_item($item)
    {
        $this->open_db_table();
        $result = $this->table_->insert($item, $this->auto_id());
        if ($result == FALSE) {
            return array(success => false, message => $this->table_->error());
        } else {
            return array(success => true, data => $result);
        }
    }
    
    function mod_item($item, $filter = NULL)
    {
        $this->open_db_table();
        if ($filter == NULL)
            $filter = array(id => $item->id);
        $result = $this->table_->update($item, $filter);
        if ($result == FALSE) {
            return array(success => false, message => $this->table_->error());
        } else {
            return array(success => true);
        }
    }
    
    function del_item($id)
    {
        $this->open_db_table();
        $result = $this->table_->delete($id);
        if ($result == FALSE) {
            return array(success => false, message => $this->table_->error());
        } else {
            return array(success => true);
        }
    }
    
    function query($sql)
    {
        $this->open_db_table();
        $result = $this->table_->query($sql);
        if ($result == FALSE) {
            return array(success => false, message => $this->table_->error());
        } else {
            return array(success => true, data => $result);
        }
    }

    function error()
    {
        return $this->table_->error();
    }
    
    static function make_filter($list)
    {
    	  //var_dump($list);
        $filter = array();
        foreach ($list as $f) {
        foreach ($f->data as $d) {
            switch ($d->type) {
                case 'string' :
                    $filter[] = $f->field . " LIKE '%" . $d->value . "%'";
                    break;
                case 'list' :
                    $v = $d->value;
                    if (is_string($v)) {
                        $v = explode(',', $v);
                    }
                    for ($q = 0; $q < count($v); $q++){
                        $v[$q] = "'" . $v[$q] . "'";
                    }
                    if (count($v) == 0) {
                        $filter[] = "0"; 
                    } else if (count($v) == 1) {
                        $filter[] = $f->field . " = " . $v[0]; 
                    } else {
                        $filter[] = $f->field . " IN (" . implode(',', $v) . ")"; 
                    }
                    break;
                case 'boolean' :
                    $filter[] = $f->field . " = '" . $d->value . "'"; 
                    break;
                case 'numeric' :
                    switch ($d->comparison) {
                        case 'eq' :
                            $filter[] = $f->field . " = '" . $d->value . "'"; 
                            break;
                        case 'lt' :
                            $filter[] = $f->field . " < '" . $d->value . "'"; 
                            break;
                        case 'gt' :
                            $filter[] = $f->field . " > '" . $d->value . "'"; 
                            break;
                    }
                    break;
                case 'date' :
                    $timestamp = strtotime($d->value);
                    $time_str = date('Y-m-d H:i:s', $timestamp);
                    $time_str1 = date('Y-m-d H:i:s', $timestamp + 3600 * 24);
                    switch ($d->comparison) {
                        case 'eq' :
                            $filter[] = $f->field . " >= '" . $time_str . "'"; 
                            $filter[] = $f->field . " < '" . $time_str1 . "'"; 
                            break;
                        case 'lt' :
                            $filter[] = $f->field . " < '" . $time_str . "'"; 
                            break;
                        case 'gt' :
                            $filter[] = $f->field . " > '" . $time_str1 . "'"; 
                            break;
                    }
                    break;
            }
        }
        }
        return $filter;
    }
    
    function process_request($req)
    {
        if ($req['o'] == 'get') {
            $param = new stdClass;
            if (isset($req['start'])) {
                $param->start = $req['start'];
            }
            if (isset($req['limit'])) {
                $param->limit = $req['limit'];
            }
            if (isset($req['sort'])) {
                $param->sort= $req['sort'];
            }
            if (isset($req['dir'])) {
                $param->dir = $req['dir'];
            }
            if (isset($req['filter'])) {
                $filter = my_json_decode($req['filter']);
                $param->filter = object_set::make_filter($filter);
            }
            $result = $this->get_items($param);
        } else if ($req['o'] == 'count') {
            $filter = NULL;
            if (isset($req['filter'])) {
                $filter = my_json_decode($req['filter']);
            }
            $result = $this->count_items($filter);
        } else if ($req['o'] == 'add') {
            $item = my_json_decode($req['item']);
            $result = $this->add_item($item);
            if (isset($result['data'])) {
                // JosnStore需要一个记录集，即数组
                //$result['data'] = array($result['data']);
            }
        } else if ($req['o'] == 'mod') {
            $item = my_json_decode($req['item']);
            $filter = NULL;
            if (isset($req['filter'])) {
                $filter = my_json_decode($req['filter']);
            }
            $result = $this->mod_item($item, $filter	);
            if (isset($result['data'])) {
                // JosnStore需要一个记录集，即数组
                //$result['data'] = array($result['data']);
            }
        } else if ($req['o'] == 'del') {
            if (isset($req['id'])) {
                $id = my_json_decode($req['id']);
                $result = $this->del_item($id);
            } else if (isset($req['ids'])) {
                $ids = my_json_decode($req['ids']);
                foreach ($ids as $id) {
                    $result[] = $this->del_item($id);
                }
                $result = array(success => true, data => $result);
            }
        } else {
            $result = array(success => false, message => '未知请求类型');
        }
        return $result;
    }
        
    function process()
    {
        $result = check_grant_for($this->table_name(), $_REQUEST['o']);
        if ($result['success']) {
            $result = $this->process_request($_REQUEST);
        }
        //if (!isset($result['data'])) {
        //    $result['data'] = array();
        //}
        make_out($result);
    }
    
    function table_name()
    {
        return $_REQUEST['t'];
    }
    
    function auto_id()
    {
        return $_REQUEST['a'];
    }
    
    private function open_db_table()
    {
        global $db_host;
        global $db_user;
        global $db_pass;
        global $db_name;
        
        if (!$table_) {
            $dblink = new dblink;
            $dblink->connect($db_host, $db_user, $db_pass, $db_name);
            $this->table_ = new table($dblink, $this->table_name());
        }
    }
    
    private $table_;
};

?>
