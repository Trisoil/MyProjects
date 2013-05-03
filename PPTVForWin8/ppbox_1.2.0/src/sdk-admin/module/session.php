<?php

session_start();

require_once("config.php");
require_once('dblink.php');
require_once('convert.php');

function login($name, $pass)
{
    global $db_host;
    global $db_user;
    global $db_pass;
    global $db_name;
    
    $dblink = new dblink;
    $result = $dblink->connect($db_host, $db_user, $db_pass, $db_name);
    if (is_bool($result) && $result == FALSE) {
        return array(success => false, message => 'Could not connect to database');
    }
    $result = $dblink->query("SELECT * FROM t_target;");
    if (is_bool($result) && $result == FALSE) {
        return array(success => false, message => $dblink->error());
    }
    $targets = array();
    while ($obj = $dblink->fetch_object($result)) {
        $targets[$obj->id] = $obj->name;
    }
    $_SESSION['targets'] = $targets;
    $result = $dblink->query("SELECT * FROM t_user WHERE name = '{$name}';");
    if (is_bool($result) && $result == FALSE) {
        return array(success => false, message => $dblink->error());
    }
    if ($dblink->num_rows($result) == 0) {
        return array(success => false, message => '用户不存在');
    }
    $user = $dblink->fetch_object($result);
    $_SESSION['uid'] = $user->id;
    $_SESSION['user'] = $user;
    $dblink->free_result($result);
    if ($user->pass != md5($pass)) {
        return array(success => false, message => '密码错误');
    }
    $result = $dblink->query("SELECT target, operation FROM t_grant WHERE role = '{$user->role}'");
    if (is_bool($result) && $result == FALSE) {
        return array(success => false, message => $dblink->error());
    }
    $operations = array(0, 8, 12, 11, 15);
    $grants = array();
    while ($obj = $dblink->fetch_object($result)) {
        $grants[$targets[$obj->target]] = $operations[$obj->operation];
    }
    $_SESSION['grants'] = $grants;
    //var_dump($_SESSION['grants']);
        
    $time = date('Y-m-d H:i:s');
    $dblink->query("UPDATE t_user SET t_online = '{$time}' WHERE id = '{$user->id}'");
    return array(success => true, result => $grants);
}

function logout()
{
    if (!isset($_SESSION['uid'])) {
        return array(success => false, message => '没有登录');
    }
    unset($_SESSION['uid']);
    unset($_SESSION['grants']);
    return array(success => true);
}

function modify_pass($old, $new)
{
    if (!isset($_SESSION['uid'])) {
        return array(success => false, message => '没有登录');
    }
    
    global $db_host;
    global $db_user;
    global $db_pass;
    global $db_name;
    
    $dblink = new dblink;
    $dblink->connect($db_host, $db_user, $db_pass, $db_name);
    
    $old_md5 = md5($old);
    $new_md5 = md5($new);
    $result = $dblink->query("UPDATE t_user SET pass = '{$new_md5}' WHERE id = '{$id}' AND pass = '{$old_md5}'");
    if (is_bool($result) && $result == FALSE) {
        return array(success => false, message => $dblink->error());
    }
    if ($dblink->affected_rows() == 0) {
        return array(success => false, message => '原密码错误');
    }
    return array(success => true);
}

function check_grant_for($target, $operation)
{
    //var_dump($_SESSION['grants']);
    $operations = array(
        'add'   => 1,  
        'del'   => 2,  
        'mod'   => 4,  
        'get'   => 8, 
    );
    $targets = $_SEESION['targets'];
    if (!isset($_SESSION['uid'])) {
        return array(success => false, message => '没有登录');
    }
    if (!isset($_SESSION['grants'][$target])) {
        return array(success => false, message => '未授权限');
    }
    if (($operations[$operation] & $_SESSION['grants'][$target]) != $operations[$operation]) {
        return array(success => false, message => '未授权限2');
    }
    return array(success => true);
}

function make_out($result)
{
    echo my_json_encode($result);
}

if ($_REQUEST['o'] == 'login') {
    $result = login($_REQUEST['name'], $_REQUEST['pass']);
    make_out($result);
    return;
}
if ($_REQUEST['o'] == 'logout') {
    $result = logout();
    make_out($result);
    return;
}
if ($_REQUEST['o'] == 'modify_pass') {
    $result = modify_pass($_REQUEST['pass'], $_REQUEST['pass_new']);
    make_out($result);
    return;
}

?>
