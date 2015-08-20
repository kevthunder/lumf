<?php 
use ICanBoogie\Inflector;

class WebformLumfHelper {
  function __construct() { 
    
  }
  
  function save($name,$data){
    $local = array('components');
    $types = webform_variable_get('webform_node_types');
    if(empty($data['type'])){
      if(count($types) == 1){
        $data['type'] = reset($types);
      }
    }else if(!in_array($data['type'],$types)){
      throw new Exception('Node of type '.$data['type'].' cant be webforms');
    }
    $node = lumf('node')->save($name,array_diff_key($data,array_flip($local)));

    if(!empty($data['components'])){
      $this->saveComponents($node,$data['components']);
    }
  }
  
  function saveComponents($node,$components,$pid = null){
    $i = 0;
    foreach($components as $form_key => $compnt ){
      $component = $this->formatComponent($compnt);
      $component['form_key'] = $form_key;
      $component['nid'] = $node->nid;
      if(!is_null($pid)) $component['pid'] = $pid;
      
      $existing = $this->findComponent($node,$form_key);
      if(empty($existing)){
        $defaults = $this->componentDefaults($component);
        $defaults['weight'] = $i;
        $data = array_replace_recursive($defaults,$component);

        $cid = webform_component_insert($data);
      }else{
        $cid = $existing['cid'];
        if(!lumf('array')->options_are_eq($component,$existing)){
          $data = array_replace_recursive($existing,$component);
          webform_component_update($data);
        }
      }
      if(!empty($compnt['children']) && !empty($cid)){
        $this->saveComponents($node,$compnt['children'], $cid);
      }
      $i++;
    }
  }
  
  function formatComponent($component){
    $fields = array('nid','cid','pid','form_key','name','type','value','extra','mandatory','weight');
    
    if(!is_array($component)){
      $component = array('type'=> 'markup','value'=>$component);
    }
    
    if(method_exists ( $this , 'formatComponent_' . $component['type'] )){
      $component = call_user_func(array($this,'formatComponent_' . $component['type']),$component);
    }
    
    unset($component['children']);
    
    $component['extra'] = array_merge(empty($component['extra'])?array():$component['extra'],array_diff_key($component,array_flip($fields)));
    $component = array_intersect_key($component,array_flip($fields));
    return $component;
  }
  
  function formatComponent_select($component){
    if(!empty($component['options'])){
      $component['extra']['items'] = $this->formatOptionItems($component['options']);
      unset($component['options']);
    }
    return $component;
  }
  
  function formatComponent_grid($component){
    if(!empty($component['options'])){
      $component['extra']['options'] = $this->formatOptionItems($component['options']);
      unset($component['options']);
    }
    if(!empty($component['questions'])){
      $component['extra']['questions'] = $this->formatOptionItems($component['questions']);
      unset($component['questions']);
    }
    return $component;
  }
  
  
  function componentDefaults($component){
    $inflector = Inflector::get();
    $defaults = array(
      'name' => $inflector->humanize($component['form_key']),
      'extra' => array(),
      'type' => 'textfield',
      'pid' => 0,
    );
    if(method_exists ( $this , 'componentDefaults_' . $component['type'] )){
      $defaults = call_user_func(array($this,'componentDefaults_' . $component['type']), $component, $defaults);
    }
    return $defaults;
  }
  
  function componentDefaults_markup($component,$defaults){
    $defaults['extra']['format'] = 'full_html';
    return $defaults;
  }
  
  function formatOptionItems($options){
    $items = array();
    foreach ($options as $key => $val) {
      $items[] = $key .'|'. $val;
    }
    return implode("\n",$items);
  }
  
  function findComponent($node,$form_key){
    if(!empty($node->webform['components'])){
      foreach($node->webform['components'] as $component){
        if($component['form_key'] == $form_key){
          return $component;
        }
      }
    }
  }
}
 ?>