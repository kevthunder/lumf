<?php 
class FieldLumfHelper {
  function __construct() { 
    
  }

  function save($info){
    $def = array(
      'entity_type' => 'node'
    );
    $info = array_merge($def, $info);
    if(empty($info['bundle'])) $info['bundle'] = $info['entity_type'];
    $field = field_read_field($info['field_name']);
    $instance = $field ? field_read_instance($info['entity_type'], $info['field_name'], $info['bundle']) : false;
    $fieldKeys = array('field_name','type','cardinality','locked','indexes','storage');
    $instanceKeys = array('field_name','entity_type','bundle','label','description','required','default_value_function','widget','display');
    $localKeys = array();
    
    if(method_exists ($this,'applyDefaultFor_'.$info['type'])){
      $info = $this->{'applyDefaultFor_'.$info['type']}($info);
    }
    
    $fieldInfo = array_diff_key($info,array_flip(array_diff($instanceKeys,$fieldKeys)),array_flip($localKeys));
    $instanceInfo = array_diff_key($info,array_flip(array_diff($fieldKeys,$instanceKeys)),array_flip($localKeys));
    
    module_load_include('inc', 'lumf', 'array_helper');
    
    if(!empty($field)){
      if(!lumf('array')->options_are_eq($fieldInfo,$field)){
        field_update_field(array_merge($field,$fieldInfo));
      }
    }else{
      $existing_table = db_query("SHOW TABLES LIKE 'field_data_".$info['field_name']."'")->fetchCol();
      if(!empty($existing_table)){
        trigger_error('The tables for the field '.$info['field_name'].' exists but the fields does not. A module could have been uninstaled incorectly.',E_USER_WARNING);
        db_query('DROP TABLE IF EXISTS field_data_'.$info['field_name'].', field_revision_'.$info['field_name']);
      }
      field_create_field($fieldInfo);
    }
    
    if(!empty($instance)){
      if(!lumf('array')->options_are_eq($instanceInfo,$instance)){
        field_update_instance(array_merge($instance,$instanceInfo));
      }
    }else{
      field_create_instance($instanceInfo);
    }
  }
  
  function applyDefaultFor_list_boolean($info){
    if(empty($info['settings']['allowed_values'])) $info['settings']['allowed_values'] = array('','');
    if(empty($info['widget']['type'])) $info['widget']['type'] = 'options_onoff';
    if( $info['widget']['type'] == 'options_onoff' && !isset($info['widget']['settings']['display_label']) ){
      $info['widget']['settings']['display_label'] = 1;
    }
    return $info;
  }
  
  function save_multi($infos,$def = array()){
    foreach($infos as $info){
      $this->save(array_merge_recursive($def,$info));
    }
  }
  
  
  function formatValues($data,$entityType,$bundle){
    foreach ($data as $key => $val) {
      $data[$key] = $this->formatValue($val,$key,$entityType,$bundle);
    }
    return $data;
  }
  
  function formatValue($val,$fieldName,$entityType,$bundle){
    if($field = $this->propIsField($fieldName,$entityType,$bundle)){
      if(!is_array($val)){
        return array(LANGUAGE_NONE=>array($this->addColsToFieldVal($field,$val)));
      }else{
        $lang_keys = array_merge(array_keys(language_list()),array(LANGUAGE_NONE));
        if(!count(array_intersect(array_keys($val),$lang_keys))){
          $val = array(LANGUAGE_NONE=>$val);
        }
        foreach ($val as $lang => &$n1) {
          if(!is_array($n1)){
            $n1 = array($this->addColsToFieldVal($field,$n1));
          }else{
            if(count(array_filter(array_keys($n1), 'is_numeric'))){
              foreach ($n1 as $index => &$n2) {
                if(!is_array($n2)){
                  $n2 = $this->addColsToFieldVal($field,$n2);
                }
              }
            }else{
              $n1 = array($n1);
            }
          }
        }
      }
    }
    return $val;
  }
  
  
  function addColsToFieldVal($field,$val){
    return array_merge($field['defaults'],array(key($field['columns'])=>$val));
  }
  
  function propIsField($field,$entityType,$bundle){
    $instance = field_info_instance($entityType,$field,$bundle);
    if($instance){
      $field = (array)field_info_field($field);
      $field['instance'] = $instance;
      foreach($field['columns'] as $col => $opt){
        if($col == 'format'){
          $field['defaults'][$col] = 'filtered_html';
        }else if($col == 'summary'){
          $field['defaults'][$col] = '';
        }else if(!$opt['not null']){
          $field['defaults'][$col] = null;
        }
      }
      return $field;
    }
    return null;
  }
  
}
 ?>