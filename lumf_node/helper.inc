<?php 
class NodeLumfHelper {
  function __construct() { 
    
  }
  
  function create_or_update_type($data) {
    # deprecated, use this instead :
    return $this->save_type($data);
  }
  
  function save_type($data) {
    $default = array(
      'type' => null,
      'name' => null,
      'base' => 'node_content',
      'module' => null,
    );
    $fields = array('type','name','base','has_title','title_label','description','help','custom','modified','locked','disabled','module');
    if(empty($data['module'])){
      $default['module'] = $this->gessModule();
    }
    $data = array_merge($default, $data);
    $object = new stdClass();
    foreach ($data as $key => $value){
      $object->$key = $value;
    }
    node_type_save($object);
    $toVar = array_diff_key($data,array_flip($fields));
    foreach($toVar as $key => $val){
      $variableName = $key . '_' . $object->type;
      if(is_array($val)){
        $val = array_merge_recursive(variable_get($variableName, array()),$val);
      }
      variable_set($variableName, $val);
    }
  }
  
  function gessModule($ignore=array()){
    $ignore[] = 'lumf';
    $ignore[] = 'lumf_node';
    $backtrace = debug_backtrace(DEBUG_BACKTRACE_IGNORE_ARGS);
    $modules = module_list();
    foreach ($backtrace as $step) {
      if(!empty($step['file'])){
        $file = substr(str_replace('\\','/',str_replace(DRUPAL_ROOT,'',$step['file'])),1);
        do{
          if(!in_array(basename($file),$ignore) && !empty($modules[basename($file)]) && drupal_get_path('module', basename($file)) == $file){
            return basename($file);
          }
          $file = dirname($file);
        }while($file != '.');
      }
    }
  }
  
  function create_or_update($name,$data) {
    # deprecated, use this instead :
    return $this->save($name,$data);
  }
  
  function save($name,$data) {
    $default = array(
      'language' => LANGUAGE_NONE
    );
    if(!empty($data['language']) && $data['language'] != LANGUAGE_NONE){
      $ids = machinenamed_translated_node_nids($name);
      if(!empty($ids[$data['language']])){
        $node = node_load($ids[$data['language']]);
        unset($ids[$data['language']]);
      }else if(!empty($ids[LANGUAGE_NONE])){
        $node = node_load($ids[LANGUAGE_NONE]);
        unset($ids[LANGUAGE_NONE]);
      }
      if(count($ids)){
        $original_id = reset($ids);
        $original = node_load($original_id);
        // kpr($original);
        if(empty($original->tnid)){
          $original->tnid = $original->nid;
          node_save($original);
        }
        $data['tnid'] = $original->tnid;
      }
    }else{
      $node = machinenamed_node_load($name);
    }
    $data = $this->formatFields($data);

    if(!$node || !lumf('array')->options_are_eq($data,(array)$node)){
      $data['machine_name'] = $name;
      if($node) {
        $data = $this->_mergeDataToNode($data,$node);
      }else{
        $data = (object)array_merge($data,$default);
        node_object_prepare($data);
      }
      if(!empty($data)){
        node_save($data);
        return $data;
      }
      
    }else{
      return $node;
    }
  }
  
  function mergeDataToNode($data,$node){
    $data = $this->formatFields($data,$node->type);
    return $this->_mergeDataToNode($data,$node);
  }
  function _mergeDataToNode($data,$node){
    $tmpdata = $node;
    foreach ($data as $key => $val) {
      if(is_array($val) && isset($tmpdata->{$key})){
        $tmpdata->{$key} = array_replace_recursive($tmpdata->{$key},$val);
      }else{
        $tmpdata->{$key} = $val;
      }
    }
    return $tmpdata;
  }
  
  function formatFields($data,$type=null){
    if(empty($type) && !empty($data['type'])) $type = $data['type'];
    return lumf('field')->formatValues($data,'node',$type);
  }
  
  function translated_nid($nid, $langcode = null){
    if(is_null($langcode)) $langcode = $GLOBALS['language']->language;
    $cache = &drupal_static('lumf_node_translated_nid');
    if(is_null($cache)){
      $cache = array();
      // $cacheObj = cache_get('lumf_node_translated_nid');
      // if($cacheObj){
        // $cache = $cacheObj->data;
      // }
    }

    $newNid = $nid;
      
    if(!empty($cache[$langcode][$nid])){
      $newNid = $cache[$langcode][$nid];
    }else{
      $node = node_load($nid);
      if($langcode == LANGUAGE_NONE){
        if (!empty($node->tnid)) {
          $newNid = $node->tnid;
        }
      }else{
        if (!empty($node->tnid)) {
          $translations = translation_node_get_translations($node->tnid);
          foreach($translations as $tlang => $translation){
            if(!empty($translation->nid)){
              $cache[$tlang][$nid] = $translation->nid;
            }
          }
        }
      }
      if(!empty($cache[$langcode][$nid])){
        $newNid = $cache[$langcode][$nid];
      }else{
        $cache[$langcode][$nid] = $newNid;
      }
      //cache_set('lumf_node_translated_nid', $cache, 'cache',  CACHE_TEMPORARY);
    }
    
    return $newNid;
  }
}
 ?>