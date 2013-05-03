<?php

function iconvert($from, $to, $target)
{
    if (is_array($target)) {
        foreach ($target as $key => $value) {
            $target[$key] = iconvert($from, $to, $target[$key]);
        }
        return $target;
    } else if (is_object($target)) {
        foreach ($target as $key => $value) {
            $target->$key = iconvert($from, $to, $target->$key);
        }
        return $target;
    } else if (is_string($target)) {
        //return iconv($from, $to, $target);
        return mb_convert_encoding($target, $to, $from);
    } else {
        return $target;
    }
}

function iconvert_out($target)
{
    return iconvert('gbk', 'utf8', $target);
}

function iconvert_in($target)
{
    return iconvert('utf8', 'gbk', $target);
}

function my_json_encode($data)
{
    //var_dump($data);
    $data = iconvert_out($data);
    return json_encode($data);
}

function my_json_decode($data)
{
    //var_dump($data);
    $data = stripslashes($data);
    $data = json_decode($data, FALSE);
    return iconvert_in($data);
}

?>
