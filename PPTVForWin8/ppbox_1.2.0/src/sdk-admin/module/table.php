<?php

class table
{
    function __construct($link, $tabname)
    {
        $this->link_ = $link;
        $this->tabname_ = $tabname;
    }

    /* fields array(string)
        */
    function select($fields = null, $param)
    {
        $sql = "SELECT ";
        if (!$fields) {
            $sql = $sql . "*";
        } else {
            $sql = $sql . table::array_list($fields, ", ");
        }
        $sql = $sql . " FROM {$this->tabname_}";
        if (isset($param->filter) && $param->filter) {
            $sql = $sql . " WHERE " . table::array_list($param->filter, " AND ", "[value]");
        }
        if (isset($param->sort) && isset($param->dir)) {
            $sql = $sql . " ORDER BY {$param->sort} {$param->dir}";
        }
        if (isset($param->start) && isset($param->limit)) {
            $sql = $sql . " LIMIT {$param->start}, {$param->limit}";
        }
        //var_dump($sql);
        $result = $this->link_->query($sql);
        if (is_bool($result) && $result == FALSE)
            return FALSE;
        $rows = array();
        while($obj = $this->link_->fetch_object($result)) {
            $rows[] = $obj;
        }
        $this->link_->free_result($result);
        return $rows;
    }
    
    function insert($record, $return_id = FALSE)
    {
        if (return_id)
            unset($record->id);
        $sql = "INSERT INTO {$this->tabname_}";
        $sql = $sql . " (" . table::array_list($record, ", ", "[key]") . ")";
        $sql = $sql . " VALUES (" . table::array_list($record, ", ", '[strvalue]') . ")";
        $result = $this->link_->query($sql);
        if (!$result)
            return FALSE;
        if (!$return_id)
            return $result;
        $result = $this->link_->query("SELECT LAST_INSERT_ID() AS id");
        if (!$result)
            return FALSE;
        $row = $this->link_->fetch_object($result);
        $this->link_->free_result($result);
        return $row;
    }

    function update($record, $where = null)
    {
        $sql = "UPDATE {$this->tabname_} SET ";
        $sql = $sql . table::array_list($record, ", ", " = ");
        if ($where) {
            $sql = $sql . " WHERE " . table::array_list($where, " AND ", ' = ');
        }
        $result = $this->link_->query($sql);
        if (!$result)
            return FALSE;
        if ($this->link_->affected_rows() == 0) {
            $this->errmsg_ = 'No match row';
            return FALSE;
        }
        return TRUE;
    }

    function delete($where)
    {
        $sql = "DELETE FROM {$this->tabname_} WHERE " . table::array_list($where, " AND ", " = ");
        $result = $this->link_->query($sql);
        if (!$result)
            return FALSE;
        if ($this->link_->affected_rows() == 0) {
            $this->errmsg_ = 'No match row';
            return FALSE;
        }
        return TRUE;
    }

    function query($sql)
    {
        return $this->link_->query($sql);
    }

    function error()
    {
        if ($this->errmsg_) {
            return $this->errmsg_;
            $this->errmsg_ = '';
        } else {
            return $this->link_->error();
        }
    }
    
    static function array_list($array, $delim = ', ', $op)
    {
        if (is_array($array) || is_object($array)) {
            $list = "";
            if ($op == '[strvalue]') {
                foreach ($array as $key => $value) {
                    $list = $list .  $delim . "'" . $value . "'";
                }
            } else if ($op == '[key]') {
                foreach ($array as $key => $value) {
                    $list = $list .  $delim . "`" . $key . "`";
                }
            } else if ($op == '[value]') {
                foreach ($array as $key => $value) {
                    $list = $list .  $delim . $value;
                }
            } else {
                foreach ($array as $key => $value) {
                    $list = $list .  $delim . "`" . $key . "`" . $op . "'" . $value . "'";
                }
            }
            return substr($list, strlen($delim));
        } else {
            return $array;
        }
    }
    

    private $link_;
    private $tabname_;
    private $errmsg_;

};

/*
echo table::array_list(array(1, 2, 3));
*/

?>
