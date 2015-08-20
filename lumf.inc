<?php 
use ICanBoogie\Inflector;

function lumf($module){
  return LumfHelperBase::get($module);
}
function less_ui_more_fun($module){
  return LumfHelperBase::get($module);
}

class LumfHelperBase {
  function __construct() {
    
  }
  static function get($module){
    include_once 'vendor/autoload.php';
    $inflector = Inflector::get();
    
    $className = $inflector->camelize($module).'LumfHelper';
    $moduleNames = array('lumf_'.$module, $module.'_lumf');
   
    if(LumfHelperBase::multi_include($moduleNames) || module_load_include('inc', 'lumf', $module.'_helper')){
      return new $className();
    }else{
      $files = system_rebuild_module_data();
      if($foundModule = LumfHelperBase::findModule($moduleNames)){
        if($foundModule->status){
          throw new Exception('The module '.$foundModule->name.' does not seems to implement the '.$module.' helper');
        }else{
          throw new Exception('The '.$module.' helper was not found but the module '.$foundModule->name.' seems desabled you may need tu activate it : drush en '.$foundModule->name.' -y');
        }
      }else{
        throw new Exception('No "Less UI More Fun" helper with the name '.$module.' was found');
      }
    }
  }
  
  static function findModule($moduleNames){
    $files = system_rebuild_module_data();
    foreach ($moduleNames as $name) {
      if(!empty($files[$name])) return $files[$name];
    }
    return false;
  }
  
  static function multi_include($moduleNames){
    foreach ($moduleNames as $name) {
      $res = module_load_include('inc', $name, 'helper');
      if($res) return $res;
    }
    return false;
  }
}

 ?>