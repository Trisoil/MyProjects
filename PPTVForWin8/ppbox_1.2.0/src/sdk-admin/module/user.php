<?php

require_once('object_set.php');

class user extends object_set
{
    function add_item($item)
    {
        $item->pass = md5($item->pass);
        $item->t_add = date('Y-m-d H:i:s');
        $item->t_online = '0000-00-00 00:00:00';
        $result = parent::add_item($item);
        if ($result['success']) {
            $result['data'] = array(pass => $item->pass, t_add => $item->t_add, t_online => $item->t_online);
        }
        return $result;
    }
    
    function mod_item($item)
    {
        $item->pass = md5($item->pass);
        $result = parent::mod_item($item);
        if ($result['success']) {
            $result['data'] = array(pass => $item->pass);
        }
        return $result;
    }
        
    function table_name()
    {
        return "t_user";
    }

    function auto_id()
    {
        return true;
    }    
};

$user = new user;
$user->process();

?>
