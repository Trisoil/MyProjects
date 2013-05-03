<?php
/*
    class a
    {
        function f()
        {
            $this->f1();
        }
        
        function f1()
        {
            echo "a::f1";
        }
    };
    
    class b extends a
    {
        function f1()
        {
            echo "b::f1";
        }
    }
    
    $a = new b;
    $a->f();

    $arr = array("k1" => "1", "k2" => "2");
    $keys = array_keys($arr);
    var_dump($keys);
    echo is_string($keys[0]) ? "true" : "false";
*/
/*
    class A
    {
        public $i = 0;
        public $j = "sss";
    };
    
    $a = new a;
    foreach ($a as $k => $v) {
        echo '<p>' . $k . ' : ' . $v;
    }
    */
    require_once('object_set.php');
    var_dump(object_set::make_filter($_REQUEST['filter']));

?>
