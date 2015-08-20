<?php
class StrLumfHelper {
  function __construct() { 
    
  }
  
  // module_load_include('inc', 'lumf', 'array_helper');
  
  function recursive_replace($data,$replace,$iterateObj = false){
    if(is_string($data)){
      $data = str_replace(array_keys($replace),array_values($replace),$data);
    }
    if(is_array($data) || ($iterateObj && is_object($data))){
      foreach($data as &$child){
        $child = $this->recursive_replace($child,$replace);
      }
    } 
    return $data;
  }
}
 ?>