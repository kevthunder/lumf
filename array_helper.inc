<?php
class ArrayLumfHelper {
  function __construct() { 
    
  }
  
  // module_load_include('inc', 'lumf', 'array_helper');
  
  function options_are_eq($opt1,$opt2){
    if(is_array($opt1)){
      if(!is_array($opt2)) return false;
      foreach($opt1 as $key => $val){
        if(!array_key_exists($key,$opt2)) return false;
        if(!$this->options_are_eq($val,$opt2[$key])) return false;
      }
    }else if((string)$opt1 !== (string)$opt2){
      return false;
    }
    return true;
  }
}
 ?>